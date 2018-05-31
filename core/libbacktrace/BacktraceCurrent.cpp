/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define _GNU_SOURCE 1
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <ucontext.h>
#include <unistd.h>

#include <stdlib.h>

#include <string>

#include <backtrace/Backtrace.h>
#include <backtrace/BacktraceMap.h>

#include "BacktraceAsyncSafeLog.h"
#include "BacktraceCurrent.h"
#include "ThreadEntry.h"
#include "thread_utils.h"

bool BacktraceCurrent::ReadWord(uintptr_t ptr, word_t* out_value) {
  if (!VerifyReadWordArgs(ptr, out_value)) {
    return false;
  }

  backtrace_map_t map;
  FillInMap(ptr, &map);
  if (BacktraceMap::IsValid(map) && map.flags & PROT_READ) {
    *out_value = *reinterpret_cast<word_t*>(ptr);
    return true;
  } else {
    BACK_ASYNC_SAFE_LOGW("pointer %p not in a readable map", reinterpret_cast<void*>(ptr));
    *out_value = static_cast<word_t>(-1);
    return false;
  }
}

size_t BacktraceCurrent::Read(uintptr_t addr, uint8_t* buffer, size_t bytes) {
  backtrace_map_t map;
  FillInMap(addr, &map);
  if (!BacktraceMap::IsValid(map) || !(map.flags & PROT_READ)) {
    return 0;
  }
  bytes = MIN(map.end - addr, bytes);
  memcpy(buffer, reinterpret_cast<uint8_t*>(addr), bytes);
  return bytes;
}

bool BacktraceCurrent::Unwind(size_t num_ignore_frames, ucontext_t* ucontext) {
  if (GetMap() == nullptr) {
    // Without a map object, we can't do anything.
    error_ = BACKTRACE_UNWIND_ERROR_MAP_MISSING;
    return false;
  }

  error_ = BACKTRACE_UNWIND_NO_ERROR;
  if (ucontext) {
    return UnwindFromContext(num_ignore_frames, ucontext);
  }

  if (Tid() != gettid()) {
    return UnwindThread(num_ignore_frames);
  }

  return UnwindFromContext(num_ignore_frames, nullptr);
}

bool BacktraceCurrent::DiscardFrame(const backtrace_frame_data_t& frame) {
  if (BacktraceMap::IsValid(frame.map)) {
    const std::string library = basename(frame.map.name.c_str());
    if (library == "libunwind.so" || library == "libbacktrace.so") {
      return true;
    }
  }
  return false;
}

static pthread_mutex_t g_sigaction_mutex = PTHREAD_MUTEX_INITIALIZER;

// Since errno is stored per thread, changing it in the signal handler
// modifies the value on the thread in which the signal handler executes.
// If a signal occurs between a call and an errno check, it's possible
// to get the errno set here. Always save and restore it just in case
// code would modify it.
class ErrnoRestorer {
 public:
  ErrnoRestorer() : saved_errno_(errno) {}
  ~ErrnoRestorer() {
    errno = saved_errno_;
  }

 private:
  int saved_errno_;
};

static void SignalLogOnly(int, siginfo_t*, void*) {
  ErrnoRestorer restore;

  BACK_ASYNC_SAFE_LOGE("pid %d, tid %d: Received a spurious signal %d\n", getpid(), gettid(),
                       THREAD_SIGNAL);
}

#ifdef DEBUG_DUMP_BACKTRACE_TIMEOUT
time_t g_debug_tm[7];
#endif

static void SignalHandler(int, siginfo_t*, void* sigcontext) {
  ErrnoRestorer restore;

  ThreadEntry* entry = ThreadEntry::Get(getpid(), gettid(), false);
  if (!entry) {
    BACK_ASYNC_SAFE_LOGE("pid %d, tid %d entry not found", getpid(), gettid());
    return;
  }

  entry->CopyUcontextFromSigcontext(sigcontext);

#ifdef DEBUG_DUMP_BACKTRACE_TIMEOUT
  g_debug_tm[1] = time(nullptr);  // before wake 1
#endif
  // Indicate the ucontext is now valid.
  entry->Wake();

  // Pause the thread until the unwind is complete. This avoids having
  // the thread run ahead causing problems.
  // The number indicates that we are waiting for the second Wake() call
  // overall which is made by the thread requesting an unwind.
  if (entry->Wait(2)) {
    // Do not remove the entry here because that can result in a deadlock
    // if the code cannot properly send a signal to the thread under test.
#ifdef DEBUG_DUMP_BACKTRACE_TIMEOUT
    g_debug_tm[5] = time(nullptr);  // before wake 3
#endif
    entry->Wake();
  } else {
#ifdef DEBUG_DUMP_BACKTRACE_TIMEOUT
    g_debug_tm[5] = time(nullptr);  // wait 2 fail
#endif
    // At this point, it is possible that entry has been freed, so just exit.
    BACK_ASYNC_SAFE_LOGE("Timed out waiting for unwind thread to indicate it completed.");
  }
}

bool BacktraceCurrent::UnwindThread(size_t num_ignore_frames) {
  // Prevent multiple threads trying to set the trigger action on different
  // threads at the same time.
  pthread_mutex_lock(&g_sigaction_mutex);

  ThreadEntry* entry = ThreadEntry::Get(Pid(), Tid());
  entry->Lock();

  struct sigaction act, oldact;
  memset(&act, 0, sizeof(act));
  act.sa_sigaction = SignalHandler;
  act.sa_flags = SA_RESTART | SA_SIGINFO | SA_ONSTACK;
  sigemptyset(&act.sa_mask);
  if (sigaction(THREAD_SIGNAL, &act, &oldact) != 0) {
    BACK_ASYNC_SAFE_LOGE("sigaction failed: %s", strerror(errno));
    ThreadEntry::Remove(entry);
    pthread_mutex_unlock(&g_sigaction_mutex);
    error_ = BACKTRACE_UNWIND_ERROR_INTERNAL;
    return false;
  }

#ifdef DEBUG_DUMP_BACKTRACE_TIMEOUT
  g_debug_tm[0] = time(nullptr);  // before THREAD_SIGNAL
#endif

  if (tgkill(Pid(), Tid(), THREAD_SIGNAL) != 0) {
    // Do not emit an error message, this might be expected. Set the
    // error and let the caller decide.
    if (errno == ESRCH) {
      error_ = BACKTRACE_UNWIND_ERROR_THREAD_DOESNT_EXIST;
    } else {
      error_ = BACKTRACE_UNWIND_ERROR_INTERNAL;
    }

    sigaction(THREAD_SIGNAL, &oldact, nullptr);
    ThreadEntry::Remove(entry);
    pthread_mutex_unlock(&g_sigaction_mutex);
    return false;
  }

  // Wait for the thread to get the ucontext. The number indicates
  // that we are waiting for the first Wake() call made by the thread.
  bool wait_completed = entry->Wait(1);

#ifdef DEBUG_DUMP_BACKTRACE_TIMEOUT
  g_debug_tm[2] = time(nullptr);  // after wait 1
#endif

  if (!wait_completed && oldact.sa_sigaction == nullptr) {
    // If the wait failed, it could be that the signal could not be delivered
    // within the timeout. Add a signal handler that's simply going to log
    // something so that we don't crash if the signal eventually gets
    // delivered. Only do this if there isn't already an action set up.
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = SignalLogOnly;
    act.sa_flags = SA_RESTART | SA_SIGINFO | SA_ONSTACK;
    sigemptyset(&act.sa_mask);
    sigaction(THREAD_SIGNAL, &act, nullptr);
  } else {
    sigaction(THREAD_SIGNAL, &oldact, nullptr);
  }
  // After the thread has received the signal, allow other unwinders to
  // continue.
  pthread_mutex_unlock(&g_sigaction_mutex);

  bool unwind_done = false;
  if (wait_completed) {
#ifdef DEBUG_DUMP_BACKTRACE_TIMEOUT
    g_debug_tm[3] = time(nullptr);  // before unwind
#endif
    unwind_done = UnwindFromContext(num_ignore_frames, entry->GetUcontext());
#ifdef DEBUG_DUMP_BACKTRACE_TIMEOUT
    g_debug_tm[4] = time(nullptr);  // after unwind
#endif

    // Tell the signal handler to exit and release the entry.
    entry->Wake();

    // Wait for the thread to indicate it is done with the ThreadEntry.
    if (!entry->Wait(3)) {
      // Send a warning, but do not mark as a failure to unwind.
      BACK_ASYNC_SAFE_LOGW("Timed out waiting for signal handler to indicate it finished.");
#ifdef DEBUG_DUMP_BACKTRACE_TIMEOUT
     g_debug_tm[6] = time(nullptr);  // wait 3 timeout
     BACK_ASYNC_SAFE_LOGE("unwind_done:%d, tid:%d, time:%lu-%lu-%lu-%lu-%lu-%lu-%lu",
                         unwind_done, Tid(), g_debug_tm[0],
                         g_debug_tm[1],g_debug_tm[2], g_debug_tm[3],
                         g_debug_tm[4],g_debug_tm[5], g_debug_tm[6]);
#endif
    }
  } else {
    // Check to see if the thread has disappeared.
    if (tgkill(Pid(), Tid(), 0) == -1 && errno == ESRCH) {
      error_ = BACKTRACE_UNWIND_ERROR_THREAD_DOESNT_EXIST;
    } else {
      error_ = BACKTRACE_UNWIND_ERROR_THREAD_TIMEOUT;
      BACK_ASYNC_SAFE_LOGE("Timed out waiting for signal handler to get ucontext data.");
    }
  }

  ThreadEntry::Remove(entry);

  return unwind_done;
}

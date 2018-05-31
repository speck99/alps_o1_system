
/* Copyright Statement:
 * *
 * * This software/firmware and related documentation ("MediaTek Software") are
 * * protected under relevant copyright laws. The information contained herein
 * * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * * Without the prior written permission of MediaTek inc. and/or its licensors,
 * * any reproduction, modification, use or disclosure of MediaTek Software,
 * * and information contained herein, in whole or in part, shall be strictly prohibited.
 * *
 * * MediaTek Inc. (C) 2016. All rights reserved.
 * *
 * * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 * *
 * * The following software/firmware and/or related documentation ("MediaTek Software")
 * * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * * applicable license agreements with MediaTek Inc.
 * */

#define LOG_TAG "bt_snoop_twrite"

#include "twrite.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <memory>
#include <mutex>
#include <vector>

#include <base/bind.h>
#include <base/logging.h>
#include <base/memory/singleton.h>
#include <base/message_loop/message_loop.h>
#include <base/run_loop.h>
#include <base/sequenced_task_runner.h>
#include <chrono>

#include "bt_hci_bdroid.h"
#include "btsnoop_mem.h"
#include "hci_inbound_data_monitor.h"
#include "log_time.h"
#include "module.h"
#include "mtk_stack_config.h"
#include "mtk_util.h"
#include "osi/include/future.h"
#include "osi/include/log.h"
#include "osi/include/osi.h"
#include "osi/include/properties.h"
#include "osi/include/thread.h"
#include "stack_config.h"

// To disable by default
#define DBG_LOG_ENABLE FALSE

#if DBG_LOG_ENABLE == TRUE
#define DBG_LOG(tag, fmt, args...) LOG_DEBUG(LOG_TAG, fmt, ##args)
#else
#define DBG_LOG(tag, fmt, args...) ((void)0)
#endif

void btsnoop_net_open();
void btsnoop_net_close();
void btsnoop_net_write(const void* data, size_t length);

// Module life cycle functions
static future_t* start_up(void) {
  vendor::mediatek::bluetooth::stack::BTSnoop::GetInstance()->StartUp();
  return NULL;
}

static future_t* shut_down(void) {
  vendor::mediatek::bluetooth::stack::BTSnoop::GetInstance()->Stop();
  return NULL;
}

EXPORT_SYMBOL extern const module_t mtk_btsnoop_module = {
    .name = MTK_BTSNOOP_MODULE,
    .init = NULL,
    .start_up = start_up,
    .shut_down = shut_down,
    .clean_up = NULL,
    .dependencies = {STACK_CONFIG_MODULE, NULL}};

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

const std::string kBTSnoopPathProperty("persist.bluetooth.btsnooppath");
const std::string kBTSnoopDefaultPathProperty(
    "/sdcard/mtklog/btlog/btsnoop_hci.log");

class BTSnoopImpl {
 public:
  enum PacketType {
    kCommandPacket = 1,
    kAclPacket = 2,
    kScoPacket = 3,
    kEventPacket = 4
  };

  struct __attribute__((__packed__)) btsnoop_header_t {
    uint32_t length_original;
    uint32_t length_captured;
    uint32_t flags;
    uint32_t dropped_packets;
    uint64_t timestamp;
    uint8_t type;
  };

  BTSnoopImpl()
      : kBTSnoopFileHeader_("btsnoop\0\0\0\0\1\0\0\x3\xea", 16),
        message_loop_(nullptr),
        run_loop_(nullptr),
        thread_(NULL),
        logfile_fd_(INVALID_FD),
        is_logging_(false),
        is_started_(false),
        inbound_data_monitor_(nullptr) {}
  ~BTSnoopImpl() = default;

  void StartUp();
  void Capture(const BT_HDR* buffer, bool is_received);
  void Stop();

 private:
  std::string GetBTSnoopLogPath() {
    char log_path[PROPERTY_VALUE_MAX] = {0};
    osi_property_get(kBTSnoopPathProperty.c_str(), log_path,
                     kBTSnoopDefaultPathProperty.c_str());
    return std::string(log_path);
  }

  void BackupLogFile(const std::string& log_path) {
    // Save the old log if configured to do so
    if (StackConfig::GetInstance()->IsBTSnoopSaveLastEnabled()) {
      std::string last_log_path =
          log_path + std::string("_") + LogTime::GetInstance()->GetLogTimeTag();
      if (rename(log_path.c_str(), last_log_path.c_str()) && errno != ENOENT)
        LOG_ERROR(LOG_TAG, "%s unable to rename '%s' to '%s': %s", __func__,
                  log_path.c_str(), last_log_path.c_str(), strerror(errno));
    }
  }

  bool CreateLogFile(const std::string& log_path) {
    mode_t prevmask = umask(0);
    logfile_fd_ = open(log_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC,
                       S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if (logfile_fd_ == INVALID_FD) {
      LOG_ERROR(LOG_TAG, "%s unable to open '%s': %s", __func__,
                log_path.c_str(), strerror(errno));
      umask(prevmask);
      return false;
    }
    umask(prevmask);
    return true;
  }

  std::string InitCustLogPath(const std::string& log_path);
  // TODO(Peng): Sorry to apply software workaround, to remove it
  bool MakeDir(const std::string &dir, uint8_t retry_times);
  void RemoveAllLogs(const std::string& log_path);
  void WritePacket(PacketType type, uint8_t* packet, bool is_received,
                   uint64_t timestamp_us);

  static void BTSnoopInitialize(BTSnoopImpl* impl);
  static void RunMessageLoop(void* context);
  static void WriteData(int fd, btsnoop_header_t header,
                        std::vector<uint8_t> packets);

  const std::string kBTSnoopFileHeader_;

  std::mutex btsnoop_mutex_;
  std::mutex message_loop_mutex_;
  base::MessageLoop* message_loop_;
  base::RunLoop* run_loop_;
  thread_t* thread_;
  int logfile_fd_;
  bool is_logging_;
  bool is_started_;
  std::unique_ptr<InboundDataMonitor> inbound_data_monitor_;
};

void BTSnoopImpl::StartUp() {
  if (is_started_) {
    return;
  }
  thread_ = thread_new("mbtsnoop_thread");
  if (!thread_) {
    LOG_ERROR(LOG_TAG, "%s unable to create thread.", __func__);
  } else {
    if (!thread_set_priority(thread_, 0)) {
      LOG_ERROR(LOG_TAG, "%s unable to make thread priority.", __func__);
    }
    thread_post(thread_, BTSnoopImpl::RunMessageLoop, this);
    is_started_ = true;
  }

  // For low power concern, don't default turn on inbound data monitor
  if (StackConfig::GetInstance()->IsInboundDataMonitorEnabled()) {
    inbound_data_monitor_.reset(
        new InboundDataMonitor(std::string(MTK_BTSNOOP_MODULE)));
    if (inbound_data_monitor_ != nullptr) {
      inbound_data_monitor_->InboundDataFilterUpdate(kInboundDataVSEFWLog, true);
    }
  }
}

void BTSnoopImpl::Stop() {
  if (!is_started_) {
    return;
  }

  std::lock_guard<std::mutex> lock(btsnoop_mutex_);
  if (logfile_fd_ != INVALID_FD) close(logfile_fd_);
  logfile_fd_ = INVALID_FD;
  btsnoop_net_close();

  {
    std::lock_guard<std::mutex> lock(message_loop_mutex_);
    // Error handling for messge_loop_ is still NULL during thread creation
    // but stop event is coming too soon
    if (message_loop_ != nullptr) {
      message_loop_->task_runner()->PostTask(
          FROM_HERE, run_loop_->QuitClosure());
    }
  }

  // Stop the thread to prevent Send() calls.
  if (thread_) {
    thread_stop(thread_);
    thread_join(thread_);
    thread_free(thread_);
    thread_ = NULL;
  }

  if (inbound_data_monitor_ != nullptr) {
    inbound_data_monitor_.reset();
  }
}

void BTSnoopImpl::Capture(const BT_HDR* buffer, bool is_received) {
  uint8_t* p = const_cast<uint8_t*>(buffer->data + buffer->offset);

  std::lock_guard<std::mutex> lock(btsnoop_mutex_);
  uint64_t timestamp_us = LogTime::GetInstance()->GetEpochTime();
  btsnoop_mem_capture(buffer, timestamp_us);

  if ((inbound_data_monitor_ != nullptr) && is_received) {
    inbound_data_monitor_->InboundDataUpdate(buffer);
  }

  if (logfile_fd_ == INVALID_FD) return;

  switch (buffer->event & MSG_EVT_MASK) {
    case MSG_HC_TO_STACK_HCI_EVT:
      WritePacket(kEventPacket, p, false, timestamp_us);
      break;
    case MSG_HC_TO_STACK_HCI_ACL:
    case MSG_STACK_TO_HC_HCI_ACL:
      WritePacket(kAclPacket, p, is_received, timestamp_us);
      break;
    case MSG_HC_TO_STACK_HCI_SCO:
    case MSG_STACK_TO_HC_HCI_SCO:
      WritePacket(kScoPacket, p, is_received, timestamp_us);
      break;
    case MSG_STACK_TO_HC_HCI_CMD:
      WritePacket(kCommandPacket, p, true, timestamp_us);
      break;
  }
}

void BTSnoopImpl::BTSnoopInitialize(BTSnoopImpl* impl) {
  std::lock_guard<std::mutex> lock(impl->btsnoop_mutex_);
  bool should_log = StackConfig::GetInstance()->IsBTSnoopEnabled();
  std::string log_path = impl->GetBTSnoopLogPath();
  std::string log_dir = impl->InitCustLogPath(log_path);
  if (!should_log) {
    if (!log_dir.empty()) {
      LOG_INFO(LOG_TAG, "%s: Try to delete snoop log under %s ",
               __func__, log_dir.c_str());
      impl->RemoveAllLogs(log_dir);
    }
  } else {
    impl->BackupLogFile(log_path);
    impl->is_logging_ = impl->CreateLogFile(log_path);
    if (impl->is_logging_) {
      LOG_INFO(LOG_TAG, "%s: begin to logging %s.", __func__, log_path.c_str());
      write(impl->logfile_fd_, impl->kBTSnoopFileHeader_.c_str(),
            impl->kBTSnoopFileHeader_.size());
      btsnoop_net_open();
    }
  }
}

void BTSnoopImpl::RunMessageLoop(void* context) {
  BTSnoopImpl* impl = static_cast<BTSnoopImpl*>(context);
  {
    std::lock_guard<std::mutex> lock(impl->message_loop_mutex_);
    impl->message_loop_ = new base::MessageLoop();
    impl->run_loop_ = new base::RunLoop();
  }

  impl->message_loop_->task_runner()->PostTask(
      FROM_HERE, base::Bind(&BTSnoopImpl::BTSnoopInitialize, impl));
  impl->run_loop_->Run();

  {
    std::lock_guard<std::mutex> lock(impl->message_loop_mutex_);
    delete impl->message_loop_;
    impl->message_loop_ = nullptr;
    delete impl->run_loop_;
    impl->run_loop_ = nullptr;
  }
}

std::string BTSnoopImpl::InitCustLogPath(const std::string& log_path) {
  CHECK(!log_path.empty());
  CHECK(std::string::npos != log_path.find_last_of('/'));

  std::string log_dir = log_path.substr(0, log_path.find_last_of('/') + 1);
  if (access(log_dir.c_str(), F_OK) != 0) {
    size_t pre(0), pos(0);
    std::string dir;
    const uint8_t MAKE_DIR_RETRY_TIMES(7);
    while (std::string::npos != (pos = log_dir.find('/', pre))) {
      dir = log_path.substr(0, pos++);
      pre = pos;
      if (dir.empty()) continue;  // leading with '/'
      if (!MakeDir(dir, MAKE_DIR_RETRY_TIMES)) {
        return std::string("");
      }
    }
  }
  return log_dir;
}

bool BTSnoopImpl::MakeDir(const std::string &dir, uint8_t retry_times) {
  bool is_done(false);
  uint8_t retry(retry_times);
  while (retry > 0) {
    if (mkdir(dir.c_str(), 0770) && (errno != EEXIST)) {
      LOG_ERROR(LOG_TAG, "%s mkdir for %s, error! %s",
                __func__, dir.c_str(), strerror(errno));
    } else {
      is_done = true;
      break;
    }
    retry--;
    sleep(1);
  }
  return is_done;
}

void BTSnoopImpl::RemoveAllLogs(const std::string& log_dir) {
  CHECK(!log_dir.empty());
  CHECK(log_dir[log_dir.size() - 1] == '/');

  DIR* p_dir = opendir(log_dir.c_str());
  if (p_dir != NULL) {
    struct dirent* p_file;
    while ((p_file = readdir(p_dir)) != NULL) {
      if (strcmp(p_file->d_name, "..") == 0 ||
          strcmp(p_file->d_name, ".") == 0) {
        continue;
      }
      std::string file_path(log_dir);
      file_path += std::string(p_file->d_name);
      if (remove(file_path.c_str())) {
        LOG_WARN(LOG_TAG, "%s remove fw log file failed. file:%s. errno: %s",
                 __func__, file_path.c_str(), strerror(errno));
        break;
      }
    }
    closedir(p_dir);
  }
}

void BTSnoopImpl::WritePacket(PacketType type, uint8_t* packet,
                              bool is_received, uint64_t timestamp_us) {
  uint32_t length_he = 0;
  uint32_t flags = 0;

  switch (type) {
    case kCommandPacket:
      length_he = packet[2] + 4;
      flags = 2;
      break;
    case kAclPacket:
      length_he = (packet[3] << 8) + packet[2] + 5;
      flags = is_received;
      break;
    case kScoPacket:
      length_he = packet[2] + 4;
      flags = is_received;
      break;
    case kEventPacket:
      length_he = packet[1] + 3;
      flags = 3;
      break;
  }

  btsnoop_header_t header;
  header.length_original = htonl(length_he);
  header.length_captured = header.length_original;
  header.flags = htonl(flags);
  header.dropped_packets = 0;
  header.timestamp = timestamp_us;
  header.type = type;

  DBG_LOG(LOG_TAG, "%s len_ori: %x, len_captured: %x, flags: %x, type: %x",
          __func__, header.length_original, header.length_captured,
          header.flags, header.type);
  DBG_LOG(LOG_TAG, "%s data: %s", __func__,
          DataArrayToString<uint8_t>(packet, (length_he - 1)).c_str());
  std::vector<uint8_t> packets(packet, (packet + (length_he - 1)));
  base::Closure callback = base::Bind(&BTSnoopImpl::WriteData, logfile_fd_,
                                      std::move(header), std::move(packets));
  std::lock_guard<std::mutex> lock(message_loop_mutex_);
  if (message_loop_ != nullptr) {
    message_loop_->task_runner()->PostTask(FROM_HERE, std::move(callback));
  }
}

void BTSnoopImpl::WriteData(int fd, btsnoop_header_t header,
                            std::vector<uint8_t> packets) {
  uint8_t* packet = packets.data();
  btsnoop_net_write(&header, sizeof(header));
  btsnoop_net_write(packet, packets.size());
  DBG_LOG(LOG_TAG, "%s len_ori: %x, len_captured: %x, flags: %x, type: %x",
          __func__, header.length_original, header.length_captured,
          header.flags, header.type);
  DBG_LOG(LOG_TAG, "%s data: %s", __func__,
          DataArrayToString<uint8_t>(packet, packets.size()).c_str());
  if (fd != INVALID_FD) {
    iovec iov[] = {{&header, sizeof(header)}, {packet, packets.size()}};
    ssize_t ret = TEMP_FAILURE_RETRY(writev(fd, iov, sizeof(iov) / sizeof(iov[0])));
    if (-1 == ret) {
      LOG_ERROR(LOG_TAG, "%s error write due to: %s", __func__, strerror(errno));
    }
  }
}

BTSnoop::BTSnoop() : btsnoop_impl_(new BTSnoopImpl()) {}

BTSnoop* BTSnoop::GetInstance() { return base::Singleton<BTSnoop>::get(); }

void BTSnoop::StartUp() { btsnoop_impl_->StartUp(); }

void BTSnoop::Capture(const BT_HDR* buffer, bool is_received) {
  btsnoop_impl_->Capture(buffer, is_received);
}

void BTSnoop::Stop() { btsnoop_impl_->Stop(); }

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

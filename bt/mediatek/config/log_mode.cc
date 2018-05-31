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

#define LOG_TAG "bt_logmode"

#include "log_mode.h"

#include <string>
#include <vector>

#include <base/logging.h>
#include <base/memory/singleton.h>

#include "osi/include/log.h"
#include "osi/include/properties.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

class LevelChecker {
 public:
  explicit LevelChecker(const std::string& key)
      : kEngBuild_("eng"), kUserDebugBuild_("userdebug"), key_(key) {}
  virtual ~LevelChecker() {}

  virtual TraceLevel Check() = 0;

 protected:
  bool IsPropertySet() {
    char value[PROPERTY_VALUE_MAX] = {0};
    if (osi_property_get(key_.c_str(), value, "")) {
      return true;
    } else {
      return false;
    }
  }

  TraceLevel ParseSysPropertyTestMode() {
    char value[PROPERTY_VALUE_MAX] = {0};
    osi_property_get(key_.c_str(), value, "");
    std::string test_mode = std::string(value);
    std::transform(test_mode.begin(), test_mode.end(), test_mode.begin(),
                   ::tolower);
    TraceLevel level = TraceLevel::kDefault;
    if (std::string("debug") == test_mode) {
      level = TraceLevel::kDebug;
    } else if (std::string("sqc") == test_mode) {
      level = TraceLevel::kSqc;
    }
    return level;
  }

  std::string GetBuildType() const {
    const std::string kBuildTypeKey("ro.build.type");
    char value[PROPERTY_VALUE_MAX] = {0};
    osi_property_get(kBuildTypeKey.c_str(), value, "");
    return std::string(value);
  }

  const std::string kEngBuild_;
  const std::string kUserDebugBuild_;

 private:
  const std::string key_;
};

class StackLevelChecker : public LevelChecker {
 public:
  StackLevelChecker()
      : LevelChecker(std::string("persist.bluetooth.hostloglevel")) {}
  virtual ~StackLevelChecker() {}

  TraceLevel Check() override {
    if (IsPropertySet()) {
      return ParseSysPropertyTestMode();
    } else {
      // LOG_INFO(LOG_TAG, "%s apply default setting by build", __func__);
      // return GetTraceLevelByBuild();
      return TraceLevel::kDefault;
    }
  }
};

class ControllerLevelChecker : public LevelChecker {
 public:
  ControllerLevelChecker()
      : LevelChecker(std::string("persist.bluetooth.fwloglevel")) {}
  virtual ~ControllerLevelChecker() {}

  TraceLevel Check() override {
    if (IsPropertySet()) {
      return ParseSysPropertyTestMode();
    } else {
      // LOG_INFO(LOG_TAG, "%s apply default setting by build", __func__);
      // return GetTraceLevelByBuild();
      // To align with Android N, do not turn on if without setting
      return TraceLevel::kDefault;
    }
  }
#if 0
 private:
  TraceLevel GetTraceLevelByBuild() const {
    return (LevelChecker::kEngBuild_ == GetBuildType()) ||
                   (LevelChecker::kUserDebugBuild_ == GetBuildType())
               ? TraceLevel::kSqc
               : TraceLevel::kDefault;
  }
#endif
};

class LogLevelImpl {
 public:
  LogLevelImpl()
      : trace_level_names_{std::string("Default"), std::string("SQC"),
                           std::string("Debug"), std::string("Unknown")} {}
  ~LogLevelImpl() = default;

  TraceLevel QueryTraceLevel(TraceType type) {
    std::unique_ptr<LevelChecker> checker;
    TraceLevel level = kDefault;
    switch (type) {
      case kStack: {
        checker = std::unique_ptr<LevelChecker>(new StackLevelChecker());
        level = checker->Check();
        break;
      }
      case kController: {
        checker = std::unique_ptr<LevelChecker>(new ControllerLevelChecker());
        level = checker->Check();
        break;
      }
      default:
        LOG_ALWAYS_FATAL("%s: unknown trace type: %d", __func__, type);
    }
    return level;
  }

  std::string GetTraceLevelName(TraceLevel level) const {
    return trace_level_names_[level];
  }

 private:
  std::vector<std::string> trace_level_names_;
};

LogLevel::LogLevel() : logmode_impl_(new LogLevelImpl()) {}

LogLevel* LogLevel::GetInstance() { return base::Singleton<LogLevel>::get(); }

TraceLevel LogLevel::QueryTraceLevel(TraceType type) {
  return logmode_impl_->QueryTraceLevel(type);
}

std::string LogLevel::GetTraceLevelName(TraceLevel level) const {
  return logmode_impl_->GetTraceLevelName(level);
}

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

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

#define LOG_TAG "bt_fw_logger_switch"

#include "fw_logger_switch.h"

#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <utils/misc.h>
#include <atomic>
#include <mutex>
#include <vector>

#include <base/logging.h>
#include <base/memory/singleton.h>

#include "bt_types.h"
#include "buffer_allocator.h"
#include "hci_internals.h"
#include "hci_layer.h"
#include "hci_packet_parser.h"
#include "log_mode.h"
#include "module.h"
#include "mtk_stack_config.h"
#include "mtk_util.h"
#include "osi/include/allocator.h"
#include "osi/include/future.h"
#include "osi/include/log.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

class FWLogSwitchImpl : public Singleton<FWLogSwitchImpl> {
 public:
  FWLogSwitchImpl()
      : hci_(hci_layer_get_interface()),
        buffer_allocator_(buffer_allocator_get_interface()),
        is_started_{false},
        last_trace_mode_{kUnknown} {}

  BT_HDR* MakePacket(uint16_t data_size);
  BT_HDR* MakeCommand(const uint8_t* cmd, uint16_t cmd_len);
  BT_HDR* MakeEnableCommand(const std::vector<uint8_t>& enable_cmd);
  BT_HDR* MakeFilterCommand(const std::vector<uint8_t>& filter_cmd);

  bool is_need_dispatch_vsc(TraceLevel current) const {
    return is_started_ && (kUnknown != current) &&
           (current != last_trace_mode_);
  }

  bool is_start_poll() const { return is_started_; }

  TraceLevel last_trace_mode() const { return last_trace_mode_; }

  bool IsNeedToSendFilterCmd(TraceLevel current) const {
    return kSqc == current || kDebug == current;
  }

  bool Start() {
    LOG_INFO(LOG_TAG, "%s", __func__);
    if (is_start_poll()) {
      std::lock_guard<std::mutex> lock(switch_mutex_);
      LOG_INFO(LOG_TAG, "%s: already started, do trace mode reset.", __func__);
      std::atomic_exchange(&last_trace_mode_, kUnknown);
    } else {
      ::android::add_sysprop_change_callback(FWLogConfigUpdate, 0);
      std::atomic_exchange(&is_started_, true);
    }
    // FW log configuration may be set before BT on
    // Check and update FWLog configuration for start up
    FWLogConfigUpdate();
    return is_started_;
  }

  bool Stop() {
    LOG_INFO(LOG_TAG, "%s", __func__);
    {
      std::lock_guard<std::mutex> lock(switch_mutex_);
      std::atomic_exchange(&is_started_, false);
      std::atomic_exchange(&last_trace_mode_, kUnknown);
    }
    return !is_started_;
  }

  void Transmit(BT_HDR* command) {
    CHECK(hci_ != NULL);
    BT_HDR* response = static_cast<BT_HDR*>(
        future_await(hci_->transmit_command_futured(command)));
    hci_packet_parser_get_interface()->parse_generic_command_complete(response);
  }

  static void FWLogConfigUpdate(void);

 private:
  // workaround for add_sysprop_change_callback without context in
  friend class Singleton<FWLogSwitchImpl>;

  const hci_t* hci_;
  const allocator_t* buffer_allocator_;
  std::atomic_bool is_started_;
  std::atomic<TraceLevel> last_trace_mode_;
  std::mutex switch_mutex_;
};

BT_HDR* FWLogSwitchImpl::MakePacket(uint16_t data_size) {
  BT_HDR* ret = (BT_HDR*)buffer_allocator_->alloc(sizeof(BT_HDR) + data_size);
  CHECK(ret != NULL);
  ret->event = 0;
  ret->offset = 0;
  ret->layer_specific = 0;
  ret->len = data_size;
  return ret;
}

BT_HDR* FWLogSwitchImpl::MakeCommand(const uint8_t* cmd, uint16_t cmd_len) {
  BT_HDR* packet = MakePacket(cmd_len);
  uint8_t* stream = packet->data;
  const uint8_t* p_cmd = cmd;
  ARRAY_TO_STREAM(stream, p_cmd, (int)cmd_len);
  return packet;
}

BT_HDR* FWLogSwitchImpl::MakeEnableCommand(
    const std::vector<uint8_t>& enable_cmd) {
  CHECK(!enable_cmd.empty());
  const uint8_t* p_cmd_buf = enable_cmd.data();
  LOG_INFO(LOG_TAG, "%s firmware command: %s", __func__,
           DataArrayToString<uint8_t>(p_cmd_buf, enable_cmd.size()).c_str());
  return MakeCommand(p_cmd_buf, static_cast<uint16_t>(enable_cmd.size()));
}

BT_HDR* FWLogSwitchImpl::MakeFilterCommand(
    const std::vector<uint8_t>& filter_cmd) {
  CHECK(!filter_cmd.empty());
  const uint8_t* p_cmd_buf = filter_cmd.data();
  LOG_INFO(LOG_TAG, "%s firmware command: %s", __func__,
           DataArrayToString<uint8_t>(p_cmd_buf, filter_cmd.size()).c_str());
  return MakeCommand(p_cmd_buf, static_cast<uint16_t>(filter_cmd.size()));
}

void FWLogSwitchImpl::FWLogConfigUpdate() {
  FWLogSwitchImpl* fwlog_switch = FWLogSwitchImpl::GetInstance();
  TraceLevel level = LogLevel::GetInstance()->QueryTraceLevel(kController);
  std::lock_guard<std::mutex> lock(fwlog_switch->switch_mutex_);
  if (fwlog_switch->is_need_dispatch_vsc(level)) {
    LOG_INFO(LOG_TAG, "%s: update log mode from %s to %s.", __func__,
             LogLevel::GetInstance()
                 ->GetTraceLevelName(fwlog_switch->last_trace_mode())
                 .c_str(),
             LogLevel::GetInstance()->GetTraceLevelName(level).c_str());
    std::atomic_exchange(&fwlog_switch->last_trace_mode_, level);
    const std::shared_ptr<ControllerLogModeParser> parser =
        StackConfig::GetInstance()->LoadFWLogModeParser(level);
    fwlog_switch->Transmit(fwlog_switch->MakeEnableCommand(
        StackConfig::GetInstance()->GetFWLogEnableCommandPacket(parser)));
    if (fwlog_switch->IsNeedToSendFilterCmd(level)) {
      fwlog_switch->Transmit(fwlog_switch->MakeFilterCommand(
          StackConfig::GetInstance()->GetFWLogFilterommandPacket(parser)));
    }
  }
}

FWLogSwitch::FWLogSwitch() {}

FWLogSwitch* FWLogSwitch::GetInstance() {
  return base::Singleton<FWLogSwitch>::get();
}

bool FWLogSwitch::StartUp() { return FWLogSwitchImpl::GetInstance()->Start(); }

bool FWLogSwitch::IsStarted() const {
  return FWLogSwitchImpl::GetInstance()->is_start_poll();
}

bool FWLogSwitch::Shutdown() { return FWLogSwitchImpl::GetInstance()->Stop(); }

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

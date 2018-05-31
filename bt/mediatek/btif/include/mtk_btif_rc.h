/* Copyright Statement:
 * *
 * * This software/firmware and related documentation ("MediaTek Software") are
 * * protected under relevant copyright laws. The information contained herein
 * * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * * Without the prior written permission of MediaTek inc. and/or its licensors,
 * * any reproduction, modification, use or disclosure of MediaTek Software,
 * * and information contained herein, in whole or in part, shall be strictly
 * * prohibited.
 * *
 * * MediaTek Inc. (C) 2016. All rights reserved.
 * *
 * * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * * NONINFRINGEMENT.
 * * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO
 * * SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER
 * * EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN
 * * FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE.
 * * MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES
 * * MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR
 * * OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED
 * * HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK
 * * SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE
 * * PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 * *
 * * The following software/firmware and/or related documentation ("MediaTek Software")
 * * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * * applicable license agreements with MediaTek Inc.
 * */
#if defined(MTK_BT_PROPRIETARY_HANDLING) && (MTK_BT_PROPRIETARY_HANDLING == TRUE)
#pragma once

#include <base/memory/singleton.h>
#include <hardware/bluetooth.h>
#include <hardware/bt_rc.h>

#include "avrc_defs.h"
#include "btif_common.h"

#include "bta_av_api.h"
#include "osi/include/alarm.h"

namespace base {
template <typename T>
struct DefaultSingletonTraits;
}  // namespace base

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace btif {

#if defined(MTK_AVRCP_APP_SETTINGS) && (MTK_AVRCP_APP_SETTINGS == TRUE)
using FillPduQueue = std::function<void(int index, uint8_t ctype, uint8_t label, bool pending,
                                 void* p_dev)>;

using SendRejectRsp = std::function<void(uint8_t rc_handle, uint8_t label, uint8_t pdu,
                                 uint8_t status, uint8_t opcode)>;

using OpcodeFromPdu = std::function<uint8_t(uint8_t pdu)>;

using CheckRcConnect = std::function<bt_status_t(const RawAddress* bd_addr)>;

using SendMetaRsp = std::function<void(const RawAddress* bd_addr, int index,
                                 tAVRC_RESPONSE* pmetamsg_resp)>;

class BtAvrcSettingImpl;

class BtAvrcSetting {
 public:

  static BtAvrcSetting* GetInstance();

/********************************************************************
**
** Function         SetAvrcSettingCB
**
** Description     This method is called in set callback
**                        from aosp code
**
** Return            bt_staus_t
**
*******************************************************************/
static void SetAvrcSettingCB (FillPduQueue fill_pdu_queue,
        SendRejectRsp send_reject_response,
        OpcodeFromPdu opcode_from_pdu,
        CheckRcConnect check_rc_connected,
        SendMetaRsp send_meta_rsp);

/********************************************************************
**
** Function         SetAppSettingsChange
**
** Description	This method is called in register notification rsp
**                        from aosp code
**
** Return            void
**
*******************************************************************/
void SetAppSettingsChange(tAVRC_PLAYER_APP_PARAM* dest_player_setting,
        btrc_player_settings_t* src_player_setting);

/*******************************************************************************
 *
 * Function         BtifRcUpstreamsSettingEvt
 *
 * Description      Executes AVRC UPSTREAMS events in btif context.
 *
 * Returns          void
 *
 ******************************************************************************/
 void BtifRcUpstreamsSettingEvt(btrc_callbacks_t* bt_rc_callbacks,
                                  uint16_t event, tAVRC_COMMAND* pavrc_cmd,
                                  uint8_t ctype, uint8_t label,
                                  void* p_dev, RawAddress* rc_addr, uint8_t rc_handle);

/**************************************************************************
**
** Function         ListPlayerAppAttrRsp
**
** Description      ListPlayerApplicationSettingAttributes (PDU ID: 0x11)
**                         This method is callled in response to PDU 0x11
**
** Returns          bt_status_t
**
****************************************************************************/
static bt_status_t  ListPlayerAppAttrRsp(RawAddress* bd_addr,
        int num_attr, btrc_player_attr_t *p_attrs);

/**********************************************************************
**
** Function          ListPlayerAppValueRsp
**
** Description      ListPlayerApplicationSettingValues (PDU ID: 0x12)
**                        This method is called in response to PDU 0x12
************************************************************************/
static bt_status_t  ListPlayerAppValueRsp(RawAddress* bd_addr,
        int num_val, uint8_t *value);

/**********************************************************************
**
** Function  GetPlayerAppValueRsp
**
** Description  This methos is called in response to PDU ID 0x13
**
***********************************************************************/
static bt_status_t GetPlayerAppValueRsp(RawAddress* bd_addr,
        btrc_player_settings_t *p_vals);

 /********************************************************************
 **
 ** Function        GetPlayerAppAttrTextRsp
 **
 ** Description   This method is called in response to get player
 **                         applicaton attribute text response
 **
 **
 *******************************************************************/
static bt_status_t GetPlayerAppAttrTextRsp(RawAddress* bd_addr,
        int num_attr, btrc_player_setting_text_t *p_attrs);

 /********************************************************************
 **
 ** Function        GetPlayerAppValueTextRsp
 **
 ** Description   This method is called in response to Player application
 **                         value text
 **
 ** Return            bt_status_t
 **
 *******************************************************************/
static bt_status_t GetPlayerAppValueTextRsp(RawAddress* bd_addr,
        int num_attr, btrc_player_setting_text_t *p_attrs);

  /********************************************************************
  **
  ** Function        SetPlayerAppValueRsp
  **
  ** Description  This method is called in response to
  **                       application value
  **
  ** Return           bt_staus_t
  **
  *******************************************************************/
static bt_status_t SetPlayerAppValueRsp (RawAddress* bd_addr,
        btrc_status_t rsp_status);

 private:
  BtAvrcSetting();
  friend struct base::DefaultSingletonTraits<BtAvrcSetting>;
  std::unique_ptr<BtAvrcSettingImpl> setting_impl_;

  DISALLOW_COPY_AND_ASSIGN(BtAvrcSetting);

};
#endif

using RegisterNotificationRsp = std::function<bt_status_t(btrc_event_id_t event_id, btrc_notification_type_t type,
                                                btrc_register_notification_t* p_param)>;

class BtAvrcStatusHandling {
private:
  /**************************************
  *
  * Internal var for inner status maintain.
  *
  **************************************/
  static bool is_triggered;
  static long sco_disc_time;
  static bool ignore_play_key;
  static bool force_to_update;
  static bool ignore_avrcp_play_status;
  static alarm_t* fake_status_alarm;
  static RegisterNotificationRsp register_notification_rsp;

  static void sleep_ms(period_ms_t timeout_ms);
  static void StartFakeStatusTimer();
  static void FakeStatusTimerTimeout(void* data);
  static void FakeStatusTimeoutHandler(UNUSED_ATTR uint16_t event, char *data);

  BtAvrcStatusHandling() {};

public:
  /********************************************************************
  **
  ** Function        IsNeededIgnorePlay
  **
  ** Description   Called to check if the play key needed to be ignored.
  **
  **
  ** Return           true/false
  **
  *******************************************************************/
  static bool IsNeededIgnorePlay(long disc_time, tBTA_AV_REMOTE_CMD* p_cmd,
                  RegisterNotificationRsp call_back);
  static bool GetForceUpdateFlag();
  static bool GetIgnoreAvrcpStatusFlag();
  static void CancelTimer();

};

}  // namespace btif
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor
#endif

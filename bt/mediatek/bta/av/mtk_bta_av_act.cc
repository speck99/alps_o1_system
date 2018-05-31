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
#define LOG_TAG "mtk_bta_av_act"

#include "../include/mtk_bta_av_act.h"

#if (BTA_AR_INCLUDED == TRUE)
#include "bta_ar_api.h"
#endif

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace bta {

constexpr int BTA_AV_BROWSING_TIMEOUT_MS = 2 * 1000; //2s timeout

static uint16_t peer_avrc_version = AVRC_REV_1_4;
static alarm_t* av_open_browsing_timer = NULL;
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
static RawAddress av_peer_addr = RawAddress::kEmpty;
#endif
/*******************************************************************************
 *
 * Function         BtaOpenBrowsingTimerTimeout
 *
 * Description      Timer to trigger browsing channel open if the remote headset not establishes
 *                  browsing connection.
 *
 * Returns          void
 *
 ******************************************************************************/
static void BtaOpenBrowsingTimerTimeout(UNUSED_ATTR void* data) {
  uint8_t rc_handle  = PTR_TO_UINT(data);
  APPL_TRACE_DEBUG("AVRC Browse channel timeout");
  AVRC_OpenBrowse(rc_handle, AVCT_INT);
  if (alarm_is_scheduled(av_open_browsing_timer))
  {
      alarm_free(av_open_browsing_timer);
      av_open_browsing_timer = NULL;
  }
}

BtaAvAct::BtaAvAct() {}

/*******************************************************************************
**
** Function         BtaCreateBrowsingTimer
**
** Description     Call it to create browsing timer
**
** Returns          void
**
*******************************************************************************/
void BtaAvAct::BtaCreateBrowsingTimer(bool browse_feat, uint8_t rc_handle) {
  if (alarm_is_scheduled(av_open_browsing_timer))
  {
    alarm_free(av_open_browsing_timer);
    av_open_browsing_timer = NULL;
  }

  if (browse_feat)
  {
    APPL_TRACE_DEBUG("start opening AVRC Browse channel timer");
    av_open_browsing_timer = alarm_new("av_open_browsing_timer");
    alarm_set_on_mloop(av_open_browsing_timer, BTA_AV_BROWSING_TIMEOUT_MS,
        BtaOpenBrowsingTimerTimeout, UINT_TO_PTR(rc_handle));
  }
}

/*******************************************************************************
 *
 * Function         BtaCleanBrowsingTimer
 *
 * Description      Clean browsing timer
 * Returns          void
 *
 ******************************************************************************/
void BtaAvAct::BtaCleanBrowsingTimer() {
  if (alarm_is_scheduled(av_open_browsing_timer))
  {
    alarm_free(av_open_browsing_timer);
    av_open_browsing_timer = NULL;
  }
}

/*******************************************************************************
 *
 * Function         BtaUpdateAvrcCapRsp
 *
 * Description      Update avrcp capbility response
 * Returns          void
 *
 ******************************************************************************/
bool BtaAvAct::BtaUpdateAvrcCapRsp(tAVRC_RESPONSE* p_rc_rsp) {
  if (NULL == p_rc_rsp)
    return false;
  bool need_update = false;
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
  if (interop_mtk_match_addr(INTEROP_MTK_AVRCP13_USE, &av_peer_addr))
  {
    need_update = true;
  }
#endif

  if ((peer_avrc_version == AVRC_REV_1_3) || need_update)
  {
    uint8_t meta_evt[] = {
        AVRC_EVT_PLAY_STATUS_CHANGE,
        AVRC_EVT_TRACK_CHANGE,
        AVRC_EVT_PLAY_POS_CHANGED,
#if defined(MTK_AVRCP_APP_SETTINGS) && (MTK_AVRCP_APP_SETTINGS == TRUE)
        AVRC_EVT_APP_SETTING_CHANGE,
#endif
    };
    need_update = false;
    uint8_t num_id = (sizeof(meta_evt) / sizeof(meta_evt[0]));
    p_rc_rsp->get_caps.count = num_id;
    memcpy(p_rc_rsp->get_caps.param.event_id, meta_evt,num_id);
    return true;
  }
  return false;
}

/*******************************************************************************
 *
 * Function         Update_AVRC_Version
 *
 * Description      update AVRCP when get peer device version.
 *
 * Returns          bool
 *
 ******************************************************************************/
void BtaAvAct::UpdateAvrcpVersion(uint16_t get_peer_rc_version, uint16_t peer_rc_version, tBTA_AV_CB* p_cb) {
  uint8_t rc_handle;
  tBTA_AV_SCB  *p_scb = NULL;

  if ((p_cb->disc & BTA_AV_CHNL_MSK) == BTA_AV_CHNL_MSK)
  {
    /* this is the rc handle/index to tBTA_AV_RCB */
    rc_handle = p_cb->disc & (~BTA_AV_CHNL_MSK);
  }
  else
  {
    /* Validate array index*/
    if (((p_cb->disc & BTA_AV_HNDL_MSK) - 1) < BTA_AV_NUM_STRS)
    {
      p_scb = p_cb->p_scb[(p_cb->disc & BTA_AV_HNDL_MSK) - 1];
    }
    if (p_scb)
    {
      rc_handle = p_scb->rc_handle;
    }
  }
  peer_avrc_version = peer_rc_version;
  APPL_TRACE_DEBUG("bta_av_check_peer_features rc_handle 0x%x", rc_handle);
  if (get_peer_rc_version && (rc_handle == BTA_AV_RC_HANDLE_NONE) &&
      (peer_rc_version <=  AVRC_REV_1_5))
  {
    if ((peer_rc_version <=  AVRC_REV_1_3) && (p_cb->features & BTA_AV_FEAT_BROWSE))
    {
      bta_ar_update_avrc_version(AVRC_REV_1_3, BTA_ID_AV, AVRC_SUPF_TG_CAT1
 #if defined(MTK_AVRCP_APP_SETTINGS) && (MTK_AVRCP_APP_SETTINGS == TRUE)
          | AVRC_SUPF_TG_APP_SETTINGS
#endif
          );
    }
    else if (peer_rc_version >=  AVRC_REV_1_4)
    {
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
      if (interop_mtk_match_addr(INTEROP_MTK_AVRCP13_USE, &p_scb->peer_addr)
          || interop_mtk_match_name(INTEROP_MTK_AVRCP13_USE, &p_scb->peer_addr))
      {
        bta_ar_update_avrc_version(AVRC_REV_1_4, BTA_ID_AV, AVRC_SUPF_TG_CAT1
 #if defined(MTK_AVRCP_APP_SETTINGS) && (MTK_AVRCP_APP_SETTINGS == TRUE)
            | AVRC_SUPF_TG_APP_SETTINGS
#endif
            );
        av_peer_addr = p_scb->peer_addr;
      } else
#endif
        bta_ar_update_avrc_version(peer_rc_version, BTA_ID_AV, 0);
    }
  }
}

/*******************************************************************************
 *
 * Function         UpdateSdpAvrcVersion
 *
 * Description      update SDP AVRCP version to match remote device AVRC version.
 *
 * Returns          bool
 *
 ******************************************************************************/
bool BtaAvAct::UpdateSdpAvrcVersion(uint32_t sdp_handle, uint16_t profile_version, uint16_t categories)
{
  /* update profile descriptor list*/
  bool result = SDP_AddProfileDescriptorList(
          sdp_handle, UUID_SERVCLASS_AV_REMOTE_CONTROL, profile_version);
  /* add supported categories */
  if (categories)
  {
      uint8_t temp[8] = {0};
      uint8_t *p = temp;
      UINT16_TO_BE_STREAM(p, categories);
      result &= SDP_AddAttribute(sdp_handle, ATTR_ID_SUPPORTED_FEATURES, UINT_DESC_TYPE,
          (uint32_t)2, (uint8_t*)temp);
  }
  APPL_TRACE_DEBUG("Update_AVRC_Version result : %x", result);
  return result;
}

}  // namespace bta
}  // namespace bluetooth
}  // namespace mediatek
}  // names
#endif

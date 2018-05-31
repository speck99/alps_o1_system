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

#include "avct_api.h"
#include "bta_av_api.h"
#include "bt_target.h"
#include "interop_mtk.h"
#include "osi/include/alarm.h"
#include "osi/include/log.h"
#include "osi/include/osi.h"
#include "sdp_api.h"
#include "../../../bta/av/bta_av_int.h"

#include <base/memory/singleton.h>

namespace base {
template <typename T>
struct DefaultSingletonTraits;
}  // namespace base

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace bta {

class BtaAvAct {
 public:

  static BtaAvAct* GetInstance();

/*******************************************************************************
**
** Function         BtaCreateBrowsingTimer
**
** Description     Call it to create browsing timer
**
** Returns          void
**
*******************************************************************************/
  static void BtaCreateBrowsingTimer(bool browse_feat, uint8_t rc_handle);

/*******************************************************************************
 *
 * Function         BtaCleanBrowsingTimer
 *
 * Description      Clean browsing timer
 * Returns          void
 *
 ******************************************************************************/
  static void BtaCleanBrowsingTimer();

/*******************************************************************************
 *
 * Function         BtaUpdateAvrcCapRsp
 *
 * Description      Update avrcp capbility response
 * Returns          void
 *
 ******************************************************************************/
  static bool BtaUpdateAvrcCapRsp(tAVRC_RESPONSE* p_rc_rsp);

/*******************************************************************************
 *
 * Function         UpdateAvrcpVersion
 *
 * Description      update AVRCP when get peer device version.
 *
 * Returns          bool
 *
 ******************************************************************************/
  static void UpdateAvrcpVersion(uint16_t get_peer_rc_version, uint16_t peer_rc_version, tBTA_AV_CB* p_cb);

/*******************************************************************************
 *
 * Function	         UpdateSdpAvrcVersion
 *
 * Description       update SDP AVRCP version to match remote device AVRC version.
 *
 * Returns 	         bool
 *
 ******************************************************************************/
  static bool UpdateSdpAvrcVersion(uint32_t sdp_handle, uint16_t profile_version, uint16_t categories);

 private:

  BtaAvAct();

  DISALLOW_COPY_AND_ASSIGN(BtaAvAct);

};

}  // namespace bta
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor
#endif

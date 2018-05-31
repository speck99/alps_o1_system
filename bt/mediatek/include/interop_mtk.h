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

#pragma once

/******************************************************************************
 *
 *  This file contains functions for the MTK defined interop function
 *
 ******************************************************************************/
#include "mdroid_buildcfg.h"
#include "raw_address.h"

#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)

typedef enum {
// Added for HOGP/GATT/LE, @start {
  // Disable secure connections
  // This is for pre BT 4.1/2 devices that do not handle secure mode
  // very well.
  INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS,

  // Some device like Arc Touch BT Mouse will behave abnormally if their
  // required interval which is less than BTM_BLE_CONN_INT_MIN_LIMIT
  // is rejected
  INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ACCEPT,

  // Some device like Casio watch request a minor link supervision timeout which
  // can cause the link timeout frequently. So adjust their link supervision timeout
  // to default value
  INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST,

  // Devices like BSMBB09DS request a large slave latency which will slow down
  // the data transmission or break the link during profile establishment. So adjust
  // the slave latency to default value
  INTEROP_MTK_LE_CONN_LATENCY_ADJUST,

  //Some device like ELECOM cannot handle fast connection procedure
  INTEROP_MTK_LE_DISABLE_FAST_CONNECTION,

  // Some device like ELECOM cann't handle the peripheral pereferred
  // connection parameters update procedure
  INTEROP_MTK_LE_DISABLE_PREF_CONN_PARAMS,
// } @end

// Added for A2DP, @start {
  // Some device will delay send A2DP signaling. It will occur A2DP connection
  // conflict.
  // Change accept signaling time out value for above devices.
  INTEROP_MTK_A2DP_CHANGE_ACCEPT_SIGNALLING_TMS,

  // Some special device want perform START cmd itself first
  // If it not send START cmd, will close current link.
  // So for this special device, we need delay send A2DP START cmd
  // which from DUT to receive the special device cmd.
  INTEROP_MTK_A2DP_DELAY_START_CMD,

  // In particular case, IOT device has wrong song position when FF/REW.
  // Disable song position for IOT device.
  INTEROP_MTK_AVRCP_DISABLE_SONG_POS,

  // Devices requiring this workaround do not handle Bluetooth Absolute Volume
  // control correctly, leading to undesirable (potentially harmful) volume
  // levels or general lack of controlability.
  INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME,

  // Some device support avrcp but not support a2dp.
  // In this case, we do not close avrcp after audio sink sdp failed
  INTEROP_MTK_AUDIO_SINK_SDP_FAIL_NOT_DISC_AVRCP,

  // Some device not support avrcp1.5, we need use avrcp1.3 to response
  INTEROP_MTK_AVRCP13_USE,

  // Some device not send START cmd after send SUSPEND cmd by remote device.
  // So in this case, music will not auto started. In order to skip this issue, add
  // blacklist to remove flag BTIF_AV_FLAG_REMOTE_SUSPEND for trigger START by
  // DUT.
  INTEROP_MTK_SKIP_REMOTE_SUSPEND_FLAG,

  // Some BT devices are less compatible in the market, for example,
  // some devices like connect A2DP ASAP, but others are opposite.
  // Add this blacklist to adapter this case.
  INTEROP_MTK_ADVANCED_A2DP_CONNECT,

// } @end

// Added for HFP, @start {

  // Sony Ericsson HBH-DS205 has some special request for opening
  // the sco time(if create SCO command be sent before a2dp suspend,
  // the BT SCO will no voice output for the voice call), so work around
  // for this device by delay the SCO setup.
  // (Nexus has the same problem with this device).
  INTEROP_MTK_HFP_DEALY_OPEN_SCO,

  // Some device has IOT issue for HFP. Turn off bt during ring call, then
  // answer call and turn on bt, the end call UI display on Carkit;
  // So work around delay open sco to avoid this issue;
  INTEROP_MTK_HFP_DELAY_SCO_IN_ACT,

  // Some device has IOT issue for HFP 1.7 version.
  // Back to 1.6 for this devices.
  INTEROP_MTK_HFP_17_TO_16,

  // Some device has issue when setup eSCO, so force to
  // use SCO instead.
  INTEROP_MTK_HFP_FORCE_TO_USE_SCO,

  // Some device will no voice when using mSBC codec, so
  // force to use CVSD.
  INTEROP_MTK_HFP_FORCE_TO_USE_CVSD,
// } @end

// Added for HID, @start {
  // Some HID device will show connected state too slowly
  // Because when his connection had setup, HID host process
  // SDP,when peer device response slowly,it lead show connected slowly
  INTEROP_MTK_HID_DISABLE_SDP,
  // Not do sniif mode for HID profile.
  INTEROP_MTK_HID_NOT_DO_SNIFF_SUBRATING,
// } @end

// Added for OPP/RFCOMM, @start {
  // Some devices can not parse multi AT commands in one rfcomm packet.
  // So, send data separately.
  INTEROP_MTK_FORBID_COMBINE_RFCOMM_DATA,
// } @end

// Added for L2CAP, @start {
 // Some devices cannot work normally as a master if the connection is
 // initiated by themselves
 INTEROP_MTK_ACCEPT_CONN_AS_MASTER,

 // Some device cannot work normally as a slave if the connection is
 // initiated by themselves.
  INTEROP_MTK_ACCEPT_CONN_AS_SLAVE,

 // Not restart ACL link for pending request when link down
 INTEROP_MTK_NOT_RESTART_ACL,
// } @end

// Added for GAP, @start {
  // Disable automatic pairing with headsets/car-kits
  // Some car kits do not react kindly to a failed pairing attempt and
  // do not allow immediate re-pairing. Blacklist these so that the initial
  // pairing attempt makes it to the user instead.
  INTEROP_MTK_DISABLE_AUTO_PAIRING,
  // Some devices reconnect RFCOMM without security procedure,
  // disable security check for temporary
  INTEROP_MTK_DISABLE_SERVICE_SECURITY_CHECK,
  // get services from storage for some devices
  INTEROP_MTK_GET_UUIDS_FROM_STORAGE,
  // M557 mouse seldomly claims SSP is not supported in its extended features,
  // but actually it really wants SSP.
  INTEROP_MTK_FORCED_SSP,

// } @end

// Added for SDP, @start {
// } @end

// Added for Common, @start {
// } @end
} mtk_interop_feature_t;

/******************************************************************************
*
**
** Function         interop_mtk_match_addr
**
** Description      Looks up the interop_addr_database for the specified BD
**                  address.
**
** Returns          TRUE if matched, else FALSE
**
*******************************************************************************/
extern bool interop_mtk_match_addr(const mtk_interop_feature_t feature,
                                   const RawAddress* addr);

/*******************************************************************************
**
** Function         interop_mtk_match_name
**
** Description      Looks up the interop_name_database for the specified BD
**                   address.
**
** Returns          TRUE if matched, else FALSE
**
*******************************************************************************/
extern bool interop_mtk_match_name(const mtk_interop_feature_t feature,
                                   const RawAddress* addr);

/*******************************************************************************
**
** Function         interop_mtk_match_addr_name
**
** Description      Looks up the interop_addr_database and interop_name_database
**                  for the specified BD address.
**
** Returns          TRUE if matched, else FALSE
**
*******************************************************************************/
extern bool interop_mtk_match_addr_name(const mtk_interop_feature_t feature,
                                        const RawAddress* addr);
#endif

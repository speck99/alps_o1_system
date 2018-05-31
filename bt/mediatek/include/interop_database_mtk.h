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
 *  This file contains  MTK added interop devices
 *
 ******************************************************************************/

#include "interop_mtk.h"

#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)

typedef struct {
  RawAddress addr;
  size_t length;
  mtk_interop_feature_t feature;
} mtk_interop_addr_entry_t;

static const mtk_interop_addr_entry_t mtk_interop_addr_db[] = {
// Added for HOGP/GATT/LE, @start {
    //BSM mouse
    {{{0x00, 0x1b, 0xdc, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_LATENCY_ADJUST},

    // CASIO watch
    {{{0xff, 0x40, 0x3a, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST},
    {{{0xda, 0x58, 0x98, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST},
    {{{0xc2, 0x80, 0x29, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST},
    {{{0xff, 0x74, 0xe1, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST},
    {{{0xd9, 0xe6, 0xea, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST},

    //{0xd0, 0x5f, 0xb8}. /*ELECOM Laser Mouse*/
    {{{0xd0, 0x5f, 0xb8, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},
    {{{0xd0, 0x5f, 0xb8, 0, 0, 0}}, 3, INTEROP_MTK_LE_DISABLE_FAST_CONNECTION},
    {{{0xd0, 0x5f, 0xb8, 0, 0, 0}}, 3, INTEROP_MTK_LE_DISABLE_PREF_CONN_PARAMS},
    {{{0xd0, 0x5f, 0xb8, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ACCEPT},

    //Huitong BLE Remote
    {{{0x7c, 0x66, 0x9d, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},

    //XiaoYi_RC
    {{{0x04, 0xe6, 0x76, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},
// } @end

// Added for A2DP, @start {
    // INTEROP_MTK_A2DP_CHANGE_ACCEPT_SIGNALLING_TMS
    // {0x00, 0x21, 0x3c},   /* Jawbone ICON*/
    {{{0x00, 0x21, 0x3c, 0, 0, 0}},
     3,
     INTEROP_MTK_A2DP_CHANGE_ACCEPT_SIGNALLING_TMS},

    // INTEROP_MTK_A2DP_DELAY_START_CMD
    // {0x00, 0x17, 0x53}   /* Tiggo5 */
    // {0x00, 0x13, 0x04}   /* CASKA */
    // {0x00, 0x0d, 0x3c}   /* Clip Music 801 */
    // {0x0c, 0xa6, 0x94}   /* Philips Fidelio M2BT */
    // {0x33, 0x60, 0x7c}   /* DVD */
    // {0x00, 0x0a, 0x08}   /* CDE-153Series */
    // {0xbc, 0x6e, 0x64}   /* SBH54 */
    // {0x00, 0x18, 0x09}   /* MDR-XB80BS */
    {{{0x00, 0x17, 0x53, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x00, 0x13, 0x04, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x00, 0x0d, 0x3c, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x0c, 0xa6, 0x94, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x33, 0x60, 0x7c, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x00, 0x0a, 0x08, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0xbc, 0x6e, 0x64, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x00, 0x18, 0x09, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},

    // INTEROP_DISABLE_ABSOLUTE_VOLUME
    // Pioneer APS-BH80 - support AVRCP1.4, but absolute volume is not
    // implemented
    // {0x00, 0x08, 0x7A}   /* APS-BH80 */
    // {0x00, 0x08, 0x3e}   /* aigo X6 */
    // {0x00, 0x00, 0x1a}   /* Gionee */
    // {0xad, 0xcd, 0xee}   /* PISEN-LE005 */
    // {0xe8, 0x07, 0xbf}   /* Mi Bluetooth Speaker */
    {{{0x00, 0x08, 0x7A, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0x00, 0x08, 0x3e, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0x00, 0x00, 0x1a, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0xad, 0xcd, 0xee, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0xe8, 0x07, 0xbf, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},

    // INTEROP_MTK_AVRCP_DISABLE_SONG_POS
    // {0x00, 0x0e, 0x9f},   /* Toyota Touch&Go */
    {{{0x00, 0x0e, 0x9f, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_DISABLE_SONG_POS},

    // INTEROP_MTK_AUDIO_SINK_SDP_FAIL_NOT_DISC_AVRCP
    // {0xA0, 0xE6, 0xF8},   /*Fitbit Blaze*/
    {{{0xA0, 0xE6, 0xF8, 0,0,0}}, 3, INTEROP_MTK_AUDIO_SINK_SDP_FAIL_NOT_DISC_AVRCP},

    // INTEROP_MTK_AVRCP13_USE
    // {0x48, 0xf0, 0x7b},	 /* MB Bluetooth 41519 */
    {{{0x48, 0xf0, 0x7b, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP13_USE},

    // INTEROP_MTK_SKIP_REMOTE_SUSPEND_FLAG
    // {0xd4, 0xca, 0x6e},   /* Audi MMI 6610 */
    {{{0xd4, 0xca, 0x6e, 0, 0, 0}}, 3, INTEROP_MTK_SKIP_REMOTE_SUSPEND_FLAG},

    // INTEROP_MTK_ADVANCED_A2DP_CONNECT
    // {0x00, 0x0d, 0x18, 0, 0, 0}, /* Alpine */
    // {0x40, 0xef, 0x4c, 0, 0, 0}, /* UFO */
    {{{0x00, 0x0d, 0x18, 0, 0, 0}}, 3, INTEROP_MTK_ADVANCED_A2DP_CONNECT},
    {{{0x40, 0xef, 0x4c, 0, 0, 0}}, 3, INTEROP_MTK_ADVANCED_A2DP_CONNECT},

// } @end

// Added for HFP, @start {

    // INTEROP_MTK_HFP_DEALY_OPEN_SCO
    // {0x00, 0x1E, 0xDC}   /* DS205 */
    // {0x00, 0x58, 0x50}   /* BELKIN */
    // {0xFC, 0x58, 0xFA}   /*MEIZU speaker*/
    {{{0x00, 0x1E, 0xDC, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},
    {{{0x00, 0x58, 0x50, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},
    {{{0xFC, 0x58, 0xFA, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},

    // INTEROP_MTK_HFP_DELAY_SCO_IN_ACT
    // {0xB0, 0x72, 0xBF}   /*KIA MOTORS*/
    // {0xD8, 0xC4, 0x6A}   /*KIA MOTORS*/
    {{{0xB0, 0x72, 0xBF, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DELAY_SCO_IN_ACT},
    {{{0xD8, 0xC4, 0x6A, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DELAY_SCO_IN_ACT},

    // INTEROP_MTK_HFP_17_TO_16
    // {0x00, 0x17, 0x53}   /* Tiggo5 */
    // {0x04, 0xf8, 0xc2}   /* VW Radio */
    // {0x00, 0x13, 0x04}   /* JAC */
    {{{0x00, 0x17, 0x53,       0,0,0}} ,3, INTEROP_MTK_HFP_17_TO_16},
    {{{0x04, 0xf8, 0xc2,       0,0,0}} ,3, INTEROP_MTK_HFP_17_TO_16},
    {{{0x00, 0x13, 0x04,       0,0,0}} ,3, INTEROP_MTK_HFP_17_TO_16},

    // INTEROP_MTK_HFP_FORCE_TO_USE_SCO
    // {0x00, 0x58, 0x76},  /* BT800 */
    // {0xD0, 0x8A, 0x55},  /* Skull Candy XTFree */
    // {0xA0, 0xE9, 0xDB},    /* ODYS-XOUND cube */
    // {0x00, 0x58, 0x50},    /* SeeCom */
    // {0x00, 0x58, 0x56},    /* RAVIS */
    // {0x00, 0x58, 0x75},    /* Mifa F1 */
    // {0x59, 0x90, 0xa1},    /* TRACTOR-7462 */
    // {0x00, 0x58, 0x50},    /* AMETHYST MD3 */
    {{{0x00, 0x58, 0x76, 0,0,0}} ,3, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {{{0xD0, 0x8A, 0x55, 0,0,0}} ,3, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {{{0xA0, 0xE9, 0xDB, 0,0,0}} ,3, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {{{0x00, 0x58, 0x56, 0,0,0}} ,3, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {{{0x00, 0x58, 0x75, 0,0,0}} ,3, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {{{0x59, 0x90, 0xa1, 0,0,0}} ,3, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},

    // INTEROP_MTK_HFP_FORCE_TO_USE_CVSD
    // {} /* */
    //{{{0x00, 0x00, 0x00, 0,0,0}} ,3, INTEROP_MTK_HFP_FORCE_TO_USE_CVSD},

// } @end

// Added for HID, @start {
    // INTEROP_MTK_HID_DISABLE_SDP
    // {0x04, 0x0C, 0xCE},  /* Apple Magic Mouse */
    // {0x00, 0x07, 0x61},  /* Bluetooth Laser Travel Mouse */
    // {0x00, 0x1d, 0xd8},  /* Microsoft Bluetooth Notebook Mouse 5000 */
    // {0x00, 0x1f, 0x20},  /* Logitech MX Revolution Mouse */
    // {0x6c, 0x5d, 0x63},  /* Rapoo 6080 mouse */
    // {0x28, 0x18, 0x78}   /* Microsoft Sculpt Touch Mouse */
    // {0x30, 0x59, 0xb7}   /* Microsoft Sculpt Comfort Mouse */
    // {0x34, 0x88, 0x5d}   /* Bluetooth Mouse M557*/
    {{{0x04, 0x0C, 0xCE, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},
    {{{0x00, 0x07, 0x61, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},
    {{{0x00, 0x1d, 0xd8, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},
    {{{0x00, 0x1f, 0x20, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},
    {{{0x6c, 0x5d, 0x63, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},
    {{{0x28, 0x18, 0x78, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},
    {{{0x30, 0x59, 0xb7, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},
    {{{0x34, 0x88, 0x5d, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},

    // INTEROP_MTK_HID_NOT_DO_SNIFF_SUBRATING
    // {0x54, 0x46, 0x6b},  /* JW MT002 Bluetooth Mouse */
    // /*LMP version&subversion  - 5, 8721 & LMP Manufacturer - 15*/
    // {0x6c, 0x5d, 0x63}, /* Rapoo 6610 Bluetooth Mouse */
    // {0x60, 0x8c, 0x2b},  /* FelTouch Magic */
    {{{0x54, 0x46, 0x6b, 0, 0, 0}}, 3, INTEROP_MTK_HID_NOT_DO_SNIFF_SUBRATING},
    {{{0x6c, 0x5d, 0x63, 0, 0, 0}}, 3, INTEROP_MTK_HID_NOT_DO_SNIFF_SUBRATING},
    {{{0x60, 0x8c, 0x2b, 0, 0, 0}}, 3, INTEROP_MTK_HID_NOT_DO_SNIFF_SUBRATING},
// } @end

// Added for OPP/RFCOMM, @start {
    // INTEROP_MTK_FORBID_COMBINE_RFCOMM_DATA
    // {0x00, 0x0A, 0x08},   /*BMW94506*/  Remote device can not parse multi at
    // commands.
    {{{0x00, 0x0A, 0x08, 0, 0, 0}}, 3, INTEROP_MTK_FORBID_COMBINE_RFCOMM_DATA},
// } @end

// Added for L2CAP, @start {
    // INTEROP_MTK_ACCEPT_CONN_AS_MASTER
    // {0x6c, 0x5d, 0x63},  /* Rapoo 6610 mouse */
    // {0x00, 0x02, 0xc7},  /* HANDS FREE carkit of Infineon Technologies AG */
    // {0x00, 0x58, 0x75},  /* Mifa_F1 */
    // {0x00, 0x58, 0x76},  /* BT800, this device does not send detach when power off */
    // {0x20, 0x16, 0x03},  /* 56s */
    {{{0x6c, 0x5d, 0x63, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_MASTER},
    {{{0x00, 0x02, 0xc7, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_MASTER},
    {{{0x00, 0x58, 0x75, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_MASTER},
    {{{0x00, 0x58, 0x76, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_MASTER},
    {{{0x20, 0x16, 0x03, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_MASTER},

    // INTEROP_MTK_ACCEPT_CONN_AS_SLAVE
    // {0x00, 0x1e, 0xae},  /* SYNC, FORD carkit */
    // {0x00, 0x26, 0xb4},  /* NAC ford, 2013 Lincoln */
    // {0x00, 0x26, 0xe8},  /* Nissan Murano */
    // {0x00, 0x37, 0x6d},  /* Lexus ES300h */
    // {0x9c, 0x3a, 0xaf},  /* SAMSUNG HM1900 */
    {{{0x00, 0x1e, 0xae, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_SLAVE},
    {{{0x00, 0x26, 0xb4, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_SLAVE},
    {{{0x00, 0x26, 0xe8, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_SLAVE},
    {{{0x00, 0x37, 0x6d, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_SLAVE},
    {{{0x9c, 0x3a, 0xaf, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_SLAVE},

    // INTEROP_MTK_NOT_RESTART_ACL
    // {0x00, 0x06, 0xf7},  /* MB Bluetooth, carkit */
    {{{0x00, 0x06, 0xf7,       0,0,0}}, 3, INTEROP_MTK_NOT_RESTART_ACL},
// } @end

// Added for GAP, @start {
    // TEMIC SDS (Porsche,Audi exp(PCM)) - auto-pairing fails
    {{{0x00, 0x0e, 0x9f, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_AUTO_PAIRING},
    // Jabra STONE2 - reconnect without sercurity procedure
    {{{0x50, 0xc9, 0x71, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_SERVICE_SECURITY_CHECK},
    {{{0x00, 0x23, 0x78, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_SERVICE_SECURITY_CHECK},
    // INTEROP_MTK_GET_UUIDS_FROM_STORAGE
    // {0x0c, 0xfc, 0x83},  /* ADYSS-A6 */
    {{{0x0c, 0xfc, 0x83, 0, 0, 0}}, 3, INTEROP_MTK_GET_UUIDS_FROM_STORAGE},
    // INTEROP_MTK_FORCED_SSP
    // {0x34, 0x88, 0x5d},   /*Bluetooth Mouse M557*/
    {{{0x34, 0x88, 0x5d, 0, 0, 0}}, 3, INTEROP_MTK_FORCED_SSP},
    {{{0x00, 0x1f, 0x20, 0xaa, 0, 0}}, 4, INTEROP_MTK_FORCED_SSP},

// } @end

// Added for SDP, @start {
// } @end

// Added for Common, @start {
// } @end
};

typedef struct {
  char name[30];
  size_t length;
  mtk_interop_feature_t feature;
} mtk_interop_name_entry_t;

static const mtk_interop_name_entry_t mtk_interop_name_db[] = {
// Added for HOGP/GATT/LE, @start {
    {"honor zero-", 11, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},
    {"小米蓝牙遥控", 18, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},
    {"Arc Touch BT Mouse", 18, INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ACCEPT},
    {"BSMBB09DS", 9, INTEROP_MTK_LE_CONN_LATENCY_ADJUST},
    {"CASIO GB-6900A*", 15, INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST},
    {"Huitong BLE Remote", 18, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},
    {"ELECOM Laser Mouse", 18, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},
    {"ELECOM Laser Mouse", 18, INTEROP_MTK_LE_DISABLE_FAST_CONNECTION},
    {"ELECOM Laser Mouse", 18, INTEROP_MTK_LE_DISABLE_PREF_CONN_PARAMS},
    {"ELECOM Laser Mouse", 18, INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ACCEPT},
    {"FeiZhi", 6, INTEROP_MTK_LE_DISABLE_PREF_CONN_PARAMS},
    {"XiaoYi_RC", 9, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},
// } @end

// Added for A2DP, @start {
    {"Toyota Touch&Go", 15, INTEROP_MTK_AVRCP_DISABLE_SONG_POS},
    // RDA Microelectronics HBS device will a2dp no sound after reject the call
    // {0x00, 0x0d, 0x18}   /* RDA HBS device*/
    {"HBS8864", 7, INTEROP_MTK_A2DP_DELAY_START_CMD},

    // Tiguan RNS315 will no sound after play wechat voice message.
    // {0x00, 0x07, 0x04}   /* Tiguan RNS carkit*/
    {"Tiguan RNS315 Bluetooth", 13, INTEROP_MTK_A2DP_DELAY_START_CMD},
// } @end

// Added for HFP, @start {
    {"Parrot ASTEROID Smart", 21, INTEROP_MTK_HFP_FORCE_TO_USE_CVSD},
    {"Jabra Classic v0.5.3", 20, INTEROP_MTK_HFP_FORCE_TO_USE_CVSD},

    {"CARDVD-BT", 9, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {"CarKit10BF", 10, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {"NXO-3", 5, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {"SeeCom", 6, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {"AMETHYST MD3", 12, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
// } @end

// Added for HID, @start {
// } @end

// Added for OPP/RFCOMM, @start {
// } @end

// Added for L2CAP, @start {
// } @end

// Added for GAP, @start {
    {"Bluetooth Mouse M557", 20, INTEROP_MTK_FORCED_SSP},
// } @end

// Added for SDP, @start {
// } @end

// Added for Common, @start {
// } @end
};
#endif

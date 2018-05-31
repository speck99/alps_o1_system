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

#include "gatts_mtk.h"

#include <base/logging.h>
#include <base/strings/stringprintf.h>

#include "btif/include/btif_config.h"
#include "osi/include/osi.h"
#include "stack/btm/btm_ble_int.h"
#include "stack/gatt/gatt_int.h"

using base::StringPrintf;

/*****************************************************************************
 *  Local type definitions
 ****************************************************************************/
#define BTIF_STORAGE_PATH_SERVICE_CHANGE "SrvChgInd"

/*****************************************************************************
 *  Static variables
 ****************************************************************************/
static uint16_t clt_cfg_hdl;

/*****************************************************************************
 *  Externally called functions
 ****************************************************************************/
void gatts_srv_chg_set_cccd_handle(uint16_t handle) { clt_cfg_hdl = handle; }

bool gatts_srv_chg_ind_get(const RawAddress& bda) {
  int clt_cfg = 0;
  std::string addrstr = bda.ToString();
  const char* bd_addr_str = addrstr.c_str();

  if (!btif_config_get_int(bd_addr_str, BTIF_STORAGE_PATH_SERVICE_CHANGE, &clt_cfg) ||
      !clt_cfg)
    return false;
  return true;
}

void gatts_srv_chg_ind_set(const RawAddress& bda, bool ind) {
  std::string addrstr = bda.ToString();
  const char* bd_addr_str = addrstr.c_str();

  btif_config_set_int(bd_addr_str, BTIF_STORAGE_PATH_SERVICE_CHANGE, ind ? 1 : 0);
}

tGATT_STATUS gatts_srv_chg_proc_write_req(const RawAddress& bda,
                                          tGATT_WRITE_REQ* p_data) {
  if (p_data == NULL) return GATT_ERROR;

  if (p_data->handle != clt_cfg_hdl) return GATT_INVALID_HANDLE;

  if (p_data->len == sizeof(tGATT_CLT_CHAR_CONFIG)) {
    tGATT_CLT_CHAR_CONFIG ind_value = 0x00;
    uint8_t* p = p_data->value;
    STREAM_TO_UINT16(ind_value, p);
    VLOG(1) << StringPrintf("%s, Srv Chg Ind? %d", __func__,
                     ind_value & GATT_CLT_CONFIG_INDICATION);
    if (ind_value & GATT_CLT_CONFIG_INDICATION) {
      tGATTS_SRV_CHG srv_chg_clt;
      srv_chg_clt.bda = bda;
      srv_chg_clt.srv_changed = false;
      if (gatt_is_bda_in_the_srv_chg_clt_list(bda) != NULL ||
          gatt_add_srv_chg_clt(&srv_chg_clt) != NULL) {
        if (btm_sec_is_a_bonded_dev(bda)) {
          gatts_srv_chg_ind_set(bda, true);
        }
      }
      return GATT_SUCCESS;
    } else {
      gatt_delete_dev_from_srv_chg_clt_list(bda);
      if (btm_sec_is_a_bonded_dev(bda)) {
        gatts_srv_chg_ind_set(bda, false);
      }
      return GATT_SUCCESS;
    }
  }
  return GATT_INVALID_PDU;
}

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

#include "raw_address.h"
#include "bta/include/bta_gatt_api.h"


/*******************************************************************************
 *
 * Function         gatts_srv_chg_set_handle
 *
 * Description       Set the allocated attribute handle of the Client
 *                     Characteristic Configuration Descriptor
 *                     of the Service Changed Characteristic.
 *
 * Parameter        attribute handle of the Client Characteristic Configuration
 *                     Descriptor
 *
 * Returns          void.
 *
 ******************************************************************************/
extern void gatts_srv_chg_set_cccd_handle(uint16_t handle);

/*******************************************************************************
 *
 * Function         gatts_srv_chg_ind_get
 *
 * Description       Get stored indication configuration from NVRAM
 *
 * Parameter        bda - BT address of the remote device
 *
 * Returns          true - remote device has configure to be indicated, otherwise false
 *
 ******************************************************************************/
extern bool gatts_srv_chg_ind_get(const RawAddress& bda);

/*******************************************************************************
 *
 * Function         gatts_srv_chg_ind_set
 *
 * Description       Stores the indication configuration to NVRAM
 *
 * Parameter        bda - BT address of the remote device
 *                        ind -  whether Characteristic value is configured to indicated or not
 *
 * Returns          void.
 *
 ******************************************************************************/

extern void gatts_srv_chg_ind_set(const RawAddress& bda, bool ind);


/*******************************************************************************
 *
 * Function         gatts_srv_chg_proc_write_req
 *
 * Description       Write Request process of Service Changed Characteristic
 *
 * Parameter        bda - BT address of the remote device
 *                        p_data -  write request data
 *
 * Returns          void.
 *
 ******************************************************************************/
extern tGATT_STATUS gatts_srv_chg_proc_write_req(const RawAddress& bda,
                                                 tGATT_WRITE_REQ* p_data);


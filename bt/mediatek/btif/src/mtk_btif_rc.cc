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
#define LOG_TAG "mtk_btif_rc"

#include "../include/mtk_btif_rc.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace btif {

#if defined(MTK_AVRCP_APP_SETTINGS) && (MTK_AVRCP_APP_SETTINGS == TRUE)
constexpr int IDX_LIST_APP_ATTR_RSP = 1;
constexpr int IDX_LIST_APP_VALUE_RSP = 2;
constexpr int IDX_GET_CURR_APP_VAL_RSP = 3;
constexpr int IDX_SET_APP_VAL_RSP = 4;
constexpr int IDX_GET_APP_ATTR_TXT_RSP = 5;
constexpr int IDX_GET_APP_VAL_TXT_RSP = 6;

struct AVRC_SETTING_CB{
    FillPduQueue   fill_pdu_queue_ex;
    SendRejectRsp send_reject_response_ex;
    OpcodeFromPdu opcode_from_pdu_ex;
    CheckRcConnect check_rc_connected_ex;
    SendMetaRsp send_meta_rsp_ex;
};

class BtAvrcSettingImpl {
 public:
  BtAvrcSettingImpl() {}
  ~BtAvrcSettingImpl() = default;

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
        btrc_player_settings_t* src_player_setting)
{
  dest_player_setting->num_attr = src_player_setting->num_attr;
  //change 2 attr_ids and 2 attr_values to controller of the response one time
  memcpy(&dest_player_setting->attr_id, src_player_setting->attr_ids, 2);
  memcpy(&dest_player_setting->attr_value, src_player_setting->attr_values, 2);
}

};


AVRC_SETTING_CB avrc_setting_cb;

BtAvrcSetting::BtAvrcSetting() : setting_impl_(new BtAvrcSettingImpl()) {}

BtAvrcSetting* BtAvrcSetting::GetInstance() {
  return base::Singleton<BtAvrcSetting>::get();
}

/********************************************************************
**
** Function         set_player_app_setting_callback
**
** Description     This method is called in set callback
**                        from aosp code
**
** Return            bt_staus_t
**
*******************************************************************/
void BtAvrcSetting::SetAvrcSettingCB (FillPduQueue fill_pdu_queue,
        SendRejectRsp send_reject_response,
        OpcodeFromPdu opcode_from_pdu,
        CheckRcConnect check_rc_connected,
        SendMetaRsp send_meta_rsp)
{
  avrc_setting_cb.fill_pdu_queue_ex = fill_pdu_queue;
  avrc_setting_cb.send_reject_response_ex = send_reject_response;
  avrc_setting_cb.opcode_from_pdu_ex = opcode_from_pdu;
  avrc_setting_cb.check_rc_connected_ex = check_rc_connected;
  avrc_setting_cb.send_meta_rsp_ex = send_meta_rsp;
}

/********************************************************************
**
** Function         SetAppSettingsChange
**
** Description     This method is called in register notification rsp
**                        from aosp code
**
** Return             void
**
*******************************************************************/
void BtAvrcSetting::SetAppSettingsChange(tAVRC_PLAYER_APP_PARAM* dest_player_setting,
        btrc_player_settings_t* src_player_setting)
{
  setting_impl_->SetAppSettingsChange(dest_player_setting, src_player_setting);
}

 /*******************************************************************************
 *
 * Function         btif_rc_upstreams_evt
 *
 * Description      Executes AVRC_PDU_GET_CUR_PLAYER_APP_VALUE events in btif context.
 *
 * Returns          void
 *
 ******************************************************************************/
void BtifRcGetAppValueEvt(btrc_callbacks_t* bt_rc_callbacks,
                                  tAVRC_COMMAND* pavrc_cmd, uint8_t ctype, uint8_t label,
                                  void* p_dev, RawAddress* rc_addr, uint8_t rc_handle)
{
  BTIF_TRACE_DEBUG("PLAYER_APP_VALUE PDU 0x13 = %d",pavrc_cmd->get_cur_app_val.num_attr);
  if ((pavrc_cmd->get_cur_app_val.num_attr == 0) ||
      (pavrc_cmd->get_cur_app_val.num_attr > BTRC_MAX_ELEM_ATTR_SIZE))
  {
    avrc_setting_cb.send_reject_response_ex (rc_handle, label, pavrc_cmd->pdu,
      AVRC_STS_BAD_PARAM, pavrc_cmd->cmd.opcode);
    return;
  }
  btrc_player_attr_t player_attr[BTRC_MAX_ELEM_ATTR_SIZE];
  memset(player_attr, 0, sizeof(player_attr));
  for (uint8_t player_attr_num = 0 ; player_attr_num < pavrc_cmd->get_cur_app_val.num_attr;
      ++player_attr_num)
  {
    player_attr[player_attr_num] = (btrc_player_attr_t)pavrc_cmd->get_cur_app_val.attrs[player_attr_num];
  }
  avrc_setting_cb.fill_pdu_queue_ex(IDX_GET_CURR_APP_VAL_RSP, ctype, label, true, p_dev);
  HAL_CBACK(bt_rc_callbacks, get_player_app_value_cb ,
    pavrc_cmd->get_cur_app_val.num_attr, player_attr, rc_addr);
}

 /*******************************************************************************
 *
 * Function         BtifRcSetAppValueEvt
 *
 * Description      Executes AVRC_PDU_SET_PLAYER_APP_VALUE events in btif context.
 *
 * Returns          void
 *
 ******************************************************************************/
void BtifRcSetAppValueEvt(btrc_callbacks_t* bt_rc_callbacks,
                                  tAVRC_COMMAND* pavrc_cmd, uint8_t ctype, uint8_t label,
                                  void* p_dev, RawAddress* rc_addr, uint8_t rc_handle)
{
  if ((pavrc_cmd->set_app_val.num_val== 0) ||
      (pavrc_cmd->set_app_val.num_val > BTRC_MAX_ELEM_ATTR_SIZE))
  {
    avrc_setting_cb.send_reject_response_ex (rc_handle, label, pavrc_cmd->pdu,
      AVRC_STS_BAD_PARAM, pavrc_cmd->cmd.opcode);
    return;
  }
  else
  {
    btrc_player_settings_t attr;
    for(uint8_t count = 0; count < pavrc_cmd->set_app_val.num_val ; ++count)
    {
      attr.attr_ids[count] = pavrc_cmd->set_app_val.p_vals[count].attr_id ;
      attr.attr_values[count]= pavrc_cmd->set_app_val.p_vals[count].attr_val;
    }
    attr.num_attr  =  pavrc_cmd->set_app_val.num_val ;
    avrc_setting_cb.fill_pdu_queue_ex(IDX_SET_APP_VAL_RSP, ctype, label, true, p_dev);
    HAL_CBACK(bt_rc_callbacks, set_player_app_value_cb, &attr, rc_addr);
  }
}

 /*******************************************************************************
 *
 * Function         BtifRcGetAppTextValueEvt
 *
 * Description      Executes AVRC_PDU_GET_PLAYER_APP_ATTR_TEXT events in btif context.
 *
 * Returns          void
 *
 ******************************************************************************/
void BtifRcGetAppAttrTextEvt(btrc_callbacks_t* bt_rc_callbacks,
                                  tAVRC_COMMAND* pavrc_cmd, uint8_t ctype, uint8_t label,
                                  void* p_dev, RawAddress* rc_addr, uint8_t rc_handle)
{
  if ((pavrc_cmd->get_app_attr_txt.num_attr == 0) ||
      (pavrc_cmd->get_app_attr_txt.num_attr > BTRC_MAX_ELEM_ATTR_SIZE))
  {
    avrc_setting_cb.send_reject_response_ex (rc_handle, label, pavrc_cmd->pdu,
      AVRC_STS_BAD_PARAM, pavrc_cmd->cmd.opcode);
  }
  else
  {
    btrc_player_attr_t player_attr_txt [BTRC_MAX_ELEM_ATTR_SIZE];
    for (uint8_t count_txt = 0;count_txt < pavrc_cmd->get_app_attr_txt.num_attr ; ++count_txt)
    {
      player_attr_txt[count_txt] = (btrc_player_attr_t)pavrc_cmd->get_app_attr_txt.attrs[count_txt];
    }
    avrc_setting_cb.fill_pdu_queue_ex(IDX_GET_APP_ATTR_TXT_RSP, ctype, label, true, p_dev);
    HAL_CBACK(bt_rc_callbacks, get_player_app_attrs_text_cb,
      pavrc_cmd->get_app_attr_txt.num_attr, player_attr_txt, rc_addr);
  }
}

 /*******************************************************************************
  *
  * Function          BtifRcGetAppValueTextEvt
  *
  * Description     Executes AVRC_PDU_GET_PLAYER_APP_VALUE_TEXT events in btif context.
  *
  * Returns           void
  *
  ******************************************************************************/
 void BtifRcGetAppValueTextEvt(btrc_callbacks_t* bt_rc_callbacks,
                                  tAVRC_COMMAND* pavrc_cmd, uint8_t ctype, uint8_t label,
                                  void* p_dev, RawAddress* rc_addr, uint8_t rc_handle)
{
  if (pavrc_cmd->get_app_val_txt.attr_id == 0 ||
              pavrc_cmd->get_app_val_txt.attr_id > AVRC_PLAYER_VAL_GROUP_REPEAT)
  {
    avrc_setting_cb.send_reject_response_ex (rc_handle, label, pavrc_cmd->pdu,
      AVRC_STS_BAD_PARAM, pavrc_cmd->cmd.opcode);
    return;
  }
  if (pavrc_cmd->get_app_val_txt.num_val == 0)
  {
    avrc_setting_cb.send_reject_response_ex (rc_handle, label, pavrc_cmd->pdu,
      AVRC_STS_BAD_PARAM, pavrc_cmd->cmd.opcode);
  }
  else
  {
    avrc_setting_cb.fill_pdu_queue_ex(IDX_GET_APP_VAL_TXT_RSP, ctype, label, true, p_dev);
    HAL_CBACK(bt_rc_callbacks, get_player_app_values_text_cb,
      pavrc_cmd->get_app_val_txt.attr_id, pavrc_cmd->get_app_val_txt.num_val,
      pavrc_cmd->get_app_val_txt.vals, rc_addr);
  }
 }

/*******************************************************************************
 *
 * Function         btif_rc_upstreams_evt
 *
 * Description      Executes AVRC UPSTREAMS events in btif context.
 *
 * Returns          void
 *
 ******************************************************************************/
void BtAvrcSetting::BtifRcUpstreamsSettingEvt(btrc_callbacks_t* bt_rc_callbacks,
                                  uint16_t event, tAVRC_COMMAND* pavrc_cmd, uint8_t ctype, uint8_t label,
                                  void* p_dev, RawAddress* rc_addr, uint8_t rc_handle)
{
  BTIF_TRACE_EVENT("%s: event ID: %x",
                   __func__, pavrc_cmd->reg_notif.event_id);

   switch (event) {
    case AVRC_PDU_LIST_PLAYER_APP_ATTR:
    {
      avrc_setting_cb.fill_pdu_queue_ex (IDX_LIST_APP_ATTR_RSP, ctype, label, true, p_dev);
      HAL_CBACK(bt_rc_callbacks, list_player_app_attr_cb, rc_addr);
    }
    break;
    case AVRC_PDU_LIST_PLAYER_APP_VALUES:
    {
      BTIF_TRACE_DEBUG("AVRC_PDU_LIST_PLAYER_APP_VALUES =%d" ,pavrc_cmd->list_app_values.attr_id);
      if (pavrc_cmd->list_app_values.attr_id == 0)
      {
          avrc_setting_cb.send_reject_response_ex (rc_handle, label, pavrc_cmd->pdu,
                  AVRC_STS_BAD_PARAM, pavrc_cmd->cmd.opcode);
          break;
      }
      avrc_setting_cb.fill_pdu_queue_ex(IDX_LIST_APP_VALUE_RSP, ctype, label, true, p_dev);
      HAL_CBACK(bt_rc_callbacks, list_player_app_values_cb,
              (btrc_player_attr_t)pavrc_cmd->list_app_values.attr_id, rc_addr);
    }
    break;
    case AVRC_PDU_GET_CUR_PLAYER_APP_VALUE:
    BtifRcGetAppValueEvt(bt_rc_callbacks, pavrc_cmd,
                                  ctype, label, p_dev, rc_addr, rc_handle);
    break;
    case AVRC_PDU_SET_PLAYER_APP_VALUE:
    BtifRcSetAppValueEvt(bt_rc_callbacks, pavrc_cmd,
                                  ctype, label, p_dev, rc_addr, rc_handle);
    break;
    case AVRC_PDU_GET_PLAYER_APP_ATTR_TEXT:
    BtifRcGetAppAttrTextEvt(bt_rc_callbacks, pavrc_cmd,
                                  ctype, label, p_dev, rc_addr, rc_handle);
    break;
    case AVRC_PDU_GET_PLAYER_APP_VALUE_TEXT:
    BtifRcGetAppValueTextEvt(bt_rc_callbacks, pavrc_cmd,
                                  ctype, label, p_dev, rc_addr, rc_handle);
    break;
   }
}

/**************************************************************************
**
** Function         ListPlayerAppAttrRsp
**
** Description      ListPlayerApplicationSettingAttributes (PDU ID: 0x11)
**                  This method is callled in response to PDU 0x11
**
** Returns          bt_status_t
**
****************************************************************************/
bt_status_t  BtAvrcSetting::ListPlayerAppAttrRsp(RawAddress* bd_addr,
        int num_attr, btrc_player_attr_t *p_attrs)
{
  if (BT_STATUS_SUCCESS != avrc_setting_cb.check_rc_connected_ex(bd_addr))
  {
    return BT_STATUS_NOT_READY;
  }

  BTIF_TRACE_DEBUG("-%s on ", __FUNCTION__);

  tAVRC_RESPONSE avrc_rsp;
  memset(&(avrc_rsp.list_app_attr), 0, sizeof(avrc_rsp));
  if (num_attr == 0)
  {
    avrc_rsp.list_app_attr.status = AVRC_STS_BAD_PARAM;
  }
  else
  {
    avrc_rsp.list_app_attr.num_attr = num_attr;
    for (int i = 0 ; i < num_attr ; ++i)
    {
      avrc_rsp.list_app_attr.attrs[i] = p_attrs[i];
    }
    avrc_rsp.list_app_attr.status = AVRC_STS_NO_ERROR;
  }
  avrc_rsp.list_app_attr.pdu  = AVRC_PDU_LIST_PLAYER_APP_ATTR ;
  avrc_rsp.list_app_attr.opcode = avrc_setting_cb.opcode_from_pdu_ex(AVRC_PDU_LIST_PLAYER_APP_ATTR);
  /* Send the response */
  avrc_setting_cb.send_meta_rsp_ex(bd_addr, IDX_LIST_APP_ATTR_RSP, &avrc_rsp);
  return BT_STATUS_SUCCESS;
}

/**********************************************************************
**
** Function          ListPlayerAppValueRsp
**
** Description      ListPlayerApplicationSettingValues (PDU ID: 0x12)
**                    This method is called in response to PDU 0x12
************************************************************************/
bt_status_t  BtAvrcSetting::ListPlayerAppValueRsp(RawAddress* bd_addr,
        int num_val, uint8_t *value)
{
  if (BT_STATUS_SUCCESS != avrc_setting_cb.check_rc_connected_ex(bd_addr))
  {
    return BT_STATUS_NOT_READY;
  }

  BTIF_TRACE_DEBUG("-%s on ", __FUNCTION__);

  tAVRC_RESPONSE avrc_rsp;
  memset(&(avrc_rsp.list_app_values), 0, sizeof(tAVRC_LIST_APP_VALUES_RSP));
  if ((num_val == 0) || (num_val > AVRC_MAX_APP_ATTR_SIZE))
  {
    avrc_rsp.list_app_values.status = AVRC_STS_BAD_PARAM;
  }
  else
  {
    avrc_rsp.list_app_values.num_val = num_val;
    for (int i = 0; i < num_val; ++i)
    {
      avrc_rsp.list_app_values.vals[i] = value[i];
    }
    avrc_rsp.list_app_values.status = AVRC_STS_NO_ERROR;
  }
  avrc_rsp.list_app_values.pdu   = AVRC_PDU_LIST_PLAYER_APP_VALUES;
  avrc_rsp.list_app_attr.opcode  = avrc_setting_cb.opcode_from_pdu_ex(AVRC_PDU_LIST_PLAYER_APP_VALUES);
  /* Send the response */
  avrc_setting_cb.send_meta_rsp_ex(bd_addr, IDX_LIST_APP_VALUE_RSP, &avrc_rsp);
  return BT_STATUS_SUCCESS;
}


/**********************************************************************
**
** Function      GetPlayerAppValueRsp
**
** Description  This methos is called in response to PDU ID 0x13
**
***********************************************************************/
bt_status_t BtAvrcSetting::GetPlayerAppValueRsp(RawAddress* bd_addr,
        btrc_player_settings_t *p_vals)
{
  tAVRC_APP_SETTING app_sett[AVRC_MAX_APP_ATTR_SIZE];
  if (BT_STATUS_SUCCESS != avrc_setting_cb.check_rc_connected_ex(bd_addr))
  {
    return BT_STATUS_NOT_READY;
  }

  BTIF_TRACE_DEBUG("-%s on ", __FUNCTION__);
  tAVRC_RESPONSE avrc_rsp;
  memset(&(avrc_rsp.get_cur_app_val), 0, sizeof(tAVRC_GET_CUR_APP_VALUE_RSP));
  avrc_rsp.get_cur_app_val.p_vals   = app_sett ;
  //Check for Error Condition
  if ((p_vals == NULL) || (p_vals->num_attr== 0) || (p_vals->num_attr > AVRC_MAX_APP_ATTR_SIZE))
  {
    avrc_rsp.get_cur_app_val.status = AVRC_STS_BAD_PARAM;
  }
  else if (p_vals->num_attr <= BTRC_MAX_APP_SETTINGS)
  {
    memset(app_sett, 0, sizeof(tAVRC_APP_SETTING)*p_vals->num_attr );
    //update num_val
    avrc_rsp.get_cur_app_val.num_val  = p_vals->num_attr ;
    avrc_rsp.get_cur_app_val.p_vals   = app_sett ;
    for (int i = 0; i < p_vals->num_attr; ++i)
    {
      app_sett[i].attr_id  = p_vals->attr_ids[i] ;
      app_sett[i].attr_val = p_vals->attr_values[i];
      BTIF_TRACE_DEBUG("%s attr_id:0x%x, charset_id:0x%x, num_element:%d",
        __FUNCTION__, (unsigned int)app_sett[i].attr_id,
        app_sett[i].attr_val ,p_vals->num_attr );
    }
    //Update PDU , status aind
    avrc_rsp.get_cur_app_val.status = AVRC_STS_NO_ERROR;
  }
  avrc_rsp.get_cur_app_val.pdu = AVRC_PDU_GET_CUR_PLAYER_APP_VALUE;
  avrc_rsp.get_cur_app_val.opcode = avrc_setting_cb.opcode_from_pdu_ex(AVRC_PDU_GET_CUR_PLAYER_APP_VALUE);
  avrc_setting_cb.send_meta_rsp_ex(bd_addr, IDX_GET_CURR_APP_VAL_RSP, &avrc_rsp);
  return BT_STATUS_SUCCESS;
}

 /********************************************************************
 **
 ** Function        GetPlayerAppAttrTextRsp
 **
 ** Description   This method is called in response to get player
 **                         applicaton attribute text response
 **
 **
 *******************************************************************/
 bt_status_t BtAvrcSetting::GetPlayerAppAttrTextRsp(RawAddress* bd_addr,
         int num_attr, btrc_player_setting_text_t *p_attrs)
{
  tAVRC_APP_SETTING_TEXT attr_txt[AVRC_MAX_APP_ATTR_SIZE];
  if (BT_STATUS_SUCCESS != avrc_setting_cb.check_rc_connected_ex(bd_addr))
  {
    return BT_STATUS_NOT_READY;
  }

  BTIF_TRACE_DEBUG("-%s ", __FUNCTION__);
  tAVRC_RESPONSE avrc_rsp;
  memset(&(avrc_rsp.get_app_attr_txt), 0, sizeof(tAVRC_GET_APP_ATTR_TXT_RSP));
  if (num_attr == 0)
  {
    avrc_rsp.get_app_attr_txt.status = AVRC_STS_BAD_PARAM;
  }
  else
  {
    for (int i =0; i< num_attr; ++i)
    {
      attr_txt[i].charset_id = AVRC_CHARSET_ID_UTF8;
      attr_txt[i].attr_id = p_attrs[i].id ;
      attr_txt[i].str_len = (uint8_t)strnlen((char *)p_attrs[i].text, BTRC_MAX_ATTR_STR_LEN);
      attr_txt[i].p_str = p_attrs[i].text ;
      BTIF_TRACE_DEBUG("%s attr_id:0x%x, charset_id:0x%x, str_len:%d, str:%s",
        __FUNCTION__, (unsigned int)attr_txt[i].attr_id,
        attr_txt[i].charset_id , attr_txt[i].str_len, attr_txt[i].p_str);
    }
    avrc_rsp.get_app_attr_txt.status = AVRC_STS_NO_ERROR;
  }
  avrc_rsp.get_app_attr_txt.p_attrs = attr_txt ;
  avrc_rsp.get_app_attr_txt.num_attr = (uint8_t)num_attr;
  avrc_rsp.get_app_attr_txt.pdu = AVRC_PDU_GET_PLAYER_APP_ATTR_TEXT;
  avrc_rsp.get_app_attr_txt.opcode = avrc_setting_cb.opcode_from_pdu_ex(AVRC_PDU_GET_PLAYER_APP_ATTR_TEXT);
  /* Send the response */
  avrc_setting_cb.send_meta_rsp_ex(bd_addr, IDX_GET_APP_ATTR_TXT_RSP, &avrc_rsp);
  return BT_STATUS_SUCCESS;
}

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
 bt_status_t BtAvrcSetting::GetPlayerAppValueTextRsp(RawAddress* bd_addr,
         int num_attr, btrc_player_setting_text_t *p_attrs)
{
  tAVRC_APP_SETTING_TEXT attr_txt[AVRC_MAX_APP_ATTR_SIZE];
  if (BT_STATUS_SUCCESS != avrc_setting_cb.check_rc_connected_ex(bd_addr))
  {
    return BT_STATUS_NOT_READY;
  }

  BTIF_TRACE_DEBUG("- %s ", __FUNCTION__);
  tAVRC_RESPONSE avrc_rsp;
  memset(&(avrc_rsp.get_app_val_txt), 0, sizeof(tAVRC_GET_APP_ATTR_TXT_RSP));
  if (num_attr == 0)
  {
    avrc_rsp.get_app_val_txt.status = AVRC_STS_BAD_PARAM;
  }
  else
  {
    for (int i =0; i< num_attr; ++i)
    {
      attr_txt[i].charset_id = AVRC_CHARSET_ID_UTF8;
      attr_txt[i].attr_id  = p_attrs[i].id ;
      attr_txt[i].str_len  = (uint8_t)strnlen((char *)p_attrs[i].text ,BTRC_MAX_ATTR_STR_LEN );
      attr_txt[i].p_str = p_attrs[i].text ;
      BTIF_TRACE_DEBUG("%s attr_id:0x%x, charset_id:0x%x, str_len:%d, str:%s",
        __FUNCTION__, (unsigned int)attr_txt[i].attr_id,
        attr_txt[i].charset_id , attr_txt[i].str_len,attr_txt[i].p_str);
    }
    avrc_rsp.get_app_val_txt.status = AVRC_STS_NO_ERROR;
  }
  avrc_rsp.get_app_val_txt.p_attrs = attr_txt;
  avrc_rsp.get_app_val_txt.num_attr = (uint8_t)num_attr;
  avrc_rsp.get_app_val_txt.pdu = AVRC_PDU_GET_PLAYER_APP_VALUE_TEXT;
  avrc_rsp.get_app_val_txt.opcode = avrc_setting_cb.opcode_from_pdu_ex(AVRC_PDU_GET_PLAYER_APP_VALUE_TEXT);
  /* Send the response */
  avrc_setting_cb.send_meta_rsp_ex(bd_addr, IDX_GET_APP_VAL_TXT_RSP, &avrc_rsp);
  return BT_STATUS_SUCCESS;
}

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
 bt_status_t BtAvrcSetting::SetPlayerAppValueRsp (RawAddress* bd_addr,
          btrc_status_t rsp_status)
{
  BTIF_TRACE_DEBUG("-%s", __FUNCTION__);

  if (BT_STATUS_SUCCESS != avrc_setting_cb.check_rc_connected_ex(bd_addr))
  {
    return BT_STATUS_NOT_READY;
  }
  tAVRC_RESPONSE avrc_rsp;
  memset(&(avrc_rsp.set_app_val), 0, sizeof(tAVRC_RSP));
  avrc_rsp.set_app_val.opcode = avrc_setting_cb.opcode_from_pdu_ex(AVRC_PDU_SET_PLAYER_APP_VALUE);
  avrc_rsp.set_app_val.pdu =  AVRC_PDU_SET_PLAYER_APP_VALUE ;
  avrc_rsp.set_app_val.status =  rsp_status ;
  avrc_setting_cb.send_meta_rsp_ex(bd_addr, IDX_SET_APP_VAL_RSP, &avrc_rsp);
  return BT_STATUS_SUCCESS;
}
#endif

bool BtAvrcStatusHandling::is_triggered = false;
long BtAvrcStatusHandling::sco_disc_time = 0;
bool BtAvrcStatusHandling::ignore_play_key = false;
bool BtAvrcStatusHandling::force_to_update = false;
bool BtAvrcStatusHandling::ignore_avrcp_play_status = false;
alarm_t* BtAvrcStatusHandling::fake_status_alarm = NULL;
RegisterNotificationRsp BtAvrcStatusHandling::register_notification_rsp = NULL;

void BtAvrcStatusHandling::sleep_ms(period_ms_t timeout_ms) {
  struct timespec delay;
  delay.tv_sec = timeout_ms / 1000;
  delay.tv_nsec = 1000 * 1000 * (timeout_ms % 1000);

  OSI_NO_INTR(nanosleep(&delay, &delay));
}

void BtAvrcStatusHandling::FakeStatusTimeoutHandler(UNUSED_ATTR uint16_t event, char *data) {
  BTIF_TRACE_DEBUG("%s:", __func__);
  ignore_avrcp_play_status = false;
  force_to_update = true;

  if (register_notification_rsp == NULL) {
    BTIF_TRACE_ERROR("%s, register_notification_rsp is null", __func__);
    return;
  }

  //Handle the status update
  btrc_register_notification_t notification_msg;
  memset(&notification_msg, 0, sizeof(notification_msg));
  notification_msg.play_status = BTRC_PLAYSTATE_PLAYING;

  register_notification_rsp(BTRC_EVT_PLAY_STATUS_CHANGED,
        BTRC_NOTIFICATION_TYPE_CHANGED,
        &notification_msg);

  sleep_ms(100);

  notification_msg.play_status = BTRC_PLAYSTATE_PAUSED;
  register_notification_rsp(BTRC_EVT_PLAY_STATUS_CHANGED,
        BTRC_NOTIFICATION_TYPE_CHANGED,
        &notification_msg);

  force_to_update = false;
  is_triggered = false;

  //Free the timer
  if (fake_status_alarm != NULL) {
    alarm_free(fake_status_alarm);
    fake_status_alarm = NULL;
  }
}

void BtAvrcStatusHandling::FakeStatusTimerTimeout(void* data) {
  BTIF_TRACE_DEBUG("%s:", __func__);
  btif_transfer_context(FakeStatusTimeoutHandler, 0, NULL, 0, NULL);
}


void BtAvrcStatusHandling::StartFakeStatusTimer() {
  BTIF_TRACE_DEBUG("%s: ", __func__);
  ignore_avrcp_play_status = true;
  is_triggered = true;

  if (fake_status_alarm == NULL) {
    fake_status_alarm = alarm_new("btif_rc.fake_state_timer");
  }
  alarm_set_on_mloop(fake_status_alarm, 2000,
                       FakeStatusTimerTimeout, NULL);
}

bool BtAvrcStatusHandling::IsNeededIgnorePlay(long disc_time, tBTA_AV_REMOTE_CMD* p_cmd,
                RegisterNotificationRsp call_back) {
  BTIF_TRACE_ERROR("%s.", __func__);
  if (p_cmd == NULL || p_cmd->rc_id != BTA_AV_RC_PLAY) {
    return false;
  }

  if (is_triggered || alarm_is_scheduled(fake_status_alarm)) {
    return true;
  }

  if (is_triggered == false) {
    sco_disc_time = disc_time;
  }

  if(time(NULL) - disc_time > 3) {
    return false;
  }

  register_notification_rsp = call_back;
  StartFakeStatusTimer();
  return true;
}

bool BtAvrcStatusHandling::GetForceUpdateFlag() {
  return force_to_update;
}

bool BtAvrcStatusHandling::GetIgnoreAvrcpStatusFlag() {
  return ignore_avrcp_play_status;
}

void BtAvrcStatusHandling::CancelTimer() {
  if (fake_status_alarm == NULL) {
    return;
  }
  alarm_cancel(fake_status_alarm);
  force_to_update = false;
  ignore_avrcp_play_status = false;
  is_triggered = false;

  if (fake_status_alarm != NULL) {
    alarm_free(fake_status_alarm);
    fake_status_alarm = NULL;
  }
}

}  // namespace btif
}  // namespace bluetooth
}  // namespace mediatek
}  // names
#endif

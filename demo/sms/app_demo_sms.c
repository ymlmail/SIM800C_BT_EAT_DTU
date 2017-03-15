/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   app_demo_sms.c
 * version       :   0.10
 * Description   :   
 * Authors       :   dingfen.zhu
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2014-02-27, dingfen.zhu, create originally.
 *
 *--------------------------------------------------------------------
 * File Description
 * AT+CEAT=parma1,param2
 * param1 param2 
 *   1      1    

 *
 *--------------------------------------------------------------------
 ********************************************************************/
/********************************************************************
 * Include Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>
#include <string.h>

#include "eat_modem.h"
#include "eat_interface.h"
#include "eat_uart.h"

#include "app_at_cmd_envelope.h"
#include "app_sms_pdu_decode.h"

/********************************************************************
 * Macros
 ********************************************************************/
 #define EAT_SMSAL_MAX_TPDU_SIZE         (175)
#define EAT_MAX_DIGITS_USSD          183
#define EAT_RMMI_IS_LOWER( alpha_char )   \
  ( ( (alpha_char >= 'a') && (alpha_char <= 'z') ) ?  1 : 0 )

#define EAT_RMMI_IS_NUMBER( alpha_char )   \
   ( ( (alpha_char >= '0') && (alpha_char <= '9') ) ? 1 : 0 )

/********************************************************************
 * Types
 ********************************************************************/
 typedef struct smsContactDataTag
{
    u8 useData[EAT_SMSAL_MAX_TPDU_SIZE*2+1];
    u8 segement;
    u8 useLen;
}smsContactData;

typedef struct smsContactInfoArrayTag
{
    smsContactData smsContactDataInfo[10];
    u8 total;
    u8 count;
}smsContactInfoArray;

/********************************************************************
 * Extern Variables (Extern /Global)
 ********************************************************************/
 
/********************************************************************
 * Local Variables:  STATIC
 ********************************************************************/
 static ResultNotifyCb p_smsCmgfCb = NULL;
static ResultNotifyCb p_smsCscaCb = NULL;
static ResultNotifyCb p_smsCmgsCb = NULL;
static SmsReadCB p_smsCmgrCb = NULL;
static ResultNotifyCb p_smsCmgdCb = NULL;
static ResultNotifyCb p_smsCnmiCb = NULL;
static eat_bool s_smsReadyState = FALSE;
static smsContactInfoArray s_smsContactData = {0};




/********************************************************************
* Local Function declaration
 ********************************************************************/
 static AtCmdRsp AtCmdCb_cmgf(u8* pRspStr);
static AtCmdRsp AtCmdCb_csca(u8* pRspStr); 
static AtCmdRsp AtCmdCb_cmgs(u8* pRspStr);
static AtCmdRsp AtCmdCb_cmgs_ex(u8* pRspStr);
static AtCmdRsp AtCmdCb_cmgr(u8* pRspStr);
static AtCmdRsp AtCmdCb_cmgd(u8* pRspStr);
static AtCmdRsp AtCmdCb_cnmi(u8* pRspStr);
eat_bool eat_module_test_sms(u8 param1, u8 param2);


/********************************************************************
* Local Function
 ********************************************************************/
void cmd_cmgf_cb(eat_bool result)
{
    eat_trace("cmd_cmgf_cb result = %d", result);
}

void cmd_cnmi_cb(eat_bool result)
{
    eat_trace("cmd_cnmi_cb result = %d", result);
}

void cmd_cmgd_cb(eat_bool result)
{
    eat_trace("cmd_cmgd_cb result = %d", result);
}

void cmd_cmgr_cb(u8* number,u8 *msg)
{
    eat_trace("cmd_cmgd_cb number = %s", number);
    eat_trace("cmd_cmgd_cb msg = %s", msg);

	//读取短信后回调函数。
}

void cmd_cmgs_cb(eat_bool result)
{
    eat_trace("cmd_cmgs_cb result = %d", result);
}

eat_bool simcom_sms_format_set(u8 nFormatType, ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    u8 count = 0 ;
    u8 pAtCpin[20] = {0};
    AtCmdEntity atCmdInit={
        "AT+CMGF=", 0, AtCmdCb_cmgf /* 0  */
    };
    eat_trace("simcom_sms_format_set");
    /* set at cmd func's range,must first set */
    
    sprintf(pAtCpin,"%s%d%s",atCmdInit.p_atCmdStr,nFormatType,AT_CMD_END);
    atCmdInit.p_atCmdStr = pAtCpin;
    atCmdInit.cmdLen = strlen(pAtCpin);

    result = simcom_atcmd_queue_append(atCmdInit);
    eat_trace("simcom_sms_format_set,result=%d",result);
    if(result&&(NULL != pResultCb)){
        p_smsCmgfCb = pResultCb;
    } 
    return result;
}
eat_bool simcom_sms_sc_set(u8* number, ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    u8 count = 0 ;
    u8 pAtCpin[50] = {0};
    AtCmdEntity atCmdInit={
        "AT+CSCA=", 0, NULL /* 0  */
    };

    if(FALSE == s_smsReadyState)
    {
        return FALSE;
    }

    eat_trace("simcom_sms_sc_set");
    /* set at cmd func's range,must first set */
    
    sprintf(pAtCpin,"%s%s%s",atCmdInit.p_atCmdStr,number,AT_CMD_END);
    atCmdInit.p_atCmdStr = pAtCpin;
    atCmdInit.cmdLen = strlen(pAtCpin);

    result = simcom_atcmd_queue_append(atCmdInit);
    eat_trace("simcom_sms_sc_setsimcom_sms_sc_set,result=%d",result);
    if(result&&(NULL != pResultCb)){
        p_smsCscaCb = pResultCb;
    } 
    return result;
}
eat_bool simcom_sms_text_send(u8* number, u8* msg, u16 msgLen,ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    AtCmdEntity atCmdInit[]={
        {"AT+CMGF=1"AT_CMD_END,11,NULL},
        {"AT+CMGS=",0,AtCmdCb_cmgs},
        {"",0,AtCmdCb_cmgs},
        };
    u8 pAtCmd[40] = {0};
    u8 pAtCmdMsg[161] = {0};
    
    if(NULL == number){
        return FALSE;
    }

    if(msgLen > 160){
        return FALSE;
    }

    if(FALSE == s_smsReadyState)
    {
        return FALSE;
    }

    eat_trace("simcom_sms_text_send");

    sprintf(pAtCmd,"%s\"%s\"%s",atCmdInit[1].p_atCmdStr,number,AT_CMD_END);
    atCmdInit[1].p_atCmdStr = pAtCmd;
    atCmdInit[1].cmdLen = strlen(pAtCmd);

    sprintf(pAtCmdMsg,"%s%s%s",atCmdInit[2].p_atCmdStr,msg,AT_CMD_CTRL_Z);
    atCmdInit[2].p_atCmdStr = pAtCmdMsg;
    atCmdInit[2].cmdLen = strlen(pAtCmdMsg);
	//SIMCOM add by hanjun.liu @20140317 for CR_MKBug00021506_hanjun.liu code by jumping start
    result = simcom_atcmd_queue_fun_append(atCmdInit,sizeof(atCmdInit) / sizeof(atCmdInit[0]),pResultCb);
	//SIMCOM add by hanjun.liu @20140317 for CR_MKBug00021506_hanjun.liu code by jumping end

    if(result&&(NULL != pResultCb)){
        p_smsCmgsCb = pResultCb;
    } 
    return TRUE;
}
eat_bool simcom_sms_pdu_send(u8* ptrPdu, u16 pduLen,ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    AtCmdEntity atCmdInit[]={
        {"AT+CMGF=0"AT_CMD_END,11,NULL},
        {"AT+CMGS=",0,AtCmdCb_cmgs},
        {"",0,AtCmdCb_cmgs},
        };
    u8 pAtCmd[40] = {0};
    u8 pAtCmdMsg[161] = {0};
    
    if(NULL == ptrPdu){
        return FALSE;
    }


    if(FALSE == s_smsReadyState)
    {
        return FALSE;
    }

    eat_trace("simcom_sms_pdu_send");

    sprintf(pAtCmd,"%s%d%s",atCmdInit[1].p_atCmdStr,pduLen,AT_CMD_END);
    atCmdInit[1].p_atCmdStr = pAtCmd;
    atCmdInit[1].cmdLen = strlen(pAtCmd);

    sprintf(pAtCmdMsg,"%s%s%s",atCmdInit[2].p_atCmdStr,ptrPdu,AT_CMD_CTRL_Z);
    atCmdInit[2].p_atCmdStr = pAtCmdMsg;
    atCmdInit[2].cmdLen = strlen(pAtCmdMsg);
    
	//SIMCOM add by hanjun.liu @20140317 for CR_MKBug00021506_hanjun.liu code by jumping start
    result = simcom_atcmd_queue_fun_append(atCmdInit,sizeof(atCmdInit) / sizeof(atCmdInit[0]),pResultCb);
	//SIMCOM add by hanjun.liu @20140317 for CR_MKBug00021506_hanjun.liu code by jumping end
	if(result&&(NULL != pResultCb)){
        p_smsCmgsCb = pResultCb;
    } 
    return TRUE;
}
eat_bool simcom_sms_msg_read(u16 index, SmsReadCB pUrcCb)
{
    eat_bool result = FALSE;
    AtCmdEntity atCmdInit[]={
        {"AT+CMGF=1"AT_CMD_END,11,NULL}, /* 0  */
        {"AT+CMGR=",0,AtCmdCb_cmgr} /*1   */
    };
    u8 pAtCmd[40] = {0};

    if(FALSE == s_smsReadyState)
    {
        return FALSE;
    }

    eat_trace("simcom_sms_msg_read");
    sprintf(pAtCmd,"%s%d,%d%s",atCmdInit[1].p_atCmdStr,index,TRUE,AT_CMD_END);
    eat_trace("simcom_sms_msg_read,at cmd(%s)",pAtCmd);
    atCmdInit[1].p_atCmdStr = pAtCmd;
    atCmdInit[1].cmdLen = strlen(pAtCmd);
    
	//SIMCOM add by hanjun.liu @20140317 for CR_MKBug00021506_hanjun.liu code by jumping start
    result = simcom_atcmd_queue_fun_append(atCmdInit,sizeof(atCmdInit) / sizeof(atCmdInit[0]),pUrcCb);
	//SIMCOM add by hanjun.liu @20140317 for CR_MKBug00021506_hanjun.liu code by jumping end
    eat_trace("simcom_sms_msg_read, result= %d",result);
    if(result&&(NULL != pUrcCb)){
        p_smsCmgrCb = pUrcCb;
    } 

    return TRUE;
}
eat_bool simcom_sms_msg_delete(u16 index, ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    u8 count = 0 ;
    u8 pAtCpin[20] = {0};
    AtCmdEntity atCmdInit={
        "AT+CMGD=",0,AtCmdCb_cmgd /*0   */
    };
    u8 pAtCmd[40] = {0};

    if(FALSE == s_smsReadyState)
    {
        return FALSE;
    }

    eat_trace("simcom_sms_msg_delete");
    sprintf(pAtCmd,"%s%d%s",atCmdInit.p_atCmdStr,index,AT_CMD_END);
    atCmdInit.p_atCmdStr = pAtCmd;
    atCmdInit.cmdLen = strlen(pAtCmd);

    result = simcom_atcmd_queue_append(atCmdInit);
    eat_trace("simcom_sms_msg_delete,result=%d",result);
    if(result&&(NULL != pResultCb)){
        p_smsCmgdCb = pResultCb;
    } 
    
    return TRUE;
}

eat_bool simcom_sms_cnmi_set(u8 mode, u8 mt, u8 bm,u8 ds, u8 bfr,ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    u8 count = 0 ;
    u8 pAtCpin[40] = {0};
    AtCmdEntity atCmdInit={
        "AT+CNMI=", 0, AtCmdCb_cnmi /* 0  */
    };

    if(FALSE == s_smsReadyState)
    {
        return FALSE;
    }
    eat_trace("simcom_sms_cnmi_set");

    sprintf(pAtCpin,"%s%d,%d,%d,%d,%d%s",atCmdInit.p_atCmdStr,mode,mt,bm,ds,bfr,AT_CMD_END);
    atCmdInit.p_atCmdStr = pAtCpin;
    atCmdInit.cmdLen = strlen(pAtCpin);
    eat_trace("simcom_sms_cnmi_set,(%s)",pAtCpin);

    result = simcom_atcmd_queue_append(atCmdInit);
    eat_trace("simcom_sms_cnmi_set,result=%d",result);
    if(result&&(NULL != pResultCb)){
        p_smsCnmiCb = pResultCb;
    }
    return result;
}

void simcom_read_sms_interface_register(SmsReadCB smsReadSmsCB)
{
    if(NULL != smsReadSmsCB){
        p_smsCmgrCb = smsReadSmsCB;
    }
}

static AtCmdRsp AtCmdCb_cmgf(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR","OK"};
    s16  rspType = -1;
    u8  i = 0;
    u8  *p = pRspStr;
    while(p) {
        /* ignore \r \n */
        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }

        for (i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if (!strncmp(rspStrTable[i], p, strlen(rspStrTable[i])))
            {
                rspType = i;
                break;
            }
        }

        p = (u8*)strchr(p,AT_CMD_LF);
    }
    eat_trace("AtCmdCb_cmgf(), rspType(%d)",rspType);
    switch (rspType)
    {

        case 0:  /* ERROR */
        rspValue = AT_RSP_ERROR;
        break;

        case 1:  /* OK */
        rspValue  = AT_RSP_FINISH;

        if(p_smsCmgfCb) {
            p_smsCmgfCb(TRUE);
            p_smsCmgfCb = NULL;
        }
        break;

        default:
        rspValue = AT_RSP_WAIT;
        break;
    }

    return rspValue;
}
static AtCmdRsp AtCmdCb_csca(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR","OK"};
    s16  rspType = -1;
    u8  i = 0;
    u8  *p = pRspStr;
    while(p) {
        /* ignore \r \n */
        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }

        for (i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if (!strncmp(rspStrTable[i], p, strlen(rspStrTable[i])))
            {
                rspType = i;
                break;
            }
        }

        p = (u8*)strchr(p,AT_CMD_LF);
    }
    eat_trace("AtCmdCb_csca(), rspType(%d)",rspType);
    switch (rspType)
    {

        case 0:  /* ERROR */
        rspValue = AT_RSP_ERROR;
        break;

        case 1:  /* OK */
        rspValue  = AT_RSP_FINISH;

        if(p_smsCscaCb) {
            p_smsCscaCb(TRUE);
            p_smsCscaCb = NULL;
        }
        break;

        default:
        rspValue = AT_RSP_WAIT;
        break;
    }

    return rspValue;
}
static AtCmdRsp AtCmdCb_cmgs(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR","> ","OK"};
    s16  rspType = -1;
    u8  i = 0;
    u8  *p = pRspStr;
    while(p) {
        /* ignore \r \n */
        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }

        for (i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if (!strncmp(rspStrTable[i], p, strlen(rspStrTable[i])))
            {
                rspType = i;
                break;
            }
        }

        p = (u8*)strchr(p,AT_CMD_LF);
    }
    eat_trace("AtCmdCb_cmgs(), rspType(%d)",rspType);
    switch (rspType)
    {

        case 0:  /* ERROR */
        rspValue = AT_RSP_ERROR;
        break;

        case 1:  /* >  */
        rspValue  = AT_RSP_CONTINUE;

        break;
        case 2: /* OK */
        rspValue = AT_RSP_FUN_OVER;
        break;

        default:
        rspValue = AT_RSP_WAIT;
        break;
    }

    return rspValue;
}
static AtCmdRsp AtCmdCb_cmgs_ex(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR","OK"};
    s16  rspType = -1;
    u8  i = 0;
    u8  *p = pRspStr;
    while(p) {
        /* ignore \r \n */
        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }

        for (i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if (!strncmp(rspStrTable[i], p, strlen(rspStrTable[i])))
            {
                rspType = i;
                break;
            }
        }

        p = (u8*)strchr(p,AT_CMD_LF);
    }

    eat_trace("AtCmdCb_cmgs_ex(), rspType(%d)",rspType);
    switch (rspType)
    {

        case 0:  /* ERROR */
        rspValue = AT_RSP_ERROR;
        break;

        case 1:  /* OK */
        rspValue  = AT_RSP_FUN_OVER;

        if(p_smsCmgsCb) {
            p_smsCmgsCb(TRUE);
            p_smsCmgsCb = NULL;
        }
        break;

        default:
        rspValue = AT_RSP_WAIT;
        break;
    }

    return rspValue;
}
static AtCmdRsp AtCmdCb_cnmi(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR","OK"};
    s16  rspType = -1;
    u8  i = 0;
    u8  *p = pRspStr;
    while(p) {
        /* ignore \r \n */
        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }

        for (i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if (!strncmp(rspStrTable[i], p, strlen(rspStrTable[i])))
            {
                rspType = i;
                break;
            }
        }

        p = (u8*)strchr(p,AT_CMD_LF);
    }

    switch (rspType)
    {

        case 0:  /* ERROR */
        rspValue = AT_RSP_ERROR;
        break;

        case 1:  /* OK */
        rspValue  = AT_RSP_FINISH;

        if(p_smsCnmiCb) {
            p_smsCnmiCb(TRUE);
            p_smsCnmiCb = NULL;
        }
        break;

        default:
        rspValue = AT_RSP_WAIT;
        break;
    }

    return rspValue;
}
static AtCmdRsp AtCmdCb_cmgr(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR","+CMGR: ","OK"};
    s16  rspType = -1;
    u8  i = 0;
    u8  *p = pRspStr;
    u8  *pNumEnd = NULL;
	u8  *pMsgEnd = NULL;
    u8 szTime[200] = {0};
    u8 szNumber[40] = {0};
    u8 szName[40] = {0};
    u8 szDate[40] = {0};
    u8 szMsg[161] = {0};
    u16 len = 0;
    
    while(p) {
        /* ignore \r \n */
        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }

        for (i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if (!strncmp(rspStrTable[i], p, strlen(rspStrTable[i])))
            {
                rspType = i;
                break;
            }
        }

        if(1 == rspType){
            break;
        }
        
        p = (u8*)strchr(p,AT_CMD_LF);
    }

    eat_trace("AtCmdCb_cmgr: rspType(%d)",rspType);
    switch (rspType)
    {

        case 0:  /* ERROR */
        rspValue = AT_RSP_ERROR;
        break;

        case 1:  /* +CMGR:  */
        rspValue  = AT_RSP_FUN_OVER;
        p = (u8*)strchr(p,',');
        p = p+2;
        pNumEnd = (char*)strchr(p,',');
	    len = pNumEnd - p - 1;
        memcpy(szNumber,p,len);
        p = (u8*)strchr(p,AT_CMD_LF);
		p=p+1;
        pMsgEnd = (u8*)strchr(p,AT_CMD_CR);
        len = pMsgEnd - p;
        memcpy(szMsg,p,len);
        eat_trace("AtCmdCb_cmgr: number(%s),msg(%s)",szNumber,szMsg);
		 if (!strncmp("13320984483", szNumber, strlen("13320984483")))
            {
                eat_trace("the phone number is ==\n");//字符串相等
            }
		
		eat_trace("---------------------------------");
		 eat_trace("szMsg++++++%s\n",szMsg);
		 eat_trace("szMsg------%s\n","SOS,A,13320984483#");
		 for(i=0;i<20;i++)
		 	{
		 	eat_trace("msg[%d]=0x%x,",i,szMsg[i]);
		 	}
		 eat_trace("---------------------------------");
		if (!strncmp("SOS,A,13320984483#", szMsg, strlen("SOS,A,13320984483#")))
            {
                eat_trace("msg is ==SOS,A,13320984483#\n");//字符串相等
                 eat_trace("szMsg len++++++%d\n",len);
            }
		else
			{
			eat_trace("msg is !=SOS,A,13320984483#\n");
			}
		if (!strncmp("SOS,A,1", "SOS,A,1", 7))
            {
                eat_trace("msg is ==SOS,A,1\n");
            }
        //simcom_atcmd_queue_fun_out();
        if(p_smsCmgrCb) {
            p_smsCmgrCb(szNumber,szMsg);
        }
        break;
        case 2:/* OK */
        rspValue = AT_RSP_WAIT;
        break;

        default:
        rspValue = AT_RSP_WAIT;
        break;
    }

    return rspValue;
}
static AtCmdRsp AtCmdCb_cmgd(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR","OK"};
    s16  rspType = -1;
    u8  i = 0;
    u8  *p = pRspStr;
    while(p) {
        /* ignore \r \n */
        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }

        for (i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if (!strncmp(rspStrTable[i], p, strlen(rspStrTable[i])))
            {
                rspType = i;
                break;
            }
        }

        p = (u8*)strchr(p,AT_CMD_LF);
    }

    eat_trace("AtCmdCb_cmgd: rspType(%d)",rspType);

    switch (rspType)
    {
        case 0:  /* ERROR */
        rspValue = AT_RSP_ERROR;
        break;

        case 1:  /* OK */
        rspValue  = AT_RSP_FINISH;

        if(p_smsCmgdCb) {
            p_smsCmgdCb(TRUE);
            p_smsCmgdCb = NULL;
        }
        break;

        default:
        rspValue = AT_RSP_WAIT;
        break;
    }

    return rspValue;
}

void UrcCb_sms_ready(u8* pRspStr, u16 len)
{
    eat_trace(" UrcCb_sms_ready()");
    s_smsReadyState = TRUE;
}
void UrcCb_NewSmsIndicator(u8* pRspStr, u16 len)
{
	AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"+CMTI: "};
    s16  rspType = -1;
    u8   szBuffer[20]= 0;
    u16  index = 0;
    u8  i = 0;
    u8  *p = pRspStr;

    eat_trace("UrcCb_NewSmsIndicator()");
    
    while(p) {
        /* ignore \r \n */
        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }

        if (!strncmp(rspStrTable[0], p, strlen(rspStrTable[0]))){
            rspType =0;
            if (rspType == 0){
                eat_trace("UrcCb_NewSmsIndicator: urc (%s)",p);
                p = (u8*)strchr(p,',');
                p = p + 1;
                eat_trace("UrcCb_NewSmsIndicator: urc1 (%s)",p);
                index= atoi(p); 

                //sscanf(p+strlen(rspStrTable[rspType]),"\"%s\",%d",szBuffer,&index);
                eat_trace("UrcCb_NewSmsIndicator: index(%d)",index);
            }
            break;
        }
    }

    if(0 != index){
       // s_smsIndex = index;
        simcom_sms_msg_read(index, NULL);
    }
}

/*****************************************************************************
* FUNCTION
*  eat_sms_tpdu_addr2_sms_tp_addr
* DESCRIPTION
*   This function converts TP address to L4 address.
*
* PARAMETERS
*  a  IN          tpdu_addr, raw data of address in TPDU.
*  b  IN/OUT      l4_addr, used as return
*                 length indicates number of bytes in addr_bcd
*
*  example: 
*     tpdu_addr = {0x0b, 0x91, 0x88, 0x96, 0x93, 0x24, 0x42, 0xf1 }
*                  len , type,  
*     tp_addr   = 0x0b  { 0x91, 0x88, 0x96, 0x93, 0x24, 0x42, 0xf1, 0xff }
*                 
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/

void eat_sms_tpdu_addr2_sms_tp_addr(u8 *tpdu_addr, eat_sms_addr_struct *tp_addr)
{
    tp_addr->addr_length = tpdu_addr[0];
    tp_addr->addr_bcd[0] = tpdu_addr[1];

    if (tpdu_addr[0] > 0)
    {
        memcpy(&tp_addr->addr_bcd[1], &tpdu_addr[2], (tpdu_addr[0] + 1) / 2);
    }
}

/*****************************************************************************
 * FUNCTION
 *  eat_get_ch_byte_24008
 * DESCRIPTION
 *  This is get_ch_byte_24008 function of L4C module.
 *  convert a BCD byte (lower byte only) to ascii char
 *  this obey spec 24.008 10.5.118 table diff form 11.11 SIM spec
 * PARAMETERS
 *  bcd     [IN]        Input BCd byte
 * RETURNS
 *  kal_uint8      output ascii char
 *****************************************************************************/
u8 eat_get_ch_byte_24008(u8 bcd)
{
    u8 digit;

    if (bcd <= 9)
    {
        digit = bcd + '0';
    }
    else
    {
        switch (bcd)
        {
            case 0x0a:
                digit = '*';
                break;
            case 0x0b:
                digit = '#';
                break;
            case 0x0c:
                digit = 'a';
                break;
            case 0x0d:
                digit = 'b';
                break;
            case 0x0e:
                digit = 'c';
                break;
            default:
                digit = '\0';
                break;
        }
    }
    return digit;
}

/*****************************************************************************
 * FUNCTION
 *  eat_get_ch_byte
 * DESCRIPTION
 *  This is get_ch_byte function of L4C module.
 *  convert a BCD byte (lower byte only) to ascii char
 * PARAMETERS
 *  bcd         [IN]        Input BCd byte
 *  type        [IN]        Type of definition
 *  value 0: for original defintion(MTK make use of 0xd) , value 1: new definition(with wild char support)(?)
 * RETURNS
 *  kal_uint8      output ascii char
 *****************************************************************************/
u8 eat_get_ch_byte(u8 bcd, u8 type)
{
    u8 digit;
    u8 value_d_char = 'w';       // original defnition

    /* mtk02514 ************************
    * When type = CONVERT_TO_DIGIT_WILD,
    * we should convert 0x0d to wild char
    *********************************/

    //mtk01616_080428: new definition (with wild character support), other part is the same as original definition
    if (type == EAT_CONVERT_TO_DIGIT_WILD)
    {
        value_d_char = '?';
    }

    if (bcd <= 9)
    {
        digit = bcd + '0';
    }
    else
    {
        switch (bcd)
        {
            case 0x0a:
                digit = '*';
                break;
            case 0x0b:
                digit = '#';
                break;
            case 0x0c:
                digit = 'p';
                break;
            case 0x0d:
                digit = value_d_char;
                break;
            case 0x0e:
                digit = '+';
                break;
            default:
                digit = '\0';
                break;
        }
    }
    return digit;
}

/*****************************************************************************
 * FUNCTION
 *  convert_to_general_digit
 * DESCRIPTION
 *  This is convert_to_digit function of L4C module.
 *  convert BCD string to ascii string of numbers
 * PARAMETERS
 *  source      [IN]        Input BCD string
 *  dest        [OUT]       Output ascii string
 * RETURNS
 *  kal_uint8      number of bytes of the output ascii string
 *****************************************************************************/
u8 eat_convert_to_general_digit(u8 *source, u8 *dest, u8 type)
{
    u8 ch1, ch2;
    u8 i = 0, j = 0;
    u16 len = EAT_MAX_CC_ADDR_LEN-1;

    if ((source == NULL) || (dest == NULL))
    {
        eat_trace("eat_convert_to_general_digit failed");
        return 0;
    }

    /* mtk02514 ***********************************
    * for CONVERT_TO_DIGIT_EXT, 
    * we should use MAX_DIGITS_USSD as the maximum length
    *********************************************/

    if(type==EAT_CONVERT_TO_DIGIT_EXT)
    {
        len = EAT_MAX_DIGITS_USSD - 1;
    }

    while ((source[i] != 0xff) && j < len)
    {
        ch1 = source[i] & 0x0f;
        ch2 = (source[i] & 0xf0) >> 4;

        /* mtk02514 **************************
        * For CONVERT_TO_DIGIT_24008,
        * we should use get_ch_byte_24008 insead
        * of get_ch_byte
        ***********************************/

        if (type==EAT_CONVERT_TO_DIGIT_24008)
            *((u8*) dest + j) = eat_get_ch_byte_24008(ch1);
        else
            *((u8*) dest + j) = eat_get_ch_byte(ch1, type);

        if (ch2 == 0x0f)
        {
            *((u8*) dest + j + 1) = '\0';
            return j + 1;
        }
        else
        {
            if (type==EAT_CONVERT_TO_DIGIT_24008)
                *((u8*) dest + j + 1) = eat_get_ch_byte_24008(ch2);
            else
                *((u8*) dest + j + 1) = eat_get_ch_byte(ch2, type);

        }
        i++;
        j += 2;
    }
    *((u8*) dest + j) = '\0';
    return j;
}

static eat_bool sms_orig_address_convert(u8* ptrOrigAddr, u8* ptrDestAddr)
{
    eat_sms_addr_struct         dest_addr = {0};
    eat_l4c_number_struct       addr = {0};
    
    if((NULL == ptrOrigAddr) || (NULL == ptrDestAddr ))
    {
        return EAT_FALSE; 
    }

    eat_sms_tpdu_addr2_sms_tp_addr(ptrOrigAddr, &dest_addr);
                
    addr.length = eat_convert_to_general_digit(&(dest_addr.addr_bcd[1]), addr.number, EAT_CONVERT_TO_DIGIT_NORMAL);
    addr.type = dest_addr.addr_bcd[0];

    if((addr.type != 0x81) && (addr.type != 0x91) && (addr.type != 0xa1) && (addr.type != 0xb1))
    {
        addr.type = 0x81;
    }

    memcpy(ptrDestAddr,&(addr.number[0]),addr.length);

    return EAT_TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  EatToUpper
 * DESCRIPTION
 *  
 * PARAMETERS
 *  str     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void EatToUpper(u8 *str)
{
    u8 *ptr;

    if(str == NULL)
    {
        eat_trace("EatToUpper(),failed");
        return ;
    }
    //ASSERT(str != NULL);
    ptr = str;
    while (*ptr != 0)
    {
        if (EAT_RMMI_IS_LOWER(*ptr))
        {
            *ptr += 'A' - 'a';
        }
        ptr++;
    }
}

/*****************************************************************************
 * EatToUpper
 *  eat_check_hex_value_ext
 * DESCRIPTION
 *  same as check_hex_value, but make sure that input string combined of "0"~"9","A"~"F"
 *  "1234" -> 0x1234
 * PARAMETERS
 *  str     [IN]        String
 *  val     [OUT]       Hex value.
 * RETURNS
 *  TRUE/FALSE depends on success or fail.
 *****************************************************************************/
u16 eat_check_hex_value_ext(u8 *str, u8 *val)
{
    u16 i = 0, j = 0;
    u16 temp;

    if((str == NULL) || (val == NULL))
    {
        eat_trace("eat_check_hex_value_ext(),failed");
        return 0;
    }
    //ASSERT((str != NULL) && (val != NULL));
    EatToUpper(str);

    while (str[i] != '\0')
    {
        if (EAT_RMMI_IS_NUMBER(str[i]))
        {
            temp = str[i] - '0';
        }
        else if ((str[i] >= 'A') && (str[i] <= 'F'))
        {
            temp = (str[i] - 'A') + 10;
        }
        else
        {
            return 0;
        }

        if (EAT_RMMI_IS_NUMBER(str[i + 1]))
        {
            temp = (temp << 4) + (str[i + 1] - '0');
        }
        else if ((str[i + 1] >= 'A') && (str[i + 1] <= 'F'))
        {
            temp = (temp << 4) + ((str[i + 1] - 'A') + 10);
        }
        else
        {
            return 0;
        }

        val[j] = temp;
        i += 2;
        j++;
    }

    val[j] = 0;
    eat_trace("eat_check_hex_value_ext(), len = %d",j);
    return j;
}

static void sms_scts_to_data_time(u8 *scts,eat_rtc_date* Date,eat_rtc_time* Time)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    u8      dest[25]={0};
    u8      year[5]={0};
    u8      month[3]={0};
    u8      day[3]={0};
    u8      hours[3]={0};
    u8      minutes[3]={0};
    u8      sec[3]={0};
    char*   ptr = NULL;
    ptr =   &dest[0];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/ 
    if( scts == NULL)
    {
        eat_trace("sms_scts_to_data_time failed");
        return;
    }
    if (strcmp((char*)scts, "\0\0\0\0\0\0\0") == 0)
    {
        dest[0] = '\0';
        return;
    }

	sprintf(
        (u8*) dest,

        "20%d%d%d%d%d%d%d%d%d%d%d%d%d%d\0",// "20%d%d/%d%d/%d%d,%d%d:%d%d:%d%d%c%d%d\0",
        scts[0] & 0xf,
        (scts[0] >> 4),
        scts[1] & 0xf,
        (scts[1] >> 4),
        scts[2] & 0xf,
        (scts[2] >> 4),
        scts[3] & 0xf,
        (scts[3] >> 4),
        scts[4] & 0xf,
        (scts[4] >> 4),
        scts[5] & 0xf,
        (scts[5] >> 4),
        scts[6] & 0x7,
        (scts[6] >> 4));

    memset((void *)Date,0,sizeof(eat_rtc_date));
    strncpy(year,ptr,4);
    ptr +=4;
    Date->year = atoi(year);
    strncpy(month,ptr,2);
    ptr +=2;
    Date->month = atoi(month);
    strncpy(day,ptr,2);
    ptr +=2;
    Date->day = atoi(day);
    memset((void *)Time,0,sizeof(eat_rtc_time));
    strncpy(hours,ptr,2);
    ptr +=2;
    Time->hours = atoi(hours);
    strncpy(minutes,ptr,2);
    ptr +=2;
    Time->minutes= atoi(minutes);
    strncpy(sec,ptr,2);
    Time->seconds = atoi(sec);
}

/*****************************************************************************
 * FUNCTION
 *  eat_check_sms_pdu_string
 * DESCRIPTION
 *  
 * PARAMETERS
 *  length      [IN]        
 *  bytes       [?]         
 *  str         [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void eat_check_sms_pdu_string(u16 length, u8 *bytes, u8 *str)
{
    u16 i = 0;
    u16 j = 0;
  
    if((str == NULL) || (bytes == NULL))
    {
        eat_trace("eat_check_sms_pdu_string() failed");
        return;    
    }
    
    while (i < length)
    {
        j += sprintf((char*)str + j, "%02x", bytes[i]);
        i++;
    }
    str[j] = 0;
    EatToUpper(str);
}

eat_bool simcom_sms_decode_pdu(u8 * data)
{
    eat_smsal_pdu_decode_struct sms_pdu = {0};
    eat_bool ret_val = EAT_FALSE;
    u16 pdu_len = 0;
    u8 ptr_pdu[EAT_SMSAL_MAX_TPDU_SIZE * 2] = {0};
    u8 useData[EAT_SMSAL_MAX_TPDU_SIZE * 2] = {0};
    u8 szDestAddr[43] = {0};
    eat_rtc_date Date = {0};
    eat_rtc_time Time = {0};
    u8 dateTime[40] = {0};
    u16 i =0;
    u8 dcs = 0;
    u16 userDataLen = 0;
    eat_smsal_concat_struct concatInfo = {0};
    u8* ptr = NULL;
    eat_smsal_alphabet_enum alphabet_type;

    memset(&sms_pdu,0,sizeof(eat_smsal_pdu_decode_struct));
    eat_trace("simcom_sms_decode_pdu(), sms pdu (%s),(%d)",data,strlen(data));
    
    pdu_len = eat_check_hex_value_ext(data, ptr_pdu);

    eat_trace("simcom_sms_decode_pdu(), pdu_len (%d)",pdu_len);
    
    if( eat_decode_sms_pdu(ptr_pdu, pdu_len,&sms_pdu))
    {
        dcs = sms_pdu.tpdu.data.deliver_tpdu.dcs;//dcs ?
        alphabet_type = sms_pdu.tpdu.alphabet_type;//????
        concatInfo.ref = sms_pdu.tpdu.concat_info.ref;//???
        concatInfo.seg = sms_pdu.tpdu.concat_info.seg;//?к
        concatInfo.total_seg = sms_pdu.tpdu.concat_info.total_seg;//??
        userDataLen = sms_pdu.tpdu.data.deliver_tpdu.user_data_len;//???
        sms_orig_address_convert(sms_pdu.tpdu.data.deliver_tpdu.orig_addr, szDestAddr);//?绰
        sms_scts_to_data_time(sms_pdu.tpdu.data.deliver_tpdu.scts,&Date,&Time);//??
        sprintf(dateTime,"%04d-%02d-%02d,%02d:%02d:%02d",Date.year, Date.month, 
                        Date.day, Time.hours,Time.minutes, Time.seconds);

        eat_trace("simcom_sms_decode_pdu(), date time (%s)",dateTime);
        eat_trace("simcom_sms_decode_pdu(), number (%s)",szDestAddr);
        eat_trace("simcom_sms_decode_pdu(), total (%d),seg (%d),ref(%d)",concatInfo.total_seg,concatInfo.seg,concatInfo.ref);
        eat_trace("simcom_sms_decode_pdu(), dcs(%d)",dcs);
        eat_trace("simcom_sms_decode_pdu(), alphabet_type(%d)",alphabet_type);

        if(concatInfo.total_seg > 1)//???
        {
            s_smsContactData.total = concatInfo.total_seg;
            s_smsContactData.smsContactDataInfo[concatInfo.seg].segement = concatInfo.seg;
            //???
            ptr = sms_pdu.tpdu.data.deliver_tpdu.user_data + 6;
            userDataLen -= 6;
            s_smsContactData.smsContactDataInfo[concatInfo.seg].useLen = userDataLen;

            //?s_smsContactData.smsContactDataInfo[seg].useData
            memcpy(s_smsContactData.smsContactDataInfo[concatInfo.seg].useData, ptr, userDataLen);
            s_smsContactData.count++;

            if(s_smsContactData.count == s_smsContactData.total)//???
            {
                eat_trace("simcom_sms_decode_pdu(), total = %d ",s_smsContactData.total);
                for(i = 1; i <= s_smsContactData.total;i++ )//ж
                {
                    eat_check_sms_pdu_string(s_smsContactData.smsContactDataInfo[i].useLen,s_smsContactData.smsContactDataInfo[i].useData,useData);
                    eat_trace("simcom_sms_decode_pdu(), user data(%s)",useData);
                    memset(useData,0, EAT_SMSAL_MAX_TPDU_SIZE * 2);
                }
                
                memset(&s_smsContactData, 0, sizeof(smsContactInfoArray));
            }
        }
        else
        {
            s_smsContactData.total = 1;
            s_smsContactData.smsContactDataInfo[0].segement = 1;
            //?s_smsContactData.smsContactDataInfo[0].useData
            memcpy(s_smsContactData.smsContactDataInfo[0].useData, sms_pdu.tpdu.data.deliver_tpdu.user_data, userDataLen);
    
            //???
            eat_check_sms_pdu_string(userDataLen,s_smsContactData.smsContactDataInfo[0].useData,useData);
            eat_trace("simcom_sms_decode_pdu(), user data len (%d)",userDataLen);
            eat_trace("simcom_sms_decode_pdu(), user data(%s)",useData);

            memset(&s_smsContactData, 0, sizeof(smsContactInfoArray));
        }
        
        eat_sms_free_tpdu_decode_struct(&sms_pdu.tpdu);
        ret_val = EAT_TRUE;
    }
    else
    {
         eat_sms_free_tpdu_decode_struct(&sms_pdu.tpdu);
    }

    return ret_val;
}

void UrcCb_FlashNewSmsIndicator(u8* pRspStr, u16 len)
{
	AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"+CMT: "};
    s16  rspType = -1;
    u8   szBuffer[20]= 0;
    u16  index = 0;
    u8  i = 0;
    u8  *p = pRspStr;
    u8  *ptrTemp = NULL;
    u8 data[EAT_SMSAL_MAX_TPDU_SIZE * 2] = {0};

    eat_trace("UrcCb_FlashNewSmsIndicator()");
    
    while(p) {
        /* ignore \r \n */
        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }

        if (!strncmp(rspStrTable[0], p, strlen(rspStrTable[0]))){
            rspType =0;
            if (rspType == 0){
                eat_trace("UrcCb_FlashNewSmsIndicator: urc (%s)",p);
                p = (u8*)strchr(p,AT_CMD_LF);
                p = p + 1;
                ptrTemp = (u8*)strchr(p,AT_CMD_CR);
                memcpy(data,p,ptrTemp-p);
                eat_trace("UrcCb_FlashNewSmsIndicator: urc1 (%s)",p);

                simcom_sms_decode_pdu(data);
            }
            break;
        }
    }
}
static eat_bool eat_module_test_fun(u8 param1, u8 param2)
{
    /***************************************
     * example 1
     ***************************************/
    eat_trace("eat_module_test_fun:%d,%d" ,param1, param2);
    eat_module_test_sms(param1, param2);
}

void UrcCb_modem_data_parse(u8* pRspStr, u16 len)
{
    u8* buf_ptr = NULL;
    u8 param1 = 0;
    u8 param2 = 0;

    eat_trace("data_parse:%s",pRspStr);
    /*param:%d,extern_param:%d*/
     buf_ptr = (u8*)strstr((const char *)pRspStr,"param");
    if( buf_ptr != NULL)
    {
        sscanf((const char *)buf_ptr, "param:%d,extern_param:%d",(int*)&param1, (int*)&param2);
        eat_trace("data parse param1:%d param2:%d",param1, param2);
        eat_module_test_fun(param1, param2);
    }
}


 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_module_test_sms
 *  Description: test sms function
 *        Input:
 			param1:
 			param2:
 *       Output:
 *       Return:
 *       author: dingfen.zhu
 * =====================================================================================
 */
eat_bool eat_module_test_sms(u8 param1, u8 param2)
{
	eat_trace("eat_module_test_sms: param1 = %d. param2 = %d", param1, param2);
	if(param1 == 1)//AT+CMGF=0/1
	{
		if(param2 == 1)//AT+CMGF=0
		{
            simcom_sms_format_set(0, cmd_cmgf_cb);
		}
		else if(param2 == 2)//AT+CMGF=1
		{
            simcom_sms_format_set(1, cmd_cmgf_cb);
		}
	}
    else if(param1 == 2)//AT+CNMI=,,,,,
    {
        if(param2 == 1)//AT+CNMI=2,1,0,0,0 
		{
            simcom_sms_cnmi_set(2, 1,0,0,0,cmd_cnmi_cb);
		}
		else if(param2 == 2)//AT+CNMI=2,2,0,0,0
		{
            simcom_sms_cnmi_set(2, 2,0,0,0,cmd_cnmi_cb);
		}
    }
    else if(param1 == 3)//AT+CMGD=INDEX
    {
        if(param2 == 1)//AT+CMGD=1
		{
            simcom_sms_msg_delete(1, cmd_cmgd_cb);
		}
    }
    else if(param1 == 4)//AT+CMGR=INDEX
    {//读取param2位index的短信息内容
            simcom_sms_msg_read(param2, cmd_cmgr_cb);
			
    }
    else if(param1 == 5)//TEXT MODE ----AT+CMGS="13574856585"
    {
        if(param2 == 1)
		{
            u8 number[] = "13320984483"; 
            u8 msg[] = "1234567890";
            u16 len = strlen(msg);
            simcom_sms_text_send(number, msg, len,cmd_cmgs_cb);
		}
    }
    else if(param1 == 6)//PDU MODE----AT+CMGS=19
    {
        if(param2 == 1)
		{
            u8 ptrPDU[]= "0011000D91683185959606F50018010400410042";
            u16 len = 19;
            simcom_sms_pdu_send(ptrPDU, len,cmd_cmgs_cb);
		}
    }
    else if(param1 == 7)//Contact sms receive ,at+cnmi=2,2
    {
        if(param2 == 1)
		{
            u8 ptrPDU[]= "0051000D91683123904884F30008010A0500032302014F60597D";
            u16 len = 25;
            simcom_sms_pdu_send(ptrPDU, len,cmd_cmgs_cb);
		}
        else if(param2 == 2)
		{
            u8 ptrPDU[]= "0051000D91683185959606F50008010A0500032302024F60597D";
            u16 len = 25;
            simcom_sms_pdu_send(ptrPDU, len,cmd_cmgs_cb);
		}
		
        else if(param2 == 3)
		{
            u8 ptrPDU[]= "0891683108100005F011000D91683123904884F30008B01E4F60597D53F6830267970079006D006C0030003000310032003300610062";
            u16 len = 34;
            simcom_sms_pdu_send(ptrPDU, len,cmd_cmgs_cb);
		}
    }
	else if(9== param1)
    {
        //read single message
       // u16 index = param2;
       // eat_bool ret_val = EAT_FALSE;
       // ret_val = eat_read_sms(index,eat_sms_read_cb);
        
       // eat_trace("eat_module_test_sms eat_read_sms ret_val=%d",ret_val);
    }
}


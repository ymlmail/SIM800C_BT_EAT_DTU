/*
 * =====================================================================================
 *
 *       Filename:  app_at_cmd_envelope.c
 *
 *    Description:  envelope function with at cmd implement
 *
 *        Version:  1.0
 *        Created:  2014-1-11 15:56:08
 *       Revision:  none
 *       Compiler:  armcc
 *
 *         Author:  Jumping (apple), zhangping@sim.com
 *   Organization:  www.sim.com
 *
 * =====================================================================================
 */

/* #####   HEADER FILE INCLUDES   ################################################### */
#include <string.h>
#include "eat_interface.h"
#include "app_custom.h"
#include "app_at_cmd_envelope.h"

#include "App_include.h"

/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ################################### */
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define MODEM_READ_BUFFER_LEN 2048


/* #####   TYPE DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ######################### */

typedef struct UrcEntityQueueTag
{
    UrcEntity urcEntityArray[URC_QUEUE_COUNT]; /* AT命令队列 */
    u8 count;                                 /* 当前执行的index */
}UrcEntityQueue;

typedef struct FunInfoTag
{
    u8 first;
    u8 last;
    ResultNotifyCb pCallback;
}FunInfo;
typedef struct AtCmdEntityQueueTag
{
    u8 current;                                 /* 当前执行的index */
    u8 last;                                    /* 队列中最后一条的index */
    FunInfo fun[2];	
    u8 funCount;
    AtCmdEntity atCmdEntityArray[AT_CMD_QUEUE_COUNT]; /* AT命令队列 */

}AtCmdEntityQueue;
/* #####   DATA TYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */


/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */
static AtCmdRsp AtCmdCbDefault(u8* pRspStr);
static AtCmdRsp AtCmdCb_cpin(u8* pRspStr);
static AtCmdRsp AtCmdCb_creg(u8* pRspStr);
static AtCmdRsp AtCmdCb_cgatt(u8* pRspStr);
static AtCmdRsp AtCmdCb_sapbr(u8* pRspStr);
static eat_bool simcom_atcmd_queue_head_out(void);
static eat_bool simcom_atcmd_queue_tail_out(void);
static eat_bool simcom_atcmd_queue_fun_out(void);
static eat_bool AtCmdOverTimeStart(void);
static eat_bool AtCmdOverTimeStop(void);
static void eat_timer1_handler(void);
static void eat_modem_ready_read_handler(void);
static void global_urc_handler(u8* pUrcStr, u16 len);
static eat_bool AtCmdDelayExe(u16 delay);
static AtCmdRsp AtCmdCb_ftpgettofs(u8* pRspStr);
static void UrcCb_FTPGETTOFS(u8* pRspStr, u16 len);
extern void UrcCb_NewSmsIndicator(u8* pRspStr, u16 len);
static void UrcCb_LBSIndicator(u8* pRspStr, u16 len);
eat_bool ask_creg(ResultNotifyCb pResultCb);
static AtCmdRsp AtCmdCb_creg2(u8* pRspStr);
u16 stringToHex(u8 *p, u8 len);
static AtCmdRsp AtCmdCb_csq(u8* pRspStr);
eat_bool ask_CSQ(ResultNotifyCb pResultCb);

extern void UrcCb_FlashNewSmsIndicator(u8* pRspStr, u16 len);
extern void UrcCb_sms_ready(u8* pRspStr, u16 len);
extern void UrcCb_modem_data_parse(u8* pRspStr, u16 len);
static void UrcCb_IPDIndicator(u8* pRspStr, u16 len);
static void UrcCb_CLCC(u8* pRspStr, u16 len);
static void UrcCb_NormalPowerDown(u8* pRspStr, u16 len);

void clear_CallArry(void);
/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ################################ */
static UrcEntityQueue s_urcEntityQueue=
	{
		{
			{"+CMTI:",UrcCb_NewSmsIndicator}, 
			{"+CMT:",UrcCb_FlashNewSmsIndicator},	  
			{"SMS Ready",UrcCb_sms_ready}, 
			{"param:", UrcCb_modem_data_parse},
			{"+CREG:",UrcCb_LBSIndicator},
			{"+IPD",UrcCb_IPDIndicator},
			{"+CLCC:",UrcCb_CLCC},
			{"NORMAL POWER DOWN",UrcCb_NormalPowerDown},
			//{"RING",UrcCb_Ring},
			//{"NO CARRIER",UrcCb_NOCARRIER},//+IPD
			0
		},
		8// 6
	};
/*

+CLCC: 1,1,4,0,0,"13320984483",129,""

RING

RING

RING

+CLCC: 1,1,6,0,0,"13320984483",129,""

NO CARRIER

*/
#if 0
{
    {
        {"+FTPGETTOFS: 0",UrcCb_FTPGETTOFS},
        {"+CMTI:",UrcCb_NewSmsIndicator}, 
        {"+CREG:",UrcCb_LBSIndicator}, 
        0//+CREG: 2,1,
    },
    3 // 2
};
#endif
static AtCmdEntityQueue s_atCmdEntityQueue={0};
static u8 modem_read_buf[MODEM_READ_BUFFER_LEN]; 
static u8* s_atCmdWriteAgain = NULL;
static u8 s_atExecute = 0;
static ResultNotifyCb p_gsmInitCb = NULL;
static ResultNotifyCb p_gprsBearCb = NULL;
static ResultNotifyCb p_get2fsCb = NULL;
static ResultNotifyCb p_get2fsFinalUserCb = NULL;

static ResultNotifyCb p_askLBSCb = NULL;
static ResultNotifyCb p_askCSQCb = NULL;

ResultNotifyCb p_gprsConnect = NULL;
ResultNotifyCb p_gprsSend = NULL;




static u16 s_smsIndex = 0;

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ############################ */

eat_bool simcom_gsm_init(u8* cpin,ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    AtCmdEntity atCmdInit[]={
        {"AT"AT_CMD_END,4,NULL},
        {"AT"AT_CMD_END,4,NULL},
        {AT_CMD_DELAY"2000",10,NULL},
        {"AT+CPIN?"AT_CMD_END,10,AtCmdCb_cpin},
        {"AT+CPIN=",0,NULL},
        {AT_CMD_DELAY"3000",10,NULL},
        {"AT+CREG?"AT_CMD_END,10,AtCmdCb_creg},
        {AT_CMD_DELAY"2000",10,NULL},
        {"AT+CLCC=1"AT_CMD_END,11,NULL},
        {AT_CMD_DELAY"8000",10,NULL},
        {"AT+CGATT?"AT_CMD_END,11,AtCmdCb_cgatt},
        
    };
    u8 pAtCpin[15] = {0};
    sprintf(pAtCpin,"AT+CPIN=%s%s",cpin,AT_CMD_END);
    atCmdInit[4].p_atCmdStr = pAtCpin;
    atCmdInit[4].cmdLen = strlen(pAtCpin);
    
    p_gsmInitCb = pResultCb;
    
    result = simcom_atcmd_queue_fun_append(atCmdInit,sizeof(atCmdInit) / sizeof(atCmdInit[0]),p_gsmInitCb);

    return result;
}
eat_bool ask_creg(ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    AtCmdEntity atCmdInit[]={
		{"AT+CREG=2"AT_CMD_END,11,NULL},
        {AT_CMD_DELAY"2000",10,NULL},
        {"AT+CREG?"AT_CMD_END,10,AtCmdCb_creg2},

    };
	
    p_askLBSCb= pResultCb;
    result = simcom_atcmd_queue_fun_append(atCmdInit,sizeof(atCmdInit) / sizeof(atCmdInit[0]),pResultCb);
    return result;
}
eat_bool ask_CSQ(ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    AtCmdEntity atCmdInit[]={
        {"AT"AT_CMD_END,4,NULL},
		{"AT+CSQ"AT_CMD_END,8,AtCmdCb_csq}
    }; 
	p_askCSQCb= pResultCb;
    result = simcom_atcmd_queue_fun_append(atCmdInit,sizeof(atCmdInit) / sizeof(atCmdInit[0]),pResultCb);
    return result;
}
eat_bool CALL_AT_ATH(ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    AtCmdEntity atCmdInit[]={
        {"ATH"AT_CMD_END,5,NULL}
    }; 
    result = simcom_atcmd_queue_fun_append(atCmdInit,sizeof(atCmdInit) / sizeof(atCmdInit[0]),pResultCb);
    return result;
}
eat_bool CALL_AT_ATA(ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    AtCmdEntity atCmdInit[]={
        {"ATA"AT_CMD_END,5,NULL}
    }; 
    result = simcom_atcmd_queue_fun_append(atCmdInit,sizeof(atCmdInit) / sizeof(atCmdInit[0]),pResultCb);
    return result;
}
eat_bool CALL_AT_ATD(u8 SosSN,ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    AtCmdEntity atCmdInit[]={
        {"ATD"AT_CMD_END,5,NULL}
    }; 
	/*
    u8 pAtCpin[15] = {0};
    sprintf(pAtCpin,"AT+CPIN=%s%s",SosNumber[Call.phoneNum],AT_CMD_END);
    atCmdInit[4].p_atCmdStr = pAtCpin;
    atCmdInit[4].cmdLen = strlen(pAtCpin);
    
    p_gsmInitCb = pResultCb;

    result = simcom_atcmd_queue_fun_append(atCmdInit,sizeof(atCmdInit) / sizeof(atCmdInit[0]),pResultCb);
    return result;*/
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: simcom_ftp_down_file
 *  Description: example of download file from ftp server to filesystem
 *        Input:
 			cid:
 			username: username to log on
 			pwd: password to log on
 			getName: file name on server
 			getPath: file path on server
 			serv: server address
 			port: server port
 			filePath: file to save
 			pResultCb: callback function
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
eat_bool simcom_ftp_down_file(u8 *username, u8 *pwd, 
	u8 *getName, u8 *getPath, u8 *serv, u8 port, u8 *fileName, ResultNotifyCb pResultCb, ResultNotifyCb finalCb)
{
    eat_bool result = FALSE;
    u8 i = 0;
    u8 count = 0 ;
    AtCmdEntity atCmdInit[]={
        {"AT+SAPBR=3,1,CONTYPE,GPRS"AT_CMD_END, 27, NULL}, /* 0  */
        {"AT+SAPBR=3,1,APN,", 0, NULL},                    /* 1  */
        {"AT+SAPBR=3,1,USER,", 0, NULL},                   /* 2  */
        {"AT+SAPBR=3,1,PWD,", 0, NULL},                    /* 3  */
        {"AT+SAPBR=2,1"AT_CMD_END, 14, AtCmdCb_sapbr},
        {"AT+SAPBR=1,1"AT_CMD_END, 14, NULL},
        {"AT+FTPCID=1"AT_CMD_END, 13, NULL},
        {"AT+FTPUN=", 0, NULL},                            /* 7  */
        {"AT+FTPPW=", 0, NULL},                            /* 8  */
        {"AT+FTPGETNAME=", 0, NULL},                       /* 9  */
        {"AT+FTPGETPATH=", 0, NULL},                       /* 10*/
        {"AT+FTPSERV=", 0, NULL},                          /* 11 */
        {"AT+FTPPORT=", 0, NULL},                          /* 12 */
        {"AT+FTPTIMEOUT=3", 0, NULL},                      /* 13 */
        {"AT+FTPGETTOFS=0,", 0, AtCmdCb_ftpgettofs}        /* 14 */
    };

    /* set at cmd func's range,must first set */
    count = sizeof(atCmdInit) / sizeof(atCmdInit[0]);
    result = simcom_atcmd_queue_fun_set(count, AtCmdCb_ftpgettofs);

    if(!result)                                 /* the space of queue is poor */
        return FALSE;

    p_get2fsCb = pResultCb;
    p_get2fsFinalUserCb = finalCb;

    for (i=0 ;i< count ;i++ ){

        u8 pAtCmd[50] = {0};
//        memset(pAtCmd,0,sizeof(pAtCmd));
        switch ( i ) {
            case 1:	
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,"CMNET",AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 2:	
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,"\"\"",AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 3:	
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,"\"\"",AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 7:	
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,username,AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 8:	
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,pwd,AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 9:	
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,getName,AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 10:	
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,getPath,AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 11:	
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,serv,AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 12:	
                sprintf(pAtCmd,"%s%d%s",atCmdInit[i].p_atCmdStr,port,AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;
            case 13:	
                sprintf(pAtCmd,"%s%s",atCmdInit[i].p_atCmdStr,AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;
            case 14:	
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,fileName,AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            default:	
                break;
        }				/* -----  end switch  ----- */
        simcom_atcmd_queue_append(atCmdInit[i]);
    }
    return TRUE;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: simcom_atcmd_queue_init
 *  Description: initial at cmd queue 
 *        Input:
 *       Output:
 *       Return:
 *               void::
 *       author: Jumping create at 2014-1-13
 * =====================================================================================
 */
void simcom_atcmd_queue_init(void)
{
    u8 i = 0;
    u8 first = MIN(s_atCmdEntityQueue.current,s_atCmdEntityQueue.fun[0].first);
    for ( i=first; i<=s_atCmdEntityQueue.last; i++) {
        AtCmdEntity* atCmdEnt = NULL;
        atCmdEnt = &(s_atCmdEntityQueue.atCmdEntityArray[i]);
        if(atCmdEnt->p_atCmdStr){
            eat_mem_free(atCmdEnt->p_atCmdStr);
            atCmdEnt->p_atCmdStr = NULL;
        }
        atCmdEnt->p_atCmdCallBack = NULL;
    }
    memset(&s_atCmdEntityQueue,0,sizeof(AtCmdEntityQueue));
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: simcom_urc_queue_append
 *  Description: add global urc deal with call back function 
 *        Input:
 *               urcEntity::
 *       Output:
 *       Return:
 *               eat_bool::
 *       author: Jumping create at 2014-1-14
 * =====================================================================================
 */
eat_bool simcom_urc_queue_append(UrcEntity urcEntity)
{

    if (s_urcEntityQueue.count == URC_QUEUE_COUNT)
        return FALSE;
    s_urcEntityQueue.urcEntityArray[s_urcEntityQueue.count] = urcEntity;
	eat_trace("simcom_urc_queue_append: str = %s, fun = %d", 
		s_urcEntityQueue.urcEntityArray[s_urcEntityQueue.count].p_urcStr,
		s_urcEntityQueue.urcEntityArray[s_urcEntityQueue.count].p_urcCallBack);
    s_urcEntityQueue.count ++;
	eat_trace("simcom_urc_queue_append: count = %d", s_urcEntityQueue.count);
    return TRUE;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: simcom_urc_queue_remove
 *  Description: remove global urc deal with call back function 
 *        Input:
 *               urcEntity::
 *       Output:
 *       Return:
 *               eat_bool::
 *       author: hanjun.liu create at 2014-03-03
 * =====================================================================================
 */
eat_bool simcom_urc_queue_remove(UrcEntity urcEntity)
{
	u8 i = 0;
	u8 index = 0xFF;

	eat_trace("simcom_urc_queue_remove begin: count = %d", s_urcEntityQueue.count);
	for(i = 0; i < s_urcEntityQueue.count; i++)
	{
		if(strcmp(s_urcEntityQueue.urcEntityArray[i].p_urcStr, urcEntity.p_urcStr) == 0
			&& s_urcEntityQueue.urcEntityArray[i].p_urcCallBack == urcEntity.p_urcCallBack)
		{
			index = i;
			break;
		}
	}
	eat_trace("simcom_urc_queue_remove: index = %d", index);
	if(index == 0xFF)
	{
		return FALSE;
	}
	for(i = index; i < s_urcEntityQueue.count; i++)
	{
		if(i < URC_QUEUE_COUNT - 1)
		{
			s_urcEntityQueue.urcEntityArray[i] = s_urcEntityQueue.urcEntityArray[i + 1];
		}
		else
		{
			s_urcEntityQueue.urcEntityArray[i].p_urcStr = NULL;
			s_urcEntityQueue.urcEntityArray[i].p_urcCallBack = NULL;
		}
	}
    s_urcEntityQueue.count--;
	eat_trace("simcom_urc_queue_remove end: count = %d", s_urcEntityQueue.count);
	
    return TRUE;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: simcom_atcmd_queue_append
 *  Description: append a entity at the last of at cmd queue 
 *        Input:
 *               atCmdEntity:: at cmd entity
 *       Output:
 *       Return:
 *               FALSE: some error is ocur
 *               TRUE: append success
 *       author: Jumping create at 2014-1-13
 * =====================================================================================
 */
eat_bool simcom_atcmd_queue_append(AtCmdEntity atCmdEntity)
{
                                                /* get first index */
    u8 first =  MAX(s_atCmdEntityQueue.current,s_atCmdEntityQueue.fun[0].first);

    if (atCmdEntity.p_atCmdStr == NULL){
        eat_trace("ERROR: at cmd str is null!");
        return FALSE;
    }

    if (atCmdEntity.p_atCmdCallBack == NULL)    /* set default callback function */
        atCmdEntity.p_atCmdCallBack = AtCmdCbDefault;

    if((s_atCmdEntityQueue.last + 1) % AT_CMD_QUEUE_COUNT == first){
        eat_trace("ERROR: at cmd queue is full!");
        return FALSE;                           /* the queue is full */
    }
    else{
        u8* pAtCmd = NULL; ;//= eat_mem_alloc(atCmdEntity.cmdLen);

        pAtCmd = eat_mem_alloc(atCmdEntity.cmdLen);
		
			//eat_trace("atCmdEntity.cmdLen=%d",atCmdEntity.cmdLen);

        if (!pAtCmd){
            eat_trace("ERROR: memory alloc error!");
            return FALSE;
        }

        memcpy(pAtCmd,atCmdEntity.p_atCmdStr,atCmdEntity.cmdLen);
        s_atCmdEntityQueue.atCmdEntityArray[s_atCmdEntityQueue.last].cmdLen = atCmdEntity.cmdLen;
        s_atCmdEntityQueue.atCmdEntityArray[s_atCmdEntityQueue.last].p_atCmdStr = pAtCmd;
        s_atCmdEntityQueue.atCmdEntityArray[s_atCmdEntityQueue.last].p_atCmdCallBack = atCmdEntity.p_atCmdCallBack;

        if(simcom_atcmd_queue_is_empty() && s_atExecute == 0) /* add first at cmd and execute it */
            eat_send_msg_to_user(0, EAT_USER_1, EAT_FALSE, 1, 0, NULL);

        s_atCmdEntityQueue.last = (s_atCmdEntityQueue.last + 1) %  AT_CMD_QUEUE_COUNT;
        return TRUE;
    }
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: simcom_atcmd_queue_is_empty
 *  Description: judge that the queue is empty 
 *        Input:
 *       Output:
 *       Return:
 *               eat_bool::
 *       author: Jumping create at 2014-1-13
 * =====================================================================================
 */
eat_bool simcom_atcmd_queue_is_empty()
{
    return (s_atCmdEntityQueue.current == s_atCmdEntityQueue.last);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: simcom_atcmd_queue_fun_set
 *  Description: set at cmd func's range,from current id 
 *        Input:
 *               count:: the count of this at cmd func
 *       Output:
 *       Return:
 *               eat_bool::
 *       author: Jumping create at 2014-1-17
 * =====================================================================================
 */
eat_bool simcom_atcmd_queue_fun_set(u8 funCount,ResultNotifyCb callback)
{
    u8 i = 0;
    u8 first =  MAX(s_atCmdEntityQueue.current,s_atCmdEntityQueue.fun[0].first);
    u8 freeCount = 0;
    if (s_atCmdEntityQueue.funCount==2) {
//    if (s_atCmdEntityQueue.funLast != s_atCmdEntityQueue.funFirst){
        eat_trace("ERROR: fun is exist!");
        return FALSE;                           /* just one func exist */
    }

    if(simcom_atcmd_queue_is_empty()){
        freeCount = AT_CMD_QUEUE_COUNT;
    }
    else {
        freeCount = (first - s_atCmdEntityQueue.last + AT_CMD_QUEUE_COUNT) % AT_CMD_QUEUE_COUNT;
    }
    if(funCount > freeCount) {
        eat_trace("ERROR: the queue is full! %d,%d,%d",first,funCount,freeCount);
        return FALSE;                           /* the space is poor */
    }

    s_atCmdEntityQueue.funCount ++;
    s_atCmdEntityQueue.fun[s_atCmdEntityQueue.funCount-1].first = s_atCmdEntityQueue.last;
    s_atCmdEntityQueue.fun[s_atCmdEntityQueue.funCount-1].last = (s_atCmdEntityQueue.last + funCount - 1 + AT_CMD_QUEUE_COUNT) % AT_CMD_QUEUE_COUNT;
    s_atCmdEntityQueue.fun[s_atCmdEntityQueue.funCount-1].pCallback = callback;
    eat_trace("INFO: the at cmd func[%d]'s range is %d-%d", s_atCmdEntityQueue.funCount - 1, 
        s_atCmdEntityQueue.fun[s_atCmdEntityQueue.funCount-1].first, s_atCmdEntityQueue.fun[s_atCmdEntityQueue.funCount-1].last);

    return TRUE;
}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name: simcom_atcmd_queue_fun_append
 *  Description: add a group of at cmd to queue 
 *        Input:
 *               atCmdEntity[]:: at cmd group, can implemnt a function
 *       Output:
 *       Return:
 *               eat_bool::
 *       author: Jumping create at 2014-1-13
 * =====================================================================================
 */
eat_bool simcom_atcmd_queue_fun_append(AtCmdEntity atCmdEntity[],u8 funCount,ResultNotifyCb callback)
{
    u8 i = 0;
    eat_bool ret = FALSE;
    
    ret = simcom_atcmd_queue_fun_set(funCount,callback);
    if(!ret)
        return FALSE;

    for (i = 0; i < funCount; i++) {
        eat_bool ret = FALSE;
        ret = simcom_atcmd_queue_append(atCmdEntity[i]);
        if (!ret){
            break;
        }
    }

    if(i != funCount){                           /* error is ocur */
        for ( i=funCount; i > 0; i--) {
            simcom_atcmd_queue_tail_out();
        }
        eat_trace("ERROR: add to queue is error!,%d",funCount);
        return FALSE;
    }

   // eat_trace("INFO: funFirst:%d  funLast:%d", s_atCmdEntityQueue.fun[s_atCmdEntityQueue.funCount-1].first, s_atCmdEntityQueue.fun[s_atCmdEntityQueue.funCount-1].last);
    return TRUE;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name: app_at_cmd_envelope
 *  Description: at cmd deal with thread 
 *        Input:
 *               data::
 *       Output:
 *       Return:
 *               void::
 *       author: Jumping create at 2014-1-13
 * =====================================================================================
 */
void SendAtComHandle(void);
void app_at_cmd_envelope(void *data)
{
u32 num;//,num1;

EatEvent_st event;
while(EAT_TRUE)
	{
		eat_sleep(5);
	//eat_trace("-----INFO: yyy app_at_cmd_envelope----\n");
	//eat_trace("-----app_user1111---\n");
	//eat_sleep(50);
#if 0
		num = eat_get_event_num_for_user(EAT_USER_1);
		if(num>0)
#endif
		{
		eat_get_event_for_user(EAT_USER_1, &event);
#if DebugOn
		eat_trace("EAT_USER_1 MSG id(0x%x)", event.event);
#endif
#if 1
		switch(event.event)
			{
			case EAT_EVENT_TIMER :

			switch ( event.data.timer.timer_id ) 
				{
				case EAT_TIMER_1:           /* dealy to excute at cmd */
				//  eat_trace("INFO: EAT_TIMER_1 expire!");
				eat_timer1_handler();
				break;

				case EAT_TIMER_2:           /* at cmd time over */
				//eat_trace("FATAL: at cmd rsp is overtime");
				simcom_atcmd_queue_init();
				AtCmdOverTimeStop();
				if(s_atCmdEntityQueue.fun[0].pCallback)
				{
				s_atCmdEntityQueue.fun[0].pCallback(EAT_FALSE);
				s_atCmdEntityQueue.fun[0].pCallback= NULL;
				}
				eat_trace("FATAL: at cmd rsp is overtime");

				break;

				default:	
				break;
				}				/* -----  end switch  ----- */
				break;

			case EAT_EVENT_MDM_READY_RD:
				eat_modem_ready_read_handler();
				break;

			case EAT_EVENT_MDM_READY_WR:
				if (s_atCmdWriteAgain){
				u16 lenAct = 0;
				u16 len = strlen(s_atCmdWriteAgain);
				lenAct = eat_modem_write(s_atCmdWriteAgain,len);
				eat_mem_free(s_atCmdWriteAgain);
				if(lenAct<len){
				eat_trace("FATAL: modem write buffer is overflow!");
				}

				}
				break;
			case EAT_EVENT_USER_MSG:
				AtCmdDelayExe(0);
				break;
			default:
				break;
			}
#endif
		}
	}
}

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ##################### */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: AtCmdCbDefault
 *  Description: default function to deal with at cmd RSP code 
 *        Input:
 *               pRspStr::
 *       Output:
 *       Return:
 *               AtCmdRsp::
 *       author: Jumping create at 2014-1-13
 * =====================================================================================
 */
static AtCmdRsp AtCmdCbDefault(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"OK","ERROR"};
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

        p = (u8*)strchr(p,0x0a);
    }

    switch (rspType)
    {
        case 0:  /* OK */
        rspValue  = AT_RSP_CONTINUE;
        break;

        case 1:  /* ERROR */
        rspValue = AT_RSP_ERROR;
		if(s_atCmdEntityQueue.fun[0].pCallback)

		{
			s_atCmdEntityQueue.fun[0].pCallback(EAT_FALSE);
			s_atCmdEntityQueue.fun[0].pCallback = NULL;
		}
        break;

        default:
        rspValue = AT_RSP_WAIT;
        break;
    }

    return rspValue;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: AtCmdDelayExe
 *  Description: dealy 10ms to execute at cmd 
 *        Input:
 *       Output:
 *       Return:
 *               eat_bool::
 *       author: Jumping create at 2014-1-13
 * =====================================================================================
 */
static eat_bool AtCmdDelayExe(u16 delay)
{
    eat_bool ret = FALSE;
    
//    eat_trace("yymm2015-111INFO: at cmd delay execute,%d",delay);
    if (delay == 0){
        delay = AT_CMD_EXECUTE_DELAY;
    }
    ret = eat_timer_start(EAT_TIMER_1,delay);
    if (ret)
        s_atExecute = 1;
    
    return ret;
}

static eat_bool AtCmdOverTimeStart(void)
{
    eat_bool ret = FALSE;
    ret = eat_timer_start(EAT_TIMER_2,AT_CMD_EXECUTE_OVERTIME);
    return ret;
}

static eat_bool AtCmdOverTimeStop(void)
{
    eat_bool ret = FALSE;
    ret = eat_timer_stop(EAT_TIMER_2);
    return ret;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: simcom_atcmd_queue_head_out
 *  Description: delete the head of the queue 
 *        Input:
 *       Output:
 *       Return:
 *               eat_bool::
 *       author: Jumping create at 2014-1-13
 * =====================================================================================
 */
static eat_bool simcom_atcmd_queue_head_out(void)
{
    AtCmdEntity* atCmdEnt = NULL;

   if(s_atCmdEntityQueue.current == s_atCmdEntityQueue.last) /* the queue is empty */
       return FALSE;

   if(s_atCmdEntityQueue.current>=s_atCmdEntityQueue.fun[0].last
           || s_atCmdEntityQueue.current<=s_atCmdEntityQueue.fun[0].first){
       atCmdEnt = &(s_atCmdEntityQueue.atCmdEntityArray[s_atCmdEntityQueue.current]);
       if(atCmdEnt->p_atCmdStr){
           eat_mem_free(atCmdEnt->p_atCmdStr);
           atCmdEnt->p_atCmdStr = NULL;
       }
       atCmdEnt->p_atCmdCallBack = NULL;
   }
   s_atCmdEntityQueue.current = (s_atCmdEntityQueue.current + 1) %  AT_CMD_QUEUE_COUNT;
   return TRUE;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: simcom_atcmd_queue_tail_out
 *  Description: delete the tail of the queue 
 *        Input:
 *       Output:
 *       Return:
 *               eat_bool::
 *       author: Jumping create at 2014-1-13
 * =====================================================================================
 */
static eat_bool simcom_atcmd_queue_tail_out(void)
{
    AtCmdEntity* atCmdEnt = NULL;
    
    if(s_atCmdEntityQueue.current == s_atCmdEntityQueue.last)
        return FALSE;
    
    s_atCmdEntityQueue.last = (s_atCmdEntityQueue.last + AT_CMD_QUEUE_COUNT - 1) % AT_CMD_QUEUE_COUNT;
    atCmdEnt = &(s_atCmdEntityQueue.atCmdEntityArray[s_atCmdEntityQueue.last]);
    if(atCmdEnt->p_atCmdStr){
        eat_mem_free(atCmdEnt->p_atCmdStr);
        atCmdEnt->p_atCmdStr = NULL;
    }
    atCmdEnt->p_atCmdCallBack = NULL;
    
    return TRUE;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: simcom_atcmd_queue_fun_out
 *  Description: delete a group of at cmd from queue 
 *        Input:
 *       Output:
 *       Return:
 *               eat_bool::
 *       author: Jumping create at 2014-1-13
 * =====================================================================================
 */
static eat_bool simcom_atcmd_queue_fun_out(void)
{
    u8 i = 0;
    if (s_atCmdEntityQueue.funCount == 0)        /* no at cmd func */
        return FALSE;

    if (s_atCmdEntityQueue.fun[0].last < s_atCmdEntityQueue.fun[0].first)
    {
       for (i = s_atCmdEntityQueue.fun[0].first ;i < AT_CMD_QUEUE_COUNT; i++ )
       {
           AtCmdEntity* atCmdEnt = NULL;
           atCmdEnt = &(s_atCmdEntityQueue.atCmdEntityArray[i]);
           if(atCmdEnt->p_atCmdStr){
               eat_mem_free(atCmdEnt->p_atCmdStr);
               atCmdEnt->p_atCmdStr = NULL;
           }
           atCmdEnt->p_atCmdCallBack = NULL;
       }

        for (i = 0 ;i <= s_atCmdEntityQueue.fun[0].last; i++ )
        {
            AtCmdEntity* atCmdEnt = NULL;
            atCmdEnt = &(s_atCmdEntityQueue.atCmdEntityArray[i]);
            if(atCmdEnt->p_atCmdStr){
                eat_mem_free(atCmdEnt->p_atCmdStr);
                atCmdEnt->p_atCmdStr = NULL;
            }
            atCmdEnt->p_atCmdCallBack = NULL;
        }

    }
    else
    {
        for ( i=s_atCmdEntityQueue.fun[0].first ;i<=s_atCmdEntityQueue.fun[0].last ; i++) {
            AtCmdEntity* atCmdEnt = NULL;
            atCmdEnt = &(s_atCmdEntityQueue.atCmdEntityArray[i]);
            if(atCmdEnt->p_atCmdStr){
                eat_mem_free(atCmdEnt->p_atCmdStr);
                atCmdEnt->p_atCmdStr = NULL;
            }
            atCmdEnt->p_atCmdCallBack = NULL;
        }
    }
    
    if(s_atCmdEntityQueue.current != s_atCmdEntityQueue.fun[0].last){
        eat_trace("WARNING: at cmd func over did not at last!");
        s_atCmdEntityQueue.current = s_atCmdEntityQueue.fun[0].last;
    }

    s_atCmdEntityQueue.current = (s_atCmdEntityQueue.current + 1) % AT_CMD_QUEUE_COUNT;
    if(s_atCmdEntityQueue.funCount==2){
        s_atCmdEntityQueue.fun[0].last = s_atCmdEntityQueue.fun[1].last;
        s_atCmdEntityQueue.fun[0].first = s_atCmdEntityQueue.fun[1].first;
        s_atCmdEntityQueue.fun[0].pCallback = s_atCmdEntityQueue.fun[1].pCallback;
        s_atCmdEntityQueue.fun[1].last = 0;
        s_atCmdEntityQueue.fun[1].first = 0;
        s_atCmdEntityQueue.fun[1].pCallback = NULL;
    }
    else {
        s_atCmdEntityQueue.fun[0].last = 0;
        s_atCmdEntityQueue.fun[0].first = 0;
    }
    s_atCmdEntityQueue.funCount--;
    return TRUE;

}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_timer1_handler
 *  Description: 1 timer timeout handler
 *        Input:
 *       Output:
 *       Return:
 *               void::
 *       author: Jumping create at 2014-1-14
 * =====================================================================================
 */
static void eat_timer1_handler(void)
{
    u8* pCmd = s_atCmdEntityQueue.atCmdEntityArray[s_atCmdEntityQueue.current].p_atCmdStr;
    u16 len = s_atCmdEntityQueue.atCmdEntityArray[s_atCmdEntityQueue.current].cmdLen;
    s_atExecute = 0;

    if(pCmd){
        u16 lenAct = 0;
       // eat_trace("DEBUG:at cmd is:%d,%s",s_atCmdEntityQueue.current,pCmd);
        if (!strncmp(AT_CMD_DELAY, pCmd, strlen(AT_CMD_DELAY))) {  /* delay some seconds to run next cmd */
            u32 delay;
            sscanf(pCmd,AT_CMD_DELAY"%d",&delay);
            simcom_atcmd_queue_head_out();
            AtCmdDelayExe(delay);
            return;
        }
        lenAct = eat_modem_write(pCmd,len);
        if(lenAct<len){
            eat_trace("ERROR: modem write buffer is overflow!");

            if(s_atCmdWriteAgain == NULL){
                s_atCmdWriteAgain = eat_mem_alloc(len-lenAct);
                if (s_atCmdWriteAgain)
                    memcpy(s_atCmdWriteAgain,pCmd+lenAct,len-lenAct);
                else
                    eat_trace("ERROR: mem alloc error!");
            }
            else 
                eat_trace("FATAL: EAT_EVENT_MDM_READY_WR may be lost!");
        }
        else{
            eat_trace("WrToModem:%s",pCmd);
            AtCmdOverTimeStart();
        }

    }
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: global_urc_handler
 *  Description: global urc check 
 *        Input:
 *               pUrcStr::
 *               len::
 *       Output:
 *       Return:
 *               void::
 *       author: Jumping create at 2014-1-14
 * =====================================================================================
 */
static void global_urc_handler(u8* pUrcStr, u16 len)
{
    u8  i = 0;
    u8  *p = pUrcStr;
    while(p) {

        /* ignore \r \n */
        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }
		eat_trace("++++global_urc_handler+++ %s\r\n",p);

        for (i = 0; i < s_urcEntityQueue.count; i++)
        {	
            if (0==strncmp(s_urcEntityQueue.urcEntityArray[i].p_urcStr, p, strlen(s_urcEntityQueue.urcEntityArray[i].p_urcStr))) {
                if(s_urcEntityQueue.urcEntityArray[i].p_urcCallBack)
                	{
                    s_urcEntityQueue.urcEntityArray[i].p_urcCallBack(p,len);
					//TRACE_DEBUG("(s_urcEntityQueue.urcEntityArray[i].p_urcStr)=",(s_urcEntityQueue.urcEntityArray[i].p_urcStr));
                	}
            }
        }
		p = (u8*)strchr(p,0x0a);
       // p = (u8*)memchr(p,0x0a,0);
    }

}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_modem_ready_read_handler
 *  Description: read data handler from modem 
 *        Input:
 *       Output:
 *       Return:
 *               void::
 *       author: Jumping create at 2014-1-14
 * =====================================================================================
 */
 
extern eat_bool eat_modem_data_parse(u8* buffer, u16 len, u8* param1, u8* param2);

static void eat_modem_ready_read_handler(void)
{
    u16 len = eat_modem_read(modem_read_buf,MODEM_READ_BUFFER_LEN);
    if (len > 0) {
        AtCmdRsp atCmdRsp = AT_RSP_ERROR;
        memset(modem_read_buf+len,0,MODEM_READ_BUFFER_LEN-len);//把len后面的空间清零
                                                /* global urc check */
        global_urc_handler(modem_read_buf,len);
                                                /* at cmd rsp check */
        eat_trace("ReFrModem:\r\n%slen=%d",modem_read_buf,len);
        if (s_atCmdEntityQueue.atCmdEntityArray[s_atCmdEntityQueue.current].p_atCmdCallBack){
            atCmdRsp = s_atCmdEntityQueue.atCmdEntityArray[s_atCmdEntityQueue.current].p_atCmdCallBack(modem_read_buf);

            eat_trace("INFO: callback return %d,%d",s_atCmdEntityQueue.current,atCmdRsp);
            switch ( atCmdRsp ) {
                case AT_RSP_ERROR:
                    eat_trace("ERROR: at cmd execute error, initial at cmd queue!");
                    simcom_atcmd_queue_init();
                    AtCmdOverTimeStop();
                    break;
                case AT_RSP_CONTINUE:	
                case AT_RSP_FINISH:	
                    simcom_atcmd_queue_head_out();
                    AtCmdDelayExe(0);
                    AtCmdOverTimeStop();
                    break;

                case AT_RSP_FUN_OVER:
					eat_trace("delect a at function\n");
                    simcom_atcmd_queue_fun_out();
                    AtCmdDelayExe(0);
                    AtCmdOverTimeStop();
                    break;

                case AT_RSP_WAIT:
                    break;

                default:	
                    if(atCmdRsp>=AT_RSP_STEP_MIN && atCmdRsp<=AT_RSP_STEP_MAX) {
                        s8 step = s_atCmdEntityQueue.current + atCmdRsp - AT_RSP_STEP;
                       // eat_trace("DEBUG: cur step %d, to step %d",s_atCmdEntityQueue.current,step);
                        if(step<=s_atCmdEntityQueue.fun[0].last && step>= s_atCmdEntityQueue.fun[0].first){
                            s_atCmdEntityQueue.current = step;
                            AtCmdDelayExe(0);
                            AtCmdOverTimeStop();
                        }
                        else{
                            eat_trace("ERROR: return of AtCmdRsp is error!");
                        }
                    }
                    break;
            }				/* -----  end switch  ----- */
        }
    }
}

static AtCmdRsp AtCmdCb_cpin(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR","+CPIN: READY","+CPIN: SIM PIN","+CPIN: "};
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

        p = (u8*)strchr(p,0x0a);
    }

    switch (rspType)
    {

        case 0:  /* ERROR */
        rspValue = AT_RSP_ERROR;
        break;

        case 1:  /* READY */
        rspValue  = AT_RSP_STEP+3;
        break;

        case 2:  /* need sim pin */
        rspValue  = AT_RSP_STEP+1;
        break;

        default:
        break;
    }

    if(rspValue == AT_RSP_ERROR){
        eat_trace("at+cpin? return AT_RSP_ERROR");
		if(p_gsmInitCb) {
			p_gsmInitCb(EAT_FALSE);
			p_gsmInitCb = NULL;
		}
    }
    
    return rspValue;
}

static AtCmdRsp AtCmdCb_creg(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR","+CREG: "};
    s16  rspType = -1;
    u8  n = 0;
    u8  stat = 0;
    u8  i = 0;
    u8  *p = pRspStr;
    static u32 count = 0;

	u16  LAC = 0,CI = 0;
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
                if (rspType == 1){
                    sscanf(p+strlen(rspStrTable[rspType]),"%d,%d",&n,&stat);
					if(1==stat)
						{
						eat_trace("AtCmdCb_creg\n");
						p = (u8*)strchr(p,'"');
						p = p + 1;
						LAC=stringToHex(p,4);
						p = (u8*)strchr(p,',');
						p = p + 2;
						CI=stringToHex(p,4);
						LocationAreaCode.LAC=LAC;
						CellID.CI=CI;
						eat_trace("YML: CI(0x%x%x%x%x)",CellID.pkg.CI0,CellID.pkg.CI1,CellID.pkg.CI2,CellID.pkg.CI3);
						eat_trace("YML: LAC(0x%x%x)",LocationAreaCode.pkg.LAC0,LocationAreaCode.pkg.LAC1);
						}
                }
                break;
            }
        }

        p = (u8*)strchr(p,0x0a);
    }

    switch (rspType)
    {

        case 0:  /* ERROR */
        rspValue = AT_RSP_ERROR;
        break;

        case 1:  /* +CREG */
        if(1 == stat){                          /* registered */
            rspValue  = AT_RSP_STEP+2;
        }
        else if(2 == stat){                     /* searching */
            if (count == 3){
                eat_trace("at+creg? return AT_RSP_ERROR");
                rspValue = AT_RSP_ERROR;
            }
            else {
                rspValue  = AT_RSP_STEP-1;
                count++;
            }
        }
        else
            rspValue = AT_RSP_ERROR;
        break;

        default:
        break;
    }

    eat_trace("at+creg? return %d",rspValue);
    if(rspValue == AT_RSP_ERROR){
		if(p_gsmInitCb) {
			p_gsmInitCb(EAT_FALSE);
			p_gsmInitCb = NULL;
		}
    }

    return rspValue;
}
u16 stringToHex(u8 *p, u8 len)
{
u8 i=0;
u16 value=0x0000;
u16 temp=0x0000;
for(i=0;i<len;i++)
{	temp=*(p+i);
	if(temp<=0x39&&temp>=0x30)
		{
		temp-=0x30;
		}
	else if(temp<=0x46&&temp>=0x41)
		{
		switch(temp)
			{
			case 0x41:
				temp=0x0a;
				break;
			case 0x42:
				temp=0x0b;
				break;			
			case 0x43:
				temp=0x0c;
			break;
			case 0x44:
				temp=0x0d;
				break;
			case 0x45:
				temp=0x0e;
				break;
			case 0x46:
				temp=0x0f;
				break;
			default:
				break;
			}
		}
	else
		{
		eat_trace("stringToHex ++break+++\n",value);
		return value;
		}
	value=(value<<4)|temp;
	//value+=temp<<(i*4);
	
	//eat_trace("value=0x%x\n",value);

}
eat_trace("value=0x%x\n",value);

return value;

}
static AtCmdRsp AtCmdCb_creg2(u8* pRspStr)
{
	AtCmdRsp  rspValue = AT_RSP_WAIT;
	u8 *rspStrTable[ ] = {"ERROR","+CREG: "};
	s16  rspType = -1;
	u8	n = 0;
	u8	stat = 0;
	u8	i = 0;
	u8	*p = pRspStr;
	static u8 count = 0;

	u16  LAC = 0,CI = 0;
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
				if (rspType == 1){
					sscanf(p+strlen(rspStrTable[rspType]),"%d,%d",&n,&stat);
					if(1==stat)
						{
						eat_trace("AtCmdCb_creg\n");
						p = (u8*)strchr(p,'"');
						p = p + 1;
						LAC=stringToHex(p,4);
						p = (u8*)strchr(p,',');
						p = p + 2;
						CI=stringToHex(p,4);
						LocationAreaCode.LAC=LAC;
						CellID.CI=CI;
						eat_trace("YML: CI(0x%x%x%x%x)",CellID.pkg.CI0,CellID.pkg.CI1,CellID.pkg.CI2,CellID.pkg.CI3);
						eat_trace("YML: LAC(0x%x%x)",LocationAreaCode.pkg.LAC0,LocationAreaCode.pkg.LAC1);
						}
				}
				break;
			}
		}

		p = (u8*)strchr(p,0x0a);
	}

	switch (rspType)
	{

		case 0:  /* ERROR */
		rspValue = AT_RSP_ERROR;
		break;

		case 1:  /* +CREG */
		if(1 == stat){							/* registered */
			rspValue  = AT_RSP_FUN_OVER;//AT_RSP_STEP+2;
		}
		else if(2 == stat){ 					/* searching */
			if (count == 3){
				eat_trace("at+creg? return AT_RSP_ERROR");
				rspValue = AT_RSP_ERROR;
			}
			else {
				rspValue  = AT_RSP_STEP-1;
				count++;
			}
		}
		else
			rspValue = AT_RSP_ERROR;
		break;

		default:
		break;
	}

	eat_trace("at+creg? return %d",rspValue);
	if(rspValue == AT_RSP_ERROR){
		if(p_askLBSCb) {
			p_askLBSCb(EAT_FALSE);
			p_askLBSCb = NULL;
		}
	}
	else if(rspValue == AT_RSP_FUN_OVER)
	{
		if(p_askLBSCb) {
			p_askLBSCb(EAT_TRUE);
			p_askLBSCb = NULL;
		}
	}

	return rspValue;
}

static AtCmdRsp AtCmdCb_sapbr(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR","+SAPBR: "};
    s16  rspType = -1;
    u8  i = 0;
    u8  cid = 0;
    u8  status = 0;
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
                if (rspType == 1){
                    sscanf(p+strlen(rspStrTable[rspType]),"%d,%d",&cid,&status);
                }
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
        if(status == 1){                        /* connected */
            rspValue  = AT_RSP_STEP+2;
        }
        else if(status == 3)                       /* closed */
            rspValue  = AT_RSP_CONTINUE;
        else
            rspValue = AT_RSP_ERROR;
        break;

        default:
        rspValue = AT_RSP_WAIT;
        break;
    }

    return rspValue;
}

static AtCmdRsp AtCmdCb_cgatt(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR","+CGATT: 1","+CGATT: 0"};
    s16  rspType = -1;
    u8  i = 0;
    u8  *p = pRspStr;
    static count = 0;
    while(p) {
        /* ignore \r \n */
        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }

        for (i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if (!strncmp(rspStrTable[i], p, strlen(rspStrTable[i])))//将P指向的命令，与数组rep邋StrTable进行比较
            {
                rspType = i;//返回比较后 结果，与那个相同就返回那个index
                break;
            }
        }

        p = (u8*)strchr(p,AT_CMD_LF);
    }

    switch (rspType)//对比较后的结果进行解析，有三种情况0,1,2
    {

        case 0:  /* ERROR */
        rspValue = AT_RSP_ERROR;
        break;

        case 1:  /* attached */
        rspValue  = AT_RSP_FUN_OVER;
        eat_trace("at+cgatt? return AT_RSP_OK");
        if(p_gsmInitCb) {
            p_gsmInitCb(TRUE);
			p_gsmInitCb = NULL;
        }
        break;

        case 2:  /* detached */
        if (count == 5){
            eat_trace("at+cgatt? return AT_RSP_ERROR");
            rspValue = AT_RSP_ERROR;
            if(p_gsmInitCb){
                p_gsmInitCb(FALSE);
                p_gsmInitCb = NULL;
            }
        }
        else
            rspValue  = AT_RSP_STEP-1;

        count ++;
        break;

        default:
        rspValue = AT_RSP_WAIT;
        break;
    }

    return rspValue;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: simcom_phb_search_ind_hdlr
 *  Description: callback function to get result after run AT+FTPGETTOFS
 *        Input:
 			pRspStr: string to be searched
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static AtCmdRsp AtCmdCb_ftpgettofs(u8* pRspStr)
{
    AtCmdRsp rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR", "OK"};
    s16 rspType = -1;
    u8 i = 0;
    u8 *p = pRspStr;
    while(p) 
	{
        while('\x0d' == *p || '\x0a' == *p)
        {
            p++;
        }
        for(i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if(strncmp(rspStrTable[i], p, strlen(rspStrTable[i])) == 0)
            {
                rspType = i;
                break;
            }
        }
        p = (u8*)strchr(p,0x0a);
    }
    switch (rspType)
    {
        case 0:  /* ERROR */
		{
	        rspValue = AT_RSP_ERROR;
			if(p_get2fsCb ) {
				p_get2fsCb(EAT_FALSE);
				p_get2fsCb = NULL;
			}
	        break;
        }
        case 1:  /* OK */
		{
	        rspValue = AT_RSP_FUN_OVER;
			if(p_get2fsCb ) {
				p_get2fsCb(EAT_TRUE);
				p_get2fsCb = NULL;
			}
			break;
        }
        default:
		{
	        rspValue = AT_RSP_WAIT;
	        break;
        }
    }
    return rspValue;
}


static void UrcCb_FTPGETTOFS(u8* pRspStr, u16 len)
{
	eat_bool result = EAT_FALSE;
	eat_bool find = EAT_FALSE;
    u8 rspStrTable[] = {"+FTPGETTOFS: "};
    u8  stat = 0;
    u8  *p = pRspStr;

	if(p_get2fsFinalUserCb == NULL)
	{
		return;
	}
    while(p) 
	{
        /* ignore \r \n */
        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }

        if(!strncmp(rspStrTable, p, strlen(rspStrTable)))
        {
        	find = EAT_TRUE;
            sscanf(p + strlen(rspStrTable), "%d", &stat);
            break;
        }

        p = (u8*)strchr(p,0x0a);
    }

	if(find == EAT_TRUE)
	{
	    if(0 == stat)
		{
			result = EAT_TRUE;
	    }
	    else
	    {
	    	result = EAT_FALSE;
	    }
		p_get2fsFinalUserCb(result);
		p_get2fsFinalUserCb = NULL;
	}
}
#if 0
static void UrcCb_NewSmsIndicator(u8* pRspStr, u16 len)
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
        s_smsIndex = index;
        simcom_sms_msg_read(index, NULL);
    }
}
#endif

static void UrcCb_LBSIndicator(u8* pRspStr, u16 len)
{
	AtCmdRsp  rspValue = AT_RSP_WAIT;
	u8 *rspStrTable[ ] = {"+CREG: 0","+CREG: 1","+CREG: 2","+CREG: 3"};
	s16  rspType = -1;
	u8	 szBuffer[20]= 0;
	u16  LAC = 0,CI = 0;
	u8	i = 0;
	u8	*p = pRspStr;
	u8	*p2;

	eat_trace("YML ------++++UrcCb_LBSIndicator()");
	
	while(p) {
		/* ignore \r \n */
		while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
		{
			p++;
		}
        for(i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if(strncmp(rspStrTable[i], p, strlen(rspStrTable[i])) == 0)
            {
                rspType = i;
                break;
            }
        }
		switch (rspType)
		{
			case 0:  /* no regist */
			{
				eat_trace("no registered\n");
				break;
			}
			case 1:  /* OK */
			{
					p = (u8*)strchr(p,'"');
					p = p + 1;
					eat_trace("UrcCb_LBSIndicator: urc1 (%s)",p);
					LAC=stringToHex(p,4);
					
					p = (u8*)strchr(p,',');
					p = p + 2;
					#if 0
					eat_trace("UrcCb--1(0x%x)",*p);
					eat_trace("UrcCb--2(0x%x)",*(p+1));
					eat_trace("UrcCb--3(0x%x)",*(p+2));
					eat_trace("UrcCb--4(0x%x)",*(p+3));
					eat_trace("UrcCb_LBSIndicator: urc2 (%s)",p);
					#endif
					CI=stringToHex(p,4);
					eat_trace("UrcCb_LBSIndicator: LAC2(0x%x)",LAC);
					eat_trace("UrcCb_LBSIndicator: CI2(0x%x)",CI);
					LocationAreaCode.LAC=LAC;
					CellID.CI=CI;
					eat_trace("YML: CI(0x%x%x%x%x)",CellID.pkg.CI0,CellID.pkg.CI1,CellID.pkg.CI2,CellID.pkg.CI3);
					eat_trace("YML: LAC(0x%x%x)",LocationAreaCode.pkg.LAC0,LocationAreaCode.pkg.LAC1);
				break;
			}
			case 2:
				eat_trace("search net\n");
				break;
			case 3:
				eat_trace("registration denied 3\n");
				break;
			default:
				break;
		}
		p = NULL;//(u8*)strchr(p,0x0a);
	}
	//if(0 != index){
		//s_smsIndex = index;
		//simcom_sms_msg_read(index, NULL);
	//}
}
static AtCmdRsp AtCmdCb_csq(u8* pRspStr)
{
	AtCmdRsp  rspValue = AT_RSP_WAIT;
	u8 *rspStrTable[ ] = {"ERROR","+CSQ: "};
	s16  rspType = -1;
	u8	rssi= 0;
	u8	ber = 0;
	u8	i = 0;
	u8	*p = pRspStr;
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
				if (rspType == 1){
					sscanf(p+strlen(rspStrTable[rspType]),"%d,%d",&rssi,&ber);
											
				}
				break;
			}
		}

		p = (u8*)strchr(p,0x0a);
	}

	switch (rspType)
	{

		case 0:  /* ERROR */
		rspValue = AT_RSP_ERROR;
		break;

		case 1:  /* +CREG */
		if(99 == rssi){							/* not known or not detected */
			rspValue  = AT_RSP_FUN_OVER;
		}
		else if(rssi>=0&&rssi<32){ 					/* value is true */
			rspValue  = AT_RSP_FUN_OVER;
			g_creg=rssi;
		}
		else
			rspValue = AT_RSP_ERROR;
		break;

		default:
		break;
	}

	eat_trace("at+csq? return %d",rspValue);
	if(rspValue == AT_RSP_ERROR){
		if(p_askCSQCb) {
			p_askCSQCb(EAT_FALSE);
			p_askCSQCb = NULL;
		}
	}
	else if(rspValue == AT_RSP_FUN_OVER)
	{
		if(p_askCSQCb) {
			p_askCSQCb(EAT_TRUE);
			p_askCSQCb = NULL;
		}
	}

	return rspValue;
}

void SendAtComHandle(void)
{
	if(!flag.LBSmsgReady)
	  {
	if(flag.askLBSmsg)
	  {
	  flag.askLBSmsg=0;
	  //Ask_ModuleLBSmsg();
	  ask_creg(askCregCallback);
	  ask_CSQ(askCsqCallback);
	  }
	  }

}

AtCmdRsp AtCmdCbCIPSHUT(u8* pRspStr)
{
	AtCmdRsp  rspValue = AT_RSP_WAIT;
	u8 *rspStrTable[ ] = {"SHUT OK","ERROR"};
	s16  rspType = -1;
	u8	i = 0;
	u8	*p = pRspStr;
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

		p = (u8*)strchr(p,0x0a);
	}

	switch (rspType)
	{
		case 0:  /* OK */
		rspValue  = AT_RSP_CONTINUE;
		break;

		case 1:  /* ERROR */
		rspValue = AT_RSP_ERROR;
		if(s_atCmdEntityQueue.fun[0].pCallback)

		{
			s_atCmdEntityQueue.fun[0].pCallback(EAT_FALSE);
			s_atCmdEntityQueue.fun[0].pCallback = NULL;
		}
		break;

		default:
		rspValue = AT_RSP_WAIT;
		break;
	}

	return rspValue;
}

AtCmdRsp AtCmdCbCIFSR(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"10.","ERROR"};
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

        p = (u8*)strchr(p,0x0a);
    }

    switch (rspType)
    {
        case 0:  /* OK */
        rspValue  = AT_RSP_CONTINUE;
        break;

        case 1:  /* ERROR */
        rspValue = AT_RSP_ERROR;
		if(s_atCmdEntityQueue.fun[0].pCallback)

		{
			s_atCmdEntityQueue.fun[0].pCallback(EAT_FALSE);
			s_atCmdEntityQueue.fun[0].pCallback = NULL;
		}
        break;

        default:
        rspValue = AT_RSP_WAIT;
        break;
    }

    return rspValue;
}
AtCmdRsp AtCmdCbCIPSTART(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR","CONNECT OK","OK"};
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
				if(p_gprsConnect) {
			p_gprsConnect(FALSE);
			p_gprsConnect = NULL;
		}
        break;

        case 1:  /* connect ok*/
        rspValue  = AT_RSP_FUN_OVER;
        
        //simcom_atcmd_queue_fun_out();

		if(p_gprsConnect) {
			p_gprsConnect(TRUE);
			p_gprsConnect = NULL;
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
#if 1//ttttt
AtCmdRsp AtCmdCb_cips(u8* pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR",">","SEND OK","+IPD"};
    s16  rspType = -1;
    u8  i = 0;
    u8  *p = pRspStr;
	u16  dLen = 0;
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
		switch (rspType)
		{
			case 0:  /* ERROR */
			rspValue = AT_RSP_ERROR;
			break;
	
			case 1:  /* >  */
			rspValue  = AT_RSP_CONTINUE;
	
			break;
			case 2: /*SEND OK */
		//	p = (u8*)strchr(p,'+');

			rspValue = AT_RSP_FUN_OVER;
			break;
			case 3: /*+IPD */

			p = (u8*)strchr(p,',');
			p = p + 1;
			eat_trace("*+IPD * p=%s",p);
			dLen=atoi(p);
			eat_trace("+IPD dLen=%d",dLen);
			p = (u8*)strchr(p,':');
			p = p + 1;
			if (dLen>0)
			{
			eat_trace("IP Recv data len=%d, %s",dLen, p);
			for(i=0;i<SeverDataMax;i++)
			{
			if(i<dLen)
			g_RecvServerData[i]=*p++;
			else
			{
			g_RecvServerData[i]=0;
			}
			}
	
			g_RecvServerDataLen=dLen;
			eat_send_msg_to_user(EAT_USER_1, EAT_USER_0, EAT_FALSE, 14, "REC_SEVER_DATA", EAT_NULL);
#if DebugMsgOnOff
			for(i=0;i<dLen;i++)
			{
			//Read from Server.TODO something
			eat_trace("R222 data 0x=%x",g_RecvServerData[i]);
			}
#endif
			}
			else
			{
			eat_trace("IPD break 1111\n");
			}
			rspValue = AT_RSP_FUN_OVER;
			break;
	
			default:
			rspValue = AT_RSP_WAIT;
			break;
		}

        p = (u8*)strchr(p,AT_CMD_LF);
    }
	
	eat_trace("AtCmdCb_cips(), rspType(%d),P=1212%s",rspType,p);


    return rspValue;
}
#endif
static void UrcCb_IPDIndicator(u8* pRspStr, u16 len)
{
	AtCmdRsp  rspValue = AT_RSP_WAIT;
	u8 *rspStrTable[ ] = {"+IPD"};
	s16  rspType = -1;
	u16  dLen = 0;
	u8	i = 0;
	u8	*p = pRspStr;

	eat_trace("YML ------++++UrcCb_IPDIndicator()");
	
	while(p) {
		/* ignore \r \n */
		while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
		{
			p++;
		}
        for(i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if(strncmp(rspStrTable[i], p, strlen(rspStrTable[i])) == 0)
            {
                rspType = i;
                break;
            }
        }
		switch (rspType)
		{
			case 0:  /* +IPD */
			{
					p = (u8*)strchr(p,',');
					p = p + 1;
					dLen=atoi(p);
					///eat_trace("dLen=%d",dLen);
					p = (u8*)strchr(p,':');
					p = p + 1;
			if (dLen>0)
            {
			eat_trace("IP Recv data len=%d, %s",dLen, p);
			for(i=0;i<SeverDataMax;i++)
				{
				if(i<dLen)
					g_RecvServerData[i]=*p++;
				else
					{
					g_RecvServerData[i]=0;
					}
				}
			
			g_RecvServerDataLen=dLen;
			//flag.recvSeverData=1;  move to logic.c
			eat_send_msg_to_user(EAT_USER_1, EAT_USER_0, EAT_FALSE, 14, "REC_SEVER_DATA", EAT_NULL);
				//eat_trace("flag.recvSeverData=0x%x",flag.recvSeverData);
			#if DebugMsgOnOff
            for(i=0;i<dLen;i++)
            	{
                //Read from Server.TODO something
				eat_trace("R222 data 0x=%x",g_RecvServerData[i]);
            	}
			#endif
            }
            else
        	{
        	eat_trace("IPD break 1111\n");
        	}
			break;
			}

			default:
				break;
		}
		p=(u8*)strchr(p,0x0a);
	}

}
u8 CmpSOSnuber(u8 *numberString,u8 len)
{
u8 result=0;
u8 i=0;
for(i=0;i<4;i++)
	{
		TRACE_DEBUG("sosNumber[%d]=%s",i,SosNumber[i]);
		//if(0==strncmp(SosNumber[i],numberString,len))
		if(0==strcmp(SosNumber[i],numberString))
			{
			result=i+1;
			return result;
			}
	}
return result;
}

u8 *Callstat[]=
{
	"Active",		//呼入
	"Held",
	"Dialing [MO CALL]",//呼出
	"Alerting[MO CALL]",//呼出
	"inComing[MT CALL]",//呼入
	"Waiting [MT CALL]",//呼入
	"Disconnect"			//挂断
};
static void UrcCb_CLCC(u8* pRspStr, u16 len)
{
	AtCmdRsp  rspValue = AT_RSP_WAIT;
	u8 *rspStrTable[ ] = {"+CLCC:"};
	s16  rspType = -1;
	u16  dLen = 0;
	u8	i = 0;
	u8	*p = pRspStr;
	u8  *tp=p;
	u8 idx=0xFF;		//0-7  来电序号
	u8 dir=0xFF;		//0-1 方向, MO,MT
	u8 stat=0xFF;	//0-6状态，Active,Hold,Dialing,Alerting,inComing,Waiting,Disconnect
	u8 mode=0xFF;	//0-2 voice data Fax  
	u8 mpty=0xFF;	//0-1
	u8 phoneNum[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	u8 phoneNubCnt=0;
	u8 result=0xFF;
	
	eat_trace("YML ------++++UrcCb_CLCC()");
	//eat_modem_write("at+CRSL=1\r\n",strlen("at+CRSL=1\r\n"));
	//eat_modem_set_poweron_urc_dir(EAT_USER_1);//一般默认都是user0	，内核core的信息回复给EAT_USER_0

	//+CLCC: 1,1,4,0,0,"13320984483",129,""
	//while(p) {
		/* ignore \r \n */
		while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
		{
			p++;
		}
        for(i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if(strncmp(rspStrTable[i], p, strlen(rspStrTable[i])) == 0)
            {
                rspType = i;
                break;
            }
        }
		switch (rspType)
		{
			case 0:  /* +CLCC */
			{		
					p = (u8*)strchr(p,',');// 找到第一个逗号
					idx = *(p-1)-0x30;
					dir = *(p+1)-0x30;
					stat= *(p+3)-0x30;
					mode= *(p+5)-0x30;
					mpty= *(p+7)-0x30;
					eat_trace("idx=%d,dir=%d,stat=%d,mode=%d,mpty=%d",idx,dir,stat,mode,mpty);
					p = (u8*)strchr(p,'"');
					tp = p;
					while('"'!=*(++tp))//到第二个双引号结束计数
						{
						phoneNum[phoneNubCnt]=*tp;
						phoneNubCnt++;
						}
					result=CmpSOSnuber(phoneNum,phoneNubCnt);
						Call.isSOS=result;
						Call.idx=idx;
						Call.dir=dir;
						Call.stat=stat;
						Call.mode=mode;
						Call.mpty=mpty;
						Call.phoneNubCnt=phoneNubCnt;
						strcpy(Call.phoneNum,phoneNum);
						flag.CLCCin =1;	
						//if(0 == Call.stat)
						//	{
							//eat_modem_write("AT+CRSL=0\r\n",strlen("AT+CRSL=0\r\n"));
							//eat_sleep(50);							
							//eat_modem_set_poweron_urc_dir(EAT_USER_1);//一般默认都是user0	，内核core的信息回复给EAT_USER_0
							//eat_trace("phone is online \n");
							//}
						if(6==Call.stat)//如果为未连接
							{
							eat_gpio_setup(SHUT_PIN, EAT_GPIO_DIR_OUTPUT, EAT_GPIO_LEVEL_LOW);// 电话挂掉的时候将功放mute(低电平)
							clear_CallArry();							
							//eat_modem_write("AT+CRSL=100\r\n",strlen("AT+CRSL=100\r\n"));
							//eat_sleep(50);							
							//eat_modem_set_poweron_urc_dir(EAT_USER_1);//一般默认都是user0	，内核core的信息回复给EAT_USER_0
							}
						else
							{							
							eat_gpio_setup(SHUT_PIN, EAT_GPIO_DIR_OUTPUT, EAT_GPIO_LEVEL_HIGH);// 电话挂掉的时候将功放mute(低电平)
							}
					#if DebugOn
					eat_trace("phone number=%s,phoneNubCnt=%d",phoneNum,phoneNubCnt);
					eat_trace("is SOS number result=%d",result);					
					eat_trace("+++Call.stat is %s+++\r",Callstat[Call.stat]);	
					if(0==Call.dir)
						{
						eat_trace("this is a MO CALL OUT CALL");	
						}
					else if(1==Call.dir){
						eat_trace("this is a MT CALL incoming call ");	
						}
					#endif
			break;
			}

			default:
				break;
		}
	//	p=(u8*)strchr(p,0x0a);
//	}
	eat_trace("out of CLCC FUN");

}

static void UrcCb_NormalPowerDown(u8* pRspStr, u16 len)
{
eat_gpio_setup(MOTOR_PIN, EAT_GPIO_DIR_OUTPUT, EAT_GPIO_LEVEL_LOW);
eat_trace("UrcCb_NormalPowerDown+++MOTOR ON gpio low\n");
eat_sleep(200);
eat_gpio_setup(MOTOR_PIN, EAT_GPIO_DIR_OUTPUT, EAT_GPIO_LEVEL_HIGH);
eat_trace("UrcCb_NormalPowerDown+++MOTOR ON gpio up\n");

}


#if EnableGPSModule

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: global_urc_handler
 *  Description: global urc check 
 *        Input:
 *               pUrcStr::
 *               len::
 *       Output:
 *       Return:
 *               void::
 *       author: Jumping create at 2014-1-14
 * =====================================================================================
 */
 static UrcEntityQueue gps_urcEntityQueue=
	{
		{
			{"$GNRMC",NULL}, 
			{"$GNGGA",NULL},
			0
		},
		2
	};
/*
如22? 32.7658'=(22X60+32.7658)X3000=40582974，然后转换成十六进制数
40582974(10进制)= 26B3F3E（16进制）
最后值为0x02 0x6B 0x3F 0x3E。
*/

void gps_urc_handler(u8* pUrcStr, u16 len)
{
	u8 *rspStrTable[ ] = {"$GNRMC","$GNGGA"};
	s16  rspType = -1;
	u8	i = 0;
	u8	*p = pUrcStr;
	u32 timer=0;
	u32 date=0;
	u8 hour=0;
	u8 min=0;
	u8 sec=0;
	u8 year=0;
	u8 month=0;
	u8 day=0;
	double N=0;
	double W=0;
	u32 N32=0;
	u32 W32=0;
	u8 N1=0,N2=0,N3=0,N4=0;
	u8 W1=0,W2=0,W3=0,W4=0;
	double Nt=0;
	double Wt=0;
	
	char *endptr;
	eat_bool result = EAT_TRUE;
	#if EnableGpsDebuge
	eat_trace("YML -gps_urc_handler++++++");
	#endif
    while(p) {
		/* ignore \r \n */
		while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
		{
			p++;
		}
		
        for(i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
        //eat_trace("_____i=%d________________________________",i);
            if(strncmp(rspStrTable[i], p, strlen(rspStrTable[i])) == 0)
            {
                rspType = i;
                break;
            }
        }
		switch (rspType)
		{
			case 0:  /* $GNRMC */
			{
			#if EnableGpsDebuge
				eat_trace("rspType=%d,$GNRMC receive!!!!\n",rspType);
			#endif
				p = (u8*)strchr(p,',')+1;
				timer=atoi(p);
				//eat_trace("timer=%d\n",timer);
				hour=(u8)(timer/10000)+8;
				min=(u8)((timer%10000)/100);
				sec=(u8)(timer%100);
				p = (u8*)strchr(p,',')+1;
				if('A'==*p)
					{
					flag.GPSisOk=1;
					p = (u8*)strchr(p,',')+1;//找到下一个逗号
					//N=atof(p);
					//TRACE_DEBUG("N=%f\n",N);
					N=strtod(p,&endptr);
					//TRACE_DEBUG("N=%f,endchar=%c\n",N,*endptr);
					Nt=((u8)(N/100)*60+N-(u8)(N/100)*100)*30000;
					#if EnableGpsDebuge
					TRACE_DEBUG("Nt=%f,N1=0x%x,N2=0x%x,N3=0x%x,N4=0x%x\n",Nt,N1,N2,N3,N4);
					#endif
					N32=(u32)Nt;
					N1=(u8)((N32&0xFF000000ul)>>24);
					N2=(u8)((N32&0x00FF0000ul)>>16);
					N3=(u8)((N32&0x0000FF00ul)>>8);
					N4=(u8)((N32&0x000000FFul));
					#if EnableGpsDebuge
					TRACE_DEBUG("N32=0x%x,N1=0x%x,N2=0x%x,N3=0x%x,N4=0x%x\n",N32,N1,N2,N3,N4);
					#endif
					g_GpsLat_1=N1;g_GpsLat_2=N2;g_GpsLat_3=N3;g_GpsLat_4=N4;
					p = (u8*)strchr(p,',')+1;//找到下一个逗号
					if('N'==*p)
						{
						flag.GpsLatitude=1;// 北纬
						#if EnableGpsDebuge
						TRACE_DEBUG("Latitude weidu is N");
						#endif
						}
					else if('S'==*p)
						{
						flag.GpsLatitude=0;// 北纬
						#if EnableGpsDebuge
						TRACE_DEBUG("Latitude weidu is S");
						#endif
						}
					p = (u8*)strchr(p,',')+1;//找到下一个逗号
					//W=atof(p);
					//TRACE_DEBUG("W=%f\n",W);
					W=strtod(p,&endptr);
					//TRACE_DEBUG("W=%f,endchar=%c\n",W,*endptr);
					Wt=((u8)(W/100)*60+W-(u8)(W/100)*100)*30000;
					#if EnableGpsDebuge
					TRACE_DEBUG("Wt=%f,W1=0x%x,W2=0x%x,W3=0x%x,W4=0x%x\n",Wt,W1,W2,W3,W4);
					#endif
					W32=(u32)Wt;
					W1=(u8)((W32&0xFF000000ul)>>24);
					W2=(u8)((W32&0x00FF0000ul)>>16);
					W3=(u8)((W32&0x0000FF00ul)>>8);
					W4=(u8)(W32&0x000000FFul);
					g_Gpslong_1=W1;g_Gpslong_2=W2;g_Gpslong_3=W3;g_Gpslong_4=W4;
					#if EnableGpsDebuge
					TRACE_DEBUG("W32=0x%x,W1=0x%x,W2=0x%x,W3=0x%x,W4=0x%x\n",W32,W1,W2,W3,W4);
					#endif
					p = (u8*)strchr(p,',')+1;//找到纬度是经度是东经还是西经
					if('E'==*p)
						{
						flag.GpsLongitude=1;// 东经
						#if EnableGpsDebuge
						TRACE_DEBUG("Longitude jingdu is E");
						#endif
						}
					else if('W'==*p)
						{
						flag.GpsLatitude=0;// 西经
						#if EnableGpsDebuge
						TRACE_DEBUG("Longitude weidu is W");
						#endif
						}
					p = (u8*)strchr(p,',')+1;//找到速度
					g_GpsSpeed=atoi(p);
					p = (u8*)strchr(p,',')+1;//找到航向
					g_GpsMoveDir=atoi(p);
					#if EnableGpsDebuge
					TRACE_DEBUG("move speed is %d",g_GpsSpeed);
					TRACE_DEBUG("g_GpsMoveDir is %d",g_GpsMoveDir);
					#endif
					p = (u8*)strchr(p,',')+1;//找到日期
					date=atoi(p);
					//eat_trace("date=%d\n",date);
					day=(u8)(date/10000);
					month=(u8)((date%10000)/100);
					year=(u8)(date%100);
					 rtc.year = year;
					 rtc.mon = month;
					 rtc.day = day;
					 rtc.hour =hour;
					 rtc.min = min;
					 rtc.sec = sec;
					 //set rtc 
					result=eat_set_rtc(&rtc);
					 #if EnableGpsDebuge
					 if(result)
					 	{
					 	eat_trace("Timer GPS SET rtc value:%d/%d/%d %d:%d:%d",
							rtc.year,rtc.mon,rtc.day,rtc.hour,rtc.min,rtc.sec);
					 	}
					 else
					 	{
					 	eat_trace("Timer error");
					 	}
					 #endif
						GpsTime.year=year;
						GpsTime.mon=month;
						GpsTime.day=day;

						GpsTime.hour=hour;
						GpsTime.min=min;
						GpsTime.sec=sec;

					}
				else if('V'==*p)
					{
					flag.GPSisOk=0;
					#if EnableGpsDebuge
					TRACE_DEBUG("DATA IS VVVVVV\n");
					#endif
					}
				else
					{
					flag.GPSisOk=0;
					#if EnableGpsDebuge
					TRACE_DEBUG("DATA IS unknown\n");
					#endif
					}
				break;
			}
			case 1:  /* $GNGGA */
			{
				#if EnableGpsDebuge
				eat_trace("rspType=%d,$GNGGA receive!!!!++++\n",rspType);
				#endif
				#if 0
					p = (u8*)strchr(p,'"');
					p = p + 1;
					eat_trace("UrcCb_LBSIndicator: urc1 (%s)",p);
					LAC=stringToHex(p,4);
					
					p = (u8*)strchr(p,',');
					p = p + 2;
					#if 0
					eat_trace("UrcCb--1(0x%x)",*p);
					eat_trace("UrcCb--2(0x%x)",*(p+1));
					eat_trace("UrcCb--3(0x%x)",*(p+2));
					eat_trace("UrcCb--4(0x%x)",*(p+3));
					eat_trace("UrcCb_LBSIndicator: urc2 (%s)",p);
					#endif
					CI=stringToHex(p,4);
					eat_trace("UrcCb_LBSIndicator: LAC2(0x%x)",LAC);
					eat_trace("UrcCb_LBSIndicator: CI2(0x%x)",CI);
					LocationAreaCode.LAC=LAC;
					CellID.CI=CI;
					eat_trace("YML: CI(0x%x%x%x%x)",CellID.pkg.CI0,CellID.pkg.CI1,CellID.pkg.CI2,CellID.pkg.CI3);
					eat_trace("YML: LAC(0x%x%x)",LocationAreaCode.pkg.LAC0,LocationAreaCode.pkg.LAC1);
					#endif
				break;
			}
			case 2:
				//eat_trace("search net\n");
				break;
			case 3:
				//eat_trace("registration denied 3\n");
				break;
			default:
				break;
		}
		p =(u8*)strchr(p,0x0a);
	}
}
#endif

void clear_CallArry(void)
{						
Call.idx=0xFF;	
Call.dir=0xFF;
Call.stat=6;	//未连接状态
Call.mode=0xFF;
Call.mpty=0xFF;
Call.phoneNubCnt=0;
Call.isSOS=0;
AppMemSet(Call.phoneNum,0x00,12);//清空IncomeCall.phoneNum
flag.CLCCin =0;
}


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
#include "eat_interface.h"
#include "app_at_cmd_envelope.h"

/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ################################### */
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define MODEM_READ_BUFFER_LEN 2048
#define AT_CMD_ARRAY_SHORT_LENGTH 30
#define AT_CMD_ARRAY_MID_LENGTH 130
#define AT_CMD_ARRAY_LONG_LENGTH 300

/* #####   TYPE DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ######################### */

typedef struct UrcEntityQueueTag
{
    UrcEntity urcEntityArray[URC_QUEUE_COUNT]; /* AT命令队列 */
    u8 count;                                 /* 当前执行的index */
}UrcEntityQueue;

typedef struct AtCmdEntityQueueTag
{
    u8 current;                                 /* 当前执行的index */
    u8 last;                                    /* 队列中最后一条的index */
    u8 funFirst;                                /* 功能模块的第一条 */
    u8 funLast;                                 /* 功能模块的最后一条 */
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

/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ################################ */
static UrcEntityQueue s_urcEntityQueue={
    {
        {"+FTPGETTOFS: 0",UrcCb_FTPGETTOFS},
        0
    },
    1
};

static AtCmdEntityQueue s_atCmdEntityQueue={0};
static u8 modem_read_buf[MODEM_READ_BUFFER_LEN]; 
static u8* s_atCmdWriteAgain = NULL;
static u8 s_atExecute = 0;
static ResultNotifyCb p_gsmInitCb = NULL;
static ResultNotifyCb p_gprsBearCb = NULL;
static ResultNotifyCb p_get2fsCb = NULL;
static ResultNotifyCb p_get2fsFinalUserCb = NULL;

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
        {AT_CMD_DELAY"5000",10,NULL},
        {"AT+CREG?"AT_CMD_END,10,AtCmdCb_creg},
        {AT_CMD_DELAY"2000",10,NULL},
        {"AT+CGATT?"AT_CMD_END,11,AtCmdCb_cgatt},
    };
    u8 pAtCpin[15] = {0};
    sprintf(pAtCpin,"AT+CPIN=%s%s",cpin,AT_CMD_END);
    atCmdInit[4].p_atCmdStr = pAtCpin;
    atCmdInit[4].cmdLen = strlen(pAtCpin);
    result = simcom_atcmd_queue_fun_append(atCmdInit,sizeof(atCmdInit) / sizeof(atCmdInit[0]));
    if(result)
        p_gsmInitCb = pResultCb;
    return result;
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
        {"AT+SAPBR=1,1"AT_CMD_END, 14, NULL},
        {"AT+FTPCID=1"AT_CMD_END, 13, NULL},
        {"AT+FTPUN=", 0, NULL},                            /* 6  */
        {"AT+FTPPW=", 0, NULL},                            /* 7  */
        {"AT+FTPGETNAME=", 0, NULL},                       /* 8  */
        {"AT+FTPGETPATH=", 0, NULL},                       /* 9  */
        {"AT+FTPSERV=", 0, NULL},                          /* 10  */
        {"AT+FTPPORT=", 0, NULL},                          /* 11 */
        {"AT+FTPGETTOFS=0,", 0, AtCmdCb_ftpgettofs}          /* 12 */
    };

    /* set at cmd func's range,must first set */
    count = sizeof(atCmdInit) / sizeof(atCmdInit[0]);
    result = simcom_atcmd_queue_fun_set(count);

    if(!result)                                 /* the space of queue is poor */
        return FALSE;

    p_get2fsCb = pResultCb;
    p_get2fsFinalUserCb = finalCb;

    for (i=0 ;i< count ;i++ ){

        u8 pAtCmd[50] = {0};
//        memset(pAtCmd,0,sizeof(pAtCmd));
        switch ( i ) {
            case 1:	
                //    sprintf(pAtCmd,"%s%s",atCmdInit[i].p_atCmdStr,g_modemConfig.apnName,AT_CMD_END);
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,"CMNET",AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 2:	
                //    sprintf(pAtCmd,"%s%s",atCmdInit[i].p_atCmdStr,g_modemConfig.apnUserName,AT_CMD_END);
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,"\"\"",AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 3:	
                //    sprintf(pAtCmd,"%s%s",atCmdInit[i].p_atCmdStr,g_modemConfig.apnPassword,AT_CMD_END);
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,"\"\"",AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 6:	
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,username,AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 7:	
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,pwd,AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 8:	
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,getName,AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 9:	
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,getPath,AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 10:	
                sprintf(pAtCmd,"%s%s%s",atCmdInit[i].p_atCmdStr,serv,AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 11:	
                sprintf(pAtCmd,"%s%d%s",atCmdInit[i].p_atCmdStr,port,AT_CMD_END);
                atCmdInit[i].p_atCmdStr = pAtCmd;
                atCmdInit[i].cmdLen = strlen(pAtCmd);
                break;

            case 12:	
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
    u8 first = MIN(s_atCmdEntityQueue.current,s_atCmdEntityQueue.funFirst);
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
    s_urcEntityQueue.count ++;
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
    u8 first =  MAX(s_atCmdEntityQueue.current,s_atCmdEntityQueue.funFirst);

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
eat_bool simcom_atcmd_queue_fun_set(u8 funCount)
{
    u8 i = 0;
    u8 first =  MAX(s_atCmdEntityQueue.current,s_atCmdEntityQueue.funFirst);
    u8 freeCount = 0;
    if (s_atCmdEntityQueue.funLast != s_atCmdEntityQueue.funFirst){
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

    s_atCmdEntityQueue.funFirst = s_atCmdEntityQueue.last;
    s_atCmdEntityQueue.funLast = (s_atCmdEntityQueue.last + funCount - 1 + AT_CMD_QUEUE_COUNT) % AT_CMD_QUEUE_COUNT;
    eat_trace("INFO: the at cmd func's range is %d-%d", s_atCmdEntityQueue.funFirst, s_atCmdEntityQueue.funLast);
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
eat_bool simcom_atcmd_queue_fun_append(AtCmdEntity atCmdEntity[],u8 funCount)
{
    u8 i = 0;
    eat_bool ret = FALSE;
    ret = simcom_atcmd_queue_fun_set(funCount);
    if(!ret)
        return FALSE;

    for ( i=0; i<funCount; i++) {
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

    eat_trace("INFO: funFirst:%d  funLast:%d", s_atCmdEntityQueue.funFirst, s_atCmdEntityQueue.funLast);
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
void app_at_cmd_envelope(void *data)
{
    EatEvent_st event;
    while(EAT_TRUE)
    {
        eat_get_event_for_user(EAT_USER_1, &event);
        switch(event.event)
        {
            case EAT_EVENT_TIMER :
                
                switch ( event.data.timer.timer_id ) {
                    case EAT_TIMER_1:           /* dealy to excute at cmd */
                        eat_trace("INFO: EAT_TIMER_1 expire!");
                        eat_timer1_handler();
                        break;

                    case EAT_TIMER_2:           /* at cmd time over */
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
		if(p_get2fsCb) {
			p_get2fsCb(EAT_FALSE);
			p_get2fsCb = NULL;
		}

		if(p_gsmInitCb) {
			p_gsmInitCb(EAT_FALSE);
			p_gsmInitCb = NULL;
		}

		if(p_gprsBearCb) {
			p_gprsBearCb (EAT_FALSE);
			p_gprsBearCb = NULL;
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
    eat_trace("INFO: at cmd delay execute,%d",delay);
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

   if(s_atCmdEntityQueue.current>=s_atCmdEntityQueue.funLast
           || s_atCmdEntityQueue.current<=s_atCmdEntityQueue.funFirst){
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
    if (s_atCmdEntityQueue.funLast == 0)        /* no at cmd func */
        return FALSE;

    if (s_atCmdEntityQueue.funLast < s_atCmdEntityQueue.funFirst)
	{
	    for (i = s_atCmdEntityQueue.funFirst; i < AT_CMD_QUEUE_COUNT; i++) 
		{
           AtCmdEntity* atCmdEnt = NULL;
           atCmdEnt = &(s_atCmdEntityQueue.atCmdEntityArray[i]);
           if(atCmdEnt->p_atCmdStr){
               eat_mem_free(atCmdEnt->p_atCmdStr);
               atCmdEnt->p_atCmdStr = NULL;
           }
           atCmdEnt->p_atCmdCallBack = NULL;
       }
	
	    for (i = 0;i <= s_atCmdEntityQueue.funLast; i++)
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
	    for ( i=s_atCmdEntityQueue.funFirst ;i<=s_atCmdEntityQueue.funLast ; i++) {
	        AtCmdEntity* atCmdEnt = NULL;
	        atCmdEnt = &(s_atCmdEntityQueue.atCmdEntityArray[i]);
	        if(atCmdEnt->p_atCmdStr){
	            eat_mem_free(atCmdEnt->p_atCmdStr);
	            atCmdEnt->p_atCmdStr = NULL;
	        }
	        atCmdEnt->p_atCmdCallBack = NULL;
	    }
	}

    if(s_atCmdEntityQueue.current != s_atCmdEntityQueue.funLast){
        eat_trace("WARNING: at cmd func over did not at last!");
        s_atCmdEntityQueue.current = s_atCmdEntityQueue.funLast;
    }

    s_atCmdEntityQueue.current = (s_atCmdEntityQueue.current + 1) % AT_CMD_QUEUE_COUNT;
    s_atCmdEntityQueue.funLast = 0;
    s_atCmdEntityQueue.funFirst = 0;
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
        eat_trace("DEBUG:at cmd is:%d,%s",s_atCmdEntityQueue.current,pCmd);
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

        for (i = 0; i < s_urcEntityQueue.count; i++)
        {
            if (!strncmp(s_urcEntityQueue.urcEntityArray[i].p_urcStr, p, strlen(s_urcEntityQueue.urcEntityArray[i].p_urcStr))) {
                if(s_urcEntityQueue.urcEntityArray[i].p_urcCallBack)
                    s_urcEntityQueue.urcEntityArray[i].p_urcCallBack(p,len);
            }
        }

        p = (u8*)memchr(p,0x0a,0);
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
static void eat_modem_ready_read_handler(void)
{
    u16 len = eat_modem_read(modem_read_buf,MODEM_READ_BUFFER_LEN);
    if (len > 0) {
        AtCmdRsp atCmdRsp = AT_RSP_ERROR;
        memset(modem_read_buf+len,0,MODEM_READ_BUFFER_LEN-len);
                                                /* global urc check */
        global_urc_handler(modem_read_buf,len);
                                                /* at cmd rsp check */
        eat_trace("ReFrModem:%s,%d",modem_read_buf,len);
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
                    simcom_atcmd_queue_fun_out();
                    AtCmdDelayExe(0);
                    AtCmdOverTimeStop();
                    break;

                case AT_RSP_WAIT:
                    break;

                default:	
                    if(atCmdRsp>=AT_RSP_STEP_MIN && atCmdRsp<=AT_RSP_STEP_MAX) {
                        s8 step = s_atCmdEntityQueue.current + atCmdRsp - AT_RSP_STEP;
                        eat_trace("DEBUG: cur %d,step %d",s_atCmdEntityQueue.current,step);
                        if(step<=s_atCmdEntityQueue.funLast && step>= s_atCmdEntityQueue.funFirst){
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
    static count = 0;
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

static AtCmdRsp AtCmdCb_sapbr(u8* pRspStr)
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
        rspValue  = AT_RSP_FUN_OVER;

        if(p_gprsBearCb) {
            p_gprsBearCb(TRUE);
            p_gprsBearCb = NULL;
        }
        break;

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


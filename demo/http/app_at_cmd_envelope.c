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
    UrcEntity urcEntityArray[URC_QUEUE_COUNT]; /*AT Command queue */
    u8 count;                                 /* Current excute queue index*/
}UrcEntityQueue;

typedef struct FunInfoTag
{
    u8 first;
    u8 last;
    ResultNotifyCb pCallback;
}FunInfo;
typedef struct AtCmdEntityQueueTag
{
    u8 current;                                /* Current excute queue index*/
    u8 last;                                   /* Queue  last index*/
    FunInfo fun[2];	
    u8 funCount;
    AtCmdEntity atCmdEntityArray[AT_CMD_QUEUE_COUNT];  /*AT Command queue */

}AtCmdEntityQueue;
/* #####   DATA TYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */
extern void UrcCb_http_modem_data_parse(u8* pRspStr, u16 len);
extern void UrcCb_http_action_indicator(u8* pRspStr, u16 len);

/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */
static AtCmdRsp AtCmdCbDefault(u8* pRspStr);
static eat_bool simcom_atcmd_queue_head_out(void);
static eat_bool simcom_atcmd_queue_tail_out(void);
static eat_bool simcom_atcmd_queue_fun_out(void);
static eat_bool AtCmdOverTimeStart(void);
static eat_bool AtCmdOverTimeStop(void);
static void eat_timer1_handler(void);
static void eat_modem_ready_read_handler(void);
static void global_urc_handler(u8* pUrcStr, u16 len);
static eat_bool AtCmdDelayExe(u16 delay);

/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ################################ */
static UrcEntityQueue s_urcEntityQueue={
    {
        {"+HTTPACTION: ",UrcCb_http_action_indicator}, 
        {"param:", UrcCb_http_modem_data_parse},
        0
    },
    2
};

static AtCmdEntityQueue s_atCmdEntityQueue={0};
static u8 modem_read_buf[MODEM_READ_BUFFER_LEN]; 
static u8* s_atCmdWriteAgain = NULL;
static u8 s_atExecute = 0;
static ResultNotifyCb p_gsmInitCb = NULL;


/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ############################ */

eat_bool simcom_gsm_init(u8* cpin,ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    AtCmdEntity atCmdInit[]={
        {"AT"AT_CMD_END,4,NULL},
        {"ATE0"AT_CMD_END,6,NULL},
    };
    result = simcom_atcmd_queue_fun_append(atCmdInit,sizeof(atCmdInit) / sizeof(atCmdInit[0]),p_gsmInitCb);

    return result;
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
    eat_trace("INFO: the at cmd func's range is %d-%d", s_atCmdEntityQueue.fun[s_atCmdEntityQueue.funCount-1].first, s_atCmdEntityQueue.fun[s_atCmdEntityQueue.funCount-1].last);
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

    eat_trace("INFO: funFirst:%d  funLast:%d", s_atCmdEntityQueue.fun[s_atCmdEntityQueue.funCount-1].first, s_atCmdEntityQueue.fun[s_atCmdEntityQueue.funCount-1].last);
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
    simcom_gsm_init("1234",NULL);
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


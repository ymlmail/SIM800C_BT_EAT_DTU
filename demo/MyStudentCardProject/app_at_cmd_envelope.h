/*
 * =====================================================================================
 *
 *       Filename:  app_at_cmd_envelope.h
 *
 *    Description:  the head file
 *
 *        Version:  1.0
 *        Created:  2014-1-11 15:58:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jumping (apple), zhangping@sim.com
 *   Organization:  www.sim.com
 *
 * =====================================================================================
 */

/* #####   HEADER FILE INCLUDES   ################################################### */

/* #####   EXPORTED MACROS   ######################################################## */
#define URC_QUEUE_COUNT 10                      /* global urc count */
#define AT_CMD_QUEUE_COUNT 50
#define AT_CMD_EXECUTE_DELAY  10                /* 10 ms use EAT_TIMER_1*/
#define AT_CMD_EXECUTE_OVERTIME 60000           /* 60s use EAT_TIMER_2*/

#define AT_CMD_DELAY "DELAY:"
#define AT_CMD_END "\x0d\x0a"
#define AT_CMD_CR  '\x0d'
#define AT_CMD_LF  '\x0a'
#define AT_CMD_CTRL_Z "\x1a"

/* #####   EXPORTED TYPE DEFINITIONS   ############################################## */
typedef enum
{
    AT_RSP_ERROR = -1,
    AT_RSP_WAIT= 0, 
    AT_RSP_CONTINUE = 1,                        /* Continue to execute the next AT command in AT command queue*/
    AT_RSP_PAUSE= 2,                            /* Suspend to execute the AT command queue*/
    AT_RSP_FINISH = 3,                          /* Stop executing the AT command queue */

    AT_RSP_FUN_OVER = 4,                        /* Finish the current AT command queue, then clear the AT command queue*/
    AT_RSP_STEP_MIN = 10,
    AT_RSP_STEP = 20,                           /*Continue to execute current AT command */
    AT_RSP_STEP_MAX = 30,

}AtCmdRsp;

typedef AtCmdRsp (*AtCmdRspCB)(u8 *pRspStr);
typedef void (*UrcCB)(u8 *pUrcStr, u16 len);
typedef void (*ResultNotifyCb)(eat_bool result);
typedef void (*SmsReadCB)(u8* number,u8 *msg);

typedef struct AtCmdEntityTag
{
    u8* p_atCmdStr;
    u16 cmdLen;
    AtCmdRspCB p_atCmdCallBack;

}AtCmdEntity;

typedef struct UrcEntityTag
{
    u8* p_urcStr;
    UrcCB p_urcCallBack;

}UrcEntity;


/* #####   EXPORTED DATA TYPES   #################################################### */

/* #####   EXPORTED VARIABLES   ##################################################### */

/* #####   EXPORTED FUNCTION DECLARATIONS   ######################################### */
extern void simcom_atcmd_queue_init(void);
extern eat_bool simcom_atcmd_queue_append(AtCmdEntity atCmdEntity);
extern eat_bool simcom_atcmd_queue_fun_append(AtCmdEntity* atCmdEntity,u8 funCount,ResultNotifyCb callback);
extern void app_at_cmd_envelope(void *data);
extern eat_bool simcom_atcmd_queue_is_empty();
//extern eat_bool simcom_ftp_down_file( u8 *username, u8 *pwd, u8 *getName, u8 *getPath, 
 //       u8 *serv, u8 port, u8 *fileName, ResultNotifyCb pResultCb, ResultNotifyCb finalCb);
extern eat_bool simcom_atcmd_queue_fun_set(u8 count,ResultNotifyCb callback);
extern eat_bool simcom_sms_format_set(u8 nFormatType, ResultNotifyCb pResultCb);
extern eat_bool simcom_sms_sc_set(u8* number, ResultNotifyCb pResultCb);
extern eat_bool simcom_sms_text_send(u8* number, u8* msg, u16 msgLen,ResultNotifyCb pResultCb);
extern eat_bool simcom_sms_msg_read(u16 index, SmsReadCB pUrcCb);
extern eat_bool simcom_sms_msg_delete(u16 index, ResultNotifyCb pResultCb);
extern void simcom_read_sms_interface_register(SmsReadCB smsReadSmsCB);
extern eat_bool simcom_sms_pdu_send(u8* ptrPdu, u16 pduLen,ResultNotifyCb pResultCb);
extern eat_bool simcom_sms_cnmi_set(u8 mode, u8 mt, u8 bm,u8 ds, u8 bfr,ResultNotifyCb pResultCb);


extern eat_bool simcom_urc_queue_append(UrcEntity urcEntity);

extern eat_bool simcom_urc_queue_remove(UrcEntity urcEntity);

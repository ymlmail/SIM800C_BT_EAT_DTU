#if !defined (__EAT_INTERFACE_H__)
#define __EAT_INTERFACE_H__

#include "eat_type.h"
#include "eat_mem.h"
#include "eat_timer.h"
#include "eat_modem.h"
#include "eat_uart.h"
#include "eat_periphery.h"
#include "eat_fs.h"
#include "eat_flash.h"
#include "eat_audio.h"
#include "eat_inputmethod.h"
#ifdef __SIMCOM_EAT_WMMP__
#include "eat_wmmp.h"  
#endif 
#include "eat_sms.h"/*SIMCOM zhudingfen 2013-12-24 MKBug00020886*/
#ifdef __SIMCOM_EAT_SOFT_SIM__
#include "eat_soft_sim.h"
#endif
#include "eat_sim.h"
#include "eat_other.h"
#include "eat_network.h"
#include "eat_socket.h"

#define EAT_USER_MSG_MAX_SIZE 64 

/*app bin config MACRO begin*/
#define APP_MAGIC          0x004D4D4D 
#define APP_INFO_ID        "FILE_INFO" 
#define FILE_TYPE          0x7001 //0x0103
#define FLASH_TYPE         7
#define APP_FILE_INFO      0
#define APP_BIN_INFO       0x200

typedef struct CFG_Header {
    u32        magic;
    u16        size;
    u16        type;
} CFG_Header_st;

typedef struct APP_FILE_INFO_v1_
{
    CFG_Header_st   hdr;
    char            identifier[12];       // including '\0'
    u32             file_ver;

    u16             file_type;
    u8              flash_dev;
    u8              sig_type;

    u32             load_addr;
    u32             file_len;
    u32             max_size;
    u32             offset;
    u32             sig_len;
    u32             jump_offset;
    u32             attr;
} APP_FILE_INFO_v1;

typedef struct APP_BIN_INFO_v2_ {
    CFG_Header_st   hdr;
    u8              platform_id[128];
    u8              project_id[64];

    u32             ver;
    u32             combination;
    u32             combination_ex;
    u32             extsram_size;

    u32             minor_ver;
    u32             base_addr; /* For flash tool to check overlap region */
    u32             sds_len;       /* For flash tool to check overlap region */
    u32             ver_req;
    u32             addr;
    u32             len;
    u8              check[16];
    u32             reserved[12];
} APP_BIN_INFO_v2;

typedef struct
{
    APP_FILE_INFO_v1 file;
#if 0
    APP_BIN_INFO_v2  bin;
#endif
}APP_CFG_st;

#define CFG_HEADER(type, ver) { (APP_MAGIC|((ver)<<24)) , sizeof(type##_v##ver), type}

extern u32 Load$$APPCFG$$Base;
extern u32 Image$$APPCFG$$Length;

#define APP_ENTRY_FLAG  \
const APP_CFG_st app_cfg = \
{ \
    { \
        {0x014D4D4D,sizeof(APP_FILE_INFO_v1),0}, \
        "FILE_INFO", \
        1, \
        0x7001, \
        7, \
        0, \
        (u32)(&Load$$APPCFG$$Base), \
        0xffffffff, \
        0xffffffff, \
        (u32)(&Image$$APPCFG$$Length), \
        0, \
        0, \
        1 \
    } \
}; 

/*app bin config MACRO end*/

/*sem ID structure*/
typedef struct
{
    u8 unused;
} *EatSemId_st;


typedef enum {
    EAT_NO_WAIT, /*return immediately*/
    EAT_INFINITE_WAIT /*return only when get a semaphore value*/
} EatWaitMode_enum;



typedef enum   {
    EAT_EVENT_NULL = 0,
    EAT_EVENT_TIMER, /* timer time out*/
    EAT_EVENT_KEY, /* KEY*/
    EAT_EVENT_INT, /* GPIO interrupt*/
    EAT_EVENT_MDM_READY_RD, /* Read data from MODEM*/
    EAT_EVENT_MDM_READY_WR, /* Modem can receive the data*/
    EAT_EVENT_MDM_RI, /* Modem RI*/
    EAT_EVENT_UART_READY_RD, /* Received data from UART*/
    EAT_EVENT_UART_READY_WR, /* UART can receive the data*/
    EAT_EVENT_ADC, 
    EAT_EVENT_UART_SEND_COMPLETE, /* UART data send complete*/
    EAT_EVENT_USER_MSG, 
    EAT_EVENT_IME_KEY,/*The message of input method*/
#ifdef __SIMCOM_EAT_WMMP__ /*add wmmp user msg*/
    EAT_EVENT_TCPIP_STARTUP_CNF,
    EAT_EVENT_TCPIP_SHUTDOWN_CNF,
    EAT_EVENT_TCPIP_SHUTDOWN_IND,
    EAT_EVENT_TCPIP_CLOSE_CNF,
    EAT_EVENT_TCPIP_ACTIVE_OPEN_CNF,
    EAT_EVENT_TCPIP_TX_DATA_CNF,
    EAT_EVENT_TCPIP_TX_DATA_ENABLE_IND,
    EAT_EVENT_TCPIP_RX_DATA_REQ,
    EAT_EVENT_IMSI_READY,
    EAT_EVENT_NW_ATTACH_IND,
    EAT_EVENT_NEW_SMS_IND, /*new sms*/
#endif
#ifdef __SIMCOM_EAT_SOFT_SIM__
    EAT_EVENT_SIM_APDU_DATA_IND,
    EAT_EVENT_SIM_RESET_REQ,
#endif
    EAT_EVENT_SMS_SEND_CNF,   
    EAT_EVENT_AUD_PLAY_FINISH_IND, /*Play finish not in call*/
    EAT_EVENT_SND_PLAY_FINISH_IND, /*Play finish  in call*/
    EAT_EVENT_NUM
} EatEvent_enum; /* EVENT type */

/*enum value for user task*/
typedef enum {
    EAT_USER_0,
    EAT_USER_1,
    EAT_USER_2,
    EAT_USER_3,
    EAT_USER_4,
    EAT_USER_5,
    EAT_USER_6,
    EAT_USER_7,
    EAT_USER_8,
    EAT_USER_NUM
} EatUser_enum;

typedef struct {
    EatUser_enum src;
    eat_bool use_point;
    unsigned char len;
    unsigned char data[EAT_USER_MSG_MAX_SIZE];
    const void * data_p;
} EatUserMsg_st;

typedef union {
    EatTimer_st timer;
    EatKey_st key;
    EatInt_st interrupt;
    EatMdmRi_st mdm_ri;
    EatUart_st uart;
    EatAdc_st adc;
    EatUserMsg_st user_msg;
    WD_imeResult *ime_result;
#ifdef __SIMCOM_EAT_WMMP__ /*add wmmp user msg*/
    EatTcpipStartCnf_st tcpip_start_Cnf;
    EatTcpipShoutdownCnf_st tcpip_shutdown_cnf;
    EatTcpipShutdownInd_st   tcpip_shutdown_ind;
    EatTcpipCloseCnf_st    tcpip_close_cnf;
    EatTcpipActiveopenCnf_st  tcpip_activeopen_cnf;
    EatTcpipTxDataCnf_st  tcpip_tx_data_cnf;
    EatTcpipTxDataEnabledInd_st  tcpip_tx_data_enabled_ind;
    EatTcpipRxDataReq_st  tcpip_rx_data_req;
    EatNwAttachInd_st  nwAttachInd;
    EatSms_st sms;
#endif
#ifdef __SIMCOM_EAT_SOFT_SIM__
    EatAPDU_Tx_Data_st tx_apdu_data;
    SIMCARDRESET_REQ  sim_rst_req;
#endif
} EatEventData_union;

/* EVENT data that modem send to APP*/
typedef struct
{
    EatEvent_enum event;
    EatEventData_union data;
} EatEvent_st; 

typedef struct {
    eat_bool is_update_app; 
    eat_bool update_app_result; /* "update_app_result" is APP update result,only when "is_update_app" is EAT_TRUE*/
} EatEntryPara_st;

/* App entry function struct*/
typedef struct 
{
    void (*entry)(void *data); /* App entry function*/
    void (*func_ext1)(void *data); /* called in boot stage,initialize GPIO,UART and etc. */
    void (*entry_user1)(void *data);
    void (*entry_user2)(void *data);
    void (*entry_user3)(void *data);
    void (*entry_user4)(void *data);
    void (*entry_user5)(void *data);
    void (*entry_user6)(void *data);
    void (*entry_user7)(void *data);
    void (*entry_user8)(void *data);
    void (*func_ext2)(void *data);
    void (*func_ext3)(void *data);
    void (*func_ext4)(void *data);
    void (*func_ext5)(void *data);
    void (*func_ext6)(void *data);
    void (*func_ext7)(void *data);
} EatEntry_st;

/*The module enum*/
typedef enum 
{
    EAT_MODULE_MODEM,
    EAT_MODULE_UART
}EatEventEn_enum; 

/*****************************************************************************
* Function :eat_get_event
* Description:
*   The eat_get_event function gets system EVENTs from the core side tasks. When 
*   there is no event in customer task’s event queue, the task will enter suspend status.
* Parameters:
*   param1 EatEvent_st *event[IN/OUT]: pointer to get a particular event
* Returns:
*  
* NOTE
*      xxx
*****************************************************************************/
extern unsigned char (* const eat_get_event)(EatEvent_st *event);
#ifdef __SIMCOM_EAT_MULTI_APP__
extern unsigned char (* const eat_get_event_ext)(EatEvent_st *event);
#endif

/* 在Catcher上trace信息*/
extern void (* const eat_trace)(char *fmt,...);

/*****************************************************************************
* Function :eat_get_event_num
* Description:
*   get event number
* Parameters:
*  
* Returns:
*  unsigned int : current event number
* NOTE
*      this function can only be used in app_main() function
*****************************************************************************/
extern unsigned int (* const eat_get_event_num)();


/*****************************************************************************
* Function :eat_reset_module
* Description:
*   module reset
* Parameters:
*  
* Returns:
*  eat_bool:EAT_TRUE  reset module success   
*           EAT_FALSE reset module failed
* NOTE
*      xxx
*****************************************************************************/
extern eat_bool (* const eat_reset_module)();


/*****************************************************************************
* Function :eat_get_version
* Description:
*   get verion, return value is the same as AT+ATI
* Parameters:
*  
* Returns:
*  const char * : version
* NOTE
*      xxx
*****************************************************************************/
extern const char * (*const eat_get_version)();

/*****************************************************************************
* Function :eat_get_buildtime
* Description:
*   get core bin code build time
* Parameters:
*  
* Returns:
*  const char * : build time
* NOTE
*      xxx
*****************************************************************************/
extern const char * (*const eat_get_buildtime)();



/*****************************************************************************
* Function :eat_get_buildno
* Description:
*   get build inner version number,return value is the same as AT+CSUB
* Parameters:
*  
* Returns:
*  const char * : inner version number
* NOTE
*      xxx
*****************************************************************************/
extern const char * (*const eat_get_buildno)();

/*****************************************************************************
* Function : eat_get_chipid
* Description:
*   get chip id.
* Parameters:
*   buffer[OUT]: the point of the buffer that store chip id code.
*                The code is hex.
*   buf_len[IN]: the buffer size
* Returns:
*   int : the size of chipid code.
* NOTE
*    The buffer size should be greater than 16 bytes.
*****************************************************************************/
extern int (*const eat_get_chipid)(char* buffer, int buf_len);

/*****************************************************************************
* Function :eat_get_event_for_user
* Description:
*   use to get event in user task(app_user1,app_user2...app_user8),event is fixed,
*   it’s value is EAT_EVENT_USER_MSG,This function gets message sent by eat_send_msg_to_user()
* Parameters:
*  param1 EatUser_enum user[IN]:  current task user-ID
    (app_user_func)EAT_NULL,//app_user1,<==>EAT_USER_1
    (app_user_func)EAT_NULL,//app_user2,<==>EAT_USER_2
    (app_user_func)EAT_NULL,//app_user3,<==>EAT_USER_3
    (app_user_func)EAT_NULL,//app_user4,<==>EAT_USER_4
    (app_user_func)EAT_NULL,//app_user5,<==>EAT_USER_5
    (app_user_func)EAT_NULL,//app_user6,<==>EAT_USER_6
    (app_user_func)EAT_NULL,//app_user7,<==>EAT_USER_7
    (app_user_func)EAT_NULL,//app_user8,<==>EAT_USER_8
*  param2 EatEvent_st *event[IN]: event body    
* Returns:
*  eat_bool: EAT_FALSE,please check "param1 EatUser_enum user" 
             EAT_TRUE, get an event
* NOTE
*      xxx
*****************************************************************************/
extern eat_bool (* const eat_get_event_for_user)(EatUser_enum user, EatEvent_st *event);

/*****************************************************************************
* Function :eat_get_event_num_for_user
* Description:
*   get user task(app_user1,app_user2...app_user8) event number
* Parameters:
*  param1 EatUser_enum user [IN]:EatUser_enum
* Returns:
*  unsigned int : event number in current task's queue
* NOTE
*      xxx
*****************************************************************************/
extern unsigned int (* const eat_get_event_num_for_user)(EatUser_enum user);




/*****************************************************************************
* Function :eat_send_msg_to_user
* Description:
*  App_main is the system main task.Other eight tasks are user task , this interface 
*  is used to send messages between these tasks
const EatEntry_st AppEntry = 
{
    app_main,                           <==> EAT_USER_0
    app_func_ext1,
    (app_user_func)EAT_NULL,//app_user1,<==>EAT_USER_1
    (app_user_func)EAT_NULL,//app_user2,<==>EAT_USER_2
    (app_user_func)EAT_NULL,//app_user3,<==>EAT_USER_3
    (app_user_func)EAT_NULL,//app_user4,<==>EAT_USER_4
    (app_user_func)EAT_NULL,//app_user5,<==>EAT_USER_5
    (app_user_func)EAT_NULL,//app_user6,<==>EAT_USER_6
    (app_user_func)EAT_NULL,//app_user7,<==>EAT_USER_7
    (app_user_func)EAT_NULL,//app_user8,<==>EAT_USER_8

};
* Parameters:
*  param1 EatUser_enum user_src[IN]: Sending messages task ID, corresponding relation as above,
*  app_user1 corresponding EAT_USER_1... 
*  param2 EatUser_enum user_dst[IN]: Receiving messages task ID, corresponding relation with the same user_src 
*  eat_bool use_point: If the value is EAT_TRUE, user apply space to keep news content, message 
*                      data directly use const unsigned char * * data_p space;
*                      If the value is EAT_FALSE, use function internal space to save news content, 
*                      max size is EAT_USER_MSG_MAX_SIZE-1 byte, more than EAT_USER_MSG_MAX_SIZE-1 byte of data will be discarded off.

* param3 unsigned char len[IN]：  message data length
* param4 const unsigned char *data[IN]：message data body parts, if eat_bool use_point EAT_FALSE, function will 
*                      copy pointer space data, or will not use it
* param5 const unsigned char **data_p[IN]: message data section, if eat_bool use_point EAT_TRUE, function will use the pointer , so user should apply and release the space
* Returns:
*  eat_bool ：EAT_TRUE: message send success
*             EAT_FALSE: message send failed,please check whether user_src user_dst is valid
* NOTE
*      xxx
*****************************************************************************/
extern eat_bool (* const eat_send_msg_to_user)(EatUser_enum user_src, EatUser_enum user_dst, eat_bool use_point, unsigned char len, const unsigned char *data, const unsigned char **data_p);

 
/*****************************************************************************
* Function :eat_sleep_enable
* Description:
*   enable module to enter sleep mode
* Parameters:
*   param1 eat_bool en[IN]: EAT_TRUE   enable module to enter sleep mode
*                           EAT_FALSE  disable module to enter sleep mode
* Returns:
*  eat_bool en: EAT_TRUE
* NOTE
*      xxx
*****************************************************************************/ 
extern eat_bool (*const eat_sleep_enable)(eat_bool en);

/*****************************************************************************
* Function :eat_create_sem
* Description:
*   create semaphore
* Parameters:
*   param1 unsigned char *sem_name_ptr[IN]: semaphore name
*   param2 int initial_count[IN]: default semaphore value
* Returns:
*  EatSemId_st: semaphore id 
* NOTE
*      xxx
*****************************************************************************/
extern EatSemId_st (* const eat_create_sem)(unsigned char *sem_name_ptr, int initial_count);

/*****************************************************************************
* Function :eat_sem_get
* Description:
*   get semaphore
* Parameters:
*  param1 EatSemId_st sem_id id [IN], created by eat_create_sem()
*  param2 EatWaitMode_enum wait_mode [IN]: wait mode
* Returns:
*  eat_bool: EAT_TRUE get one semaphore
*            EAT_FALSE get semaphore failed,the reason may be invalid sem_id or 
*            current semaphore value equal zero
* NOTE
*      xxx
*****************************************************************************/
extern eat_bool (* const eat_sem_get)(EatSemId_st sem_id, EatWaitMode_enum wait_mode);

/*****************************************************************************
* Function :eat_sem_put
* Description:
*   add semaphore value by one
* Parameters:
*  param1 EatSemId_st sem_id[IN]: semaphore id
* Returns:
*  
* NOTE
*      xxx
*****************************************************************************/
extern void (* const eat_sem_put)(EatSemId_st sem_id);

/*****************************************************************************
* Function :eat_sem_query
* Description:
*   query current semaphore value
* Parameters:
*  param1 EatSemId_st sem_id[IN]: semaphore id
* Returns:
*  int: semaphore value
* NOTE
*      xxx
*****************************************************************************/
extern int (* const eat_sem_query)(EatSemId_st sem_id);


/*****************************************************************************
* Function :eat_power_down
* Description:
*   This function turns off the system. It has the same effect as the AT command 
*   “AT+CPOWD=1”. When the system is powered down successfully, “NORMAL POWER DOWN” 
*   will be sent to the serial port.
* Parameters:
*  
* Returns:
*  
* NOTE
*      xxx
*****************************************************************************/
extern void (* const eat_power_down)(void);

/*****************************************************************************
* Function : eat_event_en
* Description:
*            Set the EVENT notifying enable or disable
* Parameters:
*    module : EatEventEn_enum, the moudle that will be set
*    en     : eat_bool, enable or disable EVENT notifying
*    param  : void*, extra parameter
*             For example, if the module is UART, the param is EAT_UART_x.
*             If the module is Modem, the param is NULL
* Returns:
*      void
*****************************************************************************/
extern void (*const eat_event_en)(EatEventEn_enum module, eat_bool en, void* param);

/*****************************************************************************
* Function : eat_query_event_en
* Description:
* Parameters:
*   module : EatEventEn_enum
*   param  :
* Returns:
*  eat_bool: EAT_TRUE The EVENT notifying is enable
*            EAT_FALSE The EVENT notifying is disable
*****************************************************************************/
extern eat_bool (*const eat_query_event_en)(EatEventEn_enum module, void* param);

#endif

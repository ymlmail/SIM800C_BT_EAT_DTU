#ifndef _APP_CUSTOM_H_
#define _APP_CUSTOM_H_

#include "platform.h"
extern u8 NoUsedPin;
/**************Pin define begin***********************/
#if 1

#define KEY5_PIN	EAT_PIN6_UART1_DTR  /* GPIO, EINT, UART1_DTR */ 
#define KEY2_PIN	EAT_PIN5_UART1_DCD  /* GPIO, UART1_DCD */
#define KEY3_PIN	EAT_PIN3_UART1_RTS  /* GPIO, UART1_CTS */  //作为物理逻辑上与PIN3是反的。

//#define KEY4_PIN	EAT_PIN_NUM		//KEY用poweron 键代替
#define KEY1_PIN	EAT_PIN14_SIM_DET /* GPIO, EINT, SIM_DET */
//#define KEY6_PIN	EAT_PIN_NULL

#define SHUT_PIN	EAT_PIN4_UART1_CTS  /* GPIO, UART1_RTS */  //作为物理逻辑上与PIN4是反的。
#define MOTOR_PIN	EAT_PIN7_UART1_RI  /* GPIO, UART1_RI */
#define GPSPOWER_PIN	EAT_PIN42_STATUS  /* GPIO, UART1_RI */

//#define KEY1_PIN	EAT_PIN4_UART1_CTS  /* GPIO, UART1_CTS */
//#define KEY2_PIN	EAT_PIN3_UART1_RTS  /* GPIO, UART1_RTS */
//#define KEY3_PIN	EAT_PIN5_UART1_DCD  /* GPIO, UART1_DCD */
//#define KEY4_PIN	EAT_PIN6_UART1_DTR  /* GPIO, EINT, UART1_DTR */ 
//#define KEY5_PIN	EAT_PIN7_UART1_RI  /* GPIO, UART1_RI */
//#define KEY6_PIN	EAT_PIN14_SIM_DET /* GPIO, EINT, SIM_DET */
#else
#define KEY1_PIN	EAT_PIN6_UART1_DTR  /* GPIO, EINT, UART1_DTR */ 
#define KEY2_PIN	EAT_PIN14_SIM_DET /* GPIO, EINT, SIM_DET */
#define KEY3_PIN	EAT_PIN4_UART1_CTS  /* GPIO, UART1_CTS */
#define KEY4_PIN	EAT_PIN5_UART1_DCD  /* GPIO, UART1_DCD */

#define KEY5_PIN	NoUsedPin
#define KEY6_PIN	NoUsedPin

//备注powerkey用着开机和关机键，同时用着挂断键
//EAT_PIN7_UART1_RI  /* GPIO, UART1_RI */ use to conctrol motor 
//EAT_PIN3_UART1_RTS  /* GPIO, UART1_RTS */ use to amp
//status pin use to conctrol GPS module
#endif
/**************Pin define end************************/

/**************Timer define begin*********************/

/**************Timer define end**********************/

/**************UART function define begin**************/
//#define eat_uart_app EAT_UART_1
//#define app_uart_at EAT_UART_NULL
//#define app_uart_debug EAT_UART_1

#define eat_uart_app	EAT_UART_USB//EAT_UART_1
#define app_uart_debug	EAT_UART_USB
#define eat_uart_GPS	EAT_UART_2	//2015-10-17  add gps/bd导航


/**************UART function define end***************/

/**************Modem configuration define begin*********/
#define APN_NAME_LEN						20
#define APN_USER_NAME_LEN					20
#define APN_PASSWORD_LEN					20
#define SERVER_IP_LEN				        20
#define FTP_SERVER_USER_NAME_LEN			20
#define FTP_PASSWORD_LEN					20
#define FTP_FILENAME_LEN					20
#define FTP_FILEPATH_LEN					20
#define FTP_PORT_LEN						5

#define MAX_SMS_STRING_LEN          160 
#define MASTER_MOBILE_NUMBER_LEN    24

typedef struct ModemConfigContextTag
{
	ascii apnName[APN_NAME_LEN+1];
	ascii apnUserName[APN_USER_NAME_LEN+1];
	ascii apnPassword[APN_PASSWORD_LEN+1];
	ascii FTPServerIP[SERVER_IP_LEN+1];
	ascii ftpUserName[FTP_SERVER_USER_NAME_LEN+1];
	ascii ftpPassword[FTP_PASSWORD_LEN+1];
  	ascii ftpFileName[FTP_FILENAME_LEN+1];
	ascii ftpFilePath[FTP_FILEPATH_LEN+1];
	u16 ftpPort;
}ModemConfigContext;

typedef enum
{
    PDU,        
    TEXT
} SMS_FORMAT;

typedef struct
{
    SMS_FORMAT formatType;
	ascii sc_number[MASTER_MOBILE_NUMBER_LEN+1];
    u8 phone_number[MASTER_MOBILE_NUMBER_LEN+1];
    u8 sms_string[MAX_SMS_STRING_LEN];    
    u8 msg_len;
    u8 mt;      //mt = 2, flash message
} SIMCOM_SMS_INFO;

extern ModemConfigContext g_modemConfig;
/**************Modem configuration define end**********/

void GsmInitCallback(eat_bool result);
void askCregCallback(eat_bool result);
void askCsqCallback(eat_bool result);

extern void app_user2(void *data);
#endif //_APP_CUSTOM_H_

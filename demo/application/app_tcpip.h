/*
 * =====================================================================================
 *
 *       Filename:  app_tcpip.h
 *
 *    Description:  the head file
 *
 *        Version:  1.0
 *        Created:  2014-6-11 15:58:20
 *       Revision:  none
 *       Compiler:  
 *
 *         Author:  weijiang.he@sim.com
 *   Organization:  www.sim.com
 *
 * =====================================================================================
 */

#if !defined APP_TCPIP_H
#define APP_TCPIP_H
#include "App_include.h"


/* #####   HEADER FILE INCLUDES   ################################################### */
#include "eat_socket.h"

/* #####   EXPORTED MACROS   ######################################################## */

/* #####   EXPORTED TYPE DEFINITIONS   ############################################## */
typedef void (*ResultNotifyCb)(eat_bool result);

/* #####   EXPORTED DATA TYPES   #################################################### */

/* #####   EXPORTED VARIABLES   ##################################################### */

/* #####   EXPORTED FUNCTION DECLARATIONS   ######################################### */
void simcom_gprs_start(u8* apn,u8* userName,u8* password);
s8 simcom_create_server(u16 port);
#if UseATCOMMON
s8 simcom_connect_server(sockaddr_struct *addr,ResultNotifyCb pResultCb);
#else
s8 simcom_connect_server(sockaddr_struct *addr);
#endif
s32 simcom_send_to_server(s8 sid, const void *buf, s32 len);
s32 simcom_recv_from_server(s8 sid, void *buf, s32 len);
eat_bool simcom_gethostbyname(const char *domain_name);

void simcom_tcpip_test(void);
extern void App_connect_server(void);
#endif

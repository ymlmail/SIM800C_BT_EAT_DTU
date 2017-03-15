
//yemaolin creat 2015-7-30
#ifndef __APP_LOGIC_H__
#define __APP_LOGIC_H__
#include "App_include.h"

extern void AppGetEventHandle(void);
extern void AppLogicHandle(void);

extern void SetCregValue(void);
extern void SetCENGValue(void);//工程模式开启
extern void eat_event_user0_msg_handler(u8* pMsgStr, u16 len);
extern void  AppMainRunOnce(void);
#endif



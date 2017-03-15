#ifndef _USER_COMMUNICATION_H_
#define _USER_COMMUNICATION_H_

#include "platform.h"
extern u16 GetCrc16(const u8* pData, int nLength);
extern void LoginMsgPackage(void);//登录信息封装
extern void LBSPackage(void);//LBS信息封装
extern void StatePackage(void);//上报状态信息
extern void SeeSosCmdPackage(void);//上报sos号码信息
#endif

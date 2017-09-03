//yemaolin creat 2015-7-29
#ifndef __APP_INIT_H__
#define __APP_INIT_H__
#include "Platform.h"
extern void App_initTimer(void);

extern void GlobalVariableInit(void);
extern void ReadNvramData(void);
#if EnableGPSModule
extern void intGpsModule(void);
#endif
#endif //__APP_INIT_H__
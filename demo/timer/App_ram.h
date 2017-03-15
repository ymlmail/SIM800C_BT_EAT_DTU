#ifndef _APP_RAM_H
#define _APP_RAM_H
//结构体定义
#include "Platform.h"
//结构体定义

typedef union T_UNION
{
    u8 Byte;
    struct
    {
        u8 B0:1;
        u8 B1:1;
        u8 B2:1;
        u8 B3:1;
        u8 B4:1;
        u8 B5:1;
        u8 B6:1;
        u8 B7:1;
    } Bit;
} T_UNION;

typedef struct
{

    u8 timer1up:1;
    u8 :1;
    u8 :1;
    u8 :1;
    u8 :1;
    u8 :1;
    u8 :1;
    u8 :1;

}T_FLAG;


typedef struct{

    u8 phonetime:1;
    u8 :1;
    u8 :1;
    u8 :1;
    u8 :1;
    u8 :1;
    u8 :1;
    u8 :1;

}T_DELAY;

typedef struct
{
	u8  key1 :1;
	u8  key2 :1;
	u8  key3 :1;
	u8  key4 :1;
	u8  key5 :1;
	u8  key6 :1;
	u8  key7 :1;
	u8  key8 :1;
}T_KEY;

extern T_FLAG  flag;
extern T_DELAY delay;
extern T_KEY   key;
extern u8 buf[2048];
#endif


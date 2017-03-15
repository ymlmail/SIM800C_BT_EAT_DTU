#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eat_interface.h"

/*typedef*/
typedef unsigned char u8;
typedef signed char s8;
typedef signed char ascii;

typedef unsigned short int u16;
typedef signed short int s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;

typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short int uint16;
typedef signed short int int16;
typedef unsigned int uint32;
typedef signed int int32;

#define FALSE 0
#define TRUE 1

typedef unsigned char bool;
typedef unsigned char Boolean;


#define NULL 0

typedef void (*func_ptr) (void*);

#define ASSERT(expr) assert_internal(expr, __FILE__, __LINE__)
extern void assert_internal(bool expr, const char *file, int line);


#define EAT_UART_RX_BUF_LEN_MAX (2000) 

#define TRACE_DEBUG(...) eat_trace(__VA_ARGS__)

#endif


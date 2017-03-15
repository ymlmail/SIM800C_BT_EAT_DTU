/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   app_demo_timer.c
 * version       :   0.10
 * Description   :   
 * Authors       :   maobin
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2012-09-24, maobin, create originally.
 *
 *--------------------------------------------------------------------
 * File Description
 * AT+CEAT=parma1,param2
 * param1 param2 
 *   1      1    Test millisecond level timer, Start timer1 timer2 timer3
 *   1      2    Stop timer1, timer2, timer3
 *
 *   2      1    Test microsecond level timer, Start gpt timer
 *   2      2    Stop gpt timer
 *   
 *   3      1    Get elapsed time from the boot to the present
 *   3      2    Get time interval with the last time
 *
 *   4      1    Test RTC, get current rtc value
 *   4      2    Set rtc value
 *
 *--------------------------------------------------------------------
 ********************************************************************/
/********************************************************************
 * Include Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>
#include "eat_modem.h"
#include "eat_interface.h"
#include "eat_uart.h"
#include "eat_timer.h" 
#include "eat_clib_define.h" //only in main.c
//APP Include Files
#include "App_include.h"
/********************************************************************
 * Macros
 ********************************************************************/



/********************************************************************
 * Types
 ********************************************************************/
typedef void (*app_user_func)(void*);

/********************************************************************
 * Extern Variables (Extern /Global)
 ********************************************************************/
 
/********************************************************************
 * Local Variables:  STATIC
 ********************************************************************/
static u32 gpt_timer_counter = 0;
static u32 time1 = 0;
/********************************************************************
 * External Functions declaration
 ********************************************************************/
extern void APP_InitRegions(void);

/********************************************************************
 * Local Function declaration
 ********************************************************************/
void app_main(void *data);
void app_func_ext1(void *data);
/********************************************************************
 * Local Function
 ********************************************************************/
#pragma arm section rodata = "APP_CFG"
APP_ENTRY_FLAG 
#pragma arm section rodata

#pragma arm section rodata="APPENTRY"
	const EatEntry_st AppEntry = 
	{
		app_main,
		app_func_ext1,
		(app_user_func)EAT_NULL,//app_user1,
		(app_user_func)EAT_NULL,//app_user2,
		(app_user_func)EAT_NULL,//app_user3,
		(app_user_func)EAT_NULL,//app_user4,
		(app_user_func)EAT_NULL,//app_user5,
		(app_user_func)EAT_NULL,//app_user6,
		(app_user_func)EAT_NULL,//app_user7,
		(app_user_func)EAT_NULL,//app_user8,
		EAT_NULL,
		EAT_NULL,
		EAT_NULL,
		EAT_NULL,
		EAT_NULL,
		EAT_NULL
	};
#pragma arm section rodata

void app_func_ext1(void *data)
{
	/*This function can be called before Task running ,configure the GPIO,uart and etc.
	   Only these api can be used:
		 eat_uart_set_debug: set debug port
		 eat_pin_set_mode: set GPIO mode
		 eat_uart_set_at_port: set AT port
	*/

    
	#if UsbAsPort
	eat_uart_set_debug(EAT_UART_USB);
	eat_uart_set_debug_config(EAT_UART_DEBUG_MODE_UART, NULL);
	//配置USB口做调试口，需要打印ASCII时需要把上面这句也配置下
	#else	//uart2作为debuge输出口
	EatUartConfig_st cfg =
    {
        EAT_UART_BAUD_115200,
        EAT_UART_DATA_BITS_8,
        EAT_UART_STOP_BITS_1,
        EAT_UART_PARITY_NONE
    };
	eat_uart_set_debug(EAT_UART_1);
	eat_uart_set_debug_config(EAT_UART_1, &cfg);
	#endif
	
    eat_uart_set_at_port(EAT_UART_2);// UART1 is as AT PORT
}
eat_bool eat_modem_data_parse(u8* buffer, u16 len, u8* param1, u8* param2)
{
    eat_bool ret_val = EAT_FALSE;
    u8* buf_ptr = NULL;
    /*param:%d,extern_param:%d*/
     buf_ptr = (u8*)strstr((const char *)buffer,"param");
    if( buf_ptr != NULL)
    {
        sscanf((const char *)buf_ptr, "param:%d,extern_param:%d",(int*)param1, (int*)param2);
        eat_trace("data parse param1:%d param2:%d",*param1, *param2);
        ret_val = EAT_TRUE;
    }
    return ret_val;
}
void eat_gpt_cb_fun(void)
{
    gpt_timer_counter ++;
}

/****************************************************
 * Timer testing module
 *****************************************************/
eat_bool eat_module_test_timer(u8 param1, u8 param2)
{
    eat_bool result = EAT_TRUE;
    /***************************************
     * example 1
     * Start and stop 3 timers
     ***************************************/
    if( 1 == param1 )
    {

        if( 1 == param2 )
        {
            eat_timer_start(EAT_TIMER_1, 1000);
            eat_timer_start(EAT_TIMER_2, 2000);
            eat_timer_start(EAT_TIMER_3, 3000);
            eat_trace("Timer test 1, timer1,2,3 start");
        }else if( 2 == param2)
        {
            eat_timer_stop(EAT_TIMER_1);
            eat_timer_stop(EAT_TIMER_2);
            eat_timer_stop(EAT_TIMER_3);
            eat_trace("Timer test 1, timer1,2,3 stop");
        }
    }
    /***************************************
     * example 2
     * Start and stop gpt timer
     ***************************************/
    else if( 2 == param1 )
    {
        if( 1 == param2)
        {
            //clear counter
            gpt_timer_counter = 0;
            //start gpt timer, auto-repeat, (61.035*1000)us
            eat_gpt_start(16384/*1024*/,EAT_TRUE, eat_gpt_cb_fun);
            eat_trace("Timer test 2, GPT timer start");
        }else
        {
            //stop gpt timer
            eat_gpt_stop();
            eat_trace("Timer test 2, GPT timer counter:%d",gpt_timer_counter);
        }

    }
    /***************************************
     * example 3
     * Get time interval	间隔时间
     ***************************************/
    else if( 3 == param1 )
    {
        u32 time2 = 0;
        if( 1 == param2)
        {
            time1 = eat_get_current_time();
        }else if( 2 == param2 )
        {
            time2 = eat_get_duration_ms(time1);
            eat_trace("Timer test 3, interval time(ms):%d",time2);
            time2 = eat_get_duration_us(time1);
            eat_trace("Timer test 3, interval time(us):%d",time2);
        }
    }
    /***************************************
     * example 4
     * RTC test
     ***************************************/
    else if( 4 == param1 )
    {
        EatRtc_st rtc = {0};
        if( 1 == param2)
        {
            //get rtc value
            result = eat_get_rtc(&rtc);
            if( result )
            {
                eat_trace("Timer test 4 get rtc value:%d/%d/%d %d:%d:%d",
                        rtc.year,rtc.mon,rtc.day,rtc.hour,rtc.min,rtc.sec);
            }else
            {
                eat_trace("Timer test 4 get rtc fail");
            }
        }else if( 2 == param2 )
        {
            rtc.year = 12;
            rtc.mon = 10;
            rtc.day = 10;
            rtc.hour = 11;
            rtc.min = 11;
            rtc.sec = 59;
            //set rtc 
            result = eat_set_rtc(&rtc);
            if( result )
            {
                eat_trace("Timer test 4 rtc set succeed");
            }else
            {
                eat_trace("Timer test 4 rtc set fail");
            }
        }
    }else if( 5 == param1)
    {
        if( 0 == param2)
        {
            eat_sleep_enable(EAT_FALSE);
        }else
        {
            eat_sleep_enable(EAT_TRUE);
        }
    }
    return EAT_TRUE;
}


void app_main(void *data)
{

    APP_InitRegions();//Init app RAM, first step
    APP_init_clib(); //C library initialize, second step
	App_initTimer();
	#if DebugMsgOnOff
    eat_trace(" app_main ENTRYhelllll");
	#endif

	while(TRUE)
	{
	AppGetEventHandle();
	AppLogicHandle();
	}

}



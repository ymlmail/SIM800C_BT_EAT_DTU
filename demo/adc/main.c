/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   main.c
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
 *   
 *--------------------------------------------------------------------
 ********************************************************************/
/********************************************************************
 * Include Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "eat_modem.h"
#include "eat_interface.h"
#include "eat_uart.h"

#include "app_demo_adc.h"
#include "eat_clib_define.h" //only in main.c
/********************************************************************
 * Macros
 ********************************************************************/

/********************************************************************
 * Types
 ********************************************************************/
typedef void (*app_user_func)(void*);
typedef void (*event_handler_func)(const EatEvent_st *event);

/********************************************************************
 * Extern Variables (Extern /Global)
 ********************************************************************/
 
/********************************************************************
 * Local Variables:  STATIC
 ********************************************************************/
static EatEntryPara_st app_para={0};
static event_handler_func EventHandlerTable[EAT_EVENT_NUM] = {0};
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
	eat_uart_set_debug(EAT_UART_USB);
    eat_uart_set_at_port(EAT_UART_1);// UART1 is as AT PORT
}


void event_register_handler(EatEvent_enum event, event_handler_func handler)
{ 
    EventHandlerTable[event] = handler;
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

//Read data from Modem
void mdm_rx_proc(void)
{
    u8 buf[2048];
    u8 param1,param2;
    u16 len = 0;
    len = eat_modem_read(buf, 2048);
    if(len > 0)
    {
        //Get the testing parameter
        if(eat_modem_data_parse(buf,len,&param1,&param2))
        {
            eat_module_test_adc(param1, param2);
        }
        else
        {
            eat_trace("From Mdm:%s",buf);
        }
    }

}

void app_main(void *data)
{
    EatEvent_st event;
    EatEntryPara_st *para;
    event_handler_func func = NULL;
    APP_InitRegions();//Init app RAM, first step
    APP_init_clib(); //C library initialize, second step

    para = (EatEntryPara_st*)data;

    memcpy(&app_para, para, sizeof(EatEntryPara_st));
    eat_trace(" App Main ENTRY  update:%d result:%d", app_para.is_update_app,app_para.update_app_result);
    if(app_para.is_update_app && app_para.update_app_result)
    {
        eat_update_app_ok();
    }

    eat_trace(" app_main ENTRY");
    //event_register_handler(EAT_EVENT_TIMER, timer_proc);
    //event_register_handler(EAT_EVENT_KEY, key_proc);
    event_register_handler(EAT_EVENT_MDM_READY_RD, (event_handler_func)mdm_rx_proc);
    //event_register_handler(EAT_EVENT_MDM_READY_WR, mdm_tx_proc);
    //event_register_handler(EAT_EVENT_INT, int_proc);
    //event_register_handler(EAT_EVENT_UART_READY_RD, uart_rx_proc);
    //event_register_handler(EAT_EVENT_UART_READY_WR, uart_tx_proc);
    //event_register_handler(EAT_EVENT_UART_SEND_COMPLETE, uart_send_complete_proc);
    //event_register_handler(EAT_EVENT_USER_MSG, user_msg_proc);
    event_register_handler(EAT_EVENT_ADC, (event_handler_func)adc_event_proc);
    
    while(EAT_TRUE)
    {
        eat_get_event(&event);
        eat_trace("%s-%d:msg %x", __FILE__, __LINE__,event.event);
        func = EventHandlerTable[event.event];
        if(event.event < EAT_EVENT_NUM && func != NULL)
        {
            (*func)(&event);
        }
    }
}


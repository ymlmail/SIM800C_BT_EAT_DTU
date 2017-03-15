/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   main.c
 * version       :   0.10
 * Description   :   
 * Authors       :   dingfen.zhu
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2015-04-24, dingfen.zhu, create originally.
 *
 *--------------------------------------------------------------------
 * File Description
 * AT+CEAT=parma1,param2
 * param1 param2 
 *   1      1    
 *   1      2    
 * *
 *--------------------------------------------------------------------
 ********************************************************************/
/********************************************************************
 * Include Files
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eat_modem.h"
#include "eat_interface.h"
#include "eat_uart.h"
#include "eat_clib_define.h" //only in main.c

#include "app_at_cmd_envelope.h"

/********************************************************************
* Macros
 ********************************************************************/
#define EAT_MEM_MAX_SIZE 310*1024
#define EAT_UART_RX_BUF_LEN_MAX 2048

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
static u8 rx_buf[EAT_UART_RX_BUF_LEN_MAX + 1] = {0};
static u8 buf[EAT_UART_RX_BUF_LEN_MAX + 1] = {0};//for receive data from uart
static u8 s_memPool[EAT_MEM_MAX_SIZE]; 

/********************************************************************
* External Functions declaration
 ********************************************************************/
extern void APP_InitRegions(void);
extern eat_bool eat_module_test_http(u8 param1, u8 param2);

/********************************************************************
* Local Function declaration
 ********************************************************************/
void app_main(void *data);
void app_func_ext1(void *data);
static eat_bool app_mem_init();
static eat_bool eat_modem_data_parse(u8* buffer, u16 len, u8* param1, u8* param2);

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
    (app_user_func)app_at_cmd_envelope,//app_user1,
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
		 eat_uart_set_at: set AT port
	*/
	eat_uart_set_debug(EAT_UART_USB);
    eat_uart_set_at_port(EAT_UART_1);// UART1 is as AT PORT
    eat_uart_set_debug_config(EAT_UART_DEBUG_MODE_UART, NULL);
    //eat_modem_set_poweron_urc_dir(EAT_USER_1);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: app_mem_init
 *  Description: init mem for this app to malloc
 *        Input:
 *       Output:
 *       Return:
 *       author: dingfen.zhu
 * =====================================================================================
 */
static eat_bool app_mem_init(void)
{
    eat_bool ret = EAT_FALSE;
	
    ret = eat_mem_init(s_memPool, EAT_MEM_MAX_SIZE);
    if(!ret)
    {
        eat_trace("ERROR: eat memory initial error!");
    }
    return ret;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: uart_rx_proc
 *  Description: 
 *        Input:
 			data:
 *       Output:
 *       Return:
 *       author: dingfen.zhu
 * =====================================================================================
 */
static void uart_rx_proc(const EatEvent_st* event)
{
    u16 len;
    EatUart_enum uart = event->data.uart.uart;
	
    len = eat_uart_read(uart, rx_buf, EAT_UART_RX_BUF_LEN_MAX);
    if(len != 0)
    {
		rx_buf[len] = '\0';
		eat_trace("[%s] uart(%d) rx: %s", __FUNCTION__, uart, rx_buf);

		if (uart == EAT_UART_1)
		{
			eat_modem_write(rx_buf, len);
		}
		else
		{
			eat_uart_write(uart, rx_buf, len);
		}
    }
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: app_main
 *  Description: 
 *        Input:
 			data:
 *       Output:
 *       Return:
 *       author: dingfen.zhu
 * =====================================================================================
 */
void app_main(void *data)
{
    EatEvent_st event;
    u16 len = 0;
	EatUartConfig_st uart_config;
    
    APP_InitRegions();//Init app RAM
    APP_init_clib();
	
    if(eat_uart_open(EAT_UART_1) == EAT_FALSE)
    {
	    eat_trace("[%s] uart(%d) open fail!", __FUNCTION__, EAT_UART_1);
    }
	uart_config.baud = EAT_UART_BAUD_115200;
    uart_config.dataBits = EAT_UART_DATA_BITS_8;
    uart_config.parity = EAT_UART_PARITY_NONE;
    uart_config.stopBits = EAT_UART_STOP_BITS_1;	
    if(EAT_FALSE == eat_uart_set_config(EAT_UART_1, &uart_config))
    {
        eat_trace("[%s] uart(%d) set config fail!", __FUNCTION__, EAT_UART_1);
    }
	
	app_mem_init();
	eat_trace(" app_main ENTRY");
    while(EAT_TRUE)
    {	    eat_trace("+++12121+main+++");
		//eat_sleep(1);
        eat_get_event(&event);
        eat_trace("MSG id%x", event.event);
        switch(event.event)
        {
            case EAT_EVENT_MDM_READY_RD:
            {
                u8 param1,param2;
                len = 0;
                len = eat_modem_read(buf, 2048);
                if(len > 0)
                {
                    buf[len] = '\0';
                    //do something
                }
		break;
            }
            case EAT_EVENT_MDM_READY_WR:
            case EAT_EVENT_UART_READY_RD:
			{
				uart_rx_proc(&event);
                break;
            }
            case EAT_EVENT_UART_SEND_COMPLETE :
			{
                break;
            }
            default:
			{
                break;
            }
        }
    }
}

/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   app_demo_uart.c
 * version       :   0.10
 * Description   :   
 * Authors       :   Maobin
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2012-09-24, Maobin, create originally.
 *
 *--------------------------------------------------------------------
 *  Description:
 *   UART1: app channel. 
 *   UART2: Debug port
 *   UART3: AT command channel
 *          In this example,APP gets AT cmd data from UART3,and sends it to Modem,
 *          then gets response message from Modem and sends to UART3.
 *  Data flow graph:
 *         "AT" 
 *   UART1 --->APP ---> Modem AT
 *     |             "OK" |
 *      <---- APP <-------
 *
 *            "AT"
 *   UART3 <=======> Modem AT
 *            "OK"
 **************************************************************************/

/******************************************************************************
 *  Include Files
 ******************************************************************************/
//#define EAT_CORE_APP_INDEPEND
#include <string.h>
#include <stdio.h>

#include "eat_modem.h"
#include "eat_interface.h"
#include "eat_uart.h"

#include "eat_clib_define.h" //only in main.c
/********************************************************************
 * Macros
 ********************************************************************/
#define EAT_AT_TEST
//#define EAT_DEBUG_STRING_INFO_MODE //output debug string info to debug port
//#define EAT_TCP_MULIT_CHANNEL  //tcp multi channels

#define EAT_UART_RX_BUF_LEN_MAX 2048
#define EAT_BUF_LEN_MAX 1024*10


/********************************************************************
 * Types
 ********************************************************************/
typedef void (*app_user_func)(void*);

typedef struct 
{
    char* cmd;
    char* ok;
}eat_at_st;

eat_at_st init_at[]=
{
    {"ATE0\r\n","OK"},
    #ifdef EAT_TCP_MULIT_CHANNEL
    {"AT+CIPMUX=1\r\n","OK"},
    #else
    {"AT+CIPMUX=0\r\n","OK"},
    {"AT+CIPHEAD=1\r\n","OK"},
    #endif
    {"AT+CSTT=\"CMNET\"\r\n","OK"},
    {"AT+CIICR\r\n","OK"},
    {"AT+CGREG?\r\n","OK"},
    {"AT+CIFSR\r\n",""},
    #ifdef EAT_TCP_MULIT_CHANNEL
//    {"AT+CIPSTART=0,\"TCP\",\"180.166.164.118\",5107\r\n","OK"}
//    {"AT+CIPSTART=0,\"TCP\",\"222.79.65.146\",8000\r\n","OK"}
    {"AT+CIPSTART=0,\"TCP\",\"222.79.65.146\",8090\r\n","OK"}
    #else
    {"AT+CIPSTART=\"TCP\",\"222.79.65.146\",8000\r\n","OK"}
    #endif
};

/********************************************************************
 * Extern Variables (Extern /Global)
 ********************************************************************/
 
/********************************************************************
 * Local Variables:  STATIC
 ********************************************************************/
static u8 rx_buf[EAT_UART_RX_BUF_LEN_MAX + 1] = {0};
static u8 g_buf[EAT_BUF_LEN_MAX ] = {0};
static u16 wr_uart_offset = 0; //用来标记uart未写完的数据
static u16 wr_uart_len = 0; //下次需要往串口写的数据长度

static const EatUart_enum eat_uart_app= EAT_UART_1;
static const EatUart_enum eat_uart_debug = EAT_UART_2;
//static const EatUart_enum eat_uart_at = EAT_UART_3;

/********************************************************************
 * External Functions declaration
 ********************************************************************/
extern void APP_InitRegions(void);

/********************************************************************
 * Local Function declaration
 ********************************************************************/
void app_main(void *data);
void app_func_ext1(void *data);

static void mdm_rx_proc(const EatEvent_st* event);
static void uart_rx_proc(const EatEvent_st* event);
static void uart_send_complete_proc(const EatEvent_st* event);
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
/* C lib function begin*/
#ifdef EAT_CORE_APP_INDEPEND
__value_in_regs struct __initial_stackheap $Sub$$__user_initial_stackheap(unsigned R0, unsigned SP, unsigned R2, unsigned SL)
{
   struct __initial_stackheap config;
 
   config.heap_base = R0;
   config.heap_limit =  R2;
   config.stack_base =  SP; 
   config.stack_limit =   SL;
   return config;
}
void $Sub$$_fp_init(void)
{
}
void $Sub$$_initio(void)
{
}
__value_in_regs struct __argc_argv $Sub$$__ARM_get_argv(void *a )
{
    /*
    int argc;
    char **argv;
    int r2, r3;
    */
    struct __argc_argv arg;
    arg.argc = 0;
    arg.r2 = 0;
    arg.r3 = 0;
    return arg;
}
#endif
/* C lib function end*/
/***************************************************************************
 * Local Functions
 ***************************************************************************/
static void app_func_ext1(void *data)
{
    EatUartConfig_st cfg =
    {
        EAT_UART_BAUD_115200,
        EAT_UART_DATA_BITS_8,
        EAT_UART_STOP_BITS_1,
        EAT_UART_PARITY_NONE
    };
    eat_uart_set_at_port(EAT_UART_2);
    eat_uart_set_debug(EAT_UART_1);
	
    eat_uart_set_debug_config(EAT_UART_DEBUG_MODE_UART, &cfg);

}

static void uart_rx_proc(const EatEvent_st* event)
{
    u16 len;
    EatUart_enum uart = event->data.uart.uart;
	
    len = eat_uart_read(uart, rx_buf, EAT_UART_RX_BUF_LEN_MAX);
    if(len != 0)
    {
		rx_buf[len] = '\0';
		eat_trace("[%s] uart(%d) rx: %s", __FUNCTION__, uart, rx_buf);

#if defined(EAT_AT_TEST)
		if (uart == eat_uart_app)
		{
			eat_modem_write(rx_buf, len);
		}
		else
#endif
		{
			eat_uart_write(uart, rx_buf, len);
		}
    }
}

s32 bytepos(const u8* pSrc, u16 nSrc,
              const char* pSub, u16 startPos)
{
    u16 sublen  = 0;
    s32 _return = -1;
    u16 index;
    s32  cmp_return;
    //eat_trace("L%d, checkstr=%s",__LINE__,pSub);
    sublen = strlen(pSub);
    //eat_trace("L%d src_len=%d, sublen=%d startpos=%d",__LINE__, nSrc, sublen, startPos);
    if (sublen > nSrc)
    {
        return _return;
    }

    for (index = startPos; index <= (nSrc - sublen); index++)
    {
        cmp_return = memcmp(&pSrc[index], pSub, sublen);

        if (cmp_return == 0)
        {
            _return = index;
            return _return;
        }
    }

    return _return;
}

s32 w_offset = 0;
s32 r_offset = 0;
s32 g_data_counter = 0;
static void add_mdm_data_to_buf(char* data, u16 len)
{
    //eat_trace("L%d len=%d",__LINE__, len);
    if(w_offset+len>EAT_BUF_LEN_MAX)
    {
        memcpy(&g_buf[w_offset], data, EAT_BUF_LEN_MAX-w_offset);
        memcpy(&g_buf[0], &data[EAT_BUF_LEN_MAX-w_offset], len-(EAT_BUF_LEN_MAX-w_offset));
        w_offset = len-(EAT_BUF_LEN_MAX-w_offset);
    }else
    {
        memcpy(&g_buf[w_offset], data, len);
        w_offset += len;
        if(w_offset == EAT_BUF_LEN_MAX)
            w_offset = 0;
    }
}
static void mdm_data_process(void)
{
    s32 offset = 0;
    s32 data_len = 0;
    char* substr;
    char len_buf[8] = {0};
    char tmp[64];

    //eat_trace("L%d offset=%d, w_offset=%d r_offset=%d total=%d",__LINE__, offset, w_offset, r_offset,g_data_counter );
    while(w_offset!=r_offset)
    {
        if(w_offset>r_offset)
        { 
            substr = "+RECEIVE,";
            offset = bytepos(&g_buf[r_offset],w_offset-r_offset, substr , 0);
            eat_trace("L%d offset=%d, w_offset=%d r_offset=%d total=%d",__LINE__, offset, w_offset, r_offset,g_data_counter );
            if(offset != -1)
            {
                r_offset += offset+9+2; //+RECEIVE,0,400:\r\ndata
                substr = ":\x0D\x0A";
                offset = bytepos(&g_buf[r_offset],w_offset-r_offset, substr, 0);
                eat_trace("L%d offset=%d, w_offset=%d r_offset=%d",__LINE__, offset, w_offset, r_offset);
                memset(tmp,0,64);
                if(offset != -1)
                {
                    memset(len_buf,0,8);
                    memcpy(len_buf, &g_buf[r_offset], offset);
                    data_len = atoi(len_buf);
                    r_offset += offset+3; //:\r\n
                    eat_trace("L%d datalen=%d last_len=%d",__LINE__,data_len, w_offset-r_offset);
                    memset(tmp,0,64);
                    if(data_len<w_offset-r_offset)
                    {
                        //两个数据包在一起处理
                        //eat_uart_write(eat_uart_app, &g_buf[r_offset], data_len);
                        
                        r_offset += data_len;
                        g_data_counter += data_len;
                        
                        sprintf(tmp, "RECV:%d,%d\r\n",g_data_counter, data_len);
                        eat_uart_write(eat_uart_app, tmp, strlen(tmp));
                        eat_trace("L%d total=%d", __LINE__,g_data_counter );
                        continue;

                    }else if( data_len == w_offset-r_offset)
                    {
                        //eat_uart_write(eat_uart_app, &g_buf[r_offset], data_len);
                        g_data_counter += data_len;
                        
                        sprintf(tmp, "RECV:%d,%d\r\n",g_data_counter, data_len);
                        eat_uart_write(eat_uart_app, tmp, strlen(tmp));
                        eat_trace("L%d total=%d", __LINE__,g_data_counter );
                        r_offset = 0;
                        w_offset = 0;
                    }
                    else
                    {
                        eat_trace("L%d calc data len ERROR!!!! w_offset=%d, r_offset=%d",__LINE__,w_offset,r_offset);
                        eat_trace("last data=%s",&g_buf[r_offset+data_len]);
                    }
                }else
                {
                    eat_trace("L%d finds tring ERROR!!!! w_offset=%d, r_offset=%d",__LINE__,w_offset,r_offset);
                }
            }else
            {
                r_offset = 0;
                w_offset = 0;
            }
        }else
        {
            eat_trace("L%d w_offset=%d, r_offset=%d",__LINE__,w_offset,r_offset);
        }
    }
}

static void mdm_data_process_single_channel(void)
{
    s32 offset = 0;
    s32 data_len = 0;
    char* substr;
    char len_buf[8] = {0};
    char tmp[64];

    //eat_trace("L%d offset=%d, w_offset=%d r_offset=%d total=%d",__LINE__, offset, w_offset, r_offset,g_data_counter );
    while(w_offset!=r_offset)
    {
        if(w_offset>r_offset)
        { 
            substr = "+IPD,";
            offset = bytepos(&g_buf[r_offset],w_offset-r_offset, substr , 0);
            eat_trace("L%d offset=%d, w_offset=%d r_offset=%d total=%d",__LINE__, offset, w_offset, r_offset,g_data_counter );
            if(offset != -1)
            {
                r_offset += offset+5; //+IPD,400:data
                substr = ":";
                offset = bytepos(&g_buf[r_offset],w_offset-r_offset, substr, 0);
                eat_trace("L%d offset=%d, w_offset=%d r_offset=%d",__LINE__, offset, w_offset, r_offset);
                memset(tmp,0,64);
                if(offset != -1)
                {
                    memset(len_buf,0,8);
                    memcpy(len_buf, &g_buf[r_offset], offset);
                    data_len = atoi(len_buf);
                    r_offset += offset+1; //:
                    eat_trace("L%d datalen=%d last_len=%d",__LINE__,data_len, w_offset-r_offset);
                    memset(tmp,0,64);
                    if(data_len<w_offset-r_offset)
                    {
                        //两个数据包在一起处理
                        //eat_uart_write(eat_uart_app, &g_buf[r_offset], data_len);
                        
                        r_offset += data_len;
                        g_data_counter += data_len;
                        
                        sprintf(tmp, "RECV:%d,%d\r\n",g_data_counter, data_len);
                        eat_uart_write(eat_uart_app, tmp, strlen(tmp));
                        eat_trace("L%d total=%d", __LINE__,g_data_counter );
                        continue;

                    }else if( data_len == w_offset-r_offset)
                    {
                        //eat_uart_write(eat_uart_app, &g_buf[r_offset], data_len);
                        g_data_counter += data_len;
                        
                        sprintf(tmp, "RECV:%d,%d\r\n",g_data_counter, data_len);
                        eat_uart_write(eat_uart_app, tmp, strlen(tmp));
                        eat_trace("L%d total=%d", __LINE__,g_data_counter );
                        r_offset = 0;
                        w_offset = 0;
                    }
                    else
                    {
                        eat_trace("L%d calc data len ERROR!!!! w_offset=%d, r_offset=%d",__LINE__,w_offset,r_offset);
                        eat_trace("last data=%s",&g_buf[r_offset+data_len]);
                        w_offset=0;
                        r_offset=0;
                    }
                }else
                {
                    eat_trace("L%d finds tring ERROR!!!! w_offset=%d, r_offset=%d",__LINE__,w_offset,r_offset);
                    w_offset=0;
                    r_offset=0;
                }
            }else
            {
                r_offset = 0;
                w_offset = 0;
            }
        }else
        {
            eat_trace("L%d w_offset=%d, r_offset=%d",__LINE__,w_offset,r_offset);
        }
    }
}

static void mdm_rx_proc(const EatEvent_st* event)
{
    u16 len = 0;

    do
    {
        len = eat_modem_read(rx_buf, EAT_UART_RX_BUF_LEN_MAX );
        if( len > 0)
        {
            rx_buf[len] = '\0';
            if(len<50)
                eat_trace("[%s] uart(%d) rx from modem: %s", __FUNCTION__, eat_uart_app, rx_buf);
            add_mdm_data_to_buf(rx_buf, len);
        }
    }while(len==EAT_UART_RX_BUF_LEN_MAX );//从modem过来数据未读完
    #ifdef EAT_TCP_MULIT_CHANNEL
    mdm_data_process();
    #else
    mdm_data_process_single_channel();
    #endif
}

static void uart_ready_wr_proc(void)
{
    u16 len;
#if defined(EAT_AT_TEST)
    eat_uart_write(eat_uart_app, &rx_buf[wr_uart_offset], wr_uart_len);
    do
    {
        len = eat_modem_read(rx_buf, EAT_UART_RX_BUF_LEN_MAX );
        if(len>0)
        {
            rx_buf[len] = '\0';
            //eat_trace("[%s] uart(%d) rx from modem: %s", __FUNCTION__, eat_uart_app, rx_buf);
            wr_uart_offset = eat_uart_write(eat_uart_app, rx_buf, len);
            if(wr_uart_offset<len)
            {
                wr_uart_len = len - wr_uart_offset;
                break;
            }
        }
    }while(len==EAT_UART_RX_BUF_LEN_MAX );
#endif
}
static void uart_send_complete_proc(const EatEvent_st* event)
{
    eat_trace("[%s] uart(%d) send complete", __FUNCTION__, event->data.uart.uart);	
}
eat_bool eat_timer_process(void)
{
    static int index = 0;
    char* at_cmd = init_at[index++].cmd;
    eat_modem_write(at_cmd , strlen(at_cmd));
    eat_trace("index=%d,AT:%s",index, at_cmd);
	if( index >= sizeof(init_at)/sizeof(eat_at_st))
	{
	    return EAT_FALSE;///如果初始化完成，则返回0，结束初始化
	}
	return EAT_TRUE;///如果没完成则返回1，继续执行初始化。
}
static void app_main(void *data)
{
    EatUartConfig_st uart_config;
    EatEvent_st event;
    int event_num = 0;

	APP_InitRegions(); //APP RAM initialize
    APP_init_clib(); //C library initialize

    if(eat_uart_open(eat_uart_app ) == EAT_FALSE)
    {
	    eat_trace("[%s] uart(%d) open fail!", __FUNCTION__, eat_uart_app);
    }
	
    uart_config.baud = EAT_UART_BAUD_115200;
    uart_config.dataBits = EAT_UART_DATA_BITS_8;
    uart_config.parity = EAT_UART_PARITY_NONE;
    uart_config.stopBits = EAT_UART_STOP_BITS_1;
    if(EAT_FALSE == eat_uart_set_config(eat_uart_app, &uart_config))
    {
        eat_trace("[%s] uart(%d) set config fail!", __FUNCTION__, eat_uart_app);
    }
    //eat_uart_set_send_complete_event(eat_uart_app, EAT_TRUE);
    eat_timer_start(EAT_TIMER_1, 10000);
    while(1)
    {
        event_num = eat_get_event_num();
get_event:
        if(event_num>0)
        {
            eat_get_event(&event);
            eat_trace("[%s] event_id=%d", __FUNCTION__, event.event);

            if (event.event == EAT_EVENT_NULL || event.event >= EAT_EVENT_NUM)
            {
                eat_trace("[%s] invalid event type", __FUNCTION__, event.event);
                continue;
            }

            switch (event.event)
            {
                case EAT_EVENT_TIMER :
                    {
                        //Restart timer
                        if(event.data.timer.timer_id == EAT_TIMER_2)
                        {
                            eat_timer_start(EAT_TIMER_2, 50);
                        }else 
                        {
                            if(eat_timer_process())
                                eat_timer_start(event.data.timer.timer_id, 6000);
                            else
                                eat_timer_start(EAT_TIMER_2, 50);
                        }
                        // eat_trace("Timer test 1, timer ID:%d", event.data.timer.timer_id);
                    }
                    break;
                case EAT_EVENT_MDM_READY_RD:
                    mdm_rx_proc(&event);
                    break;
                case EAT_EVENT_MDM_READY_WR:
                    eat_trace("[%s] uart(%d) event :%s", __FUNCTION__, event.data.uart, "EAT_EVENT_MDM_READY_WR");
                    break;
                case EAT_EVENT_UART_READY_RD:
                    uart_rx_proc(&event);
                    break;
                case EAT_EVENT_UART_READY_WR:
                    eat_trace("[%s] uart(%d) event :%s", __FUNCTION__, event.data.uart, "EAT_EVENT_UART_READY_WR");
                    uart_ready_wr_proc();
                    break;
                case EAT_EVENT_UART_SEND_COMPLETE:
                    uart_send_complete_proc(&event);
                    break;

                default:
                    break;
            }
            event_num = eat_get_event_num();
            if(event_num>0)
                goto get_event;
            else
                eat_sleep(50);
        }else
        {
            eat_sleep(50);
        }
    }	
}

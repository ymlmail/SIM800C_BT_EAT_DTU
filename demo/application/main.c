/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   main.c
 * version        :   0.10
 * Description   :   
 * Authors       :   maobin
 * Notes          :
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
#include "platform.h"
#include "app_at_cmd_envelope.h"
#include "app_custom.h"
#include "eat_clib_define.h" //only in main.c
//#include "app_sms.h"


#include "App_include.h"//yml add 2015-8-5

/********************************************************************
 * Macros
 ********************************************************************/
#define EAT_MEM_MAX_SIZE 100*1024 

/********************************************************************
 * Types
 ********************************************************************/
typedef struct 
{
    u16 w;  //write offset
    u16 r;  //read offset
    u8  buf[EAT_UART_RX_BUF_LEN_MAX];
}app_buf_st;

typedef void (*app_user_func)(void*);

/********************************************************************
 * Extern Variables (Extern /Global)
 ********************************************************************/

/********************************************************************
 * Local Variables:  STATIC
 ********************************************************************/
static EatEntryPara_st app_para={0};
static u8 s_memPool[EAT_MEM_MAX_SIZE]; 
static app_buf_st modem_rx = {0};
static app_buf_st uart_rx = {0};
/********************************************************************
 * External Functions declaration
 ********************************************************************/
extern void APP_InitRegions(void);
extern void app_at_cmd_envelope(void* data);
extern void custom_entry(void);
extern void simcom_sms_read_cb(u16 index,u8* number,u8 *msg);

extern void testGetCrcFunc(void);
extern void LoginMsgPackage(void);//登录信息封装
extern void gps_urc_handler(u8* pUrcStr, u16 len);

/********************************************************************
 * Local Function declaration
 ********************************************************************/
void app_main(void *data);
void app_func_ext1(void *data);

void Delayms(int time);

eat_bool eat_modem_data_parse(u8* buffer, u16 len, u8* param1, u8* param2);
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
    (app_user_func)app_user2,//app_user2,
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

u32 g_pin_set_result = 0;
void app_func_ext1(void *data)
{
	u32 ret_val=0;

/*This function can be called before Task running ,configure the GPIO,uart and etc.
Only these api can be used:
eat_uart_set_debug: set debug port
eat_pin_set_mode: set GPIO mode
eat_uart_set_at_port: set AT port
*/
 // eat_sim_detect_en(EAT_FALSE); //disable SIM card detection and will use PIN54 as eint
  
#if 0
    EatUartConfig_st cfg =
    {
        EAT_UART_BAUD_115200,
        EAT_UART_DATA_BITS_8,
        EAT_UART_STOP_BITS_1,
        EAT_UART_PARITY_NONE
    };
    //2015-7-14 设置
    eat_uart_set_debug(EAT_UART_1);
   eat_uart_set_at_port(EAT_UART_2);// UART1 is as AT PORT
	eat_uart_set_debug_config(EAT_UART_1, &cfg);
	eat_uart_set_debug_config(EAT_UART_2, &cfg);
#else
    EatUartConfig_st cfg =
    {
        EAT_UART_BAUD_115200,
        EAT_UART_DATA_BITS_8,
        EAT_UART_STOP_BITS_1,
        EAT_UART_PARITY_NONE
    };

	eat_uart_set_debug(EAT_UART_USB);
	eat_uart_set_at_port(EAT_UART_1);// UART1 is as AT PORT
eat_uart_set_debug_config(EAT_UART_DEBUG_MODE_UART, NULL);
	//配置USB口做调试口，需要打印ASCII时需要把上面这句也配置下
//eat_uart_set_debug_config(EAT_UART_DEBUG_MODE_TRACE,NULL);	//需要进行reace debuge时配置此行。

    eat_uart_set_config(EAT_UART_1, &cfg);
	
#endif
eat_modem_set_poweron_urc_dir(EAT_USER_1);//一般默认都是user0   ，内核core的信息回复给EAT_USER_0
//设置GPIO的模式
#if EnableKeyFunction
ret_val =ret_val<<1||eat_pin_set_mode(KEY5_PIN, EAT_PIN_MODE_EINT);//设置为外部中断
ret_val =ret_val<<1||eat_pin_set_mode(KEY2_PIN, EAT_PIN_MODE_GPIO);
ret_val =ret_val<<1||eat_pin_set_mode(KEY3_PIN, EAT_PIN_MODE_GPIO);
//ret_val =ret_val<<1||eat_pin_set_mode(KEY4_PIN, EAT_PIN_MODE_GPIO);
//ret_val =ret_val<<1||eat_pin_set_mode(KEY5_PIN, EAT_PIN_MODE_EINT);
ret_val =ret_val<<1||eat_pin_set_mode(KEY1_PIN, EAT_PIN_MODE_GPIO);
//ret_val =ret_val<<1||eat_pin_set_mode(KEY6_PIN, EAT_PIN_MODE_GPIO);

ret_val =ret_val<<1||eat_pin_set_mode(SHUT_PIN, EAT_PIN_MODE_GPIO);
ret_val =ret_val<<1||eat_pin_set_mode(MOTOR_PIN, EAT_PIN_MODE_GPIO);
ret_val =ret_val<<1||eat_pin_set_mode(GPSPOWER_PIN, EAT_PIN_MODE_GPIO);
#endif
//ret_val =ret_val<<1||eat_pin_set_mode(EAT_PIN41_NETLIGHT, EAT_PIN_MODE_GPIO);
//ret_val =ret_val<<1||eat_pin_set_mode(EAT_PIN42_STATUS, EAT_PIN_MODE_GPIO);
//ret_val =ret_val<<1||eat_pin_set_mode(EAT_PIN23_UART2_RXD, EAT_PIN_MODE_GPIO);

g_pin_set_result = ret_val;

}    

static eat_bool app_mem_init(void)
{
    eat_bool ret = EAT_FALSE;
    ret = eat_mem_init(s_memPool,EAT_MEM_MAX_SIZE);
    if (!ret)
    	{
        eat_trace("ERROR: eat memory initial error!");
    	}
	else
		{
		TRACE_DEBUG("yml test msg:eat memory initial OK!\n");
		}
    return ret;
}
#if 0//EnableGPSModule
/*Read the data from UART */
static void uart_rx_proc(const EatEvent_st* event)
{
    u16 len;
    EatUart_enum uart = event->data.uart.uart;
    app_buf_st* rx_p = (app_buf_st*)&uart_rx;
	len=eat_uart_read(uart, rx_p->buf, EAT_UART_RX_BUF_LEN_MAX);
	eat_trace("ReFrGPS 222:\r\n%slen=%d",rx_p->buf,len);
if (len > 0)
	{
	memset(rx_p->buf+len,0,EAT_UART_RX_BUF_LEN_MAX-len);//把len后面的空间清零
											/* global urc check */
	//gps_urc_handler(u8 * pUrcStr,u16 len);
											/* at cmd rsp check */
	eat_trace("ReFrGPS 111:\r\n%slen=%d",rx_p->buf,len);
	}
}

#else
static void uart_rx_proc(const EatEvent_st* event)
{
    u16 len;
    EatUart_enum uart = event->data.uart.uart;
    app_buf_st* rx_p = (app_buf_st*)&uart_rx;

    do
    {
        if(rx_p->w == EAT_UART_RX_BUF_LEN_MAX)
        {
             rx_p->w = 0;
             rx_p->r = 0;
        } 
        len = eat_uart_read(uart, &rx_p->buf[0], EAT_UART_RX_BUF_LEN_MAX-rx_p->w);
		if(len>0)
			{
			memset(rx_p->buf+len,0,EAT_UART_RX_BUF_LEN_MAX-len);//把len后面的空间清零
			#if EnableGPSModule
			gps_urc_handler(&rx_p->buf[0],len);
			#endif
			#if EnableGpsDebuge
			eat_trace("Uart[%d] buf w=%d r=%d len=%d",event->data.uart.uart,rx_p->w, rx_p->r,len);
			eat_trace("buf=%s",&rx_p->buf[0]);
			#endif
			}
    }while(len > 0);
}
#endif
/*Read the data from Modem*/
static void mdm_rx_proc(void)
{
    u16 len;
    u16 w_len;
    app_buf_st* rx_p = (app_buf_st*)&modem_rx;
    do
    {
        len = eat_modem_read(&rx_p->buf[rx_p->w], EAT_UART_RX_BUF_LEN_MAX-rx_p->w);
        if(len==0)
        {
            break;
        }
        
        if(len>0)
        {
            rx_p->w += len;
            len = rx_p->w - rx_p->r;
            w_len = eat_uart_write(eat_uart_app, &rx_p->buf[rx_p->r], len);
            //The buffer is full, the remainder data in Modem will be process in EVENT EAT_EVENT_UART_READY_WR
            if(w_len<len)
            {
                rx_p->r += w_len;
               break;
            }else
            {
                rx_p->r = 0;
                rx_p->w = 0;
            }
        }
    }while(1 );
}

/*process the EAT_EVENT_UART_READY_WR, continues to write the data of Modem to UART*/
static void uart_ready_wr_proc(void)
{
    u16 len;
    u16 w_len;
    app_buf_st* rx_p = (app_buf_st*)&modem_rx;
    
    len = rx_p->w-rx_p->r;
    w_len = eat_uart_write(eat_uart_app, &rx_p->buf[rx_p->r], len);
    if( w_len < len)
    {
        rx_p->r += len;
        return;
    }else
    {
        rx_p->r = 0;
        rx_p->w = 0;
    }
    
    do
    {
        len = eat_modem_read(&rx_p->buf[rx_p->w], EAT_UART_RX_BUF_LEN_MAX-rx_p->w);
        if(len==0)
        {
            break;
        }
        if(len>0)
        {
            rx_p->w += len;
            len = rx_p->w - rx_p->r;
            w_len = eat_uart_write(eat_uart_app, &rx_p->buf[rx_p->r], len);
            if(w_len<len)
            {
               rx_p->r += w_len;
               //The eat uart buffer is full, the remainder data in Modem will be process in EVENT EAT_EVENT_UART_READY_WR
               break;
            }else
            {
                rx_p->r = 0;
                rx_p->w = 0;
            }
        }
    }while(1);
}

eat_bool app_uart_init(void)
{
    eat_bool result = EAT_FALSE;
    EatUartConfig_st uart_config;
    if(eat_uart_open(eat_uart_GPS) == TRUE)//2015-10-17  add gps/bd导航
    {
        if( EAT_UART_USB != eat_uart_GPS )//usb port not need to config
        {
            uart_config.baud = EAT_UART_BAUD_9600;
            uart_config.dataBits = EAT_UART_DATA_BITS_8;
            uart_config.parity = EAT_UART_PARITY_NONE;
            uart_config.stopBits = EAT_UART_STOP_BITS_1;
            if(EAT_TRUE == eat_uart_set_config(eat_uart_GPS, &uart_config))
            {
                result = EAT_TRUE;
				eat_trace("[%s] uart(%d) open OK!", __FUNCTION__, eat_uart_GPS);
            }else
            {
                eat_trace("[%s] uart(%d) set config fail!", __FUNCTION__, eat_uart_GPS);
            }
           eat_uart_set_send_complete_event(eat_uart_GPS, EAT_TRUE);
        }
    }else
    {
        eat_trace("[%s] uart(%d) open fail!", __FUNCTION__, eat_uart_GPS);
    }
    
    return result;
}
const u8 MsgLog[EAT_EVENT_NUM][35]=
{
		"EAT_EVENT_NULL",
		"EAT_EVENT_TIMER", /* timer time out*/
		"EAT_EVENT_KEY", /* KEY*/
		"EAT_EVENT_INT", /* GPIO interrupt*/
		"EAT_EVENT_MDM_READY_RD", /* Read data from MODEM*/
		"EAT_EVENT_MDM_READY_WR", /* Modem can receive the data*/
		"EAT_EVENT_MDM_RI", /* Modem RI*/
		"EAT_EVENT_UART_READY_RD", /* Received data from UART*/
		"EAT_EVENT_UART_READY_WR", /* UART can receive the data*/
		"EAT_EVENT_ADC", 
		"EAT_EVENT_UART_SEND_COMPLETE", /* UART data send complete*/
		"EAT_EVENT_USER_MSG",
		"EAT_EVENT_IME_KEY",/*The message of input method*/
#ifdef __SIMCOM_EAT_WMMP__ /*add wmmp user msg*/
		"EAT_EVENT_TCPIP_STARTUP_CNF",
		"EAT_EVENT_TCPIP_SHUTDOWN_CNF",
		"EAT_EVENT_TCPIP_SHUTDOWN_IND",
		"EAT_EVENT_TCPIP_CLOSE_CNF",
		"EAT_EVENT_TCPIP_ACTIVE_OPEN_CNF",
		"EAT_EVENT_TCPIP_TX_DATA_CNF",
		"EAT_EVENT_TCPIP_TX_DATA_ENABLE_IND",
		"EAT_EVENT_TCPIP_RX_DATA_REQ",
		"EAT_EVENT_IMSI_READY",
		"EAT_EVENT_NW_ATTACH_IND",
		"EAT_EVENT_NEW_SMS_IND", /*new sms*/
#endif
#ifdef __SIMCOM_EAT_SOFT_SIM__
		"EAT_EVENT_SIM_APDU_DATA_IND",
		"EAT_EVENT_SIM_RESET_REQ",
#endif
		"EAT_EVENT_SMS_SEND_CNF",   
		"EAT_EVENT_AUD_PLAY_FINISH_IND", /*Play finish not in call*/
		"EAT_EVENT_SND_PLAY_FINISH_IND", /*Play finish  in call*/

};

void custom_entry(void)
{
    EatEvent_st event;
 	app_mem_init();  //Memory
    eat_sleep(1000);//
	app_uart_init(); //UART
    eat_sleep(1000);//
	//eat_modem_write("AT+CALS=18,0\r\n",strlen("AT+CALS=18,0\r\n"));
	//eat_trace("END play poweron music!\n");at+CRSL=100
	//eat_modem_write("AT+CRSL=80\r\n",strlen("AT+CRSL=80\r\n"));
	GlobalVariableInit();
	eat_get_imei(g_imei_sn,16);
	TRACE_DEBUG("++++++++++++++++++++++++++++++++");
    eat_trace("%s-%d: custom_entry\n", __FILE__, __LINE__);
	TRACE_DEBUG("yemaolin VISION %s %s\n",__DATE__,__TIME__);
    eat_uart_write(eat_uart_app,"\r\nAPP entry!\r\nVersion:1.0\r\n",27);
	eat_trace("Core version=%s",eat_get_version());
	eat_trace("Core buildtime=%s",eat_get_buildtime());
	eat_trace("Core buildno=%s",eat_get_buildno());	
	eat_trace("eat_get_imei=%s",g_imei_sn);
	TRACE_DEBUG("++++++++++++++++++++++++++++++++");

	intGpsModule();
	eat_gpio_write(GPSPOWER_PIN,EAT_GPIO_LEVEL_HIGH);
#if DebugOn
	TRACE_DEBUG("GPSPOWER_PIN LOW  gps POWER off  33");
#endif

//cgatt = eat_network_get_cgatt();
//eat_trace("cgatt =%d", cgatt);
//creg=eat_network_get_creg();
//eat_trace("cgatt =%d", cgatt);
eat_sleep(200);
eat_trace("entry while true\n");
eat_modem_write("AT+CLVL=15\r\n",strlen("AT+CLVL=15\r\n"));
eat_sleep(50);
eat_modem_write("AT+CRSL=100\r\n",strlen("AT+CRSL=100\r\n"));
eat_sleep(50);
eat_modem_set_poweron_urc_dir(EAT_USER_1);//一般默认都是user0   ，内核core的信息回复给EAT_USER_0
eat_sleep(50);

while(EAT_TRUE)
{
	AppLogicHandle();
	//num = eat_get_event_num();
	{
    eat_get_event(&event);
    eat_trace("Main App MSG[%d]:%s",event.event,MsgLog[event.event]);//打印收到的是什么消息
    switch(event.event)
    {
        case EAT_EVENT_TIMER ://定时器消息   ID=1
        	    if(event.data.timer.timer_id==EAT_TIMER_4)
            	{
            	if(!flag.GSMisOk)
            		{
            		flag.timer41up =1;		//timer4 时间到
            		eat_trace("Timer41++ is up, timer41up=1\n");
            		}
				else
					{
            	flag.timer4up =1;		//timer4 时间到
				eat_trace("Timer4 is up, timer ID=4");
					}
            	}
				else if(event.data.timer.timer_id==EAT_TIMER_5)
				{
				flag.timer5up=1;
				eat_trace("Timer5 is up, timer ID=5");
				}
				else if(event.data.timer.timer_id==EAT_TIMER_6)
					{
					flag.reConnectSever=1;
					eat_trace("Timer6 is up, timer ID=6");
					}
				else if(event.data.timer.timer_id==EAT_TIMER_7)
					{
					flag.reBootTP_20=1;
					eat_trace("Timer7 is up, timer ID=7");
					}
				else if(event.data.timer.timer_id==EAT_TIMER_8)
					{
					flag.reBootTP_10=1;
					eat_trace("Timer8 is up, timer ID=8");
					}

            break;
		case EAT_EVENT_KEY:
			flag.keycome=1;
			g_KeyValue=16;	//挂断键
			break;
        case EAT_EVENT_MDM_READY_RD://EAT收到Modem发过来的数据ID=4
           	{
               mdm_rx_proc();
            }
            break;
        case EAT_EVENT_MDM_READY_WR:
            break;
        case EAT_EVENT_UART_READY_RD:
            uart_rx_proc(&event);
            break;
        case EAT_EVENT_UART_READY_WR:
            uart_ready_wr_proc();
            break;
        case EAT_EVENT_UART_SEND_COMPLETE :
            break;
        case  EAT_EVENT_INT :
			break;
		case EAT_EVENT_USER_MSG:
		  {
			eat_trace("app_user0: %d %d %d %s %x", event.data.user_msg.src, event.data.user_msg.use_point, 
			event.data.user_msg.len, event.data.user_msg.data, event.data.user_msg.data_p); 
		  	eat_event_user0_msg_handler(event.data.user_msg.data,event.data.user_msg.len);
		  }
		  
			break;

        default:
            break;
    }
}
}
}

void Delayms(int time)
{
int i,j;
for(i=0;i<time;i++)
	for(j=0;j<10000;j++);
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

void app_main(void *data)
{
	 eat_bool result = EAT_TRUE;
    EatEntryPara_st *para;
	
	eat_sim_detect_en(EAT_FALSE); //disable SIM card detection and will use PIN54 as eint
#if 0//EnableKeyFunction
	init_GPIO();
#endif
	eat_gpio_write(GPSPOWER_PIN,EAT_GPIO_LEVEL_LOW);
#if DebugOn
	TRACE_DEBUG("GPSPOWER_PIN LOW  gps POWER on  22");
#endif

    APP_InitRegions();//Init app RAM, first step
    APP_init_clib();  //C library initialize, second step

    para = (EatEntryPara_st*)data;
    TRACE_DEBUG("yml delay start111\n");
	// Delayms(5000);
   // TRACE_DEBUG("yml delay end1111\n");
	//eat_sleep_enable(EAT_FALSE);//YML ADD
    memcpy(&app_para, para, sizeof(EatEntryPara_st));
    eat_trace("App Main ENTRY update:%d result:%d", app_para.is_update_app,app_para.update_app_result);
   if(app_para.is_update_app && app_para.update_app_result)
    {
       eat_update_app_ok();
    }
    rtc.year = 16;
    rtc.mon = 1;
    rtc.day = 15;
    rtc.hour = 9;
    rtc.min = 0;
    rtc.sec = 0;
    //set rtc 
    result = eat_set_rtc(&rtc);

	result = eat_get_rtc(&rtc);
	if( result )
	{
		eat_trace("Timer test 3 get rtc value:%d/%d/%d %d:%d:%d",
				rtc.year,rtc.mon,rtc.day,rtc.hour,rtc.min,rtc.sec);
	}else
	{
		eat_trace("Timer test 3 get rtc fail");
	}		
	//eat_sim_detect_en(EAT_FALSE); //disable SIM card detection and will use PIN54 as eint
	eat_poweroff_key_sw(TRUE);		//使能开机按键可以进行长按关机
	//eat_modem_write("AT+CALS=18,1\r\n",strlen("AT+CALS=18,1\r\n"));
//	eat_trace("play poweron music!\n");


    custom_entry();
	
}


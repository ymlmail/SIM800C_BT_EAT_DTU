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
#include "eat_socket.h"
#include "eat_clib_define.h" //only in main.c
#include "app_at_cmd_envelope.h"

/********************************************************************
 * Macros
 ********************************************************************/
#define EAT_UART_RX_BUF_LEN_MAX 2048
#define EAT_MEM_MAX_SIZE 100*1024
#define UART_PORT	EAT_UART_2	

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
static u32 gpt_timer_counter = 0;
static u32 time1 = 0;
static u8 s_memPool[EAT_MEM_MAX_SIZE];
static s8 socket_id = 0;

u8 *SOC_EVENT[]={
    "SOC_READ",
    "SOC_WRITE",  
    "SOC_ACCEPT", 
    "SOC_CONNECT",
    "SOC_CLOSE", 
    "SOC_ACKED"
};

u8 *BEARER_STATE[]={
    "DEACTIVATED",
    "ACTIVATING",
    "ACTIVATED",
    "DEACTIVATING",
    "CSD_AUTO_DISC_TIMEOUT",
    "GPRS_AUTO_DISC_TIMEOUT",
    "NWK_NEG_QOS_MODIFY",
    "CBM_WIFI_STA_INFO_MODIF",
};
/********************************************************************
 * External Functions declaration
 ********************************************************************/
extern void APP_InitRegions(void);

/********************************************************************
 * Local Function declaration
 ********************************************************************/
void app_main(void *data);
void app_func_ext1(void *data);
extern void app_at_cmd_envelope(void *data);
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
		 eat_uart_set_at_port: set AT port
	*/

    /*EatUartConfig_st cfg =
    {
        EAT_UART_BAUD_115200,
        EAT_UART_DATA_BITS_8,
        EAT_UART_STOP_BITS_1,
        EAT_UART_PARITY_NONE
    };
  */
	eat_uart_set_debug(EAT_UART_USB);//eat_uart_set_debug(EAT_UART_USB);
   	eat_uart_set_at_port(EAT_UART_1); // eat_uart_set_at_port(EAT_UART_NULL);// UART1 is as AT PORT
	//eat_uart_set_debug_config(EAT_UART_1, &cfg);
	eat_uart_set_debug_config(EAT_UART_DEBUG_MODE_UART, NULL);//USB串口打印信息，必须有这个。
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
    //eat_reset_module();
}
eat_soc_notify soc_notify_cb(s8 s,soc_event_enum event,eat_bool result, u16 ack_size)
{
    u8 buffer[128] = {0};
    u8 id = 0;
    if(event&SOC_READ) {id = 0;
        socket_id = s;
    }
    else if (event&SOC_WRITE) id = 1;
    else if (event&SOC_ACCEPT) id = 2;
    else if (event&SOC_CONNECT) id = 3;
    else if (event&SOC_CLOSE){ id = 4;
        eat_soc_close(s);
    }
    else if (event&SOC_ACKED) id = 5;
    if (id == 5)
        sprintf(buffer,"SOC_NOTIFY:%d,%s,%d\r\n",s,SOC_EVENT[id],ack_size);
    else 
        sprintf(buffer,"SOC_NOTIFY:%d,%s,%d\r\n",s,SOC_EVENT[id],result);
    eat_uart_write(EAT_UART_1,buffer,strlen(buffer));

    if(SOC_ACCEPT==event){
        u8 val = 0;
        s8 ret = 0;
        sockaddr_struct clientAddr={0};
        s8 newsocket = eat_soc_accept(s,&clientAddr);
        if (newsocket < 0){
            eat_trace("eat_soc_accept return error :%d",newsocket);
        }
        else{
            sprintf(buffer,"client accept:%s,%d:%d:%d:%d\r\n",clientAddr.addr[0],clientAddr.addr[1],clientAddr.addr[2],clientAddr.addr[3]);
        }

        val = TRUE;
        ret = eat_soc_setsockopt(socket_id, SOC_NODELAY, &val, sizeof(val));
        if (ret != SOC_SUCCESS)
            eat_trace("eat_soc_setsockopt SOC_NODELAY return error :%d",ret);

    }

    eat_trace("soc_notify_cb");

}
eat_bear_notify bear_notify_cb(cbm_bearer_state_enum state,u8 ip_addr[4])
{
    u8 buffer[128] = {0};
    u8 id = 0;
    if (state & CBM_DEACTIVATED) id = 0;
    else if (state & CBM_ACTIVATING) id = 1;
    else if (state & CBM_ACTIVATED) id = 2;
    else if (state & CBM_DEACTIVATING) id = 3;
    else if (state & CBM_CSD_AUTO_DISC_TIMEOUT) id = 4;
    else if (state & CBM_GPRS_AUTO_DISC_TIMEOUT) id = 5;
    else if (state & CBM_NWK_NEG_QOS_MODIFY) id = 6;
    else if (state & CBM_WIFI_STA_INFO_MODIFY) id = 7;
    if (id == 2)
        sprintf(buffer,"BEAR_NOTIFY:%s,%d:%d:%d:%d\r\n",BEARER_STATE[id],ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);
    else
        sprintf(buffer,"BEAR_NOTIFY:%s\r\n",BEARER_STATE[id]);
    eat_uart_write(EAT_UART_1,buffer,strlen(buffer));

    if (id == 2){
        sockaddr_struct address={0};
        u8 val;
        s8 ret;
        eat_soc_notify_register(soc_notify_cb);
        socket_id = eat_soc_create(SOC_SOCK_STREAM,0);
        if(socket_id < 0)
            eat_trace("eat_soc_create return error :%d",socket_id);

        val = TRUE;
        ret = eat_soc_setsockopt(socket_id, SOC_NBIO, &val, sizeof(val));
        if (ret != SOC_SUCCESS)
            eat_trace("eat_soc_setsockopt 2 return error :%d",ret);

        val = (SOC_READ | SOC_WRITE | SOC_CLOSE | SOC_CONNECT|SOC_ACCEPT);
        ret = eat_soc_setsockopt(socket_id,SOC_ASYNC,&val,sizeof(val));
        if (ret != SOC_SUCCESS)
            eat_trace("eat_soc_setsockopt 1 return error :%d",ret);
        address.port = 111;
        eat_soc_bind(socket_id,&address);
        eat_soc_listen(socket_id,1);

        socket_id = eat_soc_create(SOC_SOCK_STREAM,0);
        if(socket_id < 0)
            eat_trace("eat_soc_create return error :%d",socket_id);

        val = TRUE;
        ret = eat_soc_setsockopt(socket_id, SOC_NBIO, &val, sizeof(val));
        if (ret != SOC_SUCCESS)
            eat_trace("eat_soc_setsockopt 2 return error :%d",ret);

        val = (SOC_READ | SOC_WRITE | SOC_CLOSE | SOC_CONNECT|SOC_ACCEPT);
        ret = eat_soc_setsockopt(socket_id,SOC_ASYNC,&val,sizeof(val));
        if (ret != SOC_SUCCESS)
            eat_trace("eat_soc_setsockopt 1 return error :%d",ret);
        address.port = 222;
        eat_soc_bind(socket_id,&address);
        eat_soc_listen(socket_id,1);

        socket_id = eat_soc_create(SOC_SOCK_STREAM,0);
        if(socket_id < 0)
            eat_trace("eat_soc_create return error :%d",socket_id);

        val = TRUE;
        ret = eat_soc_setsockopt(socket_id, SOC_NBIO, &val, sizeof(val));
        if (ret != SOC_SUCCESS)
            eat_trace("eat_soc_setsockopt 2 return error :%d",ret);

        val = (SOC_READ | SOC_WRITE | SOC_CLOSE | SOC_CONNECT|SOC_ACCEPT);
        ret = eat_soc_setsockopt(socket_id,SOC_ASYNC,&val,sizeof(val));
        if (ret != SOC_SUCCESS)
            eat_trace("eat_soc_setsockopt 1 return error :%d",ret);
        address.port = 333;
        eat_soc_bind(socket_id,&address);
        eat_soc_listen(socket_id,1);
    }
    eat_trace("bear_notify_cb");
}

eat_hostname_notify hostname_notify_cb(u32 request_id,eat_bool result,u8 ip_addr[4])
{
    u8 buffer[128] = {0};
    sprintf(buffer,"HOSTNAME_NOTIFY:%d,%d,%d:%d:%d:%d\r\n",request_id,result,ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);
    eat_uart_write(EAT_UART_1,buffer,strlen(buffer));
    eat_trace("hostname_notify_cb");
    
}


ResultNotifyCb GsmInitCallback(eat_bool result)
{
    if (result)
        eat_uart_write(EAT_UART_1,"GSM init OK\r\n",14);
    else
        eat_uart_write(EAT_UART_1,"GSM init ERROR\r\n",14);
}

ResultNotifyCb ftpgettofs_final_cb(eat_bool result)
{
	u8 buffer[128] = {0};

	sprintf(buffer, "\r\nftpgettofs_cb final result = %d\r\n", result);
	eat_uart_write(EAT_UART_1, buffer, strlen(buffer));
    eat_trace("ftpgettofs_final_cb");
}

ResultNotifyCb ftpgettofs_cb(eat_bool result)
{
	u8 buffer[128] = {0};

	sprintf(buffer, "\r\nftpgettofs_cb result = %d\r\n", result);
	eat_uart_write(EAT_UART_1, buffer, strlen(buffer));
    eat_trace("ftpgettofs_cb");
}

eat_bool simcom_server_start()
{
    s8 ret = eat_gprs_bearer_open("CMNET",NULL,NULL,bear_notify_cb);

}
/****************************************************
 * Timer testing module
 *****************************************************/
eat_bool eat_module_test_tcpip(u8 param1, u8 param2)
{
    eat_bool result = EAT_TRUE;
    s8 ret = 0;
    /***************************************
     * example 1
     * open or close bearer
     ***************************************/
    if(0 == param1){
        if( 0 == param2 )                       /* open gprs bearer */
            simcom_ftp_down_file("sim.mkr", "hj135", "henryTest.txt", "/", 
                    "116.228.221.52", 21, "henryTest.txt", ftpgettofs_cb, ftpgettofs_final_cb);
        else if(1 == param2)
            simcom_gsm_init("1234",GsmInitCallback);
    }
    else if( 1 == param1 )
    {

        if( 1 == param2 )                       /* open gprs bearer */
        {
            ret = eat_gprs_bearer_open("CMNET",NULL,NULL,bear_notify_cb);
            if(CBM_OK != ret)
                eat_trace("gprs_bearer_open return error :%d",ret);
            ret = eat_gprs_bearer_hold() ;       /* hold bearer,after call this function, the bearer will be holding. 
                                                   in default, when last actived socket be closed, the bearer 
                                                   will be released */
            if(CBM_OK != ret)
                eat_trace("eat_gprs_bearer_hold return error :%d",ret);

        }else if( 0 == param2)                  /* close gprs bearer */
        {
            ret = eat_gprs_bearer_release();
            if(CBM_OK != ret)
                eat_trace("eat_gprs_bearer_release return error :%d",ret);
            
        }
    }
    /***************************************
     * example 2
     * connect a socket or close it
     ***************************************/
    else if( 2 == param1 )
    {
        if( 1 == param2)                        /* connect server */
        {
            //clear counter
            u32 VAL;
            u8 val = 0;
            s8 ret = 0;
            sockaddr_struct address={0};
            eat_soc_notify_register(soc_notify_cb);
            socket_id = eat_soc_create(SOC_SOCK_STREAM,0);
           // if(socket_id < 0)
                eat_trace("eat_soc_create return error :%d",socket_id);
            val = (SOC_READ | SOC_WRITE | SOC_CLOSE | SOC_CONNECT|SOC_ACCEPT);
            ret = eat_soc_setsockopt(socket_id,SOC_ASYNC,&val,sizeof(val));
            if (ret != SOC_SUCCESS)
                eat_trace("eat_soc_setsockopt 1 return error :%d",ret);

            val = TRUE;
            ret = eat_soc_setsockopt(socket_id, SOC_NBIO, &val, sizeof(val));
            if (ret != SOC_SUCCESS)
                eat_trace("eat_soc_setsockopt 2 return error :%d",ret);

            val = TRUE;
            ret = eat_soc_setsockopt(socket_id, SOC_NODELAY, &val, sizeof(val));
            if (ret != SOC_SUCCESS)
                eat_trace("eat_soc_setsockopt 3 return error :%d",ret);

            ret = eat_soc_getsockopt(socket_id, SOC_NODELAY, &VAL, sizeof(VAL));
            if (ret != SOC_SUCCESS)
                eat_trace("eat_soc_getsockopt  return error :%d",ret);
            else 
                eat_trace("eat_soc_getsockopt return %d",val);

            address.sock_type = SOC_SOCK_STREAM;
            address.addr_len = 4;
            address.port = 8888;                /* TCP server port */
            address.addr[0]=222;                /* TCP server ip address */
            address.addr[1]=211;
            address.addr[2]=65;
            address.addr[3]=62;
            ret = eat_soc_connect(socket_id,&address); 
            if(ret >= 0){
                eat_trace("NEW Connection ID is :%d",ret);
            }
            else if (ret == SOC_WOULDBLOCK) {
                eat_trace("Connection is in progressing");
            }
            else {
                eat_trace("Connect return error:%d",ret);
            }


        }else                                   /* close it */
        {
            s8 ret = 0;
            ret = eat_soc_close(socket_id);
            if(ret != SOC_SUCCESS){
                eat_trace("Close error");
            }
			else
				 eat_trace("Close sucessfull");
        }


    }
    /***************************************
     * example 3
     * send data to server
     ***************************************/
    else if( 3 == param1 )
    {
        if( 1 == param2)                        /* send data */
        {
            s32 ret = 0;
            ret = eat_soc_send(socket_id,"eat socket test",15);
            if (ret < 0)
                eat_trace("eat_soc_send return error :%d",ret);
            else
                eat_trace("eat_soc_send success :%d",ret);

        }else if( 2 == param2 )                 /* receive data */
        {
            u8 buffer[1046] = {0};
            s32 ret = 0;
            ret = eat_soc_recv(socket_id,buffer,1046);
            if(ret == SOC_WOULDBLOCK){
                eat_trace("eat_soc_recv no data available");

            }
            else if(ret > 0) {
                eat_trace("eat_soc_recv data:%s",buffer);
                eat_uart_write(UART_PORT,buffer,ret);
            }
            else{
                eat_trace("eat_soc_recv return error:%d",ret);
            }
            
        }
    }
    /***************************************
     * example 4
     * get hostname
     ***************************************/
    else if( 4 == param1 )
    {
        EatRtc_st rtc = {0};
        if( 1 == param2)                        /* query hotname ip address */
        {
            s8 ret = 0;
            u8 ipaddr[4];
            u8 len;
            eat_soc_gethost_notify_register(hostname_notify_cb);
            ret = eat_soc_gethostbyname("www.baidu.com",ipaddr,&len,1234);
            if (SOC_SUCCESS == ret){
                u8 buffer[128] = {0};
                eat_trace("eat_soc_gethostbyname success");
                sprintf(buffer,"HOSTNAME:%d,%d:%d:%d:%d\r\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
                eat_uart_write(UART_PORT,buffer,strlen(buffer));

            } else if(SOC_WOULDBLOCK == ret){
                eat_trace("eat_soc_gethostbyname wait callback function");

            } else
                eat_trace("eat_soc_gethostbyname error");


        }
    }
    return EAT_TRUE;
}

u8 buf[2048];
static void uart_rx_proc(const EatEvent_st* event)
{
    u16 len;
    EatUart_enum uart = event->data.uart.uart;
	
    len = eat_uart_read(uart, rx_buf, EAT_UART_RX_BUF_LEN_MAX);
    if(len != 0)
    {
		rx_buf[len] = '\0';
		eat_trace("[%s] uart(%d) rx: %s", __FUNCTION__, uart, rx_buf);

		if (uart == UART_PORT)
		{
			eat_modem_write(rx_buf, len);
		}
		else
		{
			eat_uart_write(uart, rx_buf, len);
		}
    }
}
void app_main(void *data)
{
    EatEvent_st event;
    u16 len = 0;
    EatUartConfig_st uart_config;
    eat_bool ret;

    APP_InitRegions();//Init app RAM
    APP_init_clib(); //C library initialize, second step
   if(eat_uart_open(UART_PORT ) == EAT_FALSE)
    {
	    eat_trace("[%s] uart(%d) open fail!", __FUNCTION__, UART_PORT);
    }
	
    uart_config.baud = EAT_UART_BAUD_115200;
    uart_config.dataBits = EAT_UART_DATA_BITS_8;
    uart_config.parity = EAT_UART_PARITY_NONE;
    uart_config.stopBits = EAT_UART_STOP_BITS_1;
    if(EAT_FALSE == eat_uart_set_config(UART_PORT, &uart_config))
    {
        eat_trace("[%s] uart(%d) set config fail!", __FUNCTION__, UART_PORT);
    }
	
	eat_trace("yemaolin VISION %s %s\n",__DATE__,__TIME__);
    eat_trace("%s-%d: custom_entry\n", __FILE__, __LINE__);
    eat_uart_write(EAT_UART_USB,"\r\nAPP entry!\r\nVersion:1\r\n",25);
	
    eat_trace(" app_main ENTRY");
    ret = eat_mem_init(s_memPool,EAT_MEM_MAX_SIZE);
    if (!ret)
        eat_trace("ERROR: eat memory initial error!");
    simcom_gsm_init("1234",GsmInitCallback);
    while(EAT_TRUE)
    {
        eat_get_event(&event);
        eat_trace("MSG id%x", event.event);
        switch(event.event)
        {
            case EAT_EVENT_TIMER :
                {
                    //Restart timer
//                    eat_timer_start(event.data.timer.timer_id, 3000);
                    eat_trace("Timer test 1, timer ID:%d", event.data.timer.timer_id);
                }
                break;
            case EAT_EVENT_MDM_READY_RD:
                {
                    u8 param1,param2;
                    len = 0;
                    len = eat_modem_read(buf, 2048);
                    if(len > 0)
                    {
                        buf[len] = 0;
                        //Get the testing parameter
                        if(eat_modem_data_parse(buf,len,&param1,&param2))
                        {
                            //Entry timer test module
                            eat_module_test_tcpip(param1, param2);
                        }
                        else
                        {
                            eat_uart_write(UART_PORT,buf,strlen(buf));
                        }
                    }

                }
                break;
            case EAT_EVENT_MDM_READY_WR:
                break;
            case EAT_EVENT_UART_READY_RD:
                uart_rx_proc(&event);
                break;
            case EAT_EVENT_UART_SEND_COMPLETE :
                break;
            default:
                break;
        }

    }

}



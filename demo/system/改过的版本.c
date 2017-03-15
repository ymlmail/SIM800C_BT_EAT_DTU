/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   app_demo_sys.c
 * version       :   0.10
 * Description   :   
 * Authors       :   jinguangbo
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2012-09-24, jinguangbo, create originally.
 *
 *--------------------------------------------------------------------
 * File Description
 * AT+CEAT=parma1,param2
 * param1 param2 
 *   10  n         memory operation malloc size=100*n
 *   11  1         power down
 *   11  2         system reset
 *   11  3         put sem
 *   11  4         get sys info
 *   12  n         sleep demo
 *   13  1         send msg use external buffer
 *   13  2         send msg use internal buffer
 *   15  4         get the cgatt status
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
#include "eat_clib_define.h"
/********************************************************************
* Macros
 ********************************************************************/

//#define EAT_DEBUG_STRING_INFO_MODE
//#define EAT_TIMER_TEST  //user2~6 start a timer automatic
/********************************************************************
* Types
 ********************************************************************/
typedef void (*app_user_func)(void*);
typedef struct _eat_msg_
{
    int id;
    int len;
    u8 data[32];
}eat_msg_t;
/********************************************************************
 * Extern Variables (Extern /Global)
 ********************************************************************/
 
/********************************************************************
 * Local Variables:  STATIC
 ********************************************************************/
static const EatUart_enum eat_uart_app= EAT_UART_1;

/********************************************************************
 * External Functions declaration
 ********************************************************************/
extern void APP_InitRegions(void);

/********************************************************************
 * Local Function declaration
 ********************************************************************/
void app_main(void *data);
void app_func_ext1(void *data);
void app_user1(void *data);
void app_user2(void *data);
void app_user3(void *data);
void app_user4(void *data);
void app_user5(void *data);
void app_user6(void *data);
void app_user7(void *data);
void app_user8(void *data);
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
		(app_user_func)app_user1,//app_user1,
		(app_user_func)app_user2,//app_user2,
		(app_user_func)app_user3,//app_user3,
		(app_user_func)app_user4,//app_user4,
		(app_user_func)app_user5,//app_user5,
		(app_user_func)app_user6,//app_user6,
		(app_user_func)app_user7,//app_user7,
		(app_user_func)app_user8,//app_user8,
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
//#ifdef EAT_DEBUG_STRING_INFO_MODE
    EatUartConfig_st cfg =
    {
        EAT_UART_BAUD_115200,
        EAT_UART_DATA_BITS_8,
        EAT_UART_STOP_BITS_1,
        EAT_UART_PARITY_NONE
    };
//#endif
	eat_uart_set_at_port(EAT_UART_1);
	eat_uart_set_debug(EAT_UART_USB);
	//set debug string info to debug port
//#ifdef EAT_DEBUG_STRING_INFO_MODE
    eat_uart_set_debug_config(EAT_UART_DEBUG_MODE_UART, NULL);
//#endif
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

eat_bool App_SMS_is_ready(u8* buffer, u16 len)
{
    eat_bool ret_val = EAT_FALSE;
    u8* buf_ptr = NULL;
    /*param:%d,extern_param:%d*/
     buf_ptr = (u8*)strstr((const char *)buffer,"SMS Ready");
    if( buf_ptr != NULL)
    {
        eat_trace("data MSG2015-8-4%s ",buffer);
        ret_val = EAT_TRUE;
    }
    return ret_val;
}
eat_bool App_Msg_is_OK(u8* buffer, u16 len)
{
    eat_bool ret_val = EAT_FALSE;
    u8* buf_ptr = NULL;
    /*param:%d,extern_param:%d*/
	eat_trace("ok start test\n");
     buf_ptr = (u8*)strstr((const char *)buffer,"OK");
    if( buf_ptr != NULL)
    {
        eat_trace("ok MSG2015-8-4%s ",buffer);
        ret_val = EAT_TRUE;
    }
    return ret_val;
}



/* the following array is used as memeory pool, the alloc and free buffer is in this pool*/
#define DYNAMIC_MEM_SIZE 1024*100
static unsigned char app_dynic_mem_test[DYNAMIC_MEM_SIZE];
void* mem_prt=EAT_NULL;
eat_bool mem_ini_flag=EAT_FALSE;
EatSemId_st sem_id_user1=EAT_NULL;

/****************************************************
 * mem testing module
 * AT+CEAT=v1,v2  
 v1=10,v2=n memory operation malloc size=100*n
 
 v1=11,v2=1,power down
 v1=11,v2=2,system reset
 v1=11,v2=3,put sem
 v1=11,v2=4,get sys info
 
 v1=12,v2=n, sleep demo,n sleep duration(s)
 
 v1=13,v2=1, send msg use external buffer
 v1=13,v2=2, send msg use internal buffer
 v1=12,v2=3, send msg to internal queue

 v1=15, v2=4, get the cgatt status
 
 *****************************************************/
void* ptr_buf[100]={0};
eat_bool eat_module_test_sys(u8 param1, u8 param2)
{
    
    u32   size =100;
    u8* msg_ptr=EAT_NULL;
    eat_bool ret;

    /***************************************
     * example 1
     * mem demo
     ***************************************/
    if( 10 == param1 )
    {
               
		/* alloc the memory */
        mem_prt = eat_mem_alloc( param2*size );
        if(EAT_NULL == mem_prt)
        {
            eat_trace("eat_mem_alloc() Fail");
        }else
        {
            eat_trace("eat_mem_alloc() size =%d\n",param2*size);    
        }    
        
        
		/* free the memory */
        if(EAT_NULL != mem_prt)
        {
			/* set the pointer to NULL for avoiding the second free */
            eat_mem_free(mem_prt);
            if(EAT_NULL !=mem_prt)
            {
                mem_prt = EAT_NULL;
                eat_trace("eat_mem_free() Pass "); 
            }        
        }    
    }/*if( 10 == param1 )*/    
    
    
    /***************************************
     * example 2
     * power down ,system reset,sem,system info demo
     ***************************************/    
    if( 11 == param1 )
    {    
        if( 1 == param2 )
        {
            eat_trace("power down ......");
            eat_power_down();
        }
        if( 2 == param2 )
        {
                 
            eat_trace("module_reset_test");
            ret = eat_reset_module();
            if(EAT_TRUE == ret )
            {
                eat_trace("module_reset_test Pass");
            }else
            {
                eat_trace("module_reset_test Fail");
            } 
        }
        if( 3 == param2 )
        {
            if( EAT_NULL != sem_id_user1)
            {
                eat_trace("module_sem_test put sem one");
                eat_sem_put(sem_id_user1);
            }  
        } 
        
        if( 4 == param2 )
        {
            eat_trace("eat_get_version=%s",eat_get_version());
            eat_trace("eat_get_buildtime=%s",eat_get_buildtime());
            eat_trace("eat_get_buildno=%s",eat_get_buildno());            
            
        }    
               
    } /*if( 11 == param1 )*/

    /***************************************
     * example 3
     * sleep control demo
     ***************************************/      
    if( 12 == param1 )
    {
        if( 0==param2 )
        {
            param2 =10;
        }    
        eat_timer_start(EAT_TIMER_1, param2*1000);
        eat_trace("enter sleep,%d second later wakeup",param2);
        eat_sleep_enable(EAT_TRUE);
    }/*if( 12 == param1 )*/

    /***************************************
     * example 4
     * send msg demo
     ***************************************/     
    if( 13 == param1 )
    {    
        if( 1==param2 )
        {    
            msg_ptr = eat_mem_alloc(4);
            if(EAT_NULL == msg_ptr)
            {
                eat_trace("eat_mem_alloc() Fail\n");
                return EAT_FALSE;
            }
            memcpy(msg_ptr, "1_8", 4);            
            eat_send_msg_to_user(EAT_USER_1, EAT_USER_8, EAT_TRUE, 4, EAT_NULL, (const unsigned char**)&msg_ptr);
        }
        
        if( 2==param2 )
        {    
            
            eat_send_msg_to_user(EAT_USER_1, EAT_USER_7, EAT_FALSE, 60, "000000000011111111112222222222333333333344444444445555555555", EAT_NULL);
            
			/* the message data exceed the range of the internal buffer */
            eat_send_msg_to_user(EAT_USER_1, EAT_USER_7, EAT_FALSE, 70, "0000000000111111111122222222223333333333444444444455555555556666666666", EAT_NULL);
        }
		
		if( 3==param2 )
        {    
            eat_send_msg_to_user(EAT_USER_0, EAT_USER_0, EAT_FALSE, 60, "000000000011111111112222222222333333333344444444445555555555", EAT_NULL);           
        }
        if(4==param2)
        {
            /*Transfer struct date*/
            eat_msg_t msg;
            memset(&msg, sizeof(eat_msg_t), 0);
            msg.id = 0x100;
            msg.len = sprintf((void*)(msg.data), "accel INT");
            eat_send_msg_to_user(EAT_USER_1, EAT_USER_6, EAT_FALSE, sizeof(eat_msg_t), (u8 *)&msg, NULL);
        }
    }else if( 14 == param1 )
    {
        int i = 0;
        void* p;
        char out_buf[20]={0};
        char len;
        for(;i<100;i++)
        {
            ptr_buf[i]= eat_mem_alloc(i+2);
            len = sprintf(out_buf,"%x:%d\r\n",ptr_buf[i],i+2);
            eat_uart_write(eat_uart_app, out_buf, len);
        }
        eat_uart_write(eat_uart_app, "Free:\r\n", 7);
        for(i=0;i<5;i++)
        {
            eat_mem_free(ptr_buf[i*2]);
            len = sprintf(out_buf,"%x:%d\r\n",ptr_buf[i*2],i*2+2);
            eat_uart_write(eat_uart_app, out_buf, len);
        }
        eat_uart_write(eat_uart_app, "Malloc new:\r\n",13);
        for(i=0;i<10;i++)
        {
            p = eat_mem_alloc(i+6);
            len = sprintf(out_buf,"%x:%d\r\n",p,i+6);
            eat_uart_write(eat_uart_app, out_buf, len);
        }
        eat_uart_write(eat_uart_app, "Malloc new2:\r\n",13);
        eat_mem_free(ptr_buf[1]);
        len = sprintf(out_buf,"free %x:%d\r\n",ptr_buf[1],1+2);
        eat_uart_write(eat_uart_app, out_buf, len);
        p = eat_mem_alloc(10);
        len = sprintf(out_buf,"alloc %x:%d\r\n",p,10);
        eat_uart_write(eat_uart_app, out_buf, len);
    }
    else if (15 == param1)
    {
		if( 1 == param2)
		{
			EatCregState_enum enmCreg = eat_network_get_creg();
			eat_trace("eat_network_get_creg=%d", enmCreg);
		}
		else if( 2 == param2)
		{
			s32 nBer = eat_network_get_ber();
			eat_trace("eat_network_get_ber=%d", nBer);
		}
		else if( 3 == param2)
		{
			s32 nCsq = eat_network_get_csq();
			eat_trace("eat_network_get_csq=%d", nCsq);
		}
        else if ( 4 == param2 )
        {
            u8 cgatt = 0;
            cgatt = eat_network_get_cgatt();

            eat_trace("cgatt =%d", cgatt);
        }
		else
		{
			//do nothing
		}
        
    }
    else if( 16 == param1 )
    {
		if( 0 == param2)
        {
            eat_sleep_enable(EAT_FALSE);
        }
        else if( 1 == param2)
        {
            eat_sleep_enable(EAT_TRUE);
        }
    }
	else if(17==param1)
	{
		if(0==param2)
			{
			eat_modem_write("AT\r\n",4);
			eat_modem_write("AT\r\n",4);
			eat_modem_write("AT\r\n",4);
			eat_trace("send AT COM\n");
			}
	}

    return EAT_TRUE;
}

u8 buf[1024];
void app_main(void *data)
{
    EatEvent_st event;
    EatEntryPara_st *para = 0;
    u16 len = 0;
    EatUartConfig_st uart_config;

    APP_InitRegions();//Init app RAM, first step
    APP_init_clib(); //C library initialize, second step
    eat_trace(" app_main ENTRY");
       
    para = (EatEntryPara_st*)data;

    //memcpy(&app_para, para, sizeof(EatEntryPara_st));
    eat_trace(" App Main ENTRY  update:%d result:%d", para->is_update_app,para->update_app_result);
    if(para->is_update_app && para->update_app_result)
    {
        eat_update_app_ok();
    }

    if( EAT_FALSE == mem_ini_flag)
    {    
        mem_ini_flag = eat_mem_init(app_dynic_mem_test, sizeof(app_dynic_mem_test));
        if(EAT_TRUE == mem_ini_flag)
        {
            eat_trace("eat_mem_init() P\n"); 
        }else
        {
            eat_trace("eat_mem_init() F\n");
        }        
    }else
    {
        eat_trace("eat mem init already \n");
    }    
    if(eat_uart_open(eat_uart_app ) == EAT_FALSE)
    {
	    eat_trace("[%s] uart(%d) open fail!", __FUNCTION__, eat_uart_app);
    }else
    {
        if( EAT_UART_USB != eat_uart_app )//usb port not need to config
        {
            uart_config.baud = EAT_UART_BAUD_115200;
            uart_config.dataBits = EAT_UART_DATA_BITS_8;
            uart_config.parity = EAT_UART_PARITY_NONE;
            uart_config.stopBits = EAT_UART_STOP_BITS_1;
            if(EAT_FALSE == eat_uart_set_config(eat_uart_app, &uart_config))
            {
                eat_trace("[%s] uart(%d) set config fail!", __FUNCTION__, eat_uart_app);
            }
            //eat_uart_set_send_complete_event(eat_uart_app, EAT_TRUE);
        }
    }
	eat_modem_write("AT+CREG?\n",9);
    while(EAT_TRUE)
    {
        eat_get_event(&event);
        eat_trace("MSG id%x", event.event);
        switch(event.event)
        {
            case EAT_EVENT_TIMER :
                {
                    //Restart timer
                    if( EAT_TIMER_1 == event.data.timer.timer_id)
                    {
                        //eat_timer_start(EAT_TIMER_1, 1000);
                        eat_sleep_enable(EAT_FALSE);
                        eat_trace("Wakeup");
                    }
                }
                break;
            case EAT_EVENT_MDM_READY_RD:
                {
                    u8 param1,param2;
                    len = 0;
                    len = eat_modem_read(buf, 1024);
                    if(len > 0)
                    {
                        //Get the testing parameter
                        if(eat_modem_data_parse(buf,len,&param1,&param2))
                        {
                            //Entry timer test module
                            eat_module_test_sys(param1, param2);
                        }
                        else if(App_SMS_is_ready(buf,len))
						{	
                            eat_trace("yemaolin test 1111\n");
                        }
						else if(App_Msg_is_OK(buf,len))
						{
                            eat_trace("yemaolin test 2222\n");
                        }	
						else
                        {
                            eat_trace("From Mdm:%s",buf);
                        }
                    }

                }
                break;
            case EAT_EVENT_USER_MSG:
              {
                eat_trace("app_user0: %d %d %d %s %x", event.data.user_msg.src, event.data.user_msg.use_point, 
                event.data.user_msg.len, event.data.user_msg.data, event.data.user_msg.data_p); 
              }
              eat_trace("333 From Mdm:\n");
                break;
            case EAT_EVENT_MDM_READY_WR:
				 eat_trace("444 From Mdm:\n");
            case EAT_EVENT_UART_READY_RD:
				  eat_trace("555 From Mdm:\n");
                break;
            case EAT_EVENT_UART_SEND_COMPLETE :
				 eat_trace("666 From Mdm:\n");
                break;
            default:
				 eat_trace("777 From Mdm:\n");
                break;
        }

    }

}

    /***************************************
     * example 5
     * multi thread demo
     ***************************************/ 
void app_user1(void *data)
{
    EatEvent_st event;
    u32 num1;
    int sem_val;
    eat_bool ret;
    eat_trace("%s-%d: app_user1 ENTRY", __FILE__, __LINE__);
        /*
    sem_id_user1 = eat_create_sem("eat_test",0);
    
    
    ret = eat_sem_get(sem_id_user1,EAT_NO_WAIT);
    if( EAT_TRUE == ret )
    {
        eat_trace("EAT_NO_WAIT:get one sem");
    }else
    {
        eat_trace("EAT_NO_WAIT:get none sem");
    }        
    eat_sem_put(sem_id_user1);
    sem_val = eat_sem_query(sem_id_user1);
    eat_trace("sem value:%d",sem_val);

    do
    {   eat_trace("EAT_INFINITE_WAIT,getting sem..... ");
        eat_sem_get(sem_id_user1,EAT_INFINITE_WAIT);
        eat_trace("EAT_INFINITE_WAIT: get one sem");
    }while(1);
      */
			eat_modem_write("AT+CREG?\n",9);

   while(1)
    {
        eat_get_event_for_user(EAT_USER_1, &event);
        if(event.event == EAT_EVENT_USER_MSG)
        {
            eat_trace("app_user1: %d %d %d %s %x", event.data.user_msg.src, event.data.user_msg.use_point, 
                event.data.user_msg.len, event.data.user_msg.data, event.data.user_msg.data_p);           
       	}
		else if(event.event == EAT_EVENT_MDM_READY_RD)
        {
            num1 = eat_modem_read(buf, 200);
            if(num1 > 0)
            {
                buf[num1] = 0;
                eat_trace("from modem1111:%s",buf);
            }
        }
   }
}



void app_user2(void *data)
{
    u32 num1;

    EatEvent_st event;
    u32 timer_counter = 0;
    eat_trace("%s-%d: app_user2 ENTRY", __FILE__, __LINE__);
#ifdef EAT_TIMER_TEST
    eat_timer_start(EAT_TIMER_2, 1000);
#endif
	eat_modem_write("AT+CREG?\n",9);

    while(1)
    {
        eat_get_event_for_user(EAT_USER_2, &event);
        if(event.event == EAT_EVENT_USER_MSG)
        {
            eat_trace("app_user2: %d %d %d %s %x", event.data.user_msg.src, event.data.user_msg.use_point, 
                event.data.user_msg.len, event.data.user_msg.data, event.data.user_msg.data_p); 
                    
        }else if(EAT_EVENT_TIMER == event.event)
        {
            eat_trace("timer[%d]:%d %d",event.data.timer.timer_id,1000, timer_counter++ );
            eat_timer_start(event.data.timer.timer_id, 1000);
        }
		else if(event.event == EAT_EVENT_MDM_READY_RD)
        {
            num1 = eat_modem_read(buf, 200);
            if(num1 > 0)
            {
                buf[num1] = 0;
                eat_trace("from modem2222:%s",buf);
            }
        }
    }
}

void app_user3(void *data)
{
    u32 num1;

    EatEvent_st event;
    u32 timer_counter = 0;
    eat_trace("%s-%d: app_user3 ENTRY", __FILE__, __LINE__);
#ifdef EAT_TIMER_TEST
    eat_timer_start(EAT_TIMER_3, 500);
#endif
	eat_modem_write("AT+CREG?\n",9);

    while(1)
    {
        eat_get_event_for_user(EAT_USER_3, &event);
        if(event.event == EAT_EVENT_USER_MSG)
        {
            eat_trace("app_user3: %d %d %d %s %x", event.data.user_msg.src, event.data.user_msg.use_point, 
                event.data.user_msg.len, event.data.user_msg.data, event.data.user_msg.data_p); 
                    
        }else if(EAT_EVENT_TIMER == event.event)
        {
            eat_trace("timer[%d]:%d %d",event.data.timer.timer_id,500,timer_counter++);
            eat_timer_start(event.data.timer.timer_id, 500);
        }
		else if(event.event == EAT_EVENT_MDM_READY_RD)
        {
            num1 = eat_modem_read(buf, 200);
            if(num1 > 0)
            {
                buf[num1] = 0;
                eat_trace("from modem33333:%s",buf);
            }
        }
    }
}

void app_user4(void *data)
{
    EatEvent_st event;
	 u32 num1;
    u32 timer_counter = 0;
    eat_trace("%s-%d: app_user4 ENTRY", __FILE__, __LINE__);
#ifdef EAT_TIMER_TEST
    eat_timer_start(EAT_TIMER_4, 1300);
#endif
	eat_modem_write("AT+CREG?\n",9);

    while(1)
    {
        eat_get_event_for_user(EAT_USER_4, &event);
        if(event.event == EAT_EVENT_USER_MSG)
        {
            eat_trace("app_user4: %d %d %d %s %x", event.data.user_msg.src, event.data.user_msg.use_point, 
                event.data.user_msg.len, event.data.user_msg.data, event.data.user_msg.data_p); 
                    
        }else if(EAT_EVENT_TIMER == event.event)
        {
            eat_trace("timer[%d]:%d %d",event.data.timer.timer_id,1300, timer_counter++);
            eat_timer_start(event.data.timer.timer_id, 1300);
        }
		else if(event.event == EAT_EVENT_MDM_READY_RD)
        {
            num1 = eat_modem_read(buf, 200);
            if(num1 > 0)
            {
                buf[num1] = 0;
                eat_trace("from modem444444:%s",buf);
            }
        }
    }
}

void app_user5(void *data)
{
    EatEvent_st event;
	 u32 num1;
    u32 timer_counter = 0;
    eat_trace("%s-%d: app_user5 ENTRY", __FILE__, __LINE__);
#ifdef EAT_TIMER_TEST
    eat_timer_start(EAT_TIMER_5, 700);
#endif
	eat_modem_write("AT+CREG?\n",9);

    while(1)
    {
        eat_get_event_for_user(EAT_USER_5, &event);
        if(event.event == EAT_EVENT_USER_MSG)
        {
            eat_trace("app_user5: %d %d %d %s %x", event.data.user_msg.src, event.data.user_msg.use_point, 
                event.data.user_msg.len, event.data.user_msg.data, event.data.user_msg.data_p); 
                    
        }else if(EAT_EVENT_TIMER == event.event)
        {
            eat_trace("timer[%d]:%d %d",event.data.timer.timer_id,700, timer_counter++);
            eat_timer_start(event.data.timer.timer_id, 700);
        }
				else if(event.event == EAT_EVENT_MDM_READY_RD)
        {
            num1 = eat_modem_read(buf, 200);
            if(num1 > 0)
            {
                buf[num1] = 0;
                eat_trace("from modem55555:%s",buf);
            }
        }
    }
}

void app_user6(void *data)
{
    EatEvent_st event;
	 u32 num1;
    u32 timer_counter = 0;
    eat_msg_t msg;
    eat_trace("%s-%d: app_user6 ENTRY", __FILE__, __LINE__);
#ifdef EAT_TIMER_TEST
    eat_timer_start(EAT_TIMER_6, 3000);
#endif
	eat_modem_write("AT+CREG?\n",9);

    while(1)
    {
        eat_get_event_for_user(EAT_USER_6, &event);
        if(event.event == EAT_EVENT_USER_MSG)
        {
            eat_trace("app_user6: %d %d %d %s %x", event.data.user_msg.src, event.data.user_msg.use_point, 
                event.data.user_msg.len, event.data.user_msg.data, event.data.user_msg.data_p);

            memcpy(&msg,event.data.user_msg.data, event.data.user_msg.len);
            eat_trace("msgID=%x msglen=%x %s", msg.id, msg.len, msg.data);
        }else if(EAT_EVENT_TIMER == event.event)
        {
            eat_trace("timer[%d]:%d %d",event.data.timer.timer_id,300, timer_counter++);
            eat_timer_start(event.data.timer.timer_id, 300);
        }
				else if(event.event == EAT_EVENT_MDM_READY_RD)
        {
            num1 = eat_modem_read(buf, 200);
            if(num1 > 0)
            {
                buf[num1] = 0;
                eat_trace("from modem6666:%s",buf);
            }
        }
    }
}

u32 user7_msg_account=0;
void app_user7(void *data)
{
    EatEvent_st event;
	 u32 num1;
	 eat_modem_write("AT+CSQ\n",7);
    eat_trace("%s-%d: app_user7 ENTRY", __FILE__, __LINE__);
    while(1)
    {
        eat_get_event_for_user(EAT_USER_7, &event);
        if(event.event == EAT_EVENT_USER_MSG)
        {
            eat_trace("app_user7: %d %d %d %s %x", event.data.user_msg.src, event.data.user_msg.use_point, 
                event.data.user_msg.len, event.data.user_msg.data, event.data.user_msg.data_p);           
            user7_msg_account++;
            eat_trace("user7_msg_account=%d->%d",user7_msg_account,user7_msg_account%2);
            if(user7_msg_account%2==1)
                eat_send_msg_to_user(EAT_USER_7, EAT_USER_7, EAT_FALSE, 60, "000000000011111111112222222222333333333344444444445555555555", EAT_NULL);    
        }
		else if(event.event == EAT_EVENT_MDM_READY_RD)
        {
            num1 = eat_modem_read(buf, 200);
            if(num1 > 0)
            {
                buf[num1] = 0;
                eat_trace("from modem777777:%s",buf);
            }
        }
   }
    //    eat_modem_write("AT+CSQ\n",7); //only for testing
}

void app_user8(void *data)
{
    u32 num, num1;
    EatEvent_st event;
    eat_trace("%s-%d: app_user8 ENTRY", __FILE__, __LINE__);
	eat_modem_write("AT+CREG?\n",9);

    while(1)
    {
        num1= eat_get_event_num_for_user(EAT_USER_8);
        eat_get_event_for_user(EAT_USER_8, &event);
        num= eat_get_event_num_for_user(EAT_USER_8);
        eat_trace("user8: event num =%d,%d",num1,num);
        
        if(event.event == EAT_EVENT_USER_MSG)
        {
            eat_trace("user8: %d %d %d %s %x", event.data.user_msg.src, event.data.user_msg.use_point, 
                event.data.user_msg.len, event.data.user_msg.data, event.data.user_msg.data_p);  
                       
        }
		else if(event.event == EAT_EVENT_MDM_READY_RD)
        {
            num1 = eat_modem_read(buf, 200);
            if(num1 > 0)
            {
                buf[num1] = 0;
                eat_trace("from modem8888:%s",buf);
            }
        }
        eat_mem_free((void *)event.data.user_msg.data_p);
        //eat_modem_write("AT+CREG?\n",9); //only for testing
    }
}

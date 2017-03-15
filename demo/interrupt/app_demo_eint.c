/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   app_demo_eint.c
 * version       :   0.10
 * Description   :   
 * Authors       :   fangshengchang
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2012-09-24, fangshengchang, create originally.
 *
 *--------------------------------------------------------------------
 * File Description
 * AT+CEAT=parma1,param2
 * param1 param2 
 *   1      1    Set the EAT_PIN45_GPIO3 pin as TRIGGER_LOW_LEVEL interrupt,used callback
 *   1      2    Set the EAT_PIN45_GPIO3 pin as TRIGGER_FALLING_EDGE interrupt,used callback 
 *
 *   2      1    Set the EAT_PIN45_GPIO3 pin as TRIGGER_HIGH_LEVELinterrupt,used EVENT  
 *   2      2    Set the EAT_PIN45_GPIO3 pin as TRIGGER_RISING_EDGE interrupt,used EVENT  
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
#include "eat_clib_define.h" //only in main.c
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

/********************************************************************
 * External Functions declaration
 ********************************************************************/
extern void APP_InitRegions(void);

/********************************************************************
 * Local Function declaration
 ********************************************************************/
void app_main(void *data);
void app_user1(void *data);
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
		(app_user_func)app_user1,//app_user1,
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
    
    eat_sim_detect_en(EAT_FALSE); //disable SIM card detection and will use PIN54 as eint

    eat_pin_set_mode(EAT_PIN6_UART1_DTR, EAT_PIN_MODE_EINT);
    eat_pin_set_mode(EAT_PIN14_SIM_DET, EAT_PIN_MODE_EINT); 
    eat_pin_set_mode(EAT_PIN23_UART2_RXD, EAT_PIN_MODE_EINT);

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

void test_handler_int_level(EatInt_st *interrupt)
{
    eat_trace("test_handler_int_level interrupt->pin [%d ],interrupt->level=%d",interrupt->pin,interrupt->level);
    if(!(interrupt->level))
    {
        //high level trigger,and set low level trigger
       eat_int_set_trigger(interrupt->pin, EAT_INT_TRIGGER_HIGH_LEVEL);
       eat_trace("System interrupt by interrupt->pin [%d ]TRIGGER_LOW_LEVEL",interrupt->pin);
    }
    else
    {
        //low level trigger
        eat_int_set_trigger(interrupt->pin, EAT_INT_TRIGGER_LOW_LEVEL);
        eat_trace("System interrupt by interrupt->pin [%d ] TRIGGER_HIGH_LEVEL",interrupt->pin);        
    }
}

void test_handler_int_edge(EatInt_st *interrupt)
{
    eat_trace("test_handler_int_edge interrupt->pin [%d ],interrupt->level=%d",interrupt->pin,interrupt->level);
    if(interrupt->level)
    {
        //Rising edge trigger,and set falling trigger
       eat_int_set_trigger(interrupt->pin, EAT_INT_TRIGGER_FALLING_EDGE);
       eat_trace("System interrupt by Einterrupt->pin [%d ] TRIGGER_RISING_EDGE",interrupt->pin);
    }
    else
    {
        //Falling trigger
        eat_int_set_trigger(interrupt->pin, EAT_INT_TRIGGER_RISING_EDGE);
        eat_trace("System interrupt by interrupt->pin [%d ] TRIGGER_FALLING_EDGE",interrupt->pin);        
    }
}

void test_handler_event(EatInt_st* eint)
{
    /*if LEVEL interrupt, must set inverse Sensitivity*/
    eat_trace("System interrupt PIN:%d by  Now LEVEL:%d", eint->pin,eint->level);
}

/****************************************************
 * Int testing module
 *****************************************************/
eat_bool eat_module_test_eint(u8 param1, u8 param2)
{
    /***************************************
     * example 1
     * EAT_PIN6_UART1_DTR interrupt used callback
     ***************************************/
    eat_trace("Eeat_module_test_eint");
    if( 1 == param1 )
    {
        if( 1 == param2 )
        {
           eat_int_setup(EAT_PIN6_UART1_DTR, EAT_INT_TRIGGER_HIGH_LEVEL, 10, test_handler_int_level);
            eat_trace("EINT ...b.....");
           eat_trace("EINT test 1,1 start");
           eat_trace("System will interrupt  TRIGGER_HIGH_LEVEL");
        }else if( 2 == param2)
        {                   
           eat_int_setup(EAT_PIN6_UART1_DTR, EAT_INT_TRIGGER_FALLING_EDGE, 10, test_handler_int_edge);

           eat_trace("EINT test 1,2 start");
           eat_trace("System will interrupt  EAT_INT_TRIGGER_FALLING_EDGE");
        }
    }
    /***************************************
     * example 2
     * EAT_PIN6_UART1_DTR interrupt used EVENT
     ***************************************/
    else if( 2 == param1 )
    {
        if( 1 == param2)
        {
           eat_gpio_setup(EAT_PIN6_UART1_DTR, EAT_GPIO_DIR_INPUT, EAT_GPIO_LEVEL_LOW);
           if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(EAT_PIN6_UART1_DTR)) 
           {   
               eat_trace("EINT PIN%d HIGH LEVEL int test start", EAT_PIN6_UART1_DTR);
               eat_int_setup(EAT_PIN6_UART1_DTR, EAT_INT_TRIGGER_HIGH_LEVEL, 10, NULL);
           }
           else
           {
               eat_trace("EINT PIN%d LOW LEVEL int test start", EAT_PIN6_UART1_DTR);
               eat_int_setup(EAT_PIN6_UART1_DTR, EAT_INT_TRIGGER_LOW_LEVEL, 10, NULL);
           }
           
           eat_gpio_setup(EAT_PIN14_SIM_DET, EAT_GPIO_DIR_INPUT, EAT_GPIO_LEVEL_LOW);
           if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(EAT_PIN14_SIM_DET)) 
           {
               eat_trace("EINT PIN%d HIGH LEVEL int test start", EAT_PIN14_SIM_DET);
               eat_int_setup(EAT_PIN14_SIM_DET, EAT_INT_TRIGGER_HIGH_LEVEL, 10, NULL);
           }else
           {
               eat_trace("EINT PIN%d LOW LEVEL int test start", EAT_PIN14_SIM_DET);
               eat_int_setup(EAT_PIN14_SIM_DET, EAT_INT_TRIGGER_LOW_LEVEL, 10, NULL);
           }
           
           eat_gpio_setup(EAT_PIN23_UART2_RXD, EAT_GPIO_DIR_INPUT, EAT_GPIO_LEVEL_LOW);
           if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(EAT_PIN23_UART2_RXD)) 
           {
               eat_trace("EINT PIN%d HIGH LEVEL int test start", EAT_PIN23_UART2_RXD);
               eat_int_setup(EAT_PIN23_UART2_RXD, EAT_INT_TRIGGER_HIGH_LEVEL, 10, NULL);
           }else
           {
               eat_trace("EINT PIN%d LOW LEVEL int test start", EAT_PIN23_UART2_RXD);
               eat_int_setup(EAT_PIN23_UART2_RXD, EAT_INT_TRIGGER_LOW_LEVEL, 10, NULL);
           }

           
           eat_trace("EINT test 2,1 start");
           eat_trace("System will interrupt by  TRIGGER_LOW_LEVEL");
        }else
        {
           eat_int_setup(EAT_PIN6_UART1_DTR, EAT_INT_TRIGGER_RISING_EDGE, 10, NULL);
           //eat_int_setup(EAT_PIN24_COL4, EAT_INT_TRIGGER_RISING_EDGE, 10, NULL);
           //eat_int_setup(EAT_PIN69_UART1_DTR, EAT_INT_TRIGGER_RISING_EDGE, 10, NULL);
           
           eat_trace("EINT test 2,2 start");
           eat_trace("System will interrupt by  TRIGGER_RISING_EDGE");
        }
    } 

    return EAT_TRUE;
}


u8 buf[2048];
void app_main(void *data)
{
    EatEvent_st event;
    u16 len = 0;

    APP_InitRegions();//Init app RAM, first step
    APP_init_clib(); //C library initialize, second step

    eat_trace(" app_main ENTRY");

    while(EAT_TRUE)
    {
        eat_get_event(&event);
        eat_trace("MSG id%x", event.event);
        switch(event.event)
        {
            case EAT_EVENT_INT :
                {
                    //GPIO INT EVENT
                    //test_handler_event(&event.data.interrupt);
                    test_handler_int_level(&event.data.interrupt);
                }
                break;
           
            case EAT_EVENT_MDM_READY_RD:
                {
                    u8 param1,param2;
                    len = 0;
                    len = eat_modem_read(buf, 2048);
                    if(len > 0)
                    {
                        //Get the testing parameter
                        if(eat_modem_data_parse(buf,len,&param1,&param2))
                        {
                            //Entry gpio test module
                            eat_module_test_eint(param1, param2);
                        }
                        else
                        {
                            eat_trace("From Mdm:%s",buf);
                        }
                    }

                }
                break;
            case EAT_EVENT_MDM_READY_WR:
            case EAT_EVENT_UART_READY_RD:
                break;
            case EAT_EVENT_UART_SEND_COMPLETE :
                break;
            default:
                break;
        }

    }

}
void app_user1(void *data)
{
    u32 num,num1;
    EatEvent_st event;
    eat_trace("%s-%d: app_user1 ENTRY", __FILE__, __LINE__);

    //eat_modem_write("at+creg?\r",9);
    while(1)
    {
        num1= eat_get_event_num_for_user(EAT_USER_1);
        eat_get_event_for_user(EAT_USER_1, &event);
        num= eat_get_event_num_for_user(EAT_USER_1);
        eat_trace("user1: event num =%d,%d",num1,num);
        
        if(event.event == EAT_EVENT_USER_MSG)
        {
            eat_trace("user8: %d %d %d %s %x", event.data.user_msg.src, event.data.user_msg.use_point, 
                event.data.user_msg.len, event.data.user_msg.data, event.data.user_msg.data_p);  
                       
        }else if(event.event == EAT_EVENT_INT )
        {
              test_handler_int_level(&event.data.interrupt);
        }else if(event.event == EAT_EVENT_MDM_READY_RD)
        {
            num1 = eat_modem_read(buf, 200);
            if(num1 > 0)
            {
                buf[num1] = 0;
                eat_trace("from modem:%s",buf);
            }
        }
    }
}



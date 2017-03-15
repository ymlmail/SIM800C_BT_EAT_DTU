/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   app_demo_sms.c
 * version       :   0.10
 * Description   :   
 * Authors       :   zhudingfen
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *create
 *
 *--------------------------------------------------------------------
 * File Description
 * AT+CEAT=parma1,param2
 * param1 param2 
 *   1      1    test eat_set_sms_format
 *   2      1    test eat_set_sms_cnmi
 *   3      1    test eat_set_sms_sc
 *   4      1    test eat_set_sms_storage
 *   5      1    test eat_read_sms
 *   6      1    test eat_delete_sms
 *   7      1    test eat_send_text_sms or eat_send_pdu_sms
 *   8      1    test eat_sms_decode_tpdu
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
#include "eat_sms.h"

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
    eat_uart_set_debug(EAT_UART_USB);
    eat_uart_set_at_port(EAT_UART_1);// UART1 is as AT PORT     
    
	eat_uart_set_debug_config(EAT_UART_DEBUG_MODE_UART, NULL);
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

static void eat_check_sms_pdu_string(u16 length, u8 *bytes, u8 *str)
{
    u16 i = 0;
    u16 j = 0;
  
    if((str == NULL) || (bytes == NULL))
    {
        eat_trace("eat_check_sms_pdu_string() failed");
        return;    
    }
    
    while (i < length)
    {
        j += sprintf((char*)str + j, "%02x", bytes[i]);
        i++;
    }
    str[j] = 0;
    //EatToUpper(str);
}

static eat_sms_flash_message_cb(EatSmsReadCnf_st smsFlashMessage)
{
    u8 format =0;

    eat_get_sms_format(&format);
    eat_trace("eat_sms_flash_message_cb, format=%d",format);
    if(1 == format)//TEXT模式
    {
        eat_trace("eat_sms_read_cb, msg=%s",smsFlashMessage.data);
        eat_trace("eat_sms_read_cb, datetime=%s",smsFlashMessage.datetime);
        eat_trace("eat_sms_read_cb, name=%s",smsFlashMessage.name);
        eat_trace("eat_sms_read_cb, status=%d",smsFlashMessage.status);
        eat_trace("eat_sms_read_cb, len=%d",smsFlashMessage.len);
        eat_trace("eat_sms_read_cb, number=%s",smsFlashMessage.number);
    }
    else//PDU模式
    {
        eat_trace("eat_sms_read_cb, msg=%s",smsFlashMessage.data);
        eat_trace("eat_sms_read_cb, len=%d",smsFlashMessage.len);
    }
}


static void eat_sms_read_cb(EatSmsReadCnf_st  smsReadCnfContent)
{
    u8 format =0;

    eat_get_sms_format(&format);
    eat_trace("eat_sms_read_cb, format=%d",format);
    if(1 == format)//TEXT模式
    {
        eat_trace("eat_sms_read_cb, msg=%s",smsReadCnfContent.data);
        eat_trace("eat_sms_read_cb, datetime=%s",smsReadCnfContent.datetime);
        eat_trace("eat_sms_read_cb, name=%s",smsReadCnfContent.name);
        eat_trace("eat_sms_read_cb, status=%d",smsReadCnfContent.status);
        eat_trace("eat_sms_read_cb, len=%d",smsReadCnfContent.len);
        eat_trace("eat_sms_read_cb, number=%s",smsReadCnfContent.number);
    }
    else//PDU模式
    {
        eat_trace("eat_sms_read_cb, msg=%s",smsReadCnfContent.data);
        eat_trace("eat_sms_read_cb, name=%s",smsReadCnfContent.name);
        eat_trace("eat_sms_read_cb, status=%d",smsReadCnfContent.status);
        eat_trace("eat_sms_read_cb, len=%d",smsReadCnfContent.len);
    }
}

static void eat_sms_delete_cb(eat_bool result)
{
    eat_trace("eat_sms_delete_cb, result=%d",result);
}

static void eat_sms_send_cb(eat_bool result)
{
    eat_trace("eat_sms_send_cb, result=%d",result);
}

static eat_sms_new_message_cb(EatSmsNewMessageInd_st smsNewMessage)
{
    eat_trace("eat_sms_new_message_cb, storage=%d,index=%d",smsNewMessage.storage,smsNewMessage.index);
	
}

static void eat_sms_ready_cb(eat_bool result)
{
    eat_trace("eat_sms_ready_cb, result=%d",result);
}

/****************************************************
 * GPIO testing module
 *****************************************************/
eat_bool eat_module_test_sms(u8 param1, u8 param2)
{
    s32 ret;
    u8 read_buffer[10]=0;
    u8 write_buffer[10]=0;
    /***************************************
     * example 1
     * used eat_gpio_write test EAT_GPIO_TEST1,EAT_GPIO_TEST2
     ***************************************/
     
    eat_trace("sms test param1=%d,param2=%d",param1,param2);
    if( 1 == param1 )
    {
        if( 1 == param2 )//PDU MODE
        {
            eat_set_sms_format(0);
        }
        else if( 2 == param2 )//TEXT MODE
        {
            eat_set_sms_format(1);
        }
    }
    if( 2== param1 )
    {
        u8 mode, mt, bm, ds, bfr;
        eat_bool ret_val = EAT_FALSE;
        if(1 == param2)//cnmi=2,1
        {
            mode = 2;
            mt = 1;
            bm = 0;
            ds = 0;
            bfr = 0;
            ret_val= eat_set_sms_cnmi(mode,mt,bm,ds,bfr);
            eat_trace("eat_module_test_sms eat_set_sms_cnmi ret_val=%d",ret_val);
        }
        else if(2 == param2)//cnmi=2,2 flash message
        {

            mode = 2;
            mt = 2;
            bm = 0;
            ds = 0;
            bfr = 0;
            ret_val= eat_set_sms_cnmi(mode,mt,bm,ds,bfr);
            eat_trace("eat_module_test_sms eat_set_sms_cnmi ret_val=%d",ret_val);
        }
    }
    else if(3== param1)
    {
        if(1 == param2)//sms center
        {
            u8 scNumber[40] = {"+8613800210500"};
            eat_set_sms_sc(scNumber);
            eat_trace("eat_module_test_sms eat_set_sms_sc scNumber=%s",scNumber);
        }
    }
    else if(4== param1)
    {
        if(1 == param2)//memory storage
        {
            u8 mem1, mem2, mem3;
            eat_bool ret_val = EAT_FALSE;
            
            mem1 = EAT_ME;
            mem2 = EAT_ME;
            mem3 = EAT_ME;
            ret_val = eat_set_sms_storage(mem1, mem2, mem3);
            eat_trace("eat_module_test_sms eat_set_sms_storage ret_val=%d",ret_val);
        }
    }
    else if(5== param1)
    {
        //read single message
        u16 index = param2;
        eat_bool ret_val = EAT_FALSE;
        ret_val = eat_read_sms(index,eat_sms_read_cb);
        
        eat_trace("eat_module_test_sms eat_read_sms ret_val=%d",ret_val);
    }
    else if(6== param1)
    {
        //delete single message
        u16 index = param2;
        eat_bool ret_val = EAT_FALSE;
        ret_val = eat_delete_sms(index,eat_sms_delete_cb);
            
        eat_trace("eat_module_test_sms eat_delete_sms ret_val=%d",ret_val);
    }
    else if(7== param1)
    {
        if(1 == param2)//send a message
        {
            u8 format = 0;
            eat_bool ret_val = EAT_FALSE;
            
            eat_get_sms_format(&format);
            if(1 == format)
            {
                ret_val = eat_send_text_sms("13681673762","123456789");
            }
            else
            {
                ret_val = eat_send_pdu_sms("0011000D91683186613767F20018010400410042",19);
            }
            eat_trace("sms send format = %d, ret_val = %d",format, ret_val);
        }
    }
    else if(8== param1) // decode PDU mode message
    {
        eat_bool ret = EAT_FALSE;
        u8 ptr[]= "0891683108200105F0040D91683186613767F20000413012516585230631D98C56B301";
        u8 len = strlen(ptr);
        EatSmsalPduDecode_st sms_pdu = {0};
        u8 useData[320] = {0};
        u8 useLen = 0;
        u8 phoneNum[43] = {0};

        eat_trace("eat_sms_decode_tpdu_core, ptr = %s,len = %d",ptr,len);
        ret = eat_sms_decode_tpdu(ptr, len, &sms_pdu);
        eat_trace("eat_sms_decode_tpdu_core, ret = %d",ret );

        useLen = sms_pdu.tpdu.data.deliver_tpdu.user_data_len;
        //把短信内容转化打印出来
        eat_check_sms_pdu_string(useLen,sms_pdu.tpdu.data.deliver_tpdu.user_data,useData);
        eat_trace("simcom_sms_decode_pdu(), user data len (%d)",useLen);
        eat_trace("simcom_sms_decode_pdu(), user data(%s)",useData);

        eat_sms_orig_address_data_convert(sms_pdu.tpdu.data.deliver_tpdu.orig_addr, phoneNum);
        eat_trace("simcom_sms_decode_pdu(), phone Num(%s)",phoneNum);
    }
   
    return EAT_TRUE;
}
void app_main(void *data)
{
    EatEvent_st event;
    u8 buf[2048];
    u16 len = 0;

    APP_InitRegions();//Init app RAM
    APP_init_clib();

    eat_set_sms_operation_mode(EAT_TRUE);
    eat_sms_register_new_message_callback(eat_sms_new_message_cb);
    eat_sms_register_flash_message_callback(eat_sms_flash_message_cb);
    eat_sms_register_send_completed_callback(eat_sms_send_cb);
    eat_sms_register_sms_ready_callback(eat_sms_ready_cb);

    eat_trace(" app_main ENTRY");
    while(EAT_TRUE)
    {
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
                        //Get the testing parameter
                        if(eat_modem_data_parse(buf,len,&param1,&param2))
                        {
                            //Entry sms test module
                            eat_module_test_sms(param1, param2);
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



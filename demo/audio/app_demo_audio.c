/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   main_customer.c
 * version       :   0.10
 * Description   :   
 * Authors       :   chb
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2012-09-24, chb, create originally.
 *
 *--------------------------------------------------------------------
 * File Description
 * AT+CEAT=parma1,param2
 * param1 param2 
 *   1      1    Play tone sound
 *   1      2    Stop tone sound
 *
 *   2      1    Play sound of midi farmat
 *   2      2    Stop sound of midi farmat
 *   
 *   3      1    Play sound of wav format
 *   3      2    Stop sound of wav format
 *
 *   4      1    Play custom tone sound
 *   4      2    Stop custom tone sound
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
#include "eat_audio.h"

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
static EatAudioToneId_enum audio_test_tone_id = EAT_TONE_DIAL_CALL_GSM;
static EatAudioStyle_enum audio_test_style = EAT_AUDIO_PLAY_ONCE; //EAT_AUDIO_PLAY_INFINITE;
static EatAudioPath_enum aud_out_dev = EAT_AUDIO_PATH_SPK1;
static u8 audio_test_volum = 12;

//audio_test_volum = 12;
//aud_out_dev = EAT_AUDIO_PATH_SPK1;
//audio_test_style = EAT_AUDIO_PLAY_ONCE;
//audio_test_tone_id = EAT_TONE_DIAL_CALL_GSM;


#include "audio_resource.c"

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
	eat_uart_set_debug_config(EAT_UART_DEBUG_MODE_UART, NULL);
	//配置USB口做调试口，需要打印ASCII时需要把上面这句也配置下
	//eat_uart_set_debug(EAT_UART_2);
    eat_uart_set_at_port(EAT_UART_1);// UART1 is as AT PORT
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


/****************************************************
 * Audio testing module
 *****************************************************/
eat_bool eat_module_test_audio(u8 param1, u8 param2)
{
    /***************************************
     * example 1
     * play and stop 3 tone id
     ***************************************/
    if( 1 == param1 )
    {

        if( 1 == param2 )
        {
            eat_trace("tone_play id =%d, play style =%d,vol =%d  path = %d",audio_test_tone_id , audio_test_style, audio_test_volum, aud_out_dev);
            eat_audio_play_tone_id(audio_test_tone_id, audio_test_style, audio_test_volum, aud_out_dev);
            
        }
        else if( 2 == param2)
        {

            eat_trace("audio_test_tone_stop id =%d, play style =%d,vol =%d  path = %d", audio_test_tone_id, audio_test_style, audio_test_volum, aud_out_dev);
            eat_audio_stop_tone_id(audio_test_tone_id);
        }
    }
    /***************************************
     * example 2
     * play and stop 3 mid string
     ***************************************/
    else if( 2 == param1 )
    {
        if( 1 == param2)
        {
            eat_trace("audio_test_midi_play play style =%d,vol =%d  path = %d",  audio_test_style, audio_test_volum, aud_out_dev);
            eat_audio_play_data(audio_test_midi_data, sizeof(audio_test_midi_data), EAT_AUDIO_FORMAT_MIDI, audio_test_style, audio_test_volum, aud_out_dev);
        }
        else
        {
            eat_trace("audio_test_midi_stop play style =%d,vol =%d  path = %d",  audio_test_style, audio_test_volum, aud_out_dev);
            eat_audio_stop_data();
        }

    }
    /***************************************
     * example 3
     * play and stop 3 wav string
     ***************************************/
    else if( 3 == param1 )
    {
        if( 1 == param2)
        {
            eat_trace("audio_test_wav_play play style =%d,vol =%d  path = %d",  audio_test_style, audio_test_volum, aud_out_dev);
            eat_audio_play_data(audio_test_wav_data, sizeof(audio_test_wav_data), EAT_AUDIO_FORMAT_WAV, audio_test_style, audio_test_volum, aud_out_dev);
        }
        else if( 2 == param2 )
        {
            eat_trace("audio_test_wav_stop play style =%d,vol =%d  path = %d",  audio_test_style, audio_test_volum, aud_out_dev);
            eat_audio_stop_data();
        }
    }
    /***************************************
     * example 4
     * play and stop custom tone sound
     ***************************************/
    else if( 4 == param1 )
    {
        if( 1 == param2)
        {
            EatAudioToneTable_st tone = 
                {   300,   0, 200, 600,   1, 
                      2000,   0, 200,600,   2,
                      5000, 0, 600, 600, 3,
                      6000, 0, 500, 11, 4,
                      100, 200, 500, 10, 5,
                      200, 300, 500, 10, 6,
                      300, 400, 500, 10, 7,
                      400, 500, 500, 10, 8,
                      500, 600, 500, 10, 9,
                      600, 700, 500, 10, 10,
                      700, 800, 500, 10, 11,
                      800, 900, 500, 10, 12,
                      900,1000, 500, 10, 13,
                      1000, 1100, 500, 10, 14,
                      1100, 1200, 500, 10, 0
                  };

            eat_trace("audio_test_custom_tone_play ");
                
            eat_audio_set_custom_tone(&tone);
            eat_audio_play_tone_id(EAT_TONE_CUSTOM_DTMF, audio_test_style, audio_test_volum, aud_out_dev);


        }
        else if( 2 == param2 )
        {
            eat_audio_stop_tone_id(EAT_TONE_CUSTOM_DTMF);
        }
    }
	
    else if( 5 == param1 )
	{
        if( 1 == param2)
        {
            eat_trace("audio_test_wav_play play style =%d,vol =%d  path = %d",  audio_test_style, audio_test_volum, aud_out_dev);
            eat_audio_play_data(audio_test_wav_data, sizeof(audio_test_wav_data), EAT_AUDIO_FORMAT_WAV, audio_test_style, audio_test_volum, aud_out_dev);
        }
        else if( 2 == param2 )
        {
            eat_trace("audio_test_wav_stop play style =%d,vol =%d  path = %d",  audio_test_style, audio_test_volum, aud_out_dev);
            eat_audio_stop_data();
        }
    }
	    else if( 0 == param1 )
	{
        if( 1 == param2)
        {
        audio_test_style=0;
		audio_test_volum++;
		if(audio_test_volum>15)
			audio_test_volum=0;
		aud_out_dev=0;
		
		eat_trace("1 audio_test_style=%d,vol =%d",audio_test_style, audio_test_volum);
        }
        else if( 2 == param2 )
        {
			audio_test_style=1;
			audio_test_tone_id++;
			if(audio_test_tone_id>=EAT_MAX_TONE_ID)
				audio_test_tone_id=EAT_TONE_DTMF_0;
			aud_out_dev=0;
			
			eat_trace("2 audio_test_style=%d,vol =%d",audio_test_style, audio_test_volum);
        }
    }
    return EAT_TRUE;
}


void app_main(void *data)
{


    EatEvent_st event;
    u8 buf[2048];
    u16 len = 0;
	
	audio_test_volum = 12;
	aud_out_dev = EAT_AUDIO_PATH_SPK1;
	audio_test_style = EAT_AUDIO_PLAY_ONCE;
	audio_test_tone_id = EAT_TONE_DIAL_CALL_GSM;

    APP_InitRegions();//Init app RAM, first step
    APP_init_clib(); //C library initialize, second step
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
                    {	 memset(buf+len,0,2048-len);//把len后面的空间清零
                        //Get the testing parameter
                        if(eat_modem_data_parse(buf,len,&param1,&param2))
                        {
                            //Entry audio test module
                            eat_module_test_audio(param1, param2);
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



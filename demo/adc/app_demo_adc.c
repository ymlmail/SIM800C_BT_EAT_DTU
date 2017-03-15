/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   app_demo_adc.c
 * version       :   0.10
 * Description   :   
 * Authors       :   fangshengchang
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2012-09-24, fangshengchang, create originally.
 *0.20  2012-12-04, maobin modify.
 *         "AT"
 *   UART1 ---> modem
 *                  
 *         "AT" 
 *   UART3 --->APP ---> Modem AT
 *     |             "OK" |
 *     '<---- APP <-------'
 *
 *--------------------------------------------------------------------
 * File Description
 * AT+CEAT=parma1,param2
 * param1 param2 
 *   1      1    Read ADC of the EAT_ADC0 pin by callback func 
 *
 *   2      1    Read ADC of the EAT_ADC0 pin by app event(callback is NULL) 
 *
 *   3      1    Read ADC of the  EAT_ADC1 pin by callback func 
 *
 *   4      1    Read ADC of the  EAT_ADC1 pin by app event(callback is NULL) 
 *
 *   5      1    Read ADC directly
 *
 *   6      1    Read module temperature directly
 *          2    Read module temperature by callback func
 *   
 *--------------------------------------------------------------------
 ********************************************************************/
/********************************************************************
 * Include Files
 ********************************************************************/
#include "eat_interface.h"
#include "app_demo_adc.h"
/********************************************************************
* Macros
 ********************************************************************/
#if defined(__SIMCOM_PROJ_SIM800W__)
  //SIM800W only have one ADC
  #define EAT_ADC0 EAT_PIN6_ADC0
  #define EAT_ADC1 EAT_PIN6_ADC0
#elif defined __SIMCOM_PROJ_SIM800V__
  //SIM800V have two ADC
  #define EAT_ADC0 EAT_PIN9_ADC0
  #define EAT_ADC1 EAT_PIN5_ADC1
#elif defined __SIMCOM_PROJ_SIM800H__
  //SIM800H only have one ADC
  #define EAT_ADC0 EAT_PIN50_ADC
  #define EAT_ADC1 EAT_PIN50_ADC
#elif defined __SIMCOM_PROJ_SIM800__
  //SIM800 only have one ADC
  #define EAT_ADC0 EAT_PIN25_ADC
  #define EAT_ADC1 EAT_PIN25_ADC
#elif defined __SIMCOM_PROJ_SIM800C__
  //SIM800C only have one ADC
  #define EAT_ADC0 EAT_PIN38_ADC
  #define EAT_ADC1 EAT_PIN38_ADC
#endif
/********************************************************************
* Types
 ********************************************************************/
/********************************************************************
* Extern Variables (Extern /Global)
 ********************************************************************/
 
/********************************************************************
* Local Variables:  STATIC
 ********************************************************************/
/********************************************************************
* External Functions declaration
 ********************************************************************/

/********************************************************************
* Local Function declaration
 ********************************************************************/
/********************************************************************
* Local Function
 ********************************************************************/
static eat_temp_callback_func temp_callback(s32 *temp)
{
    eat_trace("CallBack mode temperature [%d]",(*temp));
}
//ADC callback function
void adc_cb_proc(EatAdc_st* adc)
{
    if(adc->pin == EAT_ADC0)
    {
        eat_trace("CallBack mode EAT_ADC0 = %d",adc->v); 
    }else if( adc->pin == EAT_ADC1 )
    {
        eat_trace("CallBack mode EAT_ADC1 = %d",adc->v); 
    }
}

void adc_event_proc(EatEvent_st *event)
{
    if(event->data.adc.pin == EAT_ADC0)
    {
        eat_trace("EVENT mode EAT_ADC0 = %d",event->data.adc.v); 
    }else if( event->data.adc.pin == EAT_ADC1 )
    {
        eat_trace("EVENT mode EAT_ADC1 = %d",event->data.adc.v); 
    }
}
/****************************************************
 * ADC testing module
 *****************************************************/
eat_bool eat_module_test_adc(u8 param1, u8 param2)
{
    /***************************************
     * example 1
     * callback adc 
     ***************************************/
    if( 1 == param1 )
    {
        if( 1 == param2 )
        {
            eat_adc_get(EAT_ADC0, 3000, adc_cb_proc);
            eat_trace("ADC test 1,1 start");
        }else
        {
            eat_adc_get(EAT_ADC0, 0, NULL);
            eat_trace("ADC test 1,x stop");
        }
    }
    /***************************************
     * example 2
     * Event ADC
     ***************************************/
    else if( 2 == param1 )
    {
        if( 1 == param2)
        {
            eat_adc_get(EAT_ADC0, 3000, NULL);
            eat_trace("ADC test 2,1 start");
        }else
        {
            eat_adc_get(EAT_ADC0, 0, NULL);
            eat_trace("ADC test 2,x stop");
        }

    }else if( 3 == param1 )
    {
        if( 1 == param2 )
        {
            eat_adc_get(EAT_ADC1 , 3000, adc_cb_proc);
            eat_trace("ADC test 3,1 PIN5_ADC1 start");
        }else
        {
            eat_adc_get(EAT_ADC1 , 0, NULL);
            eat_trace("ADC test 1,x EAT_ADC1 stop");
        }
    }
    /***************************************
     * example 4
     * Event ADC
     ***************************************/
    else if( 4 == param1 )
    {
        if( 1 == param2)
        {
            eat_adc_get(EAT_ADC1 , 3000, NULL);
            eat_trace("ADC test 4,1 EAT_ADC1 start");
        }else
        {
            eat_adc_get(EAT_ADC1, 0, NULL);
            eat_trace("ADC test 4,x stop");
        }
    }
    /***************************************
     * example 5
     * ADC[sync]
     ***************************************/
     else if(5 == param1)
     {        
       if (1 == param2)
        {
         u32 voltage;
         eat_get_adc_sync(EAT_ADC0 ,&voltage);
         eat_trace("ADC test 5,adc [%d]",voltage);
        }
     }
      /***************************************
     * example 6
     * Read module temperature
     ***************************************/
    else if(6 == param1)
    {
     if (1 == param2)
        {  /*sync*/
         s32 temp;
         eat_bool ret;
         ret = eat_get_module_temp_sync( &temp);
         if(ret)
            eat_trace("sync mode: temperature [%d]",temp);
        }
     else if(2 == param2)
        {/*asyn callback */
         eat_bool ret;
         ret = eat_get_module_temp_asyn( temp_callback);      
         if (!ret)
            eat_trace("CallBack mode: read temperature failed");
        }
    }else if( 7 == param1)
    {
	    /*sleep*/
        if( 1 == param2)
        {
            eat_sleep_enable(EAT_TRUE);
            eat_lcd_light_sw(EAT_FALSE,0);
            eat_kpled_sw(EAT_FALSE);
        }else
        {
            eat_sleep_enable(EAT_FALSE);
        }
    }
    return EAT_TRUE;
}

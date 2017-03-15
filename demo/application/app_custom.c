/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :  app_custom.c
 * version       :   0.10
 * Description   :   
 * Authors       :   heweijiang
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2014-07-11, heweijiang, create originally.
 *
 ********************************************************************/
/********************************************************************
 * Include Files
 ********************************************************************/
#include "app_custom.h"
#include "App_include.h"
#include "eat_audio.h" // 2016-3-4 ADD key audio 

/********************************************************************
* Macros
 ********************************************************************/

/********************************************************************
* Types
 ********************************************************************/
 
/********************************************************************
* Extern Variables (Extern /Global)
 ********************************************************************/
/********************************************************************
* Local Function declaration
 ********************************************************************/
eat_bool get_CsqValue(u8 csqValue);
eat_bool get_VoltageValue(void);
void KeyFlag(u32 keycode);
u8 KeyFunction(void);
void Scankey(void);
void ScanGpioKey(void);
/*Customer configuration*/
ModemConfigContext g_modemConfig =
{
    "CMET",             /*apnName*/
    "",                 /*apnUserName*/
    "",                 /*apnPassword*/
    "116.247.119.165",  /*FTPServerIP*/
    "admin",            /*FTPUserName*/
    "password",         /*FTPPassword*/
    "filename",         /*FTPFileName*/
    "/",                /*FTPFilePath*/
    21,                 /*FTPPort*/
};

const SIMCOM_SMS_INFO g_sms_info = 
{
    TEXT,			//TEXT模式只能发字符，不能发中文????
    "",             //sc number
    "13320984483",  //phone number
    "0123456789jason",   //message string
    15,             //message length
    1
};
/*
const SIMCOM_SMS_INFO g_sms_info = 
{
    PDU,
    "",             //sc number
    "13320984483",  //phone number
    "01叶茂林00",   //message string
    10,             //message length
    1
};*/

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
    
void GsmInitCallback(eat_bool result)
{
    if (result)
    {
        //GSM Init ok, to do something. 
        eat_trace("GSM init OK\r\n",14);
        //simcom_tcpip_test();
		flag.GSMisOk=1;
    }
    else
    {
        eat_trace("GSM init ERROR\r\n",14);        
    }
	AppMainRunOnce();
}

void askCregCallback(eat_bool result)
{
    if (result)
    {
        //askCreg ok, to do something. 
        eat_trace("askCreg ok, call back\r\n",14);
    }
    else
    {
        eat_trace("askCreg error, call back\r\n",14);        
    }
}
void askCsqCallback(eat_bool result)
{
    if (result)
    {
        //askCreg ok, to do something. 
        eat_trace("CSQ ok, call back\r\n",14);
		get_CsqValue(g_creg);
    }
    else
    {
        eat_trace("CSQ error, call back\r\n",14);        
    }
}

eat_bool get_CsqValue(u8 csqValue)
{
eat_bool ret;

u8 value=0;
eat_trace("CsqValue,Csq=[%d]",csqValue);//范围在0-31之间

if(csqValue<5) 	//无信号
{
	value=GSM_LEVEL_0;
}
else if(csqValue<10)//信号极弱
{
	value=GSM_LEVEL_1;
}
else if(csqValue<15)//信号较弱
{
	value=GSM_LEVEL_2;
}
else if(csqValue<20)//信号良好
{
	value=GSM_LEVEL_3;
}
else if(csqValue<32)//信号强
{
	value=GSM_LEVEL_4;
}
g_GsmSignalValue=value;
TRACE_DEBUG("g_GsmSignalValue=%d\n",g_GsmSignalValue);
return ret;
}

eat_bool get_VoltageValue(void)
{
eat_bool ret;
u32 voltage;
u8 value=0;
ret=eat_get_adc_sync(EAT_ADC0 ,&voltage);
eat_trace("ADC test 5,adc [%d]",voltage);//范围在0-2800之间，前三位是有效的。

voltage=voltage/10;
if(voltage<100)		//低电关机
{
	value=V_LEVEL_0;
}
else if(voltage<120)//电量不足以打电话发短信
{
	value=V_LEVEL_1;
}
else if(voltage<130)//低电压报警
{
	value=V_LEVEL_2;
}
else if(voltage<140)//低点，可以正常使用
{
	value=V_LEVEL_3;
}
else if(voltage<160)//可正常使用
{
	value=V_LEVEL_4;
}
else if(voltage<200)//可正常使用
{
	value=V_LEVEL_5;
}
else if(voltage<280)//可正常使用
{
	value=V_LEVEL_6;
}
g_VoltageValue=value;
TRACE_DEBUG("g_VoltageValue=%d\n",g_VoltageValue);
return ret;
}

#define TIMER3UP_TIMER_CNT	(5*10*60*3)
static void app2_timer_handler(void)
{
 // u32 voltage;
static u8 i=0;
  if(flag.timer3up)//20ms 时间到
  {
  	flag.timer3up=0;
	flag.keyscan =1;
  	g_CounterSec++;
	//eat_trace("timer3up 1 minute\n");
	//eat_send_msg_to_user(EAT_USER_3, EAT_USER_0, EAT_FALSE, 10, "TIMER3 UP", EAT_NULL);
	#if 0
if(g_CounterSec%100==1)
{
i++;
if(i%2==0)
{	eat_trace("PIN4 HIGH\n");
	eat_gpio_write(EAT_PIN4_UART1_CTS,EAT_GPIO_LEVEL_HIGH);
	eat_gpio_write(EAT_PIN7_UART1_RI,EAT_GPIO_LEVEL_LOW);
}
else
{eat_trace("PIN4 LOW\n");
	eat_gpio_write(EAT_PIN4_UART1_CTS,EAT_GPIO_LEVEL_LOW);
	eat_gpio_write(EAT_PIN7_UART1_RI,EAT_GPIO_LEVEL_HIGH);
}
}
#endif
  if(g_CounterSec%1000==1)
  	{
  	flag.GPS_TIMER_UP=1;
  	}
  else if(g_CounterSec>=5*10*60*3)
  	{
  	eat_send_msg_to_user(EAT_USER_3, EAT_USER_0, EAT_FALSE, 10, "TIMER3 UP", EAT_NULL);
  	flag.readADC=1;
  	g_CounterSec=0;
	}
  if(flag.readADC)
  	{
  	flag.readADC=0;
	get_VoltageValue();
  	}
  }  
  #if EnableKeyFunction
  if(flag.timer16up)
	{
	flag.timer16up=0;
	Scankey();
	}
  if(flag.keyscan)
  	{
  	flag.keyscan=0;
	ScanGpioKey();
  	}
  #endif
}

void test_handler_int_level(EatInt_st *interrupt)
{
//static u8 key_cnt=0;
#if 0
   // eat_trace("test_handler_int_level interrupt->pin [%d ],interrupt->level=%d",interrupt->pin,interrupt->level);
    if(!(interrupt->level))
	{
	//high level trigger,and set low level trigger
	eat_int_set_trigger(interrupt->pin, EAT_INT_TRIGGER_HIGH_LEVEL);
	// eat_trace("System interrupt by interrupt->pin [%d ]TRIGGER_LOW_LEVEL",interrupt->pin);
	}
    else
	{	
	//low level trigger		  
	//	   key_cnt++;
	eat_int_set_trigger(interrupt->pin, EAT_INT_TRIGGER_LOW_LEVEL);
	//eat_trace("System interrupt by interrupt->pin [%d ] EAT_INT_TRIGGER_LOW_LEVEL,key_cnt=%d",interrupt->pin,key_cnt);
	}
#endif
	eat_trace("test_handler_int_level interrupt->pin [%d ],interrupt->level=%d",interrupt->pin,interrupt->level);
   // if(1==(interrupt->level))//如果按键弹起了，则把GPIO设置为低电平触发
  //  {
//	eat_int_set_trigger(interrupt->pin, EAT_INT_TRIGGER_LOW_LEVEL);
 //   }

	if(0==interrupt->level)
		{
   		switch(interrupt->pin)
   		{
   		
		case KEY1_PIN:
			if(!key.key1_fst)
				{
				//开启定时器，并将按键标记按键已经按下
					key.key1_fst=1;
					key.key1=1;
					eat_timer_start(EAT_TIMER_16,50);
				}
			break;
		case KEY2_PIN:
			if(!key.key2_fst)
				{
				//开启定时器，并将按键标记按键已经按下
					key.key2_fst=1;
					key.key2=1;
					eat_timer_start(EAT_TIMER_16,50);
				}
			break;
		case KEY3_PIN:
			if(!key.key3_fst)
				{
				//开启定时器，并将按键标记按键已经按下
					key.key3_fst=1;
					key.key3=1;
					eat_timer_start(EAT_TIMER_16,50);
				}
			break;
	#if 0
		case KEY4_PIN:
			if(!key.key4_fst)
				{
				//开启定时器，并将按键标记按键已经按下
					key.key4_fst=1;
					key.key4=1;
					eat_timer_start(EAT_TIMER_16,50);
				}
			break;
	#endif
		case KEY5_PIN:
			if(!key.key5_fst)
				{
				//开启定时器，并将按键标记按键已经按下
					key.key5_fst=1;
					key.key5=1;
					eat_timer_start(EAT_TIMER_16,50);
				}
			break;
	#if 0
		case KEY6_PIN:
			if(!key.key6_fst)
				{
				//开启定时器，并将按键标记按键已经按下
					key.key6_fst=1;
					key.key6=1;
					eat_timer_start(EAT_TIMER_16,50);
				}
			break;
		#endif
		default:
			break;
   	}
		}
	else
		{
		switch(interrupt->pin)
   		{
		case KEY1_PIN:		
		case KEY2_PIN:		
		case KEY3_PIN:		
		case KEY5_PIN:
			eat_int_set_trigger(interrupt->pin, EAT_INT_TRIGGER_LOW_LEVEL);
		default:
			eat_int_set_trigger(interrupt->pin, EAT_INT_TRIGGER_LOW_LEVEL);
			break;
   	}
		}
}
#if EnableKeyFunction
void init_GPIO(void)
{
//初始化io 中断,利用事件来响应中断
//eat_gpio_setup(KEY1_PIN, EAT_GPIO_DIR_INPUT, EAT_GPIO_LEVEL_HIGH);
eat_gpio_setup(KEY2_PIN, EAT_GPIO_DIR_INPUT, EAT_GPIO_LEVEL_HIGH);
eat_gpio_setup(KEY3_PIN, EAT_GPIO_DIR_INPUT, EAT_GPIO_LEVEL_HIGH);
//eat_gpio_setup(KEY4_PIN, EAT_GPIO_DIR_INPUT, EAT_GPIO_LEVEL_HIGH);
eat_gpio_setup(KEY1_PIN, EAT_GPIO_DIR_INPUT, EAT_GPIO_LEVEL_HIGH);
//eat_gpio_setup(KEY6_PIN, EAT_GPIO_DIR_INPUT, EAT_GPIO_LEVEL_HIGH);

#if USE_INT_AUTO_INVERT
eat_int_setup_ext(KEY5_PIN, EAT_INT_TRIGGER_LOW_LEVEL, 10, NULL,TRUE);
//eat_int_setup_ext(KEY5_PIN, EAT_INT_TRIGGER_LOW_LEVEL, 10, NULL,TRUE);
#else
eat_int_setup(KEY5_PIN, EAT_INT_TRIGGER_LOW_LEVEL, 10, NULL);
//eat_int_setup(KEY5_PIN, EAT_INT_TRIGGER_LOW_LEVEL, 10, NULL);
#endif

//eat_gpio_setup(GPSPOWER_PIN, EAT_GPIO_DIR_OUTPUT, EAT_GPIO_LEVEL_HIGH); // 关GPS的电源

//eat_gpio_setup(GPSPOWER_PIN, EAT_GPIO_DIR_OUTPUT, EAT_GPIO_LEVEL_LOW); // 开GPS的电源
#if DebugOn
//TRACE_DEBUG("GPSPOWER_PIN LOW  gps POWER on  11");
#endif

eat_gpio_setup(SHUT_PIN, EAT_GPIO_DIR_OUTPUT, EAT_GPIO_LEVEL_LOW);
eat_sleep(10);//
eat_gpio_setup(SHUT_PIN, EAT_GPIO_DIR_OUTPUT, EAT_GPIO_LEVEL_HIGH);
eat_sleep(10);//
eat_gpio_setup(SHUT_PIN, EAT_GPIO_DIR_OUTPUT, EAT_GPIO_LEVEL_LOW);
eat_sleep(10);//


eat_gpio_setup(MOTOR_PIN, EAT_GPIO_DIR_OUTPUT, EAT_GPIO_LEVEL_LOW);
eat_sleep(1000);//
eat_gpio_setup(MOTOR_PIN, EAT_GPIO_DIR_OUTPUT, EAT_GPIO_LEVEL_HIGH);

}
#endif
void app_user2(void *data)
{
    EatEvent_st event;
	u32 num;
	eat_timer_start(EAT_TIMER_3,1000);//1s 计时开始
	#if EnableKeyFunction
	init_GPIO();
	#endif
    while(EAT_TRUE)
    {	//eat_sleep(50);
	 //eat_trace("-----app_user2---\n");
	 
	 app2_timer_handler();
	 //num= eat_get_event_num_for_user(EAT_USER_2);//如果不判断num，当num为0时调用eat_get_event_for_user()则线程会挂起
	// if(num>0)
	 	{
        eat_get_event_for_user(EAT_USER_2, &event);
        //eat_trace("EAT_USER_2 MSG id%x", event.event);
		switch(event.event)
        {
            case EAT_EVENT_TIMER ://定时器消息   ID=1
            	if(event.data.timer.timer_id==EAT_TIMER_3)
            		{
	            	eat_timer_start(EAT_TIMER_3,20);
					flag.timer3up=1;
            		}
				else if(event.data.timer.timer_id==EAT_TIMER_16)
					{
					flag.timer16up=1;
					eat_timer_start(EAT_TIMER_16,50);//发现有按键按下后，启动定时器100ms 侦测一次
					}
                break;
			case EAT_EVENT_KEY:
				
				break;
            case EAT_EVENT_MDM_READY_RD://EAT收到Modem发过来的数据ID=4
               	
                break;
            case EAT_EVENT_MDM_READY_WR:
                break;
            case EAT_EVENT_UART_READY_RD:
                
                break;
            case EAT_EVENT_UART_READY_WR:
                
                break;
            case EAT_EVENT_UART_SEND_COMPLETE :
                break;
            case  EAT_EVENT_INT :				
				test_handler_int_level(&event.data.interrupt);
				break;

            default:
                break;
        }
	 	}
    }
}

void Scankey(void)
{
static u8 key_cnt=0;

if(key.key1_fst)
{
	if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY1_PIN))
		{
		key_cnt++;
		if(key_cnt>=15)//如果按键按下超过15x100ms=1.5s，则长按键成立，重新扫描按键
		{
		key.key1_hold=1;
		key.key1=0;
		key_cnt=0;
		flag.key_down=1;
		key.key1_fst=0;
		eat_timer_stop(EAT_TIMER_16);
		eat_int_setup(KEY1_PIN, EAT_INT_TRIGGER_HIGH_LEVEL, 10, NULL);//100ms 中断一次，高电平
		}
		}
		else	//如果按键放开，且时间少于1.5s，则为段按键
		{
		if(key_cnt<15)
			{
			key.key1=1;
			flag.key_down=1;
			key_cnt=0;
			key.key1_fst=0;
			eat_timer_stop(EAT_TIMER_16);
			eat_int_setup(KEY1_PIN, EAT_INT_TRIGGER_LOW_LEVEL, 10, NULL);//100ms 中断一次，低电平
			}
		}

}
if(key.key2_fst)
{
	if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY2_PIN))
		{
		key_cnt++;
		if(key_cnt>=15)//如果按键按下超过15x100ms=1.5s，则长按键成立，重新扫描按键
		{
		key.key2_hold=1;
		key.key2=0;
		key_cnt=0;
		flag.key_down=1;
		key.key2_fst=0;
		eat_timer_stop(EAT_TIMER_16);
		eat_int_setup(KEY2_PIN, EAT_INT_TRIGGER_HIGH_LEVEL, 10, NULL);//100ms 中断一次，高电平
		}
		}
		else	//如果按键放开，且时间少于1.5s，则为段按键
		{
		if(key_cnt<15)
			{
			key.key2=1;
			flag.key_down=1;
			key_cnt=0;
			key.key2_fst=0;
			eat_timer_stop(EAT_TIMER_16);
			eat_int_setup(KEY2_PIN, EAT_INT_TRIGGER_LOW_LEVEL, 10, NULL);//100ms 中断一次，低电平
			}
		}

}
if(key.key3_fst)
{
	if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY3_PIN))
		{
		key_cnt++;
		if(key_cnt>=15)//如果按键按下超过15x100ms=1.5s，则长按键成立，重新扫描按键
		{
		key.key3_hold=1;
		key.key3=0;
		key_cnt=0;
		flag.key_down=1;
		key.key3_fst=0;
		eat_timer_stop(EAT_TIMER_16);
		eat_int_setup(KEY3_PIN, EAT_INT_TRIGGER_HIGH_LEVEL, 10, NULL);//100ms 中断一次，高电平
		}
		}
		else	//如果按键放开，且时间少于1.5s，则为段按键
		{
		if(key_cnt<15)
			{
			key.key3=1;
			flag.key_down=1;
			key_cnt=0;
			key.key3_fst=0;
			eat_timer_stop(EAT_TIMER_16);
			eat_int_setup(KEY3_PIN, EAT_INT_TRIGGER_LOW_LEVEL, 10, NULL);//100ms 中断一次，低电平
			}
		}

}
#if 0

if(key.key4_fst)
{
	if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY4_PIN))
		{
		key_cnt++;
		if(key_cnt>=15)//如果按键按下超过15x100ms=1.5s，则长按键成立，重新扫描按键
		{
		key.key4_hold=1;
		key.key4=0;
		key_cnt=0;
		flag.key_down=1;
		key.key4_fst=0;
		eat_timer_stop(EAT_TIMER_16);
		eat_int_setup(KEY4_PIN, EAT_INT_TRIGGER_HIGH_LEVEL, 10, NULL);//100ms 中断一次，高电平
		}
		}
		else	//如果按键放开，且时间少于1.5s，则为段按键
		{
		if(key_cnt<15)
			{
			key.key4=1;
			flag.key_down=1;
			key_cnt=0;
			key.key4_fst=0;
			eat_timer_stop(EAT_TIMER_16);
			eat_int_setup(KEY4_PIN, EAT_INT_TRIGGER_LOW_LEVEL, 10, NULL);//100ms 中断一次，低电平
			}
		}

}
#endif

#if 1
if(key.key5_fst)
{
	if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY5_PIN))
		{
		key_cnt++;
		if(key_cnt>=15)//如果按键按下超过15x100ms=1.5s，则长按键成立，重新扫描按键
		{
		key.key5_hold=1;
		key.key5=0;
		key_cnt=0;
		flag.key_down=1;
		key.key5_fst=0;
		eat_timer_stop(EAT_TIMER_16);
		#if USE_INT_AUTO_INVERT
		eat_int_setup_ext(KEY5_PIN, EAT_INT_TRIGGER_HIGH_LEVEL, 10, NULL,TRUE);//100ms 中断一次，高电平
		#else
		eat_int_setup(KEY5_PIN, EAT_INT_TRIGGER_HIGH_LEVEL, 10, NULL);//100ms 中断一次，高电平
		#endif
		}
		}
		else	//如果按键放开，且时间少于1.5s，则为段按键
		{
		if(key_cnt<15)
			{
			key.key5=1;
			flag.key_down=1;
			key_cnt=0;
			key.key5_fst=0;
			eat_timer_stop(EAT_TIMER_16);
            #if USE_INT_AUTO_INVERT
            eat_int_setup_ext(KEY5_PIN, EAT_INT_TRIGGER_HIGH_LEVEL, 10, NULL,TRUE);//100ms 中断一次，高电平
            #else
			eat_int_setup(KEY5_PIN, EAT_INT_TRIGGER_LOW_LEVEL, 10, NULL);//100ms 中断一次，低电平
			#endif
			}
		}

}
#endif
#if 0

if(key.key6_fst)
{
	if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY6_PIN))
		{
		key_cnt++;
		if(key_cnt>=15)//如果按键按下超过15x100ms=1.5s，则长按键成立，重新扫描按键
		{
		key.key6_hold=1;
		key.key6=0;
		key_cnt=0;
		flag.key_down=1;
		key.key6_fst=0;
		eat_timer_stop(EAT_TIMER_16);
		eat_int_setup(KEY6_PIN, EAT_INT_TRIGGER_HIGH_LEVEL, 10, NULL);//100ms 中断一次，高电平
		}
		}
		else	//如果按键放开，且时间少于1.5s，则为段按键
		{
		if(key_cnt<15)
			{
			key.key6=1;
			flag.key_down=1;
			key_cnt=0;
			key.key6_fst=0;
			eat_timer_stop(EAT_TIMER_16);
			eat_int_setup(KEY6_PIN, EAT_INT_TRIGGER_LOW_LEVEL, 10, NULL);//100ms 中断一次，低电平
			}
		}
}
#endif
	//KeyFlag(precode);
	KeyFunction();
}
#define shortkeyCNT	3
#define longkeyCNT	30

void ScanGpioKey(void)
{
static u8 key1_cnt=0;
static u8 key2_cnt=0;
static u8 key3_cnt=0;
static u8 key4_cnt=0;
static u8 key5_cnt=0;
static u8 key6_cnt=0;

if( EAT_GPIO_LEVEL_HIGH== eat_gpio_read(KEY1_PIN))
	{key.key1_stateBak=1; key1_cnt=0;}
if( EAT_GPIO_LEVEL_HIGH== eat_gpio_read(KEY2_PIN))
	{key.key2_stateBak=1; key2_cnt=0;}
if( EAT_GPIO_LEVEL_HIGH== eat_gpio_read(KEY3_PIN))
	{key.key3_stateBak=1; key3_cnt=0;}
//if( EAT_GPIO_LEVEL_HIGH== eat_gpio_read(KEY5_PIN))
	//{key.key5_stateBak=1; key5_cnt=0;}
	#if 1
//++++++key1+++++
if(!key.key1_fst)
{
if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY1_PIN))
	{
	key1_cnt++;
	if(key1_cnt>shortkeyCNT)
		{
		if(key.key1_stateBak)
			{
			key.key1_stateBak=0;
			key.key1_fst=1;
			key.key1=1;
			}
		}
	}
else
	{
	key1_cnt=0;
	}
}
else
{
if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY1_PIN))
	{
	key1_cnt++;
	if(key1_cnt>=longkeyCNT)//如果按键按下超过15x100ms=1.5s，则长按键成立，重新扫描按键
		{
		key.key1_hold=1;
		key.key1=0;
		key1_cnt=0;
		flag.key_down=1;
		key.key1_fst=0;
		}
	}
else	//如果按键放开，且时间少于1.5s，则为段按键
{
if(key1_cnt<longkeyCNT)
	{
	key.key1=1;
	flag.key_down=1;
	key1_cnt=0;
	key.key1_fst=0;
	}
}
}
#endif
//++++++key2+++++
if(!key.key2_fst)
{
if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY2_PIN))
	{
	key2_cnt++;
	if(key2_cnt>shortkeyCNT)
		{
		if(key.key2_stateBak)
			{
			key.key2_stateBak=0;
			key.key2_fst=1;
			key.key2=1;
			}
		}
	}
else
	{
	key2_cnt=0;
	}
}
else
{
if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY2_PIN))
	{
	key2_cnt++;
	if(key2_cnt>=longkeyCNT)//如果按键按下超过15x100ms=1.5s，则长按键成立，重新扫描按键
		{
		key.key2_hold=1;
		key.key2=0;
		key2_cnt=0;
		flag.key_down=1;
		key.key2_fst=0;
		}
	}
else	//如果按键放开，且时间少于1.5s，则为段按键
{
if(key2_cnt<longkeyCNT)
	{
	key.key2=1;
	flag.key_down=1;
	key2_cnt=0;
	key.key2_fst=0;
	}
}
}

//++++++key3+++++
if(!key.key3_fst)
{
if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY3_PIN))
	{
	key3_cnt++;
	if(key3_cnt>shortkeyCNT)
		{
		if(key.key3_stateBak)
			{
			key.key3_stateBak=0;
			key.key3_fst=1;
			key.key3=1;
			}
		}
	}
else
	{
	key3_cnt=0;
	}
}
else
{
if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY3_PIN))
	{
	key3_cnt++;
	if(key3_cnt>=longkeyCNT)//如果按键按下超过15x100ms=1.5s，则长按键成立，重新扫描按键
		{
		key.key3_hold=1;
		key.key3=0;
		key3_cnt=0;
		flag.key_down=1;
		key.key3_fst=0;
		}
	}
else	//如果按键放开，且时间少于1.5s，则为段按键
{
if(key3_cnt<longkeyCNT)
	{
	key.key3=1;
	flag.key_down=1;
	key3_cnt=0;
	key.key3_fst=0;
	}
}
}
#if 0
//++++++key4+++++
if(!key.key4_fst)
{
if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY4_PIN))
	{
	key4_cnt++;
	if(key4_cnt>shortkeyCNT)
		{
		if(key.key4_stateBak)
			{
			key.key4_stateBak=0;
			key.key4_fst=1;
			key.key4=1;
			}
		}
	}
else
{
key4_cnt=0;
}
}
else
{
if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY4_PIN))
	{
	key4_cnt++;
	if(key4_cnt>=longkeyCNT)//如果按键按下超过15x100ms=1.5s，则长按键成立，重新扫描按键
		{
		key.key4_hold=1;
		key.key4=0;
		key4_cnt=0;
		flag.key_down=1;
		key.key4_fst=0;
		}
	}
else	//如果按键放开，且时间少于1.5s，则为段按键
	{
	//TRACE_DEBUG("long key4 release");
	if(key4_cnt<longkeyCNT)// 如果时间少于设定的长按键时间，则清除
		{
		key.key4=1;
		flag.key_down=1;
		key4_cnt=0;
		key.key4_fst=0;
		}
	}
}
#endif

#if 0
	//++++++key5+++++
	if(!key.key5_fst)
	{
	if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY5_PIN))
		{
		key5_cnt++;
		if(key5_cnt>shortkeyCNT)
			{
			if(key.key5_stateBak)
				{
				key.key5_stateBak=0;
				key.key5_fst=1;
				key.key5=1;
				}
			}
		}
	else
	{
	key5_cnt=0;
	}
	}
	else
	{
	if( EAT_GPIO_LEVEL_LOW == eat_gpio_read(KEY5_PIN))
		{
		key5_cnt++;
		if(key5_cnt>=longkeyCNT)//如果按键按下超过15x100ms=1.5s，则长按键成立，重新扫描按键
			{
			key.key5_hold=1;
			key.key5=0;
			key5_cnt=0;
			flag.key_down=1;
			key.key5_fst=0;
			}
		}
	else	//如果按键放开，且时间少于1.5s，则为段按键
		{
		//TRACE_DEBUG("long key4 release");
		if(key5_cnt<longkeyCNT)// 如果时间少于设定的长按键时间，则清除
			{
			key.key5=1;
			flag.key_down=1;
			key5_cnt=0;
			key.key5_fst=0;
			}
		}
	}
#endif

	//KeyFlag(precode);
	KeyFunction();
}

/*
KEY FLAG在本程序中没有用到
*/
#if 0
void KeyFlag(u32 keycode)
{
	switch(keycode)
		{
		case 0x0000://无按键按下
			flag.key_down = 0;
			break;
			
		case 0x0001://按键1
			key.key1 = 1; 
			flag.key_down = 1;   
			break;
			
		case 0x0010:
			key.key2 = 1;
			flag.key_down = 1; 
			break;     
			
		case 0x0100:
			key.key3  = 1; 
			flag.key_down = 1; 
			break;
		case 0x0002:
			key.key4  = 1; 
			flag.key_down = 1; 
			break; 
		case 0x0020://4
			key.key5 = 1; 
			flag.key_down = 1;   
			break;
		case 0x0200:
			key.key6 = 1;
			flag.key_down = 1; 
			break;
		default:
			 flag.key_down = 0;
			break;
		}
}
#endif
//=============================================
//生成按键实际位置
//=============================================
void GetKeypadButton(void)
{	
	u8 cnt=0;
    if( flag.key_down )//有键按下
	{
		flag.key_down = 0;
		
		 if(key.key1)
	        {
			key.key1 = 0;
			if(g_KeypadButton==BTN_Num1 ||g_KeypadButton==BTN_Repeat)
			g_KeypadButton=BTN_Repeat;
			else
			g_KeypadButton=BTN_Num1;

	         }
	      else if(key.key2)
	         {
	         	key.key2 = 0;
			if(g_KeypadButton==BTN_Num2 ||g_KeypadButton==BTN_Repeat)
				g_KeypadButton=BTN_Repeat;
			else
				g_KeypadButton=BTN_Num2;
			 }
	     else if(key.key3)
	         {
		         key.key3 = 0;    
			if(g_KeypadButton==BTN_Num3 ||g_KeypadButton==BTN_Repeat)
				g_KeypadButton=BTN_Repeat;
			else
				g_KeypadButton=BTN_Num3;

	         }
	    else if(key.key4)
	         {
		         key.key4 = 0;      
			if(g_KeypadButton==BTN_Num4 ||g_KeypadButton==BTN_Repeat)
				g_KeypadButton=BTN_Repeat;
			else
				g_KeypadButton=BTN_Num4;

	         }
	    else if(key.key5)
	         {
		         key.key5 = 0;   
			if(g_KeypadButton==BTN_Num5 ||g_KeypadButton==BTN_Repeat)
				g_KeypadButton=BTN_Repeat;
			else
				g_KeypadButton=BTN_Num5;

	         }
	    else if(key.key6)
	         {
		         key.key6 = 0;   
			if(g_KeypadButton==BTN_Num6 ||g_KeypadButton==BTN_Repeat)
				g_KeypadButton=BTN_Repeat;
			else
				g_KeypadButton=BTN_Num6;

	         }
		else  if(key.key1_hold)
	        {
			key.key1_hold = 0;
			if(g_KeypadButton==BTN_Num1_L||g_KeypadButton==BTN_Repeat)
			g_KeypadButton=BTN_Repeat;
			else
			g_KeypadButton=BTN_Num1_L;

	         }
	      else if(key.key2_hold)
	         {
	         	key.key2_hold = 0;
			if(g_KeypadButton==BTN_Num2_L||g_KeypadButton==BTN_Repeat)
				g_KeypadButton=BTN_Repeat;
			else
				g_KeypadButton=BTN_Num2_L;
			 }
	     else if(key.key3_hold)
	         {
		         key.key3_hold = 0;    
			if(g_KeypadButton==BTN_Num3_L ||g_KeypadButton==BTN_Repeat)
				g_KeypadButton=BTN_Repeat;
			else
				g_KeypadButton=BTN_Num3_L;

	         }
	    else if(key.key4_hold)
	         {
		         key.key4_hold = 0;      
			if(g_KeypadButton==BTN_Num4_L ||g_KeypadButton==BTN_Repeat)
				g_KeypadButton=BTN_Repeat;
			else
				g_KeypadButton=BTN_Num4_L;

	         }
	    else if(key.key5_hold)
	         {
		         key.key5_hold = 0;   
			if(g_KeypadButton==BTN_Num5_L ||g_KeypadButton==BTN_Repeat)
				g_KeypadButton=BTN_Repeat;
			else
				g_KeypadButton=BTN_Num5_L;

	         }
	    else if(key.key6_hold)
	         {
		         key.key6_hold = 0;   
			if(g_KeypadButton==BTN_Num6_L ||g_KeypadButton==BTN_Repeat)
				g_KeypadButton=BTN_Repeat;
			else
				g_KeypadButton=BTN_Num6_L;

	         }
	     
	     }  
	else
	{
	g_KeypadButton=BTN_Nothing;
	}
}
u8 KeyFunction(void)
{
GetKeypadButton();
if (g_KeypadButton>=BTN_Nothing)
	return g_KeypadButton;

if (g_KeypadButton<BTN_Repeat)
{
// flag.greenLedOn = 1;
// flag.LedPowerChange = 1;
#if DebugOn
TRACE_DEBUG("\n ++g_KeypadButton=%d++\n",g_KeypadButton);
#endif

//eat_trace("tone_play id =%d, play style =%d,vol =%d  path = %d",EAT_TONE_KEY_CLICK, EAT_AUDIO_PLAY_ONCE, 12, EAT_AUDIO_PATH_SPK1);
//eat_audio_play_tone_id(EAT_TONE_CALL_WAITING_STK, EAT_AUDIO_PLAY_ONCE, 12, EAT_AUDIO_PATH_SPK1);

switch(g_KeypadButton)
{
case 1:
	eat_send_msg_to_user(EAT_USER_2, EAT_USER_0, EAT_FALSE, 4, "KEY1", EAT_NULL);
	break;
case 2:
	eat_send_msg_to_user(EAT_USER_2, EAT_USER_0, EAT_FALSE, 4, "KEY2", EAT_NULL);
	break;
case 3:
	eat_send_msg_to_user(EAT_USER_2, EAT_USER_0, EAT_FALSE, 4, "KEY3", EAT_NULL);
	break;
case 4:
	eat_send_msg_to_user(EAT_USER_2, EAT_USER_0, EAT_FALSE, 4, "KEY4", EAT_NULL);
	break;
case 5:
	eat_send_msg_to_user(EAT_USER_2, EAT_USER_0, EAT_FALSE, 4, "KEY5", EAT_NULL);
	break;
case 6:
	eat_send_msg_to_user(EAT_USER_2, EAT_USER_0, EAT_FALSE, 4, "KEY6", EAT_NULL);
	break;
case 7:
	eat_send_msg_to_user(EAT_USER_2, EAT_USER_0, EAT_FALSE, 4, "KEY7", EAT_NULL);
	break;
case 8:
	eat_send_msg_to_user(EAT_USER_2, EAT_USER_0, EAT_FALSE, 4, "KEY8", EAT_NULL);
	break;
case 9:
	eat_send_msg_to_user(EAT_USER_2, EAT_USER_0, EAT_FALSE, 4, "KEY9", EAT_NULL);
	break;
case 10:
	eat_send_msg_to_user(EAT_USER_2, EAT_USER_0, EAT_FALSE, 5, "KEY10", EAT_NULL);
	break;
case 11:
	eat_send_msg_to_user(EAT_USER_2, EAT_USER_0, EAT_FALSE, 5, "KEY11", EAT_NULL);
	break;
case 12:
	eat_send_msg_to_user(EAT_USER_2, EAT_USER_0, EAT_FALSE, 5, "KEY12", EAT_NULL);
	break;

default :
	break;
	
}
}
}


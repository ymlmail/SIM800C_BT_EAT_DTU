
#include "App_logic.h"

void AppGetEventHandle(void)
{
    EatEvent_st event;
    u16 len = 0;
    eat_get_event(&event);
    eat_trace("MSG id%x", event.event);
    switch(event.event)
    {
        case EAT_EVENT_TIMER :
            {
                //Restart timer
                if(event.data.timer.timer_id==EAT_TIMER_1)
                	{
                	flag.timer1up =1;		//timer1 时间到
					eat_timer_start(event.data.timer.timer_id, 1000);
					eat_trace("Timer1 is up, timer ID:%d", event.data.timer.timer_id);
                	}
				else
					{
					eat_timer_start(event.data.timer.timer_id, 3000);
					eat_trace("other Timer's up, timer ID:%d", event.data.timer.timer_id);
					}
                
            }
            break;
        case EAT_EVENT_MDM_READY_RD:
            {
                u8 param1,param2;
                len = 0;
                len = eat_modem_read(buf, 2048);
                if(len > 0)
                {	//eat_trace("HELLO TEST11\n");
                    //Get the testing parameter
                    if(eat_modem_data_parse(buf,len,&param1,&param2))//读是否为参数
                    {
                        //Entry timer test module
                        eat_module_test_timer(param1, param2);
                    }
                    else	//如果不是参数则做另外的处理。
                    {
                        eat_trace("From Mdm:%s",buf);
                    }
                }
				else
				{
				#if DebugMsgOnOff
                eat_trace("EAT_EVENT_MDM_READY_RD occur,but buf");
				#endif
					}

            }
            break;
        case EAT_EVENT_MDM_READY_WR:
        case EAT_EVENT_UART_READY_RD:
			eat_trace("HELLO TEST222\n");
            break;
        case EAT_EVENT_UART_SEND_COMPLETE :
            break;
        default:
            break;
    }
}

void AppLogicHandle(void)
{
u8 counter=0;
  if(flag.timer1up)
  	{
  	counter++;
  	}
  if(counter>=200)
  	{
	TRACE_DEBUG("count=%d",counter);
  	counter=0;
  	}
}


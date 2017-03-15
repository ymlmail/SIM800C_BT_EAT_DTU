#if !defined (__EAT_TIMER_H__)
#define __EAT_TIMER_H__

#include "eat_type.h"

typedef enum {
    EAT_TIMER_1,
    EAT_TIMER_2,
    EAT_TIMER_3,
    EAT_TIMER_4,
    EAT_TIMER_5,
    EAT_TIMER_6,
    EAT_TIMER_7,
    EAT_TIMER_8,
    EAT_TIMER_9,
    EAT_TIMER_10,
    EAT_TIMER_11,
    EAT_TIMER_12,
    EAT_TIMER_13,
    EAT_TIMER_14,
    EAT_TIMER_15,
    EAT_TIMER_16,
    EAT_TIMER_NUM
} EatTimer_enum;

typedef struct {
    EatTimer_enum timer_id; /* timer id */
} EatTimer_st; /* EAT_EVENT_TIMER data*/


typedef struct {
    unsigned char sec; /* [0, 59] */
    unsigned char min; /* [0,59]  */
    unsigned char hour; /* [0,23]  */
    unsigned char day; /* [1,31]  */
    unsigned char mon; /* [1,12] */
    unsigned char wday; /* [1,7] */
    unsigned char year; /* [0,127] */
} EatRtc_st;
/*****************************************************************************
* Function :   eat_timer_start
* Description: Start the timer
* Parameters :
*     id : EatTimer_enum [IN], the ID of the timer
*     period : u32 [IN], the period ,units millisecond,the minimum value is 5ms
* Returns:
*  EAT_SUCESS ,Start Timer Sucess
*  Others Fail, Please check the id value
* NOTE
*    The minimum time is 5ms.
*    If the period is less than 5 while setting,the expiry message will be 
*    sent to EAT module after 5ms also.
*****************************************************************************/
extern eat_bool (* const eat_timer_start)(EatTimer_enum id, unsigned int period); 

/*****************************************************************************
* Function :   eat_timer_stop
* Description: Stop the timer
* Parameters :
*     id : EatTimer_enum [IN], the ID of the timer
* Returns:
*  EAT_SUCESS, stop the timer Sucess
*  Others Fail, Please check the id value
*****************************************************************************/
extern eat_bool (* const eat_timer_stop)(EatTimer_enum id); 

/*****************************************************************************
* Function :   eat_get_current_time
* Description: get current time value
* Parameters : void
* Returns:
*  unsigned int, each unit is 31.25us,from 0 to 0xFFFFFFFF
*****************************************************************************/
extern unsigned int (* const eat_get_current_time)(void);

/*****************************************************************************
* Function :   eat_get_duration_ms
* Description: Obtained with the current time interval
* Parameters :
*      previous_time: unsigned int, [IN],the value of eat_get_current_time()
* Returns:
*  unsigned int,the interval time,units millisecond(ms = 1/1000 second)
* NOTE
*   The return value is millsecond(ms). 
*****************************************************************************/
extern unsigned int (* const eat_get_duration_ms)(unsigned int previous_time);

/*****************************************************************************
* Function :   eat_get_duration_us
* Description:  Obtained with the current time interval
* Parameters :
*     previous_time: unsigned int, [IN],the value of eat_get_current_time()
* Returns:
*  unsigned int,the interval time,units microsecond(us = 1/1000000 second)
* NOTE
*   The return value is microsecond(us). 
*****************************************************************************/
extern unsigned int (* const eat_get_duration_us)(unsigned int previous_time);

/*****************************************************************************
* Function :   eat_sleep
* Description: Forcing a task to sleep for a duration 
* Parameters :
*     delay: u32 [IN], the delay,unit is 1 millisecond,the minimum value is 5ms
* Returns:
*     void
* NOTE
*    The minimum time is 5ms.
*    If the delay is less than 5,also set 5 in kernel.
*****************************************************************************/
extern void (* const eat_sleep)(unsigned int delay);


/*****************************************************************************
* Function :   eat_get_rtc
* Description: get the RTC value
* Parameters :
*     rtc: EatRtc_st* [IN], The rtc value,refer to construction EatRtc_st
* Returns:
*  EAT_SUCESS set sucess
*  Others Fail
*****************************************************************************/
extern eat_bool (* const eat_get_rtc)(EatRtc_st *rtc);

/*****************************************************************************
* Function :   eat_set_rtc
* Description: set the RTC value
* Parameters :
*     rtc: EatRtc_st* [OUT], The rtc value,refer to construction EatRtc_st
* Returns:
*  EAT_SUCESS set sucess
*  Others Fail
*****************************************************************************/
extern eat_bool (* const eat_set_rtc)(const EatRtc_st *rtc);

/* GPT callback function*/
typedef void (*eat_gpt_callback_func)(void);

/*****************************************************************************
* Function :   eat_gpt_start
* Description: start gpt timer
* Parameters :
*     period: unsigned int [IN], the timeout value ,echo unit is 61.035(1000000/16384)us,
*             from 0 to 0xffff.
*             If timing one second,the "period" parameter is 16384.
*     loop  : eat_bool  [IN], 
*               EAT_TRUE: auto-repeat
*               EAT_FALSE: once
*     callback: eat_gpt_callback_func [IN],the callback function pointer
* Returns:
*  EAT_SUCESS Sucess
*  Others Fail
* NOTE
*   The GPT timer is a hardware timer, not in the callback function execute 
*   blocking function, such as sleep, get semaphores, allocation memory and 
*   so on , and execution time as far as possible short.
*****************************************************************************/
extern eat_bool (* const eat_gpt_start)(unsigned int period, eat_bool loop, eat_gpt_callback_func callback);

/*****************************************************************************
* Function :   eat_gpt_stop
* Description: stop gpt timer
* Parameters :
*         void
* Returns:
*  EAT_SUCESS Sucess
*  Others Fail
*****************************************************************************/
extern eat_bool (* const eat_gpt_stop)(void);

/*****************************************************************************
* Function :   eat_watchdog_start
* Description: 
*              Start the watchdog, if calling eat_watchdog_feed exceeds the exp_time,
*              the module will reset or power down or assert.
* Parameters :
*     exp_time : u32 [IN], the expire time, units millisecond.
*                The max time is 600000(milliseconds) (=10 minutes) .
*     type     : u8  [IN] 
*               0=REBOOT, 1=Power down, 2=ASSERT
* Returns:
*  EAT_SUCESS ,Start Watchdog Sucess
*  Others Fail, Please check the exp_time or type, or the watchdog is opened.
*****************************************************************************/
extern eat_bool (*const eat_watchdog_start)(u32 exp_time, u8 type);
/*****************************************************************************
* Function :    eat_watchdog_feed
* Description:  Reset the watchdog
* Parameters :  void
* Returns:
*  EAT_SUCESS, Reset the Watchdog Sucess
*  Others Fail
*****************************************************************************/
extern eat_bool (*const eat_watchdog_feed)(void);
/*****************************************************************************
* Function :   eat_watchdog_stop
* Description: Stop the watchdog which was started
* Parameters : void
* Returns:
*  EAT_SUCESS , Stop the watchdog Sucess
*  Others Fail, Please check whether the watchdog is opened
*****************************************************************************/
extern eat_bool (*const eat_watchdog_stop)(void);
#endif

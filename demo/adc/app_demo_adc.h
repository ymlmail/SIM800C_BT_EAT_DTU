#ifndef _APP_DEMO_ADC_H_
#define _APP_DEMO_ADC_H_

#include "eat_interface.h"

void adc_cb_proc(EatAdc_st* adc);
void adc_event_proc(EatEvent_st *event);
eat_bool eat_module_test_adc(u8 param1, u8 param2);

#endif

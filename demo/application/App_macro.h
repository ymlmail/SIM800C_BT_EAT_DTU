//yemaolin Creat 2015-7-29
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

#define DebugOn				1
#define PrintPackageData	1

#define DebugMsgOnOff		1



#define SeverDataMax		100

#define UseATCOMMON			0
#define EnableGPSModule		0
#define EnableGpsDebuge		1&&EnableGPSModule&&DebugOn
#define EnableKeyFunction	0


#define USE_INT_AUTO_INVERT	0

#define BIT0		0x01
#define BIT1		0x02
#define BIT2		0x04
#define BIT3		0x08

#define BIT4		0x10
#define BIT5		0x20
#define BIT6		0x40
#define BIT7		0x80

#define BIT8		0x0100
#define BIT9		0x0200
#define BIT10		0x0400
#define BIT11		0x0800

#define BIT12		0x1000
#define BIT13		0x2000
#define BIT14		0x4000
#define BIT15		0x8000

/*
unsigned int flag;

void set_bit3(void) //÷√Œª1
{
    a |= BIT3;
}
void clear_bit3(void) //«Â¡„0
{
    a &= ~ BIT3;
}

*/


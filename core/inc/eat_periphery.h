#if !defined (__EAT_PERIPHERY_H__)
#define __EAT_PERIPHERY_H__

#include "eat_type.h"

/* This enumerate all the Periphery Pin! */
#if defined(__SIMCOM_PROJ_SIM800W__)
//SIM800W
typedef enum {
    EAT_PIN6_ADC0 = 6,        /* ADC */
    EAT_PIN8_GPIO1 = 8,       /* GPIO */
    EAT_PIN9_I2C_SDA = 9,     /* GPIO, KEY_ROW, EINT, I2C_SDA */
    EAT_PIN10_I2C_SCL = 10,   /* GPIO, KEY_COL, I2C_SCL */
    EAT_PIN11_KPLED = 11,     /* KPLED */
    EAT_PIN16_NETLIGHT = 16,  /* GPIO, PWM */
    EAT_PIN28_GPIO2 = 28,     /* GPIO, EINT */
    EAT_PIN29_KBC5 = 29,      /* GPIO, KEY_COL, EINT */
    EAT_PIN30_KBC4 = 30,      /* GPIO, KEY_COL */
    EAT_PIN31_KBC3 = 31,      /* GPIO, KEY_COL */
    EAT_PIN32_KBC2 = 32,      /* GPIO, KEY_COL */
    EAT_PIN33_KBC1 = 33,      /* GPIO, KEY_COL */
    EAT_PIN34_KBC0 = 34,      /* GPIO, KEY_COL */
    EAT_PIN35_KBR5 = 35,      /* GPIO, KEY_ROW, EINT */
    EAT_PIN36_KBR4 = 36,      /* GPIO, KEY_ROW */
    EAT_PIN37_KBR3 = 37,      /* GPIO, KEY_ROW */
    EAT_PIN38_KBR2 = 38,      /* GPIO, KEY_ROW */
    EAT_PIN39_KBR1 = 39,      /* GPIO, KEY_ROW */
    EAT_PIN40_KBR0 = 40,      /* GPIO, KEY_ROW, SPI_LSDI */
    EAT_PIN45_GPIO3 = 45,     /* GPIO, EINT */
    EAT_PIN46_DISP_DATA = 46, /* GPIO, SPI_LSDA */
    EAT_PIN47_DISP_CLK = 47,  /* GPIO, SPI_SCK */
    EAT_PIN48_DISP_RST = 48,  /* GPIO */
    EAT_PIN49_DISP_DC = 49,   /* GPIO, KEY_ROW, SPI_LSA */
    EAT_PIN50_DISP_CS = 50,   /* GPIO, SPI_LSCE */
    EAT_PIN51_VDD_EXT = 51,   /* VDD_EXT */
    EAT_PIN52_PCM_SYNC = 52,  /* GPIO */
    EAT_PIN53_PCM_IN = 53,    /* GPIO */
    EAT_PIN54_PCM_CLK = 54,   /* GPIO */
    EAT_PIN55_PCM_OUT = 55,   /* GPIO */
    EAT_PIN57_GPIO4 = 57,     /* GPIO, KEY_COL, EINT */
    EAT_PIN58_RXD3 = 58,      /* GPIO, UART3 */
    EAT_PIN59_TXD3 = 59,      /* GPIO, UART3 */
    EAT_PIN60_RXD = 60,       /* UART1 */
    EAT_PIN61_TXD = 61,       /* UART1 */
    EAT_PIN62_DBG_RXD = 62,   /* GPIO, UART2 */    
    EAT_PIN63_DBG_TXD = 63,   /* GPIO, UART2 */
    EAT_PIN65_LCD_LIGHT = 65, /* LCD_LIGTH */
    EAT_PIN_NUM = 68
} EatPinName_enum;
#elif defined(__SIMCOM_PROJ_SIM800V__)
//SIM800V
typedef enum {
    EAT_PIN7_VDD_EXT = 7,     /* VDD_EXT */
    EAT_PIN5_ADC1= 5,         /* ADC1 */
    EAT_PIN9_ADC0 = 9,        /* ADC */
    EAT_PIN10_PWM = 10,       /* PWM */
    EAT_PIN11_NETLIGHT = 11,  /* GPIO */
    EAT_PIN27_I2C_SDA = 27,   /* GPIO, EINT, I2C_SDA */
    EAT_PIN28_I2C_SCL = 28,   /* GPIO, I2C_SCL */
    EAT_PIN29_SIM_PRES = 29,  /* GPIO, EINT */
    EAT_PIN30_KBC4 = 30,      /* GPIO, KEY_COL, EINT */
    EAT_PIN31_KBC3 = 31,      /* GPIO, KEY_COL */
    EAT_PIN32_KBC2 = 32,      /* GPIO, KEY_COL */
    EAT_PIN33_KBC1 = 33,      /* GPIO, KEY_COL */
    EAT_PIN34_KBC0 = 34,      /* GPIO, KEY_COL */
    EAT_PIN35_KBR4 = 35,      /* GPIO, KEY_ROW */
    EAT_PIN36_KBR3 = 36,      /* GPIO, KEY_ROW */
    EAT_PIN37_KBR2 = 37,      /* GPIO, KEY_ROW */
    EAT_PIN38_KBR1 = 38,      /* GPIO, KEY_ROW */
    EAT_PIN39_KBR0 = 39,      /* GPIO, KEY_ROW, SPI_LSDI */
    EAT_PIN40_DISP_DATA = 40, /* GPIO, SPI_LSDA */
    EAT_PIN41_DISP_CLK = 41,  /* GPIO, SPI_SCK */
    EAT_PIN42_DISP_RST = 42,  /* GPIO */
    EAT_PIN43_DISP_CS = 43,   /* GPIO, SPI_LSCE */
    EAT_PIN52_PCM_SYNC = 52,  /* GPIO */
    EAT_PIN53_PCM_IN = 53,    /* GPIO */
    EAT_PIN54_PCM_CLK = 54,   /* GPIO */
    EAT_PIN55_PCM_OUT = 55,   /* GPIO */
    EAT_PIN56_RXD3 = 56,      /* GPIO, UART3 */
    EAT_PIN57_TXD3 = 57,      /* GPIO, UART3 */
    EAT_PIN58_DTR = 58,       /* GPIO, EINT */
    EAT_PIN59_DCD = 59,       /* GPIO, EINT */
    EAT_PIN60_RI = 60,        /* GPIO, EINT */
    EAT_PIN61_RTS = 61,       /* GPIO */
    EAT_PIN62_CTS = 62,       /* GPIO, SPI_LSA, DISP_DC */
    EAT_PIN63_RXD = 63,       /* UART1 */
    EAT_PIN64_TXD = 64,       /* UART1 */
    EAT_PIN65_DBG_RXD = 65,   /* GPIO, UART2 */    
    EAT_PIN66_DBG_TXD = 66,   /* GPIO, UART2 */
    EAT_PIN67_STATUS = 67,    /* GPIO, EINT */
    EAT_PIN_NUM = 68
} EatPinName_enum;
#elif defined(__SIMCOM_PROJ_SIM800H__)
//SIM800H
typedef enum {
    EAT_PIN3_GPIO1 = 3,       /* GPIO */
    EAT_PIN4_STATUS= 4,       /* GPIO */
    EAT_PIN5_BPI_BUSI= 5,     /* GPIO */
    EAT_PIN18_VDD_EXT = 18,   /* VDD_EXT */
    EAT_PIN20_COL0 = 20,      /* GPIO, KEY_COL */
    EAT_PIN21_COL3 = 21,      /* GPIO, KEY_COL */
    EAT_PIN22_COL2 = 22,      /* GPIO, KEY_COL */
    EAT_PIN23_ROW3 = 23,      /* GPIO, KEY_ROW */
    EAT_PIN24_COL4 = 24,      /* GPIO, KEY_COL, EINT */
    EAT_PIN25_COL1 = 25,      /* GPIO, KEY_COL */
    EAT_PIN26_PWM = 26,       /* GPIO, PWM, EINT */
    EAT_PIN27_GPIO2 = 27,     /* GPIO, LSA0 */
    EAT_PIN28_GPIO3 = 28,     /* GPIO, SPI_CLK */
    EAT_PIN29_PCM_CLK = 29,   /* GPIO, LSRSTB, PCMCLK */
    EAT_PIN30_PCM_OUT = 30,   /* GPIO, SPI_MISO, PCMOUT */
    EAT_PIN31_RXD = 31,       /* UART1 */
    EAT_PIN32_TXD = 32,       /* UART1 */
    EAT_PIN33_CTS = 33,       /* GPIO, U1CTS, U2TXD */
    EAT_PIN34_RTS = 34,       /* GPIO, U1RTS, U2RXD */
    EAT_PIN50_ADC= 50,        /* ADC */
    EAT_PIN54_SIM_PRE=54,     /* GPIO, EINT, PCMRST*/
    EAT_PIN60_ROW1 = 60,      /* GPIO, KEY_ROW */
    EAT_PIN61_ROW2 = 61,      /* GPIO, KEY_ROW */
    EAT_PIN62_ROW0 = 62,      /* GPIO, KEY_ROW */
    EAT_PIN63_ROW4 = 63,      /* GPIO, KEY_ROW, EINT */
    EAT_PIN64_NETLIGHT = 64,  /* GPIO */
    EAT_PIN65_PCM_SYNC = 65,  /* GPIO, SPI_CS, PCMSYNC */
    EAT_PIN66_PCM_IN = 66,    /* GPIO ,SPI_MOSI, PCMIN*/
    EAT_PIN68_UART1_RI = 68,  /* GPIO, U2CTS */
    EAT_PIN69_UART1_DTR = 69, /* GPIO, EINT, PWM */
    EAT_PIN70_UART1_DCD = 70, /* GPIO, U2RTS */   
    EAT_PIN74_SCL = 74,       /* GPIO, SCL */
    EAT_PIN75_SDA = 75,       /* GPIO, SDA */
        
    EAT_PIN_NUM = 76
} EatPinName_enum;
#elif defined(__SIMCOM_PROJ_SIM800__)
//SIM800
typedef enum {
    EAT_PIN3_DTR = 3,         /* GPIO, EINT */
    EAT_PIN4_RI = 4,          /* GPIO, U2CTS */ 
    EAT_PIN5_DCD = 5,         /* GPIO, U2RTS */  
    EAT_PIN6_PCM_OUT = 6,     /* GPIO, SPI_MISO, PCMOUT */
    EAT_PIN7_CTS = 7,         /* GPIO, U2RXD,U1CTS */
    EAT_PIN8_RTS = 8,         /* GPIO, U2TXD,U1RTS */
    EAT_PIN9_TXD = 9,         /* UART1 */
    EAT_PIN10_RXD = 10,       /* UART1 */
    EAT_PIN11_GPIO17 = 11,    /* GPIO, SPI_CLK */
    EAT_PIN12_PCM_IN = 12,    /* GPIO, SPI_MOSI, PCMIN */ 
    EAT_PIN13_GPIO19 = 13,    /* GPIO, LSA0 */ 
    EAT_PIN14_PCM_SYNC = 14,  /* GPIO, SPI_CS, PCMSYNC */ 
    EAT_PIN15_VDD_EXT = 15,   /* VDD_EXT */
    EAT_PIN25_ADC= 25,        /* ADC */
    EAT_PIN34_SIM_PRE=34,     /* GPIO, EINT, PCMRST */ 
    EAT_PIN35_PWM1 = 35,      /* GPIO, PWM, EINT */
    EAT_PIN36_PWM2 = 36,      /* GPIO, PWM, EINT */
    EAT_PIN37_SDA = 37,       /* GPIO, SDA */ 
    EAT_PIN38_SCL = 38,       /* GPIO, SCL */
    EAT_PIN40_ROW4 = 40,      /* GPIO, KEY_ROW, EINT */ 
    EAT_PIN41_ROW3 = 41,      /* GPIO, KEY_ROW */
    EAT_PIN42_ROW2 = 42,      /* GPIO, KEY_ROW */
    EAT_PIN43_ROW1 = 43,      /* GPIO, KEY_ROW */
    EAT_PIN44_ROW0 = 44,      /* GPIO, KEY_ROW */
    EAT_PIN47_COL4 = 47,      /* GPIO, KEY_COL, EINT */
    EAT_PIN48_COL3 = 48,      /* GPIO, KEY_COL */
    EAT_PIN49_COL2 = 49,      /* GPIO, KEY_COL */
    EAT_PIN50_COL1 = 50,      /* GPIO, KEY_COL */
    EAT_PIN51_COL0 = 51,      /* GPIO, KEY_COL */
    EAT_PIN52_NETLIGHT = 52,  /* GPIO */
    EAT_PIN66_STATUS= 66,     /* GPIO */
    EAT_PIN67_RF_SYNC = 67,   /* GPIO */
    EAT_PIN68_PCM_CLK = 68,   /* GPIO, PCMCLK */ 
    EAT_PIN_NUM = 69
} EatPinName_enum;
#elif defined(__SIMCOM_PROJ_SIM808__)
//SIM808
typedef enum {
    EAT_PIN9_DTR = 9,        /* GPIO, EINT */
    EAT_PIN12_CTS = 12,      /* GPIO */  
    EAT_PIN13_RTS = 13,      /* GPIO */
    EAT_PIN14_TXD = 14,      /* UART1 */
    EAT_PIN15_RXD = 15,      /* UART1 */
    EAT_PIN23_ADC1 = 23,     /* ADC */
    EAT_PIN24_ADC2 = 24,     /* ADC */
    EAT_PIN33_SIM_PRE = 33,  /* GPIO, EINT, PCMRST */
    EAT_PIN38_SDA = 38,      /* GPIO, SDA */ 
    EAT_PIN39_SCL = 39,      /* GPIO, SCL */ 
    EAT_PIN41_PWM2 = 41,     /* GPIO, PWM, EINT */ 
    EAT_PIN42_PWM1 = 42,     /* GPIO, PWM, EINT */
    EAT_PIN43_GPIO5 = 43,    /* GPIO, LSA0 */
    EAT_PIN44_GPIO6 = 44,    /* GPIO, SPI_CLK */ 
    EAT_PIN45_PCM_SYNC = 45, /* GPIO, SPI_CS, PCMSYNC */
    EAT_PIN46_PCM_CLK = 46,  /* GPIO, PCMCLK */
    EAT_PIN47_PCM_IN = 47,   /* GPIO, SPI_MOSI, PCMIN */ 
    EAT_PIN48_PCM_OUT = 48,  /* GPIO, SPI_MISO, PCMOUT */
    EAT_PIN49_STATUS = 49,   /* GPIO */ 
    EAT_PIN50_NETLIGHT = 50, /* GPIO */
    EAT_PIN55_ROW3 = 55,     /* GPIO, KEY_ROW */
    EAT_PIN56_ROW2 = 56,     /* GPIO, KEY_ROW */
    EAT_PIN57_ROW1 = 57,     /* GPIO, KEY_ROW */
    EAT_PIN58_ROW0 = 58,     /* GPIO, KEY_ROW */
    EAT_PIN59_COL3 = 59,     /* GPIO, KEY_COL */
    EAT_PIN60_COL2 = 60,     /* GPIO, KEY_COL */
    EAT_PIN61_COL1 = 61,     /* GPIO, KEY_COL */
    EAT_PIN62_COL0 = 62,     /* GPIO, KEY_COL */
    EAT_PIN_NUM = 69
} EatPinName_enum;
/*    SIMCOM WangJian 2015-02-26 fix bug for MKBug00023814 BEGIN*/
#elif defined(__SIMCOM_PROJ_SIM800C__)
typedef enum {
    EAT_PIN1_UART1_TXD  = 1,  /* UART1_TXD */
    EAT_PIN2_UART1_RXD  = 2,  /* UART1_RXD */
    EAT_PIN3_UART1_RTS  = 3,  /* GPIO, UART1_RTS */
    EAT_PIN4_UART1_CTS  = 4,  /* GPIO, UART1_CTS */
    EAT_PIN5_UART1_DCD  = 5,  /* GPIO, UART1_DCD */
    EAT_PIN6_UART1_DTR  = 6,  /* GPIO, EINT, UART1_DTR */ 
    EAT_PIN7_UART1_RI   = 7,  /* GPIO, UART1_RI */
    EAT_PIN14_SIM_DET   = 14, /* GPIO, EINT, SIM_DET */
    EAT_PIN22_UART2_TXD = 22, /* UART2_TXD */
    EAT_PIN23_UART2_RXD = 23, /* UART2_RXD */
    EAT_PIN38_ADC       = 38, /* ADC */
    EAT_PIN41_NETLIGHT  = 41, /* GPIO*/
    EAT_PIN42_STATUS    = 42, /* GPIO*/
    EAT_PIN_NUM         = 43
} EatPinName_enum;

/*    SIMCOM WangJian 2015-02-26 fix bug for MKBug00023814 END*/

#endif 

/* This enumerate all the Key Value ! */
#if defined(__SIMCOM_PROJ_SIM800V__)|| defined(__SIMCOM_PROJ_SIM800W__)
typedef enum {
    EAT_KEY_C0R0,
    EAT_KEY_C0R1,
    EAT_KEY_C0R2,
    EAT_KEY_C0R3,
    EAT_KEY_C0R4,
    EAT_KEY_C0R5,
    EAT_KEY_C1R0,
    EAT_KEY_C1R1,
    EAT_KEY_C1R2,
    EAT_KEY_C1R3,
    EAT_KEY_C1R4,
    EAT_KEY_C1R5,
    EAT_KEY_C2R0,
    EAT_KEY_C2R1,
    EAT_KEY_C2R2,
    EAT_KEY_C2R3,
    EAT_KEY_C2R4,
    EAT_KEY_C2R5,
    EAT_KEY_C3R0,
    EAT_KEY_C3R1,
    EAT_KEY_C3R2,
    EAT_KEY_C3R3,
    EAT_KEY_C3R4,
    EAT_KEY_C3R5,
    EAT_KEY_C4R0,
    EAT_KEY_C4R1,
    EAT_KEY_C4R2,
    EAT_KEY_C4R3,
    EAT_KEY_C4R4,
    EAT_KEY_C4R5,
    EAT_KEY_C5R0,
    EAT_KEY_C5R1,
    EAT_KEY_C5R2,
    EAT_KEY_C5R3,
    EAT_KEY_C5R4,
    EAT_KEY_C5R5,
    EAT_KEY_POWER,
    EAT_KEY_NUM
} EatKey_enum; 
#elif defined(__SIMCOM_PROJ_SIM800C__)
typedef enum {
    EAT_KEY_POWER,
    EAT_KEY_NUM
} EatKey_enum; 
#else
typedef enum {
    EAT_KEY_C0R0,
    EAT_KEY_C0R1,
    EAT_KEY_C0R2,
    EAT_KEY_C0R3,
    EAT_KEY_C0R4,
    EAT_KEY_C1R0,
    EAT_KEY_C1R1,
    EAT_KEY_C1R2,
    EAT_KEY_C1R3,
    EAT_KEY_C1R4,
    EAT_KEY_C2R0,
    EAT_KEY_C2R1,
    EAT_KEY_C2R2,
    EAT_KEY_C2R3,
    EAT_KEY_C2R4,
    EAT_KEY_C3R0,
    EAT_KEY_C3R1,
    EAT_KEY_C3R2,
    EAT_KEY_C3R3,
    EAT_KEY_C3R4,
    EAT_KEY_C4R0,
    EAT_KEY_C4R1,
    EAT_KEY_C4R2,
    EAT_KEY_C4R3,
    EAT_KEY_C4R4,
    EAT_KEY_POWER,
    EAT_KEY_NUM
} EatKey_enum; 
#endif
/* GPIO LEVEL type. */
typedef enum {
    EAT_GPIO_LEVEL_LOW, /* low level */
    EAT_GPIO_LEVEL_HIGH /* high levle */
} EatGpioLevel_enum; 

/* GPIO MODE type. */
typedef enum {
    EAT_GPIO_DIR_INPUT, /* input */
    EAT_GPIO_DIR_OUTPUT, /* output */
} EatGpioDir_enum; 

/* The GPIO EINT trigger mode. */
typedef enum {
    EAT_INT_TRIGGER_HIGH_LEVEL, /* high level */
    EAT_INT_TRIGGER_LOW_LEVEL, /* low level */
    EAT_INT_TRIGGER_RISING_EDGE, /* rising edge */
    EAT_INT_TRIGGER_FALLING_EDGE, /* falling edge */
    EAT_INT_TRIGGER_NUM
} EatIntTrigger_enum; 

/* SPI Wire type. */
typedef enum {
    EAT_SPI_3WIRE, /* 3 wire SPI */
    EAT_SPI_4WIRE /* 4 wire SPI */
} EatSpiWire_enum;

/* SPI Clock type. */
typedef enum {
    EAT_SPI_CLK_52M = 1, /* 52M clock */
    EAT_SPI_CLK_26M = 2, /* 26M clock */
    EAT_SPI_CLK_13M = 4  /* 13M clock */
} EatSpiClk_enum; 

/* SPI Bit type. */
typedef enum {
    EAT_SPI_BIT8, /* 8 bit */
    EAT_SPI_BIT9, /* 9 bit */
    EAT_SPI_BIT16, /* 16 bit */
    EAT_SPI_BIT24, /* 24 bit */
    EAT_SPI_BIT32  /* 32 bit */
} EatSpiBit_enum; 

/* EAT KEY configuration structure. */
typedef struct {
    EatKey_enum key_value; /* key value */
    eat_bool is_pressed; /* 1-key press down; 0-key release up */
} EatKey_st; 

/* EAT INT configuration structure. */
typedef struct {
    EatPinName_enum pin; /* the pin */
    EatGpioLevel_enum level; /* 1-high level; 0-low level */
} EatInt_st; 

/* EAT pin mode. */
typedef enum {
    EAT_PIN_MODE_GPIO,/* GPIO ENUM */
    EAT_PIN_MODE_KEY, /* KEY ENUM */
    EAT_PIN_MODE_EINT,/* EINT ENUM */
    EAT_PIN_MODE_UART,/* UART ENUM */
    EAT_PIN_MODE_SPI, /* SPI ENUM */
    EAT_PIN_MODE_PWM, /* PWM ENUM */
    EAT_PIN_MODE_I2C, /* I2C ENUM */
    EAT_PIN_MODE_CLK, /* CLK ENUM */
    EAT_PIN_MODE_NUM 
} EatPinMode_enum;

/* EAT ADC configuration structure. */
typedef struct {
    EatPinName_enum pin; /* the pin */
    unsigned int v; /* ADC value,unit is mv */ 
} EatAdc_st;

/*EAT ISINK current step.*/
typedef enum
{
    EAT_BL_STEP_04_MA ,
	EAT_BL_STEP_08_MA ,	
	EAT_BL_STEP_12_MA ,
	EAT_BL_STEP_16_MA ,
	EAT_BL_STEP_20_MA ,
	EAT_BL_STEP_24_MA ,
	EAT_BL_STEP_NUM
}EatBLStep_enum;

typedef enum
{
   EAT_I2C_OWNER_0=0,
   EAT_I2C_OWNER_1,
   EAT_I2C_OWNER_2,
   EAT_I2C_OWNER_3,
   EAT_I2C_OWNER_4,
   EAT_I2C_OWNER_5,
   EAT_I2C_OWNER_6,
   EAT_I2C_OWNER_7,
   EAT_I2C_OWNER_MAX
}EAT_I2C_OWNER;


typedef enum
{
   EAT_DEV_STATUS_OK = 0,
   EAT_DEV_STATUS_FAIL = -1,
   EAT_DEV_STATUS_INVALID_CMD = -2,
   EAT_DEV_STATUS_UNSUPPORTED = -3,
   EAT_DEV_STATUS_NOT_OPENED = -4,
   EAT_DEV_STATUS_INVALID_EVENT = -5,
   EAT_DEV_STATUS_INVALID_DCL_HANDLE = -6,
   EAT_DEV_STATUS_INVALID_CTRL_DATA = -7,
   EAT_DEV_STATUS_INVALID_CONFIGURATION = -8,
   EAT_DEV_STATUS_INVALID_ARGUMENT = -9,
   EAT_DEV_STATUS_ERROR_TIMEOUT = -10,
   EAT_DEV_STATUS_ERROR_CRCERROR = -11,
   EAT_DEV_STATUS_ERROR_READONLY = -12,
   EAT_DEV_STATUS_ERROR_WRONG_STATE = -13,
   EAT_DEV_STATUS_INVALID_DEVICE = -14,
   EAT_DEV_STATUS_ALREADY_OPENED = -15,
   EAT_DEV_STATUS_SET_VFIFO_FAIL = -16,
   EAT_DEV_STATUS_INVALID_OPERATION = -17,
   EAT_DEV_STATUS_DEVICE_NOT_EXIST = -18,

   EAT_DEV_STATUS_DEVICE_NOT_SUPPORT_DMA = -19,
   EAT_DEV_STATUS_DEVICE_IS_BUSY = -20,
   EAT_DEV_STATUS_ACKERR = -21, 
   EAT_DEV_STATUS_HS_NACKERR = -22,

   EAT_DEV_STATUS_BUFFER_EMPTY = 1
}EAT_DEV_STATUS_T;

typedef enum
{
   EAT_USB_DATA_MODE,
   EAT_USB_AC_MODE
}EAT_USB_MODE;

/****************************************************************************
* Function : eat_gpio_setup
* Description: Sets the pin as gpio mode.
* Parameters:
*     pin   EatPinName_enum [IN] The pin want to set,please see EatPinName_enum.
*     dir   EatGpioDir_enum [IN] The input or output mode want to set,please see EatGpioDir_enum.
*    level  EatGpioLevel_enum [IN] The level want to set,please see EatGpioLevel_enum.
* Returns:
*     EAT_TRUE  The operation Sucess.
*     EAT_FALSE The operation false.
* NOTE:
*   The "level" is valid when the parameter "dir" is EAT_GPIO_DIR_OUTPUT
*****************************************************************************/
extern eat_bool (* const eat_gpio_setup)(EatPinName_enum pin,  EatGpioDir_enum dir, EatGpioLevel_enum level);

/*****************************************************************************
* Function : eat_gpio_write
* Description: Sets the level of the pin.
* Parameters:
*     pin   EatPinName_enum [IN] The pin want to set,please see EatPinName_enum.
*     level  EatGpioLevel_enum  [IN] The level want to set,please see EatGpioLevel_enum.
* Returns:
*     EAT_TRUE  The operation Sucess.
*     EAT_FALSE The operation false.
* NOTE:
*     Nothing
*****************************************************************************/
extern eat_bool (* const eat_gpio_write)(EatPinName_enum pin, EatGpioLevel_enum level);

/*****************************************************************************
* Function :  eat_gpio_read
* Description: Reads the GPIO Level.
* Parameters:
*     pin  EatPinName_enum [IN] The pin wan to write to read,please see EatPinName_enum.
* Returns:
*     EAT_GPIO_LEVEL_LOW  The low Level,please see EatGpioLevel_enum.
*     EAT_GPIO_LEVEL_HIGH, The high Level.
* NOTE:
*     Nothing
*****************************************************************************/
extern EatGpioLevel_enum (* const eat_gpio_read)(EatPinName_enum pin);

/****************************************************************************
* Function :  eat_gpio_set_pull
* Description: Sets the pin pull up/down.
* Parameters:
*     pin   EatPinName_enum [IN] The pin want to set,please see EatPinName_enum.
*     en    eat_bool [IN]  enable the pull up/down.
*    level  EatGpioLevel_enum [IN] The level want to set,please see EatGpioLevel_enum.
* Returns:
*     EAT_TRUE  The operation Sucess.
*     EAT_FALSE The operation false.
* NOTE:
*   Nothing
*****************************************************************************/
extern eat_bool (* const eat_gpio_set_pull)(EatPinName_enum pin, eat_bool en, EatGpioLevel_enum level);

/*****************************************************************************
* Function :  eat_gpio_int_callback_func
* Description: The callback function of the eat_int_setup.
* Parameters:
*     interrupt  EatInt_st* [IN] The EatInt_st struct point.
* Returns:
*     No returen value (void)
* NOTE:
*     If this callback is NULL it will send EAT_EVENT_INT to App when EINT triggered.
*****************************************************************************/
typedef void (*eat_gpio_int_callback_func)(EatInt_st *interrupt);

/*****************************************************************************
* Function :  eat_int_setup
* Description: Sets the pin as interrupt mode.
* Parameters:
*     pin    EatPinName_enum    [IN] The interrupt pin want to set,please see EatPinName_enum.
*     trigger  EatIntTrigger_enum  [IN] The trigger mode want to set,please see EatIntTrigger_enum.
*     debounce_10ms   unsigned int    [IN] The debounce ,uint is 10ms,such as debounce_10ms:10 means about 100ms.
*     callback  eat_gpio_int_callback_func  [IN] The callback function,if NULL,will send 
*                                         EAT_EVENT_INT to App when EINT triggered.if not NULL,will
*                                         call the callback function when EINT triggered.
* Returns:
*     EAT_TRUE  The operation sucess.
*     EAT_FALSE The operation false.
* NOTE:
*    The pin only for defined in EatPinName_enum which can used in EINT,for example:EAT_PIN28_GPIO2 = 28, // GPIO, EINT.
*****************************************************************************/
extern eat_bool (* const eat_int_setup)(EatPinName_enum pin,  EatIntTrigger_enum trigger, unsigned int debounce_10ms, eat_gpio_int_callback_func callback);

/*****************************************************************************
* Function :  eat_int_release
* Description: Mask the pin as interrupt mode.
* Parameters:
*     pin    EatPinName_enum    [IN] The interrupt pin ,please see EatPinName_enum.
* Returns:
*     EAT_TRUE  The operation sucess.
*     EAT_FALSE The operation false.
* NOTE:
*    The pin only for defined in EatPinName_enum which can used in EINT,for example:EAT_PIN28_GPIO2 = 28, // GPIO, EINT.
*****************************************************************************/
extern eat_bool (*const eat_int_release)(EatPinName_enum pin);

/*****************************************************************************
* Function :  eat_int_setup_ext
* Description: Sets the pin as interrupt mode.
* Parameters:
*     pin    EatPinName_enum    [IN] The interrupt pin want to set,please see EatPinName_enum.
*     trigger  EatIntTrigger_enum  [IN] The trigger mode want to set,please see EatIntTrigger_enum.
*     debounce_10ms   unsigned int    [IN] The debounce ,uint is 10ms,such as debounce_10ms:10 means about 100ms.
*     callback  eat_gpio_int_callback_func  [IN] The callback function,if NULL,will send 
*                                         EAT_EVENT_INT to App when EINT triggered.if not NULL,will
*                                         call the callback function when EINT triggered.
*    auto_invert eat_bool [IN]  Auto invert trigger polarity in level mode.
* Returns:
*     EAT_TRUE  The operation sucess.
*     EAT_FALSE The operation false.
* NOTE:
*    1.The pin only for defined in EatPinName_enum which can used in EINT,for example:EAT_PIN28_GPIO2 = 28, // GPIO, EINT.
*    2.In LEVEL interrupt mode, if the auto_invert is EAT_TRUE, Core will auto invert trigger mode and the app don't need 
*      to setup trigger polarity by eat_int_set_trigger.
*    3.The "auto_invert" is effective in LEVEL interrupt mode only.
*****************************************************************************/
extern eat_bool (* const eat_int_setup_ext)(EatPinName_enum pin,  EatIntTrigger_enum trigger, unsigned int debounce_10ms, eat_gpio_int_callback_func callback, eat_bool auto_invert);

/*****************************************************************************
* Function :  eat_int_set_trigger
* Description: Sets the trigger mode of the interrupt.
* Parameters:
*     pin    EatPinName_enum    [IN] The interrupt pin want to set,please see EatPinName_enum.
*     trigger  EatIntTrigger_enum  [IN] The trigger mode want to set,please see EatIntTrigger_enum.
* Returns:
*     EAT_TRUE  The operation Sucess.
*     EAT_FALSE The operation false.
* NOTE:
*    The pin only for defined in EatPinName_enum which can used in EINT,for example:EAT_PIN28_GPIO2 = 28, // GPIO, EINT.
*****************************************************************************/
extern eat_bool (* const eat_int_set_trigger)(EatPinName_enum pin,  EatIntTrigger_enum trigger);

/*****************************************************************************
* Function :  eat_spi_init
* Description: Initializes the SPI.
* Parameters :
*     clk  EatSpiClk_enum [IN] The freg of the GPIO3 pin,you can also turn down,please see EatSpiClk_enum. 
*     wire EatSpiWire_enum [IN] Connect mode,4 wire will use LSA0DA1 pin as DC,3 wire not use,please see EatSpiWire_enum.
*     bit  EatSpiBit_enum [IN] The transmit speed,please see EatSpiBit_enum.
*     enable_SDI eat_bool [IN] Whether use PCMOUT pin as SDI1.
*     enable_cs eat_bool [IN] Whether use PCMSYNC control when read or write SPI,if not,can control by APP.
* Returns:
*     EAT_TRUE  The operation Sucess.
*     EAT_FALSE The operation false.
* NOTE:
*     Nothing
*****************************************************************************/
extern eat_bool (* const eat_spi_init)(EatSpiClk_enum clk, EatSpiWire_enum wire, EatSpiBit_enum bit, 
   eat_bool enable_SDI, eat_bool enable_cs);

/*****************************************************************************
* Function :  eat_spi_write
* Description: Writes data or command to the spi.
* Parameters :
*     data  unsigned char* [IN] The data buffer point,wan to write to spi.
*     len  unsigned char  [IN] The length of the data want to write.
*     is_command eat_bool [IN] The type want to write,is_command:1 command,is_command:0 data.
* Returns:
*     EAT_TRUE  The operation Sucess.
*     EAT_FALSE The operation false.
* NOTE:
*     Nothing
*****************************************************************************/
extern eat_bool (* const eat_spi_write)(const unsigned char *data, unsigned char len, eat_bool is_command);

/*****************************************************************************
* Function :  eat_spi_read
* Description: Reads data from the spi.
* Parameters:
*     data  unsigned char* [OUT] The buffer point,the data read from spi will save in this buffer.
*     len  unsigned char  [IN] The length of the data want to read.
* Returns:
*     The length of the data having read.
* NOTE:
*     Nothing
*****************************************************************************/
extern unsigned char  (* const eat_spi_read)(unsigned char *data, unsigned char len);

/*****************************************************************************
* Function :  eat_spi_write_read
* Description: Writes data to spi ,then reads data from the spi.
* Parameters:
*     wdata  unsigned char* [IN] The data buffer point,wan to write to spi..
*     wlen    unsigned char   [IN] The length of the data want to write.
*     rdata   unsigned char* [OUT] The buffer point,the data read from spi will save in this buffer.
*     rlen     unsigned char   [IN] The length of the data want to read.
* Returns:
*     The length of the data having read.
* NOTE:
*     Nothing
*****************************************************************************/
extern unsigned char (* const eat_spi_write_read)(const unsigned char *wdata, unsigned char wlen, unsigned char* rdata, unsigned char rlen);

#ifndef __SIMCOM_PROJ_SIM808__
#ifndef __SIMCOM_PROJ_SIM800__
/*****************************************************************************
* Function :  eat_lcd_light_sw
* Description: Controls the lcd backlight turn on or turn off.
* Parameters:
*     sw  eat_bool [IN] The control switch.sw:0 off,sw:1 on.
*     step EatBLStep_enum [IN] The step of ISINK.
* Returns:
*     No returen value (void).
* NOTE
*     If lcd backlight on,it will disable system sleep.
*****************************************************************************/
extern void (* const eat_lcd_light_sw)(eat_bool sw, EatBLStep_enum step);
#endif //800
/*****************************************************************************
* Function :  eat_kpled_sw
* Description:  Controls the keypad led turn on or turn off.
* Parameters:
*     sw  eat_bool [IN] The control switch.sw:0 off,sw:1 on.
* Returns:
*     No returen value (void)
* NOTE
*     If keypad led on,it will disable system sleep.
*****************************************************************************/
extern void (* const eat_kpled_sw)(eat_bool sw);
#endif //808

/*****************************************************************************
* Function :  eat_adc_callback_func
* Description: The callback function of the eat_adc_get.
* Parameters:
*     adc  EatAdc_st* [IN/OUT] The EatAdc_st struct point.
* Returns:
*     No returen value (void)
* NOTE:
*     If this callback is NULL,it will send EAT_EVENT_ADC to App when read the ADC.
*****************************************************************************/
typedef void (*eat_adc_callback_func)(EatAdc_st *adc);

/*****************************************************************************
* Function :  eat_adc_get
* Description: Reads the ADC value of the EAT_PIN6_ADC0.
* Parameters:
*     pin  EatPinName_enum [IN] The pin want to read ADC,note is only in
*                          EAT_PIN6_ADC0 pin.
*     period  unsigned int [IN] The mode want to read .
*                          0: read once.
*                          other: Periodically read,the unit is ms. 
*                          such as : period=1000 read the adc one time in one second.
*     callback  eat_adc_callback_func [IN] The callback function,if NULL,will send 
*                           EAT_EVENT_ADC to App when read the value.if not NULL,will
*                           call the callback function when read the value.
* Returns:
*     EAT_TRUE  The operation Sucess.
*     EAT_FALSE The operation false.
* NOTE:
*     Nothing
*****************************************************************************/
extern eat_bool (* const eat_adc_get)(EatPinName_enum pin, unsigned int period, eat_adc_callback_func callback);

/*****************************************************************************
* Function : eat_get_adc_sync
* Description: Get adc value directly.
* Parameters:
*     pin      [IN] The pin want to read ADC,available pin is listed in the enum EatPinName_enum
*                   above.
*     voltage  [OUT] store adc value,unit is mv.
* Returns:
*     if succeed    return EAT_TURE;
*     otherwise     return EAT_FALSE.
* NOTE:
*     It takes about 10 milliseconds.
*     Voltage range:[0-2800]mv.
*****************************************************************************/
extern eat_bool  (*const eat_get_adc_sync)(EatPinName_enum pin,u32 *voltage);

/*****************************************************************************
* Function :  eat_pwm_start
* Description: Exports the PWM in EAT_PIN16_NETLIGHT.
* Parameters:
*     freq  unsigned int  [IN] The freq of the PWM want to export.
*     duty  unsigned char [IN] The duty of the PWM want to export 0--100.
* Returns:
*     No returen value (void)
* NOTE:
*     Only the EAT_PIN16_NETLIGHT can Export PWM.
*****************************************************************************/
extern void (* const eat_pwm_start)(unsigned int freq, unsigned char duty);

/*****************************************************************************
* Function :  eat_pwm_stop
* Description: Stops the export of the PWM.
* Parameters:
*     No parameters (void)
* Returns:
*     No returen value (void)
* NOTE:
*     Nothing
*****************************************************************************/
extern void (* const eat_pwm_stop)();

/*****************************************************************************
* Function : eat_pin_set_mode
* Description: Sets the mode of the pin. 
* Parameters:
*     pin   EatPinName_enum [IN] The pin want to set.
*     mode  EatPinMode_enum  [IN] The mode want to set.
* Returns:
*     EAT_TRUE  The operation Sucess.
*     EAT_FALSE The operation false.
* NOTE:
*     Nothing
*****************************************************************************/
extern eat_bool (* const eat_pin_set_mode)(EatPinName_enum pin, EatPinMode_enum mode);

/*****************************************************************************
* Function :  eat_poweroff_key_sw
* Description: Switch of the powerkey shutdown function.
* Parameters:
*    sw eat_bool [IN] the control switch.
*                KAL_TRUE: enable long press Powerkey shutdown.
*                KAL_FALSE: disable.
* Returns:
*     No returen value (void)
* NOTE:
*     Open or close the powerkey shutdown function,if open longpress powerkey
*     about 4.6s will shutdown.
*****************************************************************************/
extern void (* const eat_poweroff_key_sw)(eat_bool sw);
/*****************************************************************************
* Function :   eat_sim_detect_en
* Description: Enable or disable SIM card detection(hot plug) function.
* Parameters:
*    sw eat_bool [IN] the control switch.
*                KAL_TRUE: enable simcard detection function.
*                KAL_FALSE: disable.
* Returns:
*     No returen value (void)
* NOTE:
*      The Core default setting is enable SIM card detection.
*      If want to use EAT_PINxx_SIM_PRE,must disable SIM card detection
*      first in (*func_ext1)().
*****************************************************************************/
extern void (*const eat_sim_detect_en)(eat_bool sw);

/*****************************************************************************
* Function :  eat_i2c_open
* Description: Open I2C function.
* Parameters:
*     i2cowner    EAT_I2C_OWNER    [IN] The i2c owner for different device.
*     chipid   u8                       [IN] The address of the slave device.
*     speed   u16   [IN] The Transaction speed (Kbps).
* Returns:
*      Successful: EAT_DEV_STATUS_OK
*      Failed:     Please refer to EAT_DEV_STATUS_T.
* NOTE:
*     The chipid is 7bit , for example :xxxxxxx+r/w=8bit,  xxxxxxx is the chipid.
*     The max speed is 1000Kbps.
*****************************************************************************/
extern s32 (*const eat_i2c_open)(EAT_I2C_OWNER i2cowner,u8 chipid,u16 speed);
/*****************************************************************************
* Function :  eat_i2c_write
* Description: Write I2C function.
* Parameters:
*     i2cowner    EAT_I2C_OWNER    [IN] The i2c owner for different device.
*     writebuff   u8    [IN] The pointer of the data to be writed.
*     w_len   u16    [IN] The length of the data to be writed.
* Returns:
*      Successful: EAT_DEV_STATUS_OK
*      Failed:     Please refer to EAT_DEV_STATUS_T.
* NOTE:
*     Nothing
*****************************************************************************/
extern s32 (*const eat_i2c_write)(EAT_I2C_OWNER i2cowner,u8* writebuff, u16 w_len);
/*****************************************************************************
* Function :  eat_i2c_read
* Description: Read I2C function.
* Parameters:
*     i2cowner    EAT_I2C_OWNER    [IN] The i2c owner for different device.
*     writebuff    u8   [IN] The pointer of the data to be writed.
*     w_len        u16    [IN] The length of the data to be writed.
*     readbuff     u8    [OUT] Pointer the receive for the data to be read.
*     r_len         u16    [IN] The length of the data to be read.
* Returns:
*      Successful: EAT_DEV_STATUS_OK
*      Failed:     Please refer to EAT_DEV_STATUS_T.
* NOTE:
*     Nothing
*****************************************************************************/
extern s32 (*const eat_i2c_read)(EAT_I2C_OWNER i2cowner,u8* writebuff, u16 w_len, u8*readbuff, u16 r_len);
/*****************************************************************************
* Function :  eat_i2c_close
* Description: Close I2C function.
* Parameters:
*     i2cowner    EAT_I2C_OWNER    [IN] The i2c owner for different device.
* Returns:
*      Successful: EAT_DEV_STATUS_OK
*      Failed:     Please refer to EAT_DEV_STATUS_T.
* NOTE:
*     Nothing
*****************************************************************************/
extern s32 (*const eat_i2c_close)(EAT_I2C_OWNER i2cowner);

/*****************************************************************************
* Function :  eat_usb_eint_callback_func
* Description: The callback function of the eat_usb_eint_register.
* Parameters:
*     en  eat_bool [IN] The USB plug in or out.
*     mode  EAT_USB_MODE [IN] The USB interrupt type.
* Returns:
*     No returen value (void)
* NOTE:
*     Nothing
*****************************************************************************/
typedef void (*eat_usb_eint_callback_func)(eat_bool en,EAT_USB_MODE mode);

/*****************************************************************************
* Function :  eat_usb_eint_register
* Description: Register USB interrupt callback function.
* Parameters:
*     callback  eat_usb_eint_callback_func [IN] The callback function,if NULL,will not  
*                           USB interrupt to App .if not NULL,will
*                           call the callback function when USB plug in or out.
* Returns:
*     EAT_TRUE  The operation Sucess.
*     EAT_FALSE The operation false.
* NOTE:
*     Nothing
*****************************************************************************/
extern eat_bool (*const eat_usb_eint_register)(eat_usb_eint_callback_func callback);

/*****************************************************************************
* Function :  eat_get_usb_status
* Description: Get USB status.
* Parameters: void
* Returns:
*     EAT_TRUE  The USB had plugged in.
*     EAT_FALSE The USB didn't plug in.
* NOTE:
*     Nothing
*****************************************************************************/
extern eat_bool (*const eat_get_usb_status)(void);
/*****************************************************************************
* Function : eat_set_keep_vddext_before_powdown
* Description: keep vddext bdfore power down.
* Parameters: vddext_flag - EAT_FALSE,not keep vddext before power down
*                                     EAT_TRUE,keep vddext before power down
*
* Returns:
* NOTE:
*****************************************************************************/
extern void (*const eat_set_keep_vddext_before_powdown)(eat_bool vddext_flag);

/*****************************************************************************
* Function : eat_get_module_temp_sync
* Description: Get module temperature directly.
* Parameters:
*     temp  [OUT] store the value of temperature
* Returns:
*     if succeed    return EAT_TRUE.
*     otherwise     return EAT_FALSE.
* NOTE:
*     It takes about 10 milliseconds.The value of temp is multiplied by 1000(eg 1 degrees 
*     celsius return 1000).
*****************************************************************************/
extern eat_bool (*const eat_get_module_temp_sync)(s32 *temp);

typedef void (*eat_temp_callback_func)(s32 *tmp);  
/*****************************************************************************
* Function : eat_get_module_temp_asyn
* Description: Get module temperature.
* Parameters:
*     callback  [IN] The callback function.if not NULL,will call the callback function 
*                    when read the value.if NULL, will return false.
* Returns:
*     if succeed    return EAT_TRUE.
*     otherwise     return EAT_FALSE.
* NOTE:
*     value of tmp(temperature) is multiplied by 1000(eg 1 degrees celsius return 1000).
*****************************************************************************/
extern eat_bool (*const eat_get_module_temp_asyn)(eat_temp_callback_func callback);

#endif

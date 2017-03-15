#if !defined (__EAT_UART_H__)
#define __EAT_UART_H__

/** EAT UART port enum.     
 *  This enumerate all the UART ports!
 */
typedef enum {
    EAT_UART_1, 
#ifndef __SIMCOM_PROJ_SIM808__
    EAT_UART_2, 
#endif
#ifdef __USB_COM_PORT_ENABLE__
    EAT_UART_USB, //only for AT port or DEBUG port
#endif
    EAT_UART_NUM,
    EAT_UART_NULL = 99
} EatUart_enum;

/** EAT event parameter type.
 *  This struct block carries the UART port for EAT_EVENT_UART_READY_RD and EAT_EVENT_UART_READY_WR.
 */
typedef struct {
    EatUart_enum uart;
} EatUart_st;

/** EAT UART bandrate type. */
typedef enum {
	EAT_UART_BAUD_1200          =1200,
	EAT_UART_BAUD_2400          =2400,
	EAT_UART_BAUD_4800          =4800,
	EAT_UART_BAUD_9600          =9600,
	EAT_UART_BAUD_19200         =19200,
	EAT_UART_BAUD_38400         =38400,
	EAT_UART_BAUD_57600         =57600,
	EAT_UART_BAUD_115200        =115200,
	EAT_UART_BAUD_230400        =230400,
	EAT_UART_BAUD_460800        =460800
} EatUartBaudrate;

/** EAT UART data bits mode. */
typedef enum {
      EAT_UART_DATA_BITS_5=5,
      EAT_UART_DATA_BITS_6,
      EAT_UART_DATA_BITS_7,
      EAT_UART_DATA_BITS_8
} EatUartDataBits_enum;

/** EAT UART stop bits mode. */
typedef enum {
      EAT_UART_STOP_BITS_1=1,
      EAT_UART_STOP_BITS_2,
      EAT_UART_STOP_BITS_1_5
} EatUartStopBits_enum;

/** EAT UART parity bits mode. */
typedef enum {
      EAT_UART_PARITY_NONE=0,
      EAT_UART_PARITY_ODD,
      EAT_UART_PARITY_EVEN,
      EAT_UART_PARITY_SPACE
} EatUartParity_enum;

/** EAT UART configuration structure. */
typedef struct {
    EatUartBaudrate  baud; 
    EatUartDataBits_enum  dataBits;
    EatUartStopBits_enum  stopBits;
    EatUartParity_enum  parity;
} EatUartConfig_st;

/** EAT UART DEBUG mode . */
typedef enum {
    EAT_UART_DEBUG_MODE_TRACE, /*output debug binary info and analyze by tracer tool*/
    EAT_UART_DEBUG_MODE_UART  /*output debug string info and can be displayed by uart tools*/
}EatUartDebugMode_enum;

/*****************************************************************************
* Function :   eat_uart_open
* Description: This function allocates memory in the UART driver, 
*             refreshes UART FIFO, enables UART interrupt, etc. 
* Parameters :
*     uart : EatUart_enum [IN], UART port.
* Returns:
*     EAT_TRUE if success, otherwise EAT_FALSE.
*****************************************************************************/
extern eat_bool (* const eat_uart_open)(EatUart_enum uart);

/*****************************************************************************
* Function :   eat_uart_close
* Description: This function frees any memory allocated by eat_uart_open, disables UART interrupt, etc.
*             This function is called when the EAT application no longer uses the UART port.
* Parameters :
*     uart : EatUart_enum [IN], UART port.
* Returns:
*     EAT_TRUE if success, otherwise EAT_FALSE.
*****************************************************************************/
extern eat_bool (* const eat_uart_close)(EatUart_enum uart);

/*****************************************************************************
* Function :   eat_uart_set_config
* Description: The parameters are specified in a EatUartConfig_st structure type. 
*             The user must allocate memory for the configuration and the UART driver sets the parameters. 
*             The UART driver keeps a local copy of the configuration content.
* Parameters :
*     uart  : EatUart_enum [IN], UART port.
*     config : EatUartConfig_st *[IN], Pointer to UART configuration block.
* Returns:
*     EAT_TRUE if success, otherwise EAT_FALSE.
*****************************************************************************/
extern eat_bool (* const eat_uart_set_config)(EatUart_enum uart, const EatUartConfig_st *config);

/*****************************************************************************
* Function :   eat_uart_get_config
* Description: The parameters are specified in a EatUartConfig_st structure type. 
*             The user must allocate memory for the configuration, 
*             and the UART driver copies the content of the current configuration 
*             to the address to which the EatUartConfig_st* type parameter points.
* Parameters :
*     uart  : EatUart_enum [IN], UART port.
*     config : EatUartConfig_st *[OUT], Pointer to UART configuration block.
* Returns:
*     EAT_TRUE if success, otherwise EAT_FALSE.
*****************************************************************************/
extern eat_bool (* const eat_uart_get_config)(EatUart_enum uart, EatUartConfig_st *config);

/*****************************************************************************
* Function :   eat_uart_set_baudrate
* Description: Sets the baudrate configuration.
* Parameters :
*     uart  : EatUart_enum [IN], UART port.
*     baudrate : EatUartBaudrate [IN], New baudrate.
* Returns:
*     EAT_TRUE if success, otherwise EAT_FALSE.
*****************************************************************************/
extern eat_bool (* const eat_uart_set_baudrate)(EatUart_enum uart, EatUartBaudrate baudrate);

/*****************************************************************************
* Function :   eat_uart_get_baudrate
* Description: Gets the baudrate configuration.
* Parameters :
*     uart  : EatUart_enum [IN], UART port.
* Returns:
*     The actual baudrate.
*****************************************************************************/
extern EatUartBaudrate (* const eat_uart_get_baudrate)(EatUart_enum uart);

/*****************************************************************************
* Function :   eat_uart_write
* Description: Sends data to the UART.
* Parameters :
*     uart  : EatUart_enum [IN], UART port.
*     data  : unsigned char * [IN], Pointer to the data address.
*     len   : unsigned short [IN], Maximum length.
* Returns:
*     The actual length of the bytes transmitted. 
*     If it's small than parameter len, means that UART driver's receive buffer is full. 
*     UART driver will trigger EAT_EVENT_UART_READY_WR when receive buffer get some free space.
*****************************************************************************/
extern unsigned short (* const eat_uart_write)(EatUart_enum uart, const unsigned char *data, unsigned short len);

/*****************************************************************************
* Function :   eat_uart_read
* Description: Receives data from the UART.
* Parameters :
*     uart  : EatUart_enum [IN], UART port.
*     data  : unsigned char * [OUT], Pointer to the data address.
*     len   : unsigned short [IN], Maximum length.
* Returns:
*     The actual length of the bytes received. 
*     If it's small than parameter len, means that no more data transmitted. 
*     UART driver will trigger EAT_EVENT_UART_READY_RD when receives data.
*****************************************************************************/
extern unsigned short (* const eat_uart_read)(EatUart_enum uart, unsigned char *data, unsigned short len);

/*****************************************************************************
* Function :   eat_uart_set_debug
* Description: Sets the UART debug port.
* Parameters :
*     uart : EatUart_enum [IN], UART port.
* Returns:
*     EAT_TRUE if success, otherwise EAT_FALSE.
* NOTE:
*     This function must be called in function that specified by func_ext1 in structure EatEntry_st.
*****************************************************************************/
extern eat_bool (* const eat_uart_set_debug)(EatUart_enum uart);

/*****************************************************************************
* Function :   eat_uart_set_debug_config
* Description: Sets the UART debug mode.
* Parameters :
*     mode : EatUartDebugMode_enum [IN], UART debug mode.
*     cfg  : EatUartConfig_st* [IN] config parameter
* Returns:
*     EAT_TRUE if success, otherwise EAT_FALSE.
* NOTE:
*     This function must be called in function that specified by func_ext1 in structure EatEntry_st.
*     The debug mode is EAT_UART_DEBUG_MODE_TRACE default.
*****************************************************************************/
extern eat_bool (* const eat_uart_set_debug_config)(EatUartDebugMode_enum mode, EatUartConfig_st* cfg);

/*****************************************************************************
* Function :   eat_uart_set_at_port
* Description: Sets the SIMCOM Core AT port.
* Parameters :
*     uart : EatUart_enum [IN], UART port.
* Returns:
*     EAT_TRUE if success, otherwise EAT_FALSE.
* NOTE:
*     This function must be called in function that specified by func_ext1 in structure EatEntry_st.
*****************************************************************************/
extern eat_bool (* const eat_uart_set_at_port)(EatUart_enum uart);

/*****************************************************************************
* Function :   eat_uart_set_at_port_baud
* Description: Sets the SIMCOM Core AT port baud rate.
* Parameters :
*     uart : EatUartBaudrate [IN], UART port baud rate.
* Returns:
*     EAT_TRUE if success, otherwise EAT_FALSE.
* NOTE:
*     This function must be called in function that specified by func_ext1 in structure EatEntry_st.
*****************************************************************************/
extern eat_bool (* const eat_uart_set_at_port_baud)(EatUartBaudrate baud);

/*****************************************************************************
* Function :   eat_uart_set_send_complete_event
* Description: Enable/Disable the UART driver event EAT_EVENT_UART_SEND_COMPLETE.
* Parameters :
*     uart : EatUart_enum [IN], UART port.
*     send : eat_bool [IN], EAT_TRUE - Enalbe the event, EAT_FALSE - Disable the event.
* Returns:
*     EAT_TRUE if success, otherwise EAT_FALSE.
*****************************************************************************/
extern eat_bool (* const eat_uart_set_send_complete_event)(EatUart_enum uart, eat_bool send);

/*****************************************************************************
* Function :   eat_uart_get_send_complete_status
* Description: Check if the transmitting has been completed.
* Parameters :
*     uart : EatUart_enum [IN], UART port.
* Returns:
*     EAT_TRUE if success, otherwise EAT_FALSE.
*****************************************************************************/
extern eat_bool (* const eat_uart_get_send_complete_status)(EatUart_enum uart);

/*****************************************************************************
* Function :   eat_uart_get_free_space
* Description: Query the free space of UART transmitting buffer.
* Parameters :
*     uart : EatUart_enum [IN], UART port.
* Returns:
*     The actual free space of UART transmitting buffer.
*****************************************************************************/
extern unsigned short (* const eat_uart_get_free_space)(EatUart_enum uart);

/*****************************************************************************
* Function :   eat_data_mode_read
* Description: eat data mode read data.
* Parameters :
*     data  : unsigned char * [OUT], Pointer to the data address.
*     len   : unsigned short [IN], Maximum length.
* Returns:
*     The actual length of the bytes received. 
*     If it's small than parameter len, means that no more data transmitted. 
*****************************************************************************/
extern unsigned int (* const eat_data_mode_read)(unsigned char *data, unsigned int len);

#endif


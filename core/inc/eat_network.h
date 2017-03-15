#if !defined (__EAT_NETWORK_H__)
#define __EAT_NETWORK_H__

#include "eat_type.h"

typedef enum
{
   EAT_REG_STATE_NOT_REGISTERED=0, //Î´×¢²á
   EAT_REG_STATE_REGISTERED,          //ÒÑ×¢²á
   EAT_REG_STATE_SEARCHING,           //ËÑË÷ÖÐ
   EAT_REG_STATE_REG_DENIED,          //×¢²áÊ§°Ü
   EAT_REG_STATE_UNKNOWN,             //Î´Öª
   EAT_REG_STATE_REGISTERED_ROAMING //×¢²áµ½ÂþÓÎÍøÂç
} EatCregState_enum;

/*****************************************************************************
* Function   : eat_network_get_creg
* Description: get creg's value
* Parameters:  -
*  param1   
* Returns:     EatCregState_enum
* NOTE
*****************************************************************************/
extern EatCregState_enum (* const eat_network_get_creg)(void);

/*****************************************************************************
* Function   : eat_network_get_ber
* Description: get ber's value
* Parameters:  -
*  param1   
* Returns:     ber's value
* NOTE
*****************************************************************************/
extern s32 (* const eat_network_get_ber)(void);

/*****************************************************************************
* Function   : eat_network_get_csq
* Description: get csq's value
* Parameters:  -
*  param1   
* Returns:     csq's value
* NOTE
*****************************************************************************/
extern s32 (* const eat_network_get_csq)(void);

/*****************************************************************************
* Function   : eat_network_get_cgatt
* Description:
*   get the cgatt status 
* Parameters:
*  void
* Returns:
*  1 £º gprs attach success
*  0£º gprs attach fail
* NOTE
*****************************************************************************/
extern u8 (* const eat_network_get_cgatt)(void);


#endif

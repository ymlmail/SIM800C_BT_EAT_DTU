#if !defined (__EAT_MODEM_H__)
#define __EAT_MODEM_H__

#include "eat_type.h"

/*Modem Ring level*/
typedef struct {
  unsigned char level; /* 0 - low; 1- high */ 
} EatMdmRi_st;

/*****************************************************************************
* Function :    eat_modem_write
* Description: Send data to Core(Modem)
* Parameters :
*     data: unsigned char [IN], the pointer of sending buffer
*     len: unsigned short [IN], the length of sending data
* Returns:
*  unsigned short,the number of bytes actually written
* NOTE
*   1. If the return length is less than "len",means core receiving buffer is already full, 
*   until received EAT_EVENT_MDM_READY_WR signal can continue to send.
*   2. The Core receiving buffer is 5KB
*****************************************************************************/
extern unsigned short (* const eat_modem_write)(const unsigned char *data, unsigned short len);

/*****************************************************************************
* Function :    eat_modem_read
* Description: Read data from Core(Modem)
* Parameters :
*     data: unsigned char [OUT], the pointer of receiving buffer
*     len : unsigned short [IN], the maximum number of bytes to read
* Returns:
*  unsigned short,the number of bytes read
* NOTE
*  The Core sending buffer is 5KB
*****************************************************************************/
extern unsigned short (* const eat_modem_read)(unsigned char *data, unsigned short len);

/*****************************************************************************
* Function :   eat_modem_set_poweron_urc_dir
* Description: Set URC's(Unsolicited Result Code) destination when power on.
*              if don't set ,the default destination is EAT_USER_0.
* Parameters :
*     task_id: [IN], the task id, EatUser_enum type.
* Returns:
*  eat_bool
* Note
*  Must be set in EatEntry_st.func_ext1.
*****************************************************************************/
extern eat_bool (* const eat_modem_set_poweron_urc_dir)(unsigned char task_id);

#endif

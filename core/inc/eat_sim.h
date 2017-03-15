/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                               *
 *---------------------------------------------------------------------
 * FileName      :   eat_sim.h
 * version       :   0.10
 * Description   :   
 * Authors       :   Luyong  
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2013-01-11, luyong, create originally.
 *
 ********************************************************************/
#ifndef _EAT_SIM_H_
#define _EAT_SIM_H_
/********************************************************************
 * Include Files
 ********************************************************************/
#include "eat_type.h"
/********************************************************************
 * Macros
 ********************************************************************/

/********************************************************************
 * Types
 ********************************************************************/
typedef enum 
{
    EAT_SIM_STATE_OK = 0,
    EAT_SIM_STATE_PIN,
    EAT_SIM_STATE_PUK,
    EAT_SIM_STATE_OTHER
}EAT_SIM_STATE;


/*****************************************************************************
* Function :  eat_cpin_query
* Description: Get CPIN state.
* Parameters:
*     No parameters (void)
* Returns:
*      Please refer to EAT_SIM_STATE
* NOTE:
*     Nothing
*****************************************************************************/
extern EAT_SIM_STATE (*const eat_cpin_query)(void);

/*****************************************************************************
* Function :  eat_sim_pin_callback_func
* Description: The callback function of the eat_cpin_set.
* Parameters:
*     en  eat_bool [IN] The CPIN set correct or error.
* Returns:
*     No returen value (void)
* NOTE:
*     Nothing
*****************************************************************************/
typedef void (*eat_sim_pin_callback_func)(eat_bool en);

/*****************************************************************************
* Function :  eat_cpin_set
* Description: Set CPIN function.
* Parameters:
*     pinptr    u8    [IN] The pointer of the PIN code .
*     pukptr    u8   [IN] The pointer of the PUK code.
*     callback    eat_sim_pin_callback_func    [IN] The callback function,if not NULL,can
*                           get the result of set cpin.
* Returns:
*     EAT_TRUE  The operation Success.
*     EAT_FALSE The operation False.
* NOTE:
*     If the state sim pin does not need PUK code,the pukptr could be NULL.
*****************************************************************************/
extern eat_bool (*const eat_cpin_set)(u8* pinptr,u8*pukptr,eat_sim_pin_callback_func callback);

#endif


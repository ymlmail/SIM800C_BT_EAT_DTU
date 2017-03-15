#if !defined (__EAT_MEM_H__)
#define __EAT_MEM_H__

#include "eat_type.h"

/*****************************************************************************
* Function   : eat_mem_init
* Description:
*   Initialize a piece of memory space.Then use function eat_mem_alloc() and 
*   eat_mem_free() to malloc and free memory space. 
* Parameters:
*  param1  void *mem_addr [IN]:  starting memory address to be initialized 
*  param2  unsigned int size [IN] : memory size 
* Returns:
*  EAT_TRUE £º Memory initialization success
*  EAT_FALSE£º Memory initialization failed. Check whether initialized memory 
*              space is too large, more than residual memory size 
* NOTE
*   Pay special attention please, this function  can only be called once. If 
*   return value success, do not call it again,otherwise, the previous initialization
*   space will be tied up at the same time can not be used
*****************************************************************************/
extern eat_bool (* const eat_mem_init)(void *mem_addr, unsigned int size);
#ifdef __SIMCOM_EAT_MULTI_APP__
extern eat_bool (* const eat_mem_init_ext)(void *mem_addr, unsigned int size);
#endif

/*****************************************************************************
* Function : eat_mem_alloc
* Description:
*   Allocate memory space 
* Parameters:
*  param1 unsigned int size [IN]:    memory space size to allocate (byte unit)
* Returns:
*  void * £ºStarting address of the memory space.
*           Failed if the return value equals EAT_NULL
* NOTE
*      call this function after eat_mem_init()
*****************************************************************************/
extern void * (* const eat_mem_alloc)(unsigned int size);
#ifdef __SIMCOM_EAT_MULTI_APP__
extern void * (* const eat_mem_alloc_ext)(unsigned int size);
#endif

/*****************************************************************************
* Function :eat_mem_free
* Description:
*   free memory space
* Parameters:
*  param1 void *mem_addr[IN]: the starting address of the memory space to be released
* Returns:
*  void * £ºEAT_NULL  always this value, ignore it
* NOTE
*      call this function after eat_mem_alloc()
*****************************************************************************/
extern void * (* const eat_mem_free)(void *mem_addr);
#ifdef __SIMCOM_EAT_MULTI_APP__
extern void * (* const eat_mem_free_ext)(void *mem_addr);
#endif

#endif

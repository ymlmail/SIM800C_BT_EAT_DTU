#if !defined (__EAT_CLIB_DEFINE_H__)
#define __EAT_CLIB_DEFINE_H__

#include <rt_misc.h>


#define APP_init_clib() __rt_lib_init(0xF03F8000,0xF0400000)
/* C lib function begin*/
__value_in_regs struct __initial_stackheap $Sub$$__user_initial_stackheap(unsigned R0, unsigned SP, unsigned R2, unsigned SL)
{
   struct __initial_stackheap config;
 
   config.heap_base = R0;
   config.heap_limit =  R2;
   config.stack_base =  SP; 
   config.stack_limit =   SL;
   return config;
}
void $Sub$$_fp_init(void)
{
}
void $Sub$$_initio(void)
{
}
__value_in_regs struct __argc_argv $Sub$$__ARM_get_argv(void *a )
{
    /*
    int argc;
    char **argv;
    int r2, r3;
    */
    struct __argc_argv arg;
    arg.argc = 0;
    arg.r2 = 0;
    arg.r3 = 0;
    return arg;
}
/* C lib function end*/


#endif //__EAT_CLIB_DEFINE_H__

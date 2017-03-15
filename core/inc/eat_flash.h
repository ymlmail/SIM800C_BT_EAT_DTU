#if !defined (__EAT_FLASH_H__)
#define __EAT_FLASH_H__

#include "eat_type.h"
#include "eat_periphery.h"


/*****************************************************************************
 * Function :eat_flash_erase
 * Description:This function is used to erase the flash.
 * Parameters :
 * 	address: [IN] the start flash address of erase.
 *	size: [IN]  the size of erase
 * Returns:
 *  	TRUE or FALSE  	
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern eat_bool (* const eat_flash_erase)(const void *address, unsigned int size);

/*****************************************************************************
 * Function :eat_flash_write
 * Description:This function is used to write data to flash.
 * Parameters :
 * 	address: [IN] the start flash address for write.
 *	data: [IN]  Specifies the address of the data to be written.
 *	len: [IN]  Specifies the number of bytes to write.
 * Returns:
 *  	TRUE or FALSE  	
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern eat_bool (* const eat_flash_write)(const void *address, const void *data, unsigned int len);

/*****************************************************************************
 * Function :eat_flash_read
 * Description:This function is used to read data from flash.
 * Parameters :
 * 	buffer: [IN] Destination address.
 *	address: [IN]  Source address
 *	len: [IN]  Specifies the number of bytes to read.
 * Returns:
 *  	buffer address.
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern void *(* const eat_flash_read)(void *buffer, const void *address, unsigned int len);

/*****************************************************************************
 * Function :eat_get_flash_block_size
 * Description:This function is used to get flash block size.
 * Parameters :
 * Returns:
 *  	flash block size	
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern unsigned int (* const eat_get_flash_block_size)(void);

/*****************************************************************************
 * Function :eat_update_app
 * Description:This function is used to update APP code.
 * Parameters :
 * 	app_code_addr: [IN] the executive address of App code.
 *	app_code_new_addr: [IN]  the store address of App new code.
 *	len: [IN]  the length of App new code.
 * 	pin_wd :   the pin used for given extenal WDT a singnal when update app;EAT_PIN_NUM:no use
 * 	pin_led:   the pin used for control extenal LED light when update app;EAT_PIN_NUM:no use
 * 	lcd_bl :   bool type,contrl lcd back light whether shining when update app;EAT_TRUE:yes EAT_FALSE:no
 * Returns:
 * 	Nothing	
 * NOTE
 *     The cycle of pin_wd,pin_led and lcd backlight is about 400 ms.
 *****************************************************************************/
extern void (* const eat_update_app)(const void *app_code_addr, const void *app_code_new_addr, unsigned int len, EatPinName_enum pin_wd, EatPinName_enum pin_led, eat_bool lcd_bl);

/*****************************************************************************
 * Function :eat_update_app_ok
 * Description:This function is used to clear update APP flag.
 * Parameters :
 * Returns:
 * 	Nothing	
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern void (* const eat_update_app_ok)(void);

/*****************************************************************************
 * Function :eat_get_app_base_addr
 * Description:This function is used to get app base address.
 * Parameters :
 * Returns:
 * 	Nothing	
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern unsigned long (* const eat_get_app_base_addr)(void) ;

/*****************************************************************************
 * Function :eat_get_app_space
 * Description:This function is used to get app space value.
 * Parameters :
 * Returns:
 * 	Nothing	
 * NOTE
 *     Nothing 
 *****************************************************************************/
extern unsigned long (* const eat_get_app_space)(void) ;

#ifdef __SIMCOM_EAT_MULTI_APP__
/*****************************************************************************
 * Function :eat_flash_erase_ext
 * Description:This function is used to erase the flash.
 * Parameters :
 * 	address: [IN] the start flash offset address of erase.
 *	size: [IN]  the size of erase
 * Returns:
 *  	TRUE or FALSE  	
 * NOTE
 *    1. The address is offset address 
 *    2. The (address+len) must be less than the value of eat_get_app_space_ext
 *****************************************************************************/
extern eat_bool (*const eat_flash_erase_ext)(const void *address, unsigned int size);
/*****************************************************************************
 * Function :eat_flash_write_ext
 * Description:This function is used to write data to flash.
 * Parameters :
 * 	address: [IN] the start flash offset address for writing.
 *	data: [IN]  Specifies the address of the data to be written.
 *	len: [IN]  Specifies the number of bytes to write.
 * Returns:
 *  	TRUE or FALSE  	
 * NOTE
 *    1. The address is offset address 
 *    2. The (address+len) must be less than the value of eat_get_app_space_ext
 *****************************************************************************/
extern eat_bool (*const eat_flash_write_ext)(const void *address, const void *data, unsigned int len);
/*****************************************************************************
 * Function :eat_flash_read_ext
 * Description:This function is used to read data from flash.
 * Parameters :
 * 	address: [IN] the start flash offset address for reading.
 *	data: [IN]  Specifies the space of the data reading from flash.
 *	len: [IN]  Specifies the number of bytes to read.
 * Returns:
 *  	TRUE or FALSE  	
 * NOTE
 *    1. The address is offset address 
 *    2. The (address+len) must be less than the value of eat_get_app_space_ext
 *****************************************************************************/
extern eat_bool (*const eat_flash_read_ext)(const void *address, void *data, unsigned int len);
/*****************************************************************************
 * Function :eat_get_app_space
 * Description:This function is used to get app space value.
 * Parameters :
 * Returns:
 *   Nothing 
 * NOTE
 *     The space is only first ROM space in multi-app version.
 *****************************************************************************/
extern unsigned long (*const eat_get_app_space_ext)(void);
#endif //#ifdef __SIMCOM_EAT_MULTI_APP__

#endif

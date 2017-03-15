/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   main_customer.c
 * version       :   0.10
 * Description   :   
 * Authors       :   maobin
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2012-09-24, maobin, create originally.
 *
 *--------------------------------------------------------------------
 * File Description
 * AT+CEAT=parma1,param2
 * param1 param2 
 *   1      1    Update new app from fs. 
 *   1      2    Write data to flash.
 *   1      3    Update to none app.
 *   1      4    Erase the flash that is designated address.
 *
 *--------------------------------------------------------------------
 ********************************************************************/
/********************************************************************
 * Include Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>

#include "eat_modem.h"
#include "eat_interface.h"
#include "eat_flash.h" 
#include "eat_clib_define.h" //only in main.c
/********************************************************************
* Macros
 ********************************************************************/
#define APP_UPDATE_BUFF_SIZE 0x1000
//#define APP_UPDATE_BUFF_SIZE 8192
#define DYNAMIC_MEM_SIZE 1024*200
#define APP_UPDATE_FILE L"C:\\update.bin"
//#define EAT_DEBUG_STRING_INFO_MODE

/********************************************************************
* Types
 ********************************************************************/
typedef void (*app_user_func)(void*);
/********************************************************************
* Extern Variables (Extern /Global)
 ********************************************************************/

/********************************************************************
* Local Variables:  STATIC
 ********************************************************************/
u32 APP_DATA_RUN_BASE  = 0;
u32 APP_DATA_STORAGE_BASE = 0;
static unsigned char app_dynic_mem_test[DYNAMIC_MEM_SIZE];
static char block_temp[0x4000];

static EatEntryPara_st app_para;
const unsigned char app_new_data[] = {
    #if 0
    #include "app.h" //new app
    #else //no app
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF
    #endif
};

/********************************************************************
* External Functions declaration
 ********************************************************************/
extern void APP_InitRegions(void);

/********************************************************************
* Local Function declaration
 ********************************************************************/
void app_main(void *data);
void app_func_ext1(void *data);
/********************************************************************
* Local Function
 ********************************************************************/
#pragma arm section rodata = "APP_CFG"
APP_ENTRY_FLAG 
#pragma arm section rodata

#pragma arm section rodata="APPENTRY"
	const EatEntry_st AppEntry = 
	{
		app_main,
		app_func_ext1,
		(app_user_func)EAT_NULL,//app_user1,
		(app_user_func)EAT_NULL,//app_user2,
		(app_user_func)EAT_NULL,//app_user3,
		(app_user_func)EAT_NULL,//app_user4,
		(app_user_func)EAT_NULL,//app_user5,
		(app_user_func)EAT_NULL,//app_user6,
		(app_user_func)EAT_NULL,//app_user7,
		(app_user_func)EAT_NULL,//app_user8,
		EAT_NULL,
		EAT_NULL,
		EAT_NULL,
		EAT_NULL,
		EAT_NULL,
		EAT_NULL
	};
#pragma arm section rodata

void app_func_ext1(void *data)
{
	/*This function can be called before Task running ,configure the GPIO,uart and etc.
	   Only these api can be used:
		 eat_uart_set_debug: set debug port
		 eat_pin_set_mode: set GPIO mode
		 eat_uart_set_at_port: set AT port
	*/
#ifdef EAT_DEBUG_STRING_INFO_MODE
    EatUartConfig_st cfg =
    {
        EAT_UART_BAUD_115200,
        EAT_UART_DATA_BITS_8,
        EAT_UART_STOP_BITS_1,
        EAT_UART_PARITY_NONE
    };
#endif
	eat_uart_set_at_port(EAT_UART_1);
	eat_uart_set_debug(EAT_UART_USB);
	
	eat_uart_set_debug_config(EAT_UART_DEBUG_MODE_UART, NULL);
	//配置USB口做调试口，需要打印ASCII时需要把上面这句也配置下
	//set debug string info to debug port
#ifdef EAT_DEBUG_STRING_INFO_MODE
    eat_uart_set_debug_config(EAT_UART_DEBUG_MODE_UART, &cfg);
#endif
}
eat_bool eat_modem_data_parse(u8* buffer, u16 len, u8* param1, u8* param2)
{
    eat_bool ret_val = EAT_FALSE;
    u8* buf_ptr = NULL;
    /*param:%d,extern_param:%d*/
     buf_ptr = (u8*)strstr((const char *)buffer,"param");
    if( buf_ptr != NULL)
    {
        sscanf((const char *)buf_ptr, "param:%d,extern_param:%d",(int*)param1, (int*)param2);
        eat_trace("data parse param1:%d param2:%d",*param1, *param2);
        ret_val = EAT_TRUE;
    }
    return ret_val;
}

/****************************************************
 * Update app testing module
 *****************************************************/
void update_noapp_test_start(void)
{
	eat_bool ret = EAT_FALSE;
	unsigned char *addr;
	unsigned int t1, t_erase, t_write;
	int app_datalen = sizeof(app_new_data);

	addr = (unsigned char *)(APP_DATA_STORAGE_BASE);
	eat_trace("run addr=%x storage addr=%x",APP_DATA_RUN_BASE,APP_DATA_STORAGE_BASE);
	t1 = eat_get_current_time();
	ret = eat_flash_erase(addr, app_datalen);
	t_erase = eat_get_duration_ms(t1);
	if(!ret)
	{
		eat_trace("Erase flash failed [0x%08x, %dKByte]", APP_DATA_STORAGE_BASE, app_datalen/1024);
		return;
	}
	eat_trace("START: write flash[0x%x, %dKByte]", APP_DATA_STORAGE_BASE, app_datalen/1024);
	t1 = eat_get_current_time();
	ret = eat_flash_write(addr, app_new_data, app_datalen);
	t_write = eat_get_duration_ms(t1);
	if(!ret)
	{
		eat_trace("Write flash failed [0x%08x, %dKByte]", APP_DATA_STORAGE_BASE, app_datalen/1024);
		return;
	}

	eat_trace("All use write[%d, %d]", t_erase, t_write);

	eat_update_app((void*)(APP_DATA_RUN_BASE), (void*)(APP_DATA_STORAGE_BASE), app_datalen, EAT_PIN_NUM, EAT_PIN_NUM, EAT_FALSE);

	eat_trace("Test App Over");
        
}
/*update from file in filesystem*/
void update_app_test_start(void)
{
	eat_bool ret = EAT_FALSE;
    void* buff_p = NULL;
	unsigned char *addr;
	unsigned int t1,t2, t_erase=0, t_write=0, c_write=0, read_count=0;
	unsigned int app_datalen = APP_UPDATE_BUFF_SIZE ;
    unsigned int filesize, read_len;
    int testFileHandle ;
    eat_fs_error_enum fs_op_ret;

    addr = (unsigned char *)(APP_DATA_STORAGE_BASE);

	testFileHandle = eat_fs_Open(APP_UPDATE_FILE, FS_READ_ONLY);
	if(testFileHandle<EAT_FS_NO_ERROR )
	{
        eat_trace("eat_fs_Open():Create File Fail,and Return Error is %x ",testFileHandle);
        return ;
	}
	else
	{
        eat_trace("eat_fs_Open():Create File Success,and FileHandle is %x ",testFileHandle);
    }
	fs_op_ret = (eat_fs_error_enum)eat_fs_GetFileSize(testFileHandle,&filesize);
	if(EAT_FS_NO_ERROR != fs_op_ret)
	{
		eat_trace("eat_fs_GetFileSize():Get File Size Fail,and Return Error is %d",fs_op_ret);
		eat_fs_Close(testFileHandle);
		return;
	}
	else
	{
        eat_trace("eat_fs_GetFileSize():Get File Size Success and File Size id %d",filesize);
    }

    eat_trace("erase flash addr=%x len=%x", APP_DATA_STORAGE_BASE,  filesize); 
    t1 = eat_get_current_time();
	ret = eat_flash_erase(addr, filesize);
	t_erase = eat_get_duration_ms(t1);
	if(!ret)
	{
		eat_fs_Close(testFileHandle);
		eat_trace("Erase flash failed [0x%08x, %dKByte]", APP_DATA_STORAGE_BASE,  filesize/1024);
		return;
	}
    read_count = filesize/APP_UPDATE_BUFF_SIZE; //only for testing,so don't case the completeness of file
    eat_trace("need to read file %d",read_count);
    if( read_count == 0)
    {
        //only read once
        read_count=1;
        read_len = filesize;
    }else
    {
        read_count++;
        read_len = APP_UPDATE_BUFF_SIZE;
    }
    buff_p = eat_mem_alloc(app_datalen);
    if( buff_p == NULL)
    {
        eat_trace("mem alloc fail!");
		eat_fs_Close(testFileHandle);
        return ;
    }
    filesize = 0;
    while(read_count--)
    {
        fs_op_ret = (eat_fs_error_enum)eat_fs_Read(testFileHandle,buff_p, read_len ,&app_datalen);
        if(EAT_FS_NO_ERROR != fs_op_ret )
        {	
            eat_trace("eat_fs_Read():Read File Fail,and Return Error is %d,Readlen is %d",fs_op_ret,app_datalen);
            eat_fs_Close(testFileHandle);
            eat_mem_free(buff_p);
            return;
        }
        else
        {
            //eat_trace("eat_fs_Read():Read File Success");
        }

        //eat_trace("START: write flash[0x%x, %dKByte]", APP_DATA_STORAGE_BASE, app_datalen/1024);
        t1 = eat_get_current_time();
        ret = eat_flash_write(addr+filesize , buff_p, app_datalen);
        t2 = eat_get_duration_ms(t1);
        filesize += app_datalen;
        t_write += t2; 
        c_write ++;
        eat_trace("write flash time=%d",t2);
        if(!ret)
        {
            eat_trace("Write flash failed [0x%08x, %dKByte]", APP_DATA_STORAGE_BASE, app_datalen/1024);
            eat_fs_Close(testFileHandle);
            eat_mem_free(buff_p);
            return;
        }
    }
    eat_fs_Close(testFileHandle);
    eat_mem_free(buff_p);

	eat_trace("All use %d write[%d, %d]", c_write, t_erase, t_write);
    eat_sleep(50); // for output the debug info
	eat_update_app((void*)(APP_DATA_RUN_BASE), (void*)(APP_DATA_STORAGE_BASE), filesize , EAT_PIN_NUM, EAT_PIN_NUM, EAT_FALSE);

	eat_trace("Test App Over");
        
}

/*save data testing*/
void app_test_single_block(void)
{
    eat_bool ret = EAT_FALSE;
    static char count = 0;
    char temp[24] = {0};
    char buf[24] = {0};
    char* addr = (char*)(eat_get_app_base_addr()+ eat_get_app_space() - 0x1000);
    ret = eat_flash_erase(addr, 0x30);
    memcpy(temp,"TESTID=123456789abc12", 27);
    temp[7] = '0' + count++;
    
    eat_trace("Flash erase addr=%x, len=%x ret=%d",addr, 0x30, ret);
    if( ret )
    {
        ret = eat_flash_write(addr, temp, strlen(temp));
        eat_trace("Flash write addr=%x, len=%d",addr, strlen(temp));
    }else
    {
        return ;
    }
    memcpy(buf, addr, strlen(temp));
    eat_trace("get str:%s",buf);
}

void app_test_erase_flash(u32 address, u32 length)
{
    eat_bool ret = EAT_FALSE;
    char* addr = (char*)(address);
    char* end_addr = (char*)(address + length);
    char* tmp_addr = 0;
    int len = length;
    unsigned int BLOCK_SIZE = eat_get_flash_block_size();
    int counter = 0;
    int i = 0;

    eat_trace("erase_flash start addr=%x len=%x block=%x", addr, len, BLOCK_SIZE );
    for(tmp_addr=addr;tmp_addr<end_addr;)//从开始地址到结束地址
    {
        ret = eat_flash_erase(tmp_addr, BLOCK_SIZE);//一个block一个擦除，擦除成功返回1
        if(ret != EAT_TRUE)
        {
            eat_trace("Erase fail!!! addr=%x",tmp_addr);
        }
        if(counter%10 == 0)//每10个输出一个地址信息和block的个数
        {
            eat_trace("erase addr=%x c=%d",tmp_addr,counter);
        }
        tmp_addr+=BLOCK_SIZE;	//地址增加一个block
        counter++;
        eat_sleep(20);			//睡眠20ms，目的应该是等待擦除完成
    }
    eat_trace("erase_flash erase sucess!");
    counter = 0;
    for(tmp_addr=addr;tmp_addr<end_addr;  )
    { 
        if( NULL == eat_flash_read(block_temp, tmp_addr, BLOCK_SIZE))//读取一个block的数据到block_temp数组中
        {
            eat_trace("read addr=%x size=%x ERROR!", tmp_addr, BLOCK_SIZE);
			break;
        }
        for(i=0;i<BLOCK_SIZE;i++)//逐个字节进行校验，如果都为0xff，则表示擦除成功。
        {
            if(block_temp[i]!=0xff)
            {
                eat_trace("read addr=%x offset=%d ERROR",tmp_addr,i);
                break;
            }
        }
        if(counter%10 == 0)
        {
            eat_trace("read addr=%x c=%d",tmp_addr,counter);
        }
        tmp_addr += BLOCK_SIZE;
        counter++;
        eat_sleep(20);
    }
    eat_trace("erase_flash check end");
}


eat_bool eat_module_test_update_app(u8 param1, u8 param2)
{
    /***************************************
     * example 1
     * Start Update app
     ***************************************/
    if( 1 == param1 )
    {

        if( 1 == param2 )
        {
        	update_app_test_start();
        }else if( 2 == param2)
        {
            app_test_single_block();
        }else if(3 == param2)
        {
            update_noapp_test_start();
        }else if(4 == param2)
        { 
            app_test_erase_flash(0x1030b000, 0x80000);
        }
    }
    return EAT_TRUE;
}
u8 buf[2048];
void app_main(void *data)
{
    EatEvent_st event;
    u16 len = 0;
    eat_bool mem_ini_flag=EAT_FALSE;
    EatEntryPara_st *para;
    u32 app_space_value = 0;

    APP_InitRegions();//Init app RAM, first step
    APP_init_clib();  //C library initialize, second step
    
    para = (EatEntryPara_st*)data;

    memcpy(&app_para, para, sizeof(EatEntryPara_st));
    eat_trace(" App Main ENTRY  update:%d result:%d", app_para.is_update_app,app_para.update_app_result);
    if(app_para.is_update_app && app_para.update_app_result)
    {
        eat_update_app_ok();
    }
    APP_DATA_RUN_BASE = eat_get_app_base_addr();
    app_space_value = eat_get_app_space();
    APP_DATA_STORAGE_BASE = APP_DATA_RUN_BASE + (app_space_value>>1);
	
    eat_trace("app base addr=%x size=%x",APP_DATA_RUN_BASE,app_space_value);
    mem_ini_flag = eat_mem_init(app_dynic_mem_test, sizeof(app_dynic_mem_test));
    if(EAT_TRUE == mem_ini_flag)
    {
        eat_trace("eat_mem_init() Pass\n"); 
    }else
    {
        eat_trace("eat_mem_init() Fail\n");
    }   
    eat_trace(" app_main ENTRY");
    while(EAT_TRUE)
    {
        eat_get_event(&event);
        eat_trace("MSG id%x", event.event);
        switch(event.event)
        {
            case EAT_EVENT_MDM_READY_RD:
                {
                    u8 param1,param2;
                    len = 0;
                    len = eat_modem_read(buf, 2048);
                    if(len > 0)
                    {
                        //Get the testing parameter
                        if(eat_modem_data_parse(buf,len,&param1,&param2))
                        {
                            //Entry Flash test module
                            eat_module_test_update_app(param1, param2);
                        }
                        else
                        {
                            eat_trace("From Mdm:%s",buf);
                        }
                    }

                }
                break;
            case EAT_EVENT_MDM_READY_WR:
            case EAT_EVENT_UART_READY_RD:
                break;
            case EAT_EVENT_UART_SEND_COMPLETE :
                break;
            default:
                break;
        }

    }

}



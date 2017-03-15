/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   app_utility.c
 * version       :   0.10
 * Description   :   
 * Authors       :   maobin
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2014-01-17, maobin, create originally.
 *
 ********************************************************************/
/********************************************************************
 * Include Files
 ********************************************************************/
#include "platform.h" 
#include "app_utility.h"
/********************************************************************
* Macros
 ********************************************************************/
 
/********************************************************************
* Types
 ********************************************************************/
 
/********************************************************************
* Extern Variables (Extern /Global)
 ********************************************************************/
 
/********************************************************************
* Local Variables:  STATIC
 ********************************************************************/
 
/********************************************************************
* External Functions declaration
 ********************************************************************/
 
/********************************************************************
* Local Function declaration
 ********************************************************************/
 
/********************************************************************
* Local Function
 ********************************************************************/
 s32 app_nvram_read(u8 type, void* ptr, u16 len)
 {
     char filename[64]={0};
     u32 i, file_len,  readed;
     int op_ret;
     int fs_handle = 0;
 
     if(len == 0 || ptr == NULL )
         return -1;
     
     sprintf(filename,"%d",type);
     DBG("FS filename = %s",filename);
     
     fs_handle = app_fs_open(filename, FS_READ_ONLY);
     if( fs_handle < EAT_FS_NO_ERROR)
     {
         DBG_E("FS open fail!!!!! error=%d !!!",fs_handle);
         return APP_NVRAM_UNKNOW_ERR;
     }
 
     op_ret = app_fs_get_file_size(fs_handle, &file_len);
     if( op_ret < EAT_FS_NO_ERROR ||file_len<1 )
     {
         DBG_E("FS get file len fail!!!!! error=%x !!!",fs_handle);
         app_fs_close(fs_handle);
         return APP_NVRAM_UNKNOW_ERR;
     }
     
     if( file_len > len)
     {
         file_len = len;
     }
     
     op_ret = app_fs_read(fs_handle, ptr, file_len, &readed);
     if(EAT_FS_NO_ERROR != op_ret ||file_len !=readed)
     {
         DBG_E("read file Fail!!! error=%d readed=%d", op_ret, readed);
         app_fs_close(fs_handle);
         return APP_NVRAM_READ_ERR;
     }
 
     app_fs_close(fs_handle);
 
     return readed;
 
}
 /********************************************************************
 * Function    :    app_nvram_save
 * Autor       :    maobin
 * Parameters  :    
 *              type - param type
 *              ptr  -  data point
 *              length - data len
 * Returns     :    
 * Description :    
 ********************************************************************/
 s32 app_nvram_save(u8 type, void* ptr, u16 length)
 {
     char filename[64]={0};
     int i, writed, op_ret;
     int fs_handle = 0;
     if(length == 0 || ptr == NULL )
     {
         DBG_E("param len=%d ptr=%x, type=%d",length,ptr,type);
         return APP_NVRAM_PARAM_ERR;
     }
 
     sprintf(filename,"%d",type);
     DBG("FS filename = %s",filename);
     
     fs_handle = app_fs_open(filename, FS_READ_WRITE);
     if( fs_handle == EAT_FS_FILE_NOT_FOUND)
     {
         fs_handle = app_fs_open(filename, FS_CREATE | FS_READ_WRITE);
         if( fs_handle< EAT_FS_NO_ERROR)
         {
             DBG_E("save FS open fail!!!!! error=%d !!!",fs_handle);
             return APP_NVRAM_UNKNOW_ERR;
         }
     }else if(fs_handle<EAT_FS_NO_ERROR)
     {
         DBG_E("save2 FS open fail!!!!! error=%d !!!",fs_handle);
         return APP_NVRAM_UNKNOW_ERR;
     }
 
     //if(mode)
     //  app_fs_seek(fs_handle, 0, EAT_FS_FILE_END);
     //else
     app_fs_seek(fs_handle, 0, EAT_FS_FILE_BEGIN);
 
     op_ret = app_fs_write(fs_handle, ptr, length, &writed);
     if(EAT_FS_NO_ERROR != op_ret ||length !=writed )
     {
         DBG_E("FS write fail!!!!! error=%d,len=%x,writed=%x!!!",fs_handle,length,writed);
         app_fs_close(fs_handle);
         return APP_NVRAM_WRITE_ERR;
     }
     app_fs_commit(fs_handle);
     app_fs_close(fs_handle);
     return writed;
 
 }
 
 s32 app_fs_open(const u8 * filename, u32 Flag)
 {
     int i;
     u8 filename_l[128] = {0};
     for(i=0;i<strlen(filename);i++)//转换成Unicode 64*2
     {
          filename_l[i*2] = filename[i];
          filename_l[i*2+1] = 0x00;
     }
 
     return eat_fs_Open((u16*)filename_l, Flag);
 }
 
 int app_fs_close(s32 FileHandle)
 {
     return eat_fs_Close(FileHandle);
 }
 int app_fs_read(s32 FileHandle, void * DataPtr, u32 Length, u32* Read)
 {
     return eat_fs_Read(FileHandle, DataPtr, Length, Read);
 }
 int app_fs_write(s32 FileHandle, void * DataPtr, u32 Length, u32 * Written)
 {
     return eat_fs_Write(FileHandle, DataPtr, Length, Written);
 }
 int app_fs_seek(s32 FileHandle, int Offset, int Whence)
 {
     return eat_fs_Seek(FileHandle, Offset, Whence);
 }
 int app_fs_commit(s32 FileHandle)
 {
     return eat_fs_Commit(FileHandle);
 }
 int app_fs_abort(s32 ActionHandle)
 {
     //return eat_fs_Abort(ActionHandle);
     return 0;
 }
 int app_fs_get_file_size(s32 FileHandle, u32 * Size)
 {
     return eat_fs_GetFileSize(FileHandle, Size);
 }
 
 int  app_fs_del(const u8* filename)
 {
     int i;
     u16 filename_l[50] = {0};
     for(i=0;i<strlen(filename);i++)
     {
          filename_l[i*2] = filename[i];
          filename_l[i*2+1] = 0x00;
     }
 
     return eat_fs_Delete(filename_l);
 }
 int app_fs_create_dir(const u8* DirName)
 {
     int i;
     u16 filename_l[50] = {0};
     for(i=0;i<strlen(DirName);i++)
     {
          filename_l[i*2] = DirName[i];
          filename_l[i*2+1] = 0x00;
     }
 
     return eat_fs_CreateDir(filename_l);
 }

#define APP_UPDATE_BUFF_SIZE 0x1000

 /*update from file in filesystem*/
void app_update(char* filename)
{
    eat_bool ret = EAT_FALSE;
    void* buff_p = NULL;
    unsigned char *addr;
    unsigned int t1,t2, t_erase=0, t_write=0, c_write=0, read_count=0;
    unsigned int app_datalen = APP_UPDATE_BUFF_SIZE ;
    unsigned int filesize, read_len;
    int testFileHandle ;
    eat_fs_error_enum fs_op_ret;

    addr =  (unsigned char *)(eat_get_app_base_addr() + (eat_get_app_space()>>1));

    testFileHandle = app_fs_open(filename, FS_READ_ONLY);
    if(testFileHandle<EAT_FS_NO_ERROR )
    {
        eat_trace("eat_fs_Open():Create File Fail,and Return Error is %x ",testFileHandle);
        return ;
    }
    else
    {
        eat_trace("eat_fs_Open():Create File Success,and FileHandle is %x ",testFileHandle);
    }
    fs_op_ret = (eat_fs_error_enum)app_fs_get_file_size(testFileHandle,&filesize);
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

    eat_trace("erase flash addr=%x len=%x", addr,  filesize); 
    t1 = eat_get_current_time();
    ret = eat_flash_erase(addr, filesize);
    t_erase = eat_get_duration_ms(t1);
    if(!ret)
    {
        eat_fs_Close(testFileHandle);
        eat_trace("Erase flash failed [0x%08x, %dKByte]", addr,  filesize/1024);
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
        app_fs_close(testFileHandle);
        return ;
    }
    filesize = 0;
    while(read_count--)
    {
        fs_op_ret = (eat_fs_error_enum)app_fs_read(testFileHandle,buff_p, read_len ,&app_datalen);
        if(EAT_FS_NO_ERROR != fs_op_ret )
        {   
            eat_trace("eat_fs_Read():Read File Fail,and Return Error is %d,Readlen is %d",fs_op_ret,app_datalen);
            app_fs_close(testFileHandle);
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
            eat_trace("Write flash failed [0x%08x, %dKByte]", addr, app_datalen/1024);
            app_fs_close(testFileHandle);
            eat_mem_free(buff_p);
            return;
        }
    }
    app_fs_close(testFileHandle);
    eat_mem_free(buff_p);

    eat_trace("All use %d write[%d, %d]", c_write, t_erase, t_write);
    eat_sleep(50);
    eat_update_app((void*)(eat_get_app_base_addr()), addr, filesize , EAT_PIN_NUM, EAT_PIN_NUM, EAT_FALSE);

    eat_trace("Test App Over");

}


void upperstr(char*s, char*d)
{
    u16 len = strlen(s);
    u16 i;
    for (i = 0; i < len; i++)
    {
        if (isalpha((int)s[i]))//判断是否为英文字母，如果大写则返回1，如果小写则返回2，如果不是字母则返回0
        {
            d[i] = toupper((int)s[i]);//如果不是大写则变为大写字母
        }
    }
}

s32 bytepos(const u8* pSrc, u16 nSrc,
              const char* pSub, u16 startPos)
{
    u16 sublen  = strlen(pSub);
    s32 _return = -1;
    u16 index;
    s32  cmp_return;

    if (sublen > nSrc)
    {
        return _return;
    }

    for (index = startPos; index <= (nSrc - sublen); index++)
    {
        cmp_return = memcmp(&pSrc[index], pSub, sublen);

        if (cmp_return == 0)
        {
            _return = index;
            return _return;
        }
    }

    return _return;
}



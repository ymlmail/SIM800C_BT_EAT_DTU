/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   app_demo_fs.c
 * version       :   0.10
 * Description   :   
 * Authors       :   luyong
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2012-09-26, luyong, create originally.
 *
 *--------------------------------------------------------------------
 * File Description
 * AT+CEAT=parma1,param2
 * param1 param2 
 *   1      1    eat_fs_interface_test1
 *   1      2    eat_fs_interface_test2
 * *
 *--------------------------------------------------------------------
 ********************************************************************/
/********************************************************************
 * Include Files
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eat_modem.h"
#include "eat_interface.h"
#include "eat_uart.h"
#include "eat_fs_errcode.h" 
#include "eat_fs_type.h" 
#include "eat_fs.h"
/********************************************************************
* Macros
 ********************************************************************/
#define TESTFILE_DEV  L"C:\\TestFolder"
#define TESTFILE_DEV4  L"C:\\TestFolder\\aaa"
#define TESTFILE_DEV1  L"TestFolder1"
#define TESTFILE_NAME  L"C:\\TestFile.txt"
#define TESTFILE_NAME_TEST  "TestFile_3"
#define TESTFILE_NAME1  L"C:\\TestFile1.txt"
#define TESTFILE_NAME2  L"C:\\TestFile2.txt"
#define TESTFILE_NAME3  L"C:\\TestFile3.txt"
#define SOFTSIM  L"C:\\Softsim"

#define TESTFILE_NAME_YML  "jasonTestFile_1"


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
static eat_fs_error_enum fs_Op_ret;
static eat_fs_error_enum fs_Op_ret_l;
static char readBuf[8192]="";
static char writeBuf[2048]="";
static char text[] =  { 
       '0','1','2','3','4','5','6','7','8','9', 
       'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z', 
       'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' 
       }; 

/********************************************************************
* External Functions declaration
 ********************************************************************/

/********************************************************************
* Local Function declaration
 ********************************************************************/
void eat_fs_yml_test1_fs(void);
void eat_fs_yml_test2_fs(void);


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

/*eat_fs_interface_test1() contain:  
*			eat_fs_GetDiskSize
*			eat_fs_GetDiskFreeSize	
*			eat_fs_Open 
*			eat_fs_GetAttributes 
*			eat_fs_Truncate 
*			eat_fs_Write 
*			eat_fs_Commit  
*			eat_fs_GetFileSize 
*			eat_fs_Seek 
*			eat_fs_GetFilePosition 
*			eat_fs_Read 
*			eat_fs_SetAttributes 
*			eat_fs_Close 
*			eat_fs_Delete	
*/
void eat_fs_interface_test1(void)
{
	int testFileHandle,i,seekRet;
	UINT writedLen,filelen,readLen,filesize,file_position;
	signed int disksize_h,disksize_l,fs_freesize_h,fs_freesize_l;
	SINT64 disksize,fs_freesize;
    char filename[32]={0};
    char filename_l[64]={0};
    int  testFileHandle_l;
    eat_trace("*********eat_fs_interface_test1:start*************");
    memcpy(filename, TESTFILE_NAME_TEST ,sizeof(TESTFILE_NAME_TEST));
	/*convert ascii string into unicode string*/
    for(i=0;i<strlen(filename);i++)
    {
         filename_l[i*2] = filename[i];
         filename_l[i*2+1] = 0x00;
    }

	/*test get file systerm free size interface :eat_fs_GetDiskFreeSize*/
	fs_Op_ret = eat_fs_GetDiskSize(EAT_FS,&disksize);
	if (fs_Op_ret >= 0)
	{        
        	disksize_h=disksize&0xFFFFFFFF00000000;
        	disksize_l=disksize&0x00000000FFFFFFFF;
        	eat_trace("eat_fs_GetDiskSize():Get  File Systerm Size Success,and DISK Size :disksize_h is %d,disksize_l=%d",disksize_h,disksize_l);
	}
	else
	{
        	eat_trace("eat_fs_GetDiskSize():Get  File Systerm Size Fail,and Return Error  is %d",disksize);
	}
	fs_Op_ret = eat_fs_GetDiskFreeSize(EAT_FS,&fs_freesize);
	if (fs_Op_ret >= 0)
	{        
            fs_freesize_h=fs_freesize&0xFFFFFFFF00000000;
            fs_freesize_l=fs_freesize&0x00000000FFFFFFFF;
        	eat_trace("eat_fs_GetDiskFreeSize():Get  File Systerm Free Size Success,and Free Size :fs_freesize_h is %d,fs_freesize_l=%d",fs_freesize_h,fs_freesize_l);
	}
	else
	{
        	eat_trace("eat_fs_GetDiskFreeSize():Get  File Systerm Free Size Fail,and Return Error  is %d",fs_freesize);
	}
	
	/*test create file interface :eat_fs_Open*/
	//the create file name shoule be two-byte aligned and UCS2 encoded ,if not need to transform
	testFileHandle = eat_fs_Open(TESTFILE_NAME,FS_CREATE_ALWAYS|FS_READ_WRITE);
	testFileHandle_l = eat_fs_Open((u16*)filename_l,FS_CREATE_ALWAYS|FS_READ_WRITE);
	if(testFileHandle<EAT_FS_NO_ERROR || testFileHandle_l<EAT_FS_NO_ERROR)
	{
        eat_trace("eat_fs_Open():Create File Fail,and Return Error is %x %x",testFileHandle, testFileHandle_l );
        return ;
	}
	else
	{
        eat_trace("eat_fs_Open():Create File Success,and FileHandle is %x %x",testFileHandle,testFileHandle_l );
    }
	
	/*test get file attribute interface :eat_fs_GetAttributes*/
	fs_Op_ret =(eat_fs_error_enum)eat_fs_GetAttributes(TESTFILE_NAME);
	fs_Op_ret_l =(eat_fs_error_enum)eat_fs_GetAttributes((u16*)filename_l);
	if( fs_Op_ret<EAT_FS_NO_ERROR || fs_Op_ret_l<EAT_FS_NO_ERROR)
	{	
        eat_trace("eat_fs_GetAttributes():Get File Attributes Fail,and Return Error is %d %d",fs_Op_ret,fs_Op_ret_l);
        eat_fs_Close(testFileHandle);
		return;
	}
	else
	{
        eat_trace("eat_fs_GetAttributes():Get File Attributes Success,and File Attribute is %d %d",fs_Op_ret, fs_Op_ret_l);
    }
	
	/*test truncate file interface: eat_fs_Truncate*/
	fs_Op_ret = (eat_fs_error_enum)eat_fs_Truncate(testFileHandle);
	fs_Op_ret_l = (eat_fs_error_enum)eat_fs_Truncate(testFileHandle_l);
	if(EAT_FS_NO_ERROR > fs_Op_ret || EAT_FS_NO_ERROR > fs_Op_ret_l)
	{	
        eat_trace("eat_fs_Truncate():Truncate File Fail,and Return Error is %d %d",fs_Op_ret, fs_Op_ret_l);
        eat_fs_Close(testFileHandle);
        eat_fs_Close(testFileHandle_l);
		return;
	}
	else
	{
        eat_trace("eat_fs_Truncate():Truncate File Success");
    }
	
	/*test write file interface: eat_fs_Write*/
	//write 2K chars to file
	memset(writeBuf,0x00,2048);
	for(i=0;i<2048;i++)
	{
		writeBuf[i] = text[rand()%63];
	}
	fs_Op_ret = (eat_fs_error_enum)eat_fs_Write(testFileHandle,writeBuf,2048,&writedLen);
	fs_Op_ret_l = (eat_fs_error_enum)eat_fs_Write(testFileHandle_l,writeBuf,2048,&writedLen);
	if(EAT_FS_NO_ERROR != fs_Op_ret ||2048 !=writedLen || EAT_FS_NO_ERROR != fs_Op_ret_l)
	{	
		eat_trace("eat_fs_Write():Write File Fail,and Return Error is %d %d",fs_Op_ret, fs_Op_ret_l);
		eat_fs_Close(testFileHandle);
		return;
	}
	else
	{
        	eat_trace("eat_fs_Write():Write File Success");
	}
	
	/*test commit file interface: eat_fs_Commit*/
	fs_Op_ret = (eat_fs_error_enum)eat_fs_Commit(testFileHandle);
	fs_Op_ret_l = (eat_fs_error_enum)eat_fs_Commit(testFileHandle_l);
	if(EAT_FS_NO_ERROR != fs_Op_ret || EAT_FS_NO_ERROR != fs_Op_ret_l)
	{
		eat_trace("eat_fs_Commit():Commit File Fail,and Return Error is %d %d",fs_Op_ret, fs_Op_ret_l);
		eat_fs_Close(testFileHandle);
		eat_fs_Close(testFileHandle_l);
		return;
	}
	else
	{
        	eat_trace("eat_fs_Commit():Commit File Success");
	}
	
	/*test get file size interface: eat_fs_GetFileSize*/
	filelen=writedLen;
	fs_Op_ret = (eat_fs_error_enum)eat_fs_GetFileSize(testFileHandle,&filesize);
	fs_Op_ret_l = (eat_fs_error_enum)eat_fs_GetFileSize(testFileHandle_l,&filesize);
	if(EAT_FS_NO_ERROR != fs_Op_ret||filelen!=filesize||EAT_FS_NO_ERROR != fs_Op_ret_l)
	{
		eat_trace("eat_fs_GetFileSize():Get File Size Fail,and Return Error is %d",fs_Op_ret, fs_Op_ret_l);
		eat_fs_Close(testFileHandle);
		eat_fs_Close(testFileHandle_l);
		return;
	}
	else
	{
        	eat_trace("eat_fs_GetFileSize():Get File Size Success and File Size id %d",filesize);
	}
	
	/*test seek file pointer interface: eat_fs_Seek*/
	//seek to file head
	seekRet = eat_fs_Seek(testFileHandle,0,EAT_FS_FILE_BEGIN);
	if(0>seekRet)
	{
		eat_trace("eat_fs_Seek():Seek File Pointer Fail");
		eat_fs_Close(testFileHandle);
		return;
	}
	else
	{
        	eat_trace("eat_fs_Seek():Seek File Pointer Success");
	}
	
	/*test get the current file pointer interface: eat_fs_GetFilePosition*/
	fs_Op_ret = (eat_fs_error_enum)eat_fs_GetFilePosition(testFileHandle,&file_position);
	if(EAT_FS_NO_ERROR != fs_Op_ret||0!=file_position)
	{
		eat_trace("eat_fs_GetFilePosition():Get Current File Pointer Fail");
		eat_fs_Close(testFileHandle);
		return;
	}
	else
	{
		eat_trace("eat_fs_GetFilePosition():Get Current File Pointer Success");
	}

	/*test read file interface: eat_fs_Read*/
	fs_Op_ret = (eat_fs_error_enum)eat_fs_Read(testFileHandle,readBuf,2500,&readLen);
	if(EAT_FS_NO_ERROR != fs_Op_ret ||2048 !=readLen)
	{	
		eat_trace("eat_fs_Read():Read File Fail,and Return Error is %d,Readlen is %d",fs_Op_ret,readLen);
		eat_fs_Close(testFileHandle);
		return;
	}
	else
	{
		eat_trace("eat_fs_Read():Read File Success");
	}
	
	/*test set file attribute interface: eat_fs_SetAttributes*/
	fs_Op_ret=(eat_fs_error_enum)eat_fs_SetAttributes(TESTFILE_NAME,FS_ATTR_READ_ONLY);	
	if(EAT_FS_NO_ERROR!=fs_Op_ret)
	{	
		eat_trace("eat_fs_SetAttributes():Set File Attribute Fail,and Return Error is %d",fs_Op_ret);
		eat_fs_Close(testFileHandle);
		return ;
	}
	else
	{
		eat_trace("eat_fs_SetAttributes():Set File Attribute Success");
	}
	
	/*test close open file handle interface: eat_fs_Close*/
	fs_Op_ret=(eat_fs_error_enum)eat_fs_Close(testFileHandle);
	fs_Op_ret_l=(eat_fs_error_enum)eat_fs_Close(testFileHandle_l);
	if(EAT_FS_NO_ERROR!=fs_Op_ret || EAT_FS_NO_ERROR!=fs_Op_ret_l)
	{	
		eat_trace("eat_fs_Close():Close File Handle Fail,and Return Error is %d %d", fs_Op_ret, fs_Op_ret_l);
		return ;
	}
	else
	{
		eat_trace("eat_fs_Close():Close File Handle Success");
	}
	
	/*test delete file interface: eat_fs_Delete*/
	fs_Op_ret = (eat_fs_error_enum)eat_fs_Delete(TESTFILE_NAME);
	fs_Op_ret_l = (eat_fs_error_enum)eat_fs_Delete((u16*)filename_l);
	if(EAT_FS_NO_ERROR!=fs_Op_ret || EAT_FS_NO_ERROR!=fs_Op_ret_l)
	{	
		eat_trace("eat_fs_Delete():Delete File Fail,and Return Error is %d %d",fs_Op_ret, fs_Op_ret_l);
		return ;
	}
	else
	{
		eat_trace("eat_fs_Delete():Delete File Success");
	}
	
    	eat_trace("*********eat_fs_interface_test1:end*************");

}

/*eat_fs_interface_test2() contain:  
*			eat_fs_CreateDir	创建文件夹
*			eat_fs_GetFolderSize	获得文件夹大小
*/
void eat_fs_interface_test2()
{
	int hFile;

    	eat_trace("*********eat_fs_interface_test2:start*************");
	hFile = eat_fs_Open(TESTFILE_DEV, FS_READ_ONLY | FS_OPEN_DIR);
       eat_trace("eat_fs_interface_test2:hFile=%d",hFile);
	if (hFile == EAT_FS_FILE_NOT_FOUND || hFile == EAT_FS_PATH_NOT_FOUND)
	{
		/*test create dir interface: eat_fs_CreateDir*/
		fs_Op_ret = (eat_fs_error_enum)eat_fs_CreateDir(TESTFILE_DEV);
		if(fs_Op_ret == EAT_FS_NO_ERROR)
		{
			eat_trace("eat_fs_CreateDir():CreateDir Success");
#if 1
			/*test get folder size interface: eat_fs_GetFolderSize*/
			fs_Op_ret=(eat_fs_error_enum)eat_fs_GetFolderSize(TESTFILE_DEV);
			if(fs_Op_ret>=0)
			{
				eat_trace("eat_fs_GetFolderSize():Get Folder Size Success,and The Folder Size is %d",fs_Op_ret);	
			}
			else
			{
				eat_trace("eat_fs_GetFolderSize():Get Folder Size Fail, and Return Error is %d",fs_Op_ret);
			
			}
#endif
		}else
		{
			eat_trace("eat_fs_CreateDir():CreateDir Fail,and Return Error is %d",fs_Op_ret);
		}
	}
	else//如果已经有该文件夹，则关闭并删除该文件夹
	{
		eat_trace("eat_fs_interface_test2():The dir has existed");
		eat_fs_Close(hFile);//关闭文件或文件夹，采用打开文件的句柄。
		
		/*test remove dir interface: eat_fs_RemoveDir*/
		fs_Op_ret = (eat_fs_error_enum)eat_fs_RemoveDir(TESTFILE_DEV);;
		if(EAT_FS_NO_ERROR!=fs_Op_ret)
		{	
			eat_trace("eat_fs_RemoveDir():Remove Dir Fail,and Return Error is %d",fs_Op_ret);
			return ;
		}
		else
		{
			eat_trace("eat_fs_RemoveDir():Remove Dir Success");
		}
	}
     	eat_trace("*********eat_fs_interface_test2:end*************");
   
}

void eat_fs_interface_test_filerename()
{
    int testFileHandle,testFileHandle3;

    eat_trace("*********eat_fs_interface_test_filerename:start*************");
    testFileHandle = eat_fs_Open(TESTFILE_NAME,FS_CREATE|FS_READ_WRITE);
    if(testFileHandle<EAT_FS_NO_ERROR)
    {
            eat_trace("eat_fs_Open():Create File Fail,and Return Error is %d",testFileHandle);
        return ;
    }
    else
    {
            eat_trace("eat_fs_Open():Create File Success,and FileHandle is %d",testFileHandle);
    }
            
    /*test close open file handle interface: eat_fs_Close*/
    fs_Op_ret=(eat_fs_error_enum)eat_fs_Close(testFileHandle);
    if(EAT_FS_NO_ERROR!=fs_Op_ret)
    {   
        eat_trace("eat_fs_Close():Close File Handle Fail,and Return Error is %d",fs_Op_ret);
    return ;
    }
    else
    {
        eat_trace("eat_fs_Close():Close File Handle Success");
    }
    fs_Op_ret=eat_fs_Rename(TESTFILE_NAME,TESTFILE_NAME3);
    if(EAT_FS_NO_ERROR!=fs_Op_ret)
    {   
        eat_trace("eat_fs_Rename():Rename File Handle Fail,and Return Error is %d",fs_Op_ret);
    return ;
    }
    else
    {
        eat_trace("eat_fs_Rename():Rename File Handle Success");
    }
    testFileHandle3 = eat_fs_Open(TESTFILE_NAME3,FS_READ_ONLY);
    if(testFileHandle3<EAT_FS_NO_ERROR)
    {
            eat_trace("eat_fs_Open():Open File3 Fail,and Return Error is %d",testFileHandle3);
        return ;
    }
    else
    {
            eat_trace("eat_fs_Open():Open File3 Success,and FileHandle is %d",testFileHandle3);
    }
    eat_fs_Close(testFileHandle3);
    eat_trace("*********eat_fs_interface_test_filerename:end*************");
   
}

void eat_fs_interface_testfilefind()
{
    FS_HANDLE handle;
    EAT_FS_DOSDirEntry fileinfo;
    WCHAR namepattern[32];
    WCHAR filename[300];
    int file_number=0;
    int testFileHandle,testFileHandle1,testFileHandle2;
    eat_trace("*********eat_fs_interface_testfilefind:start*************");  
    testFileHandle = eat_fs_Open(TESTFILE_NAME,FS_CREATE|FS_READ_WRITE);
    if(testFileHandle<EAT_FS_NO_ERROR)
    {
        	eat_trace("eat_fs_Open():Create File Fail,and Return Error is %d",testFileHandle);
    	return ;
    }
    else
    {
        	eat_trace("eat_fs_Open():Create File Success,and FileHandle is %d",testFileHandle);
    }
    	
    /*test close open file handle interface: eat_fs_Close*/
    fs_Op_ret=(eat_fs_error_enum)eat_fs_Close(testFileHandle);
    if(EAT_FS_NO_ERROR!=fs_Op_ret)
    {	
    	eat_trace("eat_fs_Close():Close File Handle Fail,and Return Error is %d",fs_Op_ret);
    	return ;
    }
    else
    {
    	eat_trace("eat_fs_Close():Close File Handle Success");
    }
    testFileHandle1 = eat_fs_Open(TESTFILE_NAME1,FS_CREATE|FS_READ_WRITE);
    if(testFileHandle1<EAT_FS_NO_ERROR)
    {
        	eat_trace("eat_fs_Open():Create File1 Fail,and Return Error is %d",testFileHandle1);
    	return ;
    }
    else
    {
        	eat_trace("eat_fs_Open():Create File1 Success,and FileHandle is %d",testFileHandle1);
    }
    	
    /*test close open file handle interface: eat_fs_Close*/
    fs_Op_ret=(eat_fs_error_enum)eat_fs_Close(testFileHandle1);
    if(EAT_FS_NO_ERROR!=fs_Op_ret)
    {	
    	eat_trace("eat_fs_Close():Close1 File Handle Fail,and Return Error is %d",fs_Op_ret);
    	return ;
    }
    else
    {
    	eat_trace("eat_fs_Close():Close1 File Handle Success");
    }
    testFileHandle2 = eat_fs_Open(TESTFILE_NAME2,FS_CREATE|FS_READ_WRITE);
    if(testFileHandle2<EAT_FS_NO_ERROR)
    {
        	eat_trace("eat_fs_Open():Create File2 Fail,and Return Error is %d",testFileHandle2);
    	return ;
    }
    else
    {
        	eat_trace("eat_fs_Open():Create File2 Success,and FileHandle is %d",testFileHandle2);
    }
    	
    /*test close open file handle interface: eat_fs_Close*/
    fs_Op_ret=(eat_fs_error_enum)eat_fs_Close(testFileHandle2);
    if(EAT_FS_NO_ERROR!=fs_Op_ret)
    {	
    	eat_trace("eat_fs_Close():Close File2 Handle Fail,and Return Error is %d",fs_Op_ret);
    	return ;
    }
    else
    {
    	eat_trace("eat_fs_Close():Close File2 Handle Success");
    }
    
    handle = eat_fs_FindFirst(L"C:\\*.*", 0, 0, &fileinfo, filename, sizeof(filename));
    eat_trace("eat_fs_interface_testfilefind:handle=%d",handle);
    if (handle > 0)
    {
       do
       {
           file_number++;
       }
       while (eat_fs_FindNext(handle, &fileinfo, filename, sizeof(filename)) == EAT_FS_NO_ERROR);
    }
    eat_fs_FindClose(handle);
    eat_trace("eat_fs_interface_test4:file_number=%d",file_number);
    eat_trace("*********eat_fs_interface_testfilefind:end*************");
   
}



void eat_fs_interface_test3()
{
    int testFileHandle,i;
    UINT writedLen;
    eat_trace("*********eat_fs_interface_test3:start*************");
    /*convert ascii string into unicode string*/
    
    /*test create file interface :eat_fs_Open*/
    //the create file name shoule be two-byte aligned and UCS2 encoded ,if not need to transform
    testFileHandle = eat_fs_Open(TESTFILE_DEV4,FS_CREATE_ALWAYS|FS_READ_WRITE);
    if(testFileHandle<EAT_FS_NO_ERROR )
    {
        eat_trace("eat_fs_Open():Create File Fail,and Return Error is %x",testFileHandle );
        return ;
    }
    else
    {
        eat_trace("eat_fs_Open():Create File Success,and FileHandle is %x",testFileHandle );
    }
        
    /*test write file interface: eat_fs_Write*/
    //write 2K chars to file
    memset(writeBuf,0x00,2048);
    for(i=0;i<2048;i++)
    {
        writeBuf[i] = text[rand()%63];
    }
    fs_Op_ret = (eat_fs_error_enum)eat_fs_Write(testFileHandle,writeBuf,2048,&writedLen);//返回写的数据的个数，参数为变量的地址
    if(EAT_FS_NO_ERROR != fs_Op_ret ||2048 !=writedLen)
    {   
        eat_trace("eat_fs_Write():Write File Fail,and Return Error is %d",fs_Op_ret);
        eat_fs_Close(testFileHandle);
        return;
    }
    else
    {
            eat_trace("eat_fs_Write():Write File Success");
    }
    
    /*test commit file interface: eat_fs_Commit*/
    fs_Op_ret = (eat_fs_error_enum)eat_fs_Commit(testFileHandle);
    if(EAT_FS_NO_ERROR != fs_Op_ret )
    {
        eat_trace("eat_fs_Commit():Commit File Fail,and Return Error is %d ",fs_Op_ret);
        eat_fs_Close(testFileHandle);
        return;
    }
    else
    {
            eat_trace("eat_fs_Commit():Commit File Success");
    }
    
    /*test close open file handle interface: eat_fs_Close*/
    fs_Op_ret=(eat_fs_error_enum)eat_fs_Close(testFileHandle);
    if(EAT_FS_NO_ERROR!=fs_Op_ret )
    {   
        eat_trace("eat_fs_Close():Close File Handle Fail,and Return Error is %d", fs_Op_ret);
        return ;
    }
    else
    {
        eat_trace("eat_fs_Close():Close File Handle Success");
    }
    
    /*test delete file interface: eat_fs_Delete*/
    fs_Op_ret = (eat_fs_error_enum)eat_fs_Delete(TESTFILE_DEV4);
    if(EAT_FS_NO_ERROR!=fs_Op_ret)
    {   
        eat_trace("eat_fs_Delete():Delete File Fail,and Return Error is %d",fs_Op_ret);
        return ;
    }
    else
    {
        eat_trace("eat_fs_Delete():Delete File Success");
    }
    
        eat_trace("*********eat_fs_interface_test3:end*************");

}

void eat_ascii_2_unic(u16* out, u8* in)
{
    u16 i=0;
    u8* outp = (u8*)out;
    u8* inp = in;
    //eat_trace("filename:%s",in);
    while( inp[i] )
    {
        outp[i*2] = inp[i];
        outp[i*2+1] = 0x00;
        i++;
    }

}
void eat_fs_interface_test5()
{
    int hFile;

    eat_trace("*********eat_fs_interface_test5:start*************");
    hFile = eat_fs_Open(SOFTSIM, FS_READ_ONLY | FS_OPEN_DIR);
    eat_trace("eat_fs_interface_test5:hFile=%d",hFile);
    if (hFile == EAT_FS_FILE_NOT_FOUND || hFile == EAT_FS_PATH_NOT_FOUND)
    {
        /*test create dir interface: eat_fs_CreateDir*/
        fs_Op_ret = (eat_fs_error_enum)eat_fs_CreateDir((const WCHAR *)SOFTSIM);
        if(fs_Op_ret == EAT_FS_NO_ERROR)
        {
            eat_trace("eat_fs_CreateDir():CreateDir Success");
            /*test get folder size interface: eat_fs_GetFolderSize*/
            fs_Op_ret=(eat_fs_error_enum)eat_fs_GetFolderSize((const WCHAR *)SOFTSIM);
            if(fs_Op_ret>=0)
            {
                eat_trace("eat_fs_GetFolderSize():Get Folder Size Success,and The Folder Size is %d",fs_Op_ret);    
            }
            else
            {
                eat_trace("eat_fs_GetFolderSize():Get Folder Size Fail, and Return Error is %d",fs_Op_ret);
            
            }
        }else
        {
            eat_trace("eat_fs_CreateDir():CreateDir Fail,and Return Error is %d",fs_Op_ret);
        }
    }
    else
    {
        eat_trace("eat_fs_interface_test2():The dir has existed");
        eat_fs_Close(hFile);
        
        /*test remove dir interface: eat_fs_RemoveDir*/
        fs_Op_ret = (eat_fs_error_enum)eat_fs_RemoveDir((const WCHAR *)SOFTSIM);
        if(EAT_FS_NO_ERROR!=fs_Op_ret)
        {   
            eat_trace("eat_fs_RemoveDir():Remove Dir Fail,and Return Error is %d",fs_Op_ret);
            return ;
        }
        else
        {
            eat_trace("eat_fs_RemoveDir():Remove Dir Success");
        }
    }
        eat_trace("*********eat_fs_interface_test5:end*************");
   
}

void eat_fs_interface_test4()
{
    int hFile;
    int testFileHandle,i;
    //char filename[32]={2,4,'A',6,4,0,'E',6,6,'B',1,5,'E',8,4,'F'};
    char filename[32]={'2','4','A','6','4','0','E','6','6','B','1','5','E','8','4','F'};
    char filename_l[60]="C:\\Softsim\\";
    u16 filename_ll[80] = {0};
    u32 writedLen;
    eat_trace("*********eat_fs_interface_test4:start*************");
    /*convert ascii string into unicode string*/
    strcat(filename_l,filename);// 将filename放到filename_1的后面

    eat_ascii_2_unic(filename_ll, filename_l);//转换成unicode
	
    //the create file name shoule be two-byte aligned and UCS2 encoded ,if not need to transform
    testFileHandle = eat_fs_Open((const unsigned short*)filename_ll,FS_CREATE_ALWAYS|FS_READ_WRITE);
    if(testFileHandle<EAT_FS_NO_ERROR )
    {
        eat_trace("eat_fs_Open():Create File Fail,and Return Error is %x",testFileHandle );
        return ;
    }
    else
    {
        eat_trace("eat_fs_Open():Create File Success,and FileHandle is %x",testFileHandle );
    }
        
    /*test write file interface: eat_fs_Write*/
    //write 2K chars to file
    memset(writeBuf,0x00,2048);
    for(i=0;i<2048;i++)
    {
        writeBuf[i] = text[rand()%63];
    }
    fs_Op_ret = (eat_fs_error_enum)eat_fs_Write(testFileHandle,writeBuf,2048,&writedLen);
    if(EAT_FS_NO_ERROR != fs_Op_ret ||2048 !=writedLen)
    {   
        eat_trace("eat_fs_Write():Write File Fail,and Return Error is %d",fs_Op_ret);
        eat_fs_Close(testFileHandle);
        return;
    }
    else
    {
            eat_trace("eat_fs_Write():Write File Success");
    }
    
    /*test commit file interface: eat_fs_Commit*/
    fs_Op_ret = (eat_fs_error_enum)eat_fs_Commit(testFileHandle);
    if(EAT_FS_NO_ERROR != fs_Op_ret )
    {
        eat_trace("eat_fs_Commit():Commit File Fail,and Return Error is %d ",fs_Op_ret);
        eat_fs_Close(testFileHandle);
        return;
    }
    else
    {
            eat_trace("eat_fs_Commit():Commit File Success");
    }
    
    /*test close open file handle interface: eat_fs_Close*/
    fs_Op_ret=(eat_fs_error_enum)eat_fs_Close(testFileHandle);
    if(EAT_FS_NO_ERROR!=fs_Op_ret )
    {   
        eat_trace("eat_fs_Close():Close File Handle Fail,and Return Error is %d", fs_Op_ret);
        return ;
    }
    else
    {
        eat_trace("eat_fs_Close():Close File Handle Success");
    }
    
    /*test delete file interface: eat_fs_Delete*/
    fs_Op_ret = (eat_fs_error_enum)eat_fs_Delete(filename_ll);
    if(EAT_FS_NO_ERROR!=fs_Op_ret)
    {   
        eat_trace("eat_fs_Delete():Delete File Fail,and Return Error is %d",fs_Op_ret);
        return ;
    }
    else
    {
        eat_trace("eat_fs_Delete():Delete File Success");
    }
    
        eat_trace("*********eat_fs_interface_test4:end*************");

}

/*The demo supposes that the filesystem  has had the follow dirs or files
C:
  <dir> TestFolder
*/

/*
 * 	FullPath : [IN]  Source path to be deleted
 * 	Flag : [IN] FS_FILE_TYPE, FS_DIR_TYPE, FS_RECURSIVE_TYPE
*/
/*
delete files of TestFolder with flag FS_FILE_TYPE,this flag only delete files of the current path.
For example:
C:\TestFolder
    aaa\
        a11.txt
    bbb\
        b11.txt
    ccc\ 
        c11.txt
    bbb.txt
    ddd.txt
    sscom.ini  

 eat_fs_XDelete(TESTFILE_DEV,FS_FILE_TYPE) only delete     
    bbb.txt
    ddd.txt
    sscom.ini  
   
*/
void eat_fs_xdelete_test_delete_file(void)
{
	int hFile;

	eat_trace("*********eat_fs_xdelete_test_delete_file:start*************");

	hFile = eat_fs_XDelete(TESTFILE_DEV,FS_FILE_TYPE);

	eat_trace("*********eat_fs_xdelete_test_delete_file:hFile=%d",hFile);	
	eat_trace("*********eat_fs_xdelete_test_delete_file:end*************");   
}
/*
delete files of TestFolder with flag FS_FILE_TYPE|FS_RECURSIVE_TYPE,this method can delete all files in current path and subdirectory.
*/
void eat_fs_xdelete_test_delete_file2(void)
{
	int hFile;

	eat_trace("*********eat_fs_xdelete_test_delete_file2:start*************");

	hFile = eat_fs_XDelete(TESTFILE_DEV,FS_FILE_TYPE|FS_RECURSIVE_TYPE);

	eat_trace("*********eat_fs_xdelete_test_delete_file2:hFile=%d",hFile);	
	eat_trace("*********eat_fs_xdelete_test_delete_file2:end*************");   
}
/*
If you want to delete the dir ,you should use with flag FS_FILE_TYPE|FS_DIR_TYPE|FS_RECURSIVE_TYPE,
otherwise it will return err.
*/
/*
delete TestFolder with flag FS_FILE_TYPE|FS_DIR_TYPE|FS_RECURSIVE_TYPE
*/
void eat_fs_xdelete_test_dir(void)
{
	int hFile;

	eat_trace("*********eat_fs_xdelete_test_delete_whole_dir:start*************");

	hFile = eat_fs_XDelete(TESTFILE_DEV,FS_FILE_TYPE|FS_DIR_TYPE|FS_RECURSIVE_TYPE);

	eat_trace("*********eat_fs_xdelete_test_delete_whole_dir:hFile=%d",hFile);	
	eat_trace("*********eat_fs_xdelete_test_delete_whole_dir:end*************");   
}

/****************************************************
 * Timer testing module
 *****************************************************/
eat_bool eat_module_test_fs(u8 param1, u8 param2)
{
    /***************************************
     * example 1
     * Start and stop 3 timers
     ***************************************/
    if( 1 == param1 )
    {

        if( 1 == param2 )
        {
 		eat_fs_interface_test1();
        }else if( 2 == param2)
        {
		eat_fs_interface_test2();
        }
        else if( 3 == param2)
        {
		eat_fs_interface_test3();
        }
        else if( 4 == param2)
        {
		eat_fs_interface_test4();
        }
        else if( 5 == param2)
        {
		eat_fs_interface_test5();
        }
        
    }
    else if(2 == param1)
    {
        if( 1 == param2 )
        {
 		eat_fs_interface_test_filerename();
        }else if( 2 == param2)
        {
		eat_fs_interface_testfilefind();
        }
    }
    else if(3 == param1)
    {
        if( 1 == param2)
        {
 		eat_fs_xdelete_test_delete_file();
        }else if( 2 == param2)
        {
		eat_fs_xdelete_test_delete_file2();
        }else if( 3 == param2)
        {
		eat_fs_xdelete_test_dir();
        }
    }
	else if(4 == param1)
    {
        if( 1 == param2)
        {
 		eat_fs_yml_test1_fs();
        }
		else if(2 == param2)
		{
		eat_fs_yml_test2_fs();
		}
    }

    return EAT_TRUE;
}


void eat_fs_yml_test1_fs(void)
{
	int testFileHandle,i,seekRet;
	UINT writedLen,filelen,readLen,filesize,file_position;
	signed int disksize_h,disksize_l,fs_freesize_h,fs_freesize_l;
	SINT64 disksize,fs_freesize;
	char filename[32]={0};
	char filename_l[64]={0};
	int  testFileHandle_l;

    char arry1[20]="name1,13320984483";
    char arry2[20]="name2,18812345678";


	eat_trace("*********eat_fs_yml_test1:start*************");
	memcpy(filename, TESTFILE_NAME_YML ,sizeof(TESTFILE_NAME_YML));
	/*convert ascii string into unicode string*/
	for(i=0;i<strlen(filename);i++)
	{
		 filename_l[i*2] = filename[i];
		 filename_l[i*2+1] = 0x00;
	}
#if 0
	/*test get file systerm free size interface :eat_fs_GetDiskFreeSize*/
	fs_Op_ret = eat_fs_GetDiskSize(EAT_FS,&disksize);
	if (fs_Op_ret >= 0)
	{		 
			disksize_h=disksize&0xFFFFFFFF00000000;
			disksize_l=disksize&0x00000000FFFFFFFF;
			eat_trace("eat_fs_GetDiskSize():Get  File Systerm Size Success,and DISK Size :disksize_h is %d,disksize_l=%d",disksize_h,disksize_l);
	}
	else
	{
			eat_trace("eat_fs_GetDiskSize():Get  File Systerm Size Fail,and Return Error  is %d",disksize);
	}
	fs_Op_ret = eat_fs_GetDiskFreeSize(EAT_FS,&fs_freesize);
	if (fs_Op_ret >= 0)
	{		 
			fs_freesize_h=fs_freesize&0xFFFFFFFF00000000;
			fs_freesize_l=fs_freesize&0x00000000FFFFFFFF;
			eat_trace("eat_fs_GetDiskFreeSize():Get  File Systerm Free Size Success,and Free Size :fs_freesize_h is %d,fs_freesize_l=%d",fs_freesize_h,fs_freesize_l);
	}
	else
	{
			eat_trace("eat_fs_GetDiskFreeSize():Get  File Systerm Free Size Fail,and Return Error  is %d",fs_freesize);
	}
	#endif
	/*test create file interface :eat_fs_Open*/
	//the create file name shoule be two-byte aligned and UCS2 encoded ,if not need to transform
	testFileHandle_l = eat_fs_Open((u16*)filename_l,FS_CREATE_ALWAYS|FS_READ_WRITE);
	if(testFileHandle_l<EAT_FS_NO_ERROR)
	{
		eat_trace("eat_fs_Open():Create File Fail,and Return Error is %x",testFileHandle_l );
		return ;
	}
	else
	{
		eat_trace("eat_fs_Open():Create File Success,and FileHandle is %x",testFileHandle_l );
	}

	/*test write file interface: eat_fs_Write*/
	//write 2K chars to file
	/*memset(writeBuf,0x00,2048);
	for(i=0;i<2048;i++)
	{
		writeBuf[i] = text[rand()%63];
	}*/
	for(i=0;i<20;i++)
	{
		writeBuf[i] = text[i];
	}

	fs_Op_ret_l = (eat_fs_error_enum)eat_fs_Write(testFileHandle_l,writeBuf,20,&writedLen);
	if(20 !=writedLen || EAT_FS_NO_ERROR != fs_Op_ret_l)
	{	
		eat_trace("eat_fs_Write():Write File Fail,and Return Error is %d %d",fs_Op_ret, fs_Op_ret_l);
		eat_fs_Close(testFileHandle);
		return;
	}
	else
	{
			eat_trace("eat_fs_Write():Write File Success");
	}
	
	/*test commit file interface: eat_fs_Commit*/
	fs_Op_ret_l = (eat_fs_error_enum)eat_fs_Commit(testFileHandle_l);
	if(EAT_FS_NO_ERROR != fs_Op_ret_l)
	{
		eat_trace("eat_fs_Commit():Commit File Fail,and Return Error is %d", fs_Op_ret_l);
		eat_fs_Close(testFileHandle_l);
		return;
	}
	else
	{
			eat_trace("eat_fs_Commit():Commit File Success");
	}
	/*test close open file handle interface: eat_fs_Close*/
	fs_Op_ret_l=(eat_fs_error_enum)eat_fs_Close(testFileHandle_l);
	if(EAT_FS_NO_ERROR!=fs_Op_ret_l)
	{	
		eat_trace("eat_fs_Close():Close File Handle Fail,and Return Error is %d", fs_Op_ret_l);
		return ;
	}
	else
	{
		eat_trace("eat_fs_Close():Close File Handle Success");
	}
	
#if 0
	/*test get file size interface: eat_fs_GetFileSize*/
	filelen=writedLen;
	fs_Op_ret_l = (eat_fs_error_enum)eat_fs_GetFileSize(testFileHandle_l,&filesize);
	if(EAT_FS_NO_ERROR != fs_Op_ret_l)
	{
		eat_trace("eat_fs_GetFileSize():Get File Size Fail,and Return Error is %d", fs_Op_ret_l);
		eat_fs_Close(testFileHandle_l);
		return;
	}
	else
	{
			eat_trace("eat_fs_GetFileSize():Get File Size Success and File Size id %d",filesize);
	}
	#endif
	
	#if 0
	/*test seek file pointer interface: eat_fs_Seek*/
	//seek to file head
	seekRet = eat_fs_Seek(testFileHandle,0,EAT_FS_FILE_BEGIN);
	if(0>seekRet)
	{
		eat_trace("eat_fs_Seek():Seek File Pointer Fail");
		eat_fs_Close(testFileHandle);
		return;
	}
	else
	{
			eat_trace("eat_fs_Seek():Seek File Pointer Success");
	}

	/*test get the current file pointer interface: eat_fs_GetFilePosition*/
	fs_Op_ret = (eat_fs_error_enum)eat_fs_GetFilePosition(testFileHandle,&file_position);
	if(EAT_FS_NO_ERROR != fs_Op_ret||0!=file_position)
	{
		eat_trace("eat_fs_GetFilePosition():Get Current File Pointer Fail");
		eat_fs_Close(testFileHandle);
		return;
	}
	else
	{
		eat_trace("eat_fs_GetFilePosition():Get Current File Pointer Success");
	}
#endif

#if 0
	/*test read file interface: eat_fs_Read*/
	fs_Op_ret = (eat_fs_error_enum)eat_fs_Read(testFileHandle,readBuf,2500,&readLen);
	if(EAT_FS_NO_ERROR != fs_Op_ret)
	{	
		eat_trace("eat_fs_Read():Read File Fail,and Return Error is %d,Readlen is %d",fs_Op_ret,readLen);
		eat_fs_Close(testFileHandle);
		return;
	}
	else
	{
		eat_trace("eat_fs_Read():Read File Success");
		eat_trace("wordis:%s",readBuf);///从debug口，输出读出来的数据字符
	}
	
	/*test set file attribute interface: eat_fs_SetAttributes*/
	fs_Op_ret=(eat_fs_error_enum)eat_fs_SetAttributes(TESTFILE_NAME,FS_ATTR_READ_ONLY); 
	if(EAT_FS_NO_ERROR!=fs_Op_ret)
	{	
		eat_trace("eat_fs_SetAttributes():Set File Attribute Fail,and Return Error is %d",fs_Op_ret);
		eat_fs_Close(testFileHandle);
		return ;
	}
	else
	{
		eat_trace("eat_fs_SetAttributes():Set File Attribute Success");
	}
	
	/*test close open file handle interface: eat_fs_Close*/
	fs_Op_ret_l=(eat_fs_error_enum)eat_fs_Close(testFileHandle_l);
	if(EAT_FS_NO_ERROR!=fs_Op_ret_l)
	{	
		eat_trace("eat_fs_Close():Close File Handle Fail,and Return Error is %d", fs_Op_ret_l);
		return ;
	}
	else
	{
		eat_trace("eat_fs_Close():Close File Handle Success");
	}
	
	/*test delete file interface: eat_fs_Delete*/
	fs_Op_ret_l = (eat_fs_error_enum)eat_fs_Delete((u16*)filename_l);
	if(EAT_FS_NO_ERROR!=fs_Op_ret_l)
	{	
		eat_trace("eat_fs_Delete():Delete File Fail,and Return Error is %d", fs_Op_ret_l);
		return ;
	}
	else
	{
		eat_trace("eat_fs_Delete():Delete File Success");
	}
	#endif
		eat_trace("*********eat_fs_interface_yml:end*************");

}

void eat_fs_yml_test2_fs(void)
{
	int testFileHandle,i,seekRet;
	UINT writedLen,filelen,readLen,filesize,file_position;
	signed int disksize_h,disksize_l,fs_freesize_h,fs_freesize_l;
	SINT64 disksize,fs_freesize;
	char filename[32]={0};
	char filename_l[64]={0};
	int  testFileHandle_l;

    //char arry1[20]="name1,13320984483";
    //char arry2[20]="name2,18812345678";


	eat_trace("*********eat_fs_yml_test2222:start*************");
	
	memcpy(filename, TESTFILE_NAME_YML ,sizeof(TESTFILE_NAME_YML));
	/*convert ascii string into unicode string*/
	for(i=0;i<strlen(filename);i++)
	{
		 filename_l[i*2] = filename[i];
		 filename_l[i*2+1] = 0x00;
	}
	/*test create file interface :eat_fs_Open*/
	//the create file name shoule be two-byte aligned and UCS2 encoded ,if not need to transform
	testFileHandle_l = eat_fs_Open((u16*)filename_l,FS_READ_ONLY);
	if(testFileHandle_l<EAT_FS_NO_ERROR)
	{
		eat_trace("eat_fs_Open():open File Fail,and Return Error is %x",testFileHandle_l );
		return ;
	}
	else
	{
		eat_trace("eat_fs_Open():open File Success,and FileHandle is %x",testFileHandle_l );
	}

	/*test read file interface: eat_fs_Read*/
	fs_Op_ret = (eat_fs_error_enum)eat_fs_Read(testFileHandle_l,readBuf,100,&readLen);
	if(EAT_FS_NO_ERROR != fs_Op_ret)
	{	
		eat_trace("eat_fs_Read():Read File Fail,and Return Error is %d,Readlen is %d",fs_Op_ret,readLen);
		eat_fs_Close(testFileHandle);
		return;
	}
	else
	{
		eat_trace("eat_fs_Read():Read File Success");
		eat_trace("wordis:%s",readBuf);///从debug口，输出读出来的数据字符
	}
		/*test close open file handle interface: eat_fs_Close*/
	fs_Op_ret_l=(eat_fs_error_enum)eat_fs_Close(testFileHandle_l);
	if(EAT_FS_NO_ERROR!=fs_Op_ret_l)
	{	
		eat_trace("eat_fs_Close():Close File Handle Fail,and Return Error is %d", fs_Op_ret_l);
		return ;
	}
	else
	{
		eat_trace("eat_fs_Close():Close File Handle Success");
	}
	
#if 0
	/*test set file attribute interface: eat_fs_SetAttributes*/
	fs_Op_ret=(eat_fs_error_enum)eat_fs_SetAttributes(TESTFILE_NAME,FS_ATTR_READ_ONLY); 
	if(EAT_FS_NO_ERROR!=fs_Op_ret)
	{	
		eat_trace("eat_fs_SetAttributes():Set File Attribute Fail,and Return Error is %d",fs_Op_ret);
		eat_fs_Close(testFileHandle);
		return ;
	}
	else
	{
		eat_trace("eat_fs_SetAttributes():Set File Attribute Success");
	}
	
	/*test close open file handle interface: eat_fs_Close*/
	fs_Op_ret_l=(eat_fs_error_enum)eat_fs_Close(testFileHandle_l);
	if(EAT_FS_NO_ERROR!=fs_Op_ret_l)
	{	
		eat_trace("eat_fs_Close():Close File Handle Fail,and Return Error is %d", fs_Op_ret_l);
		return ;
	}
	else
	{
		eat_trace("eat_fs_Close():Close File Handle Success");
	}
	
	/*test delete file interface: eat_fs_Delete*/
	fs_Op_ret_l = (eat_fs_error_enum)eat_fs_Delete((u16*)filename_l);
	if(EAT_FS_NO_ERROR!=fs_Op_ret_l)
	{	
		eat_trace("eat_fs_Delete():Delete File Fail,and Return Error is %d", fs_Op_ret_l);
		return ;
	}
	else
	{
		eat_trace("eat_fs_Delete():Delete File Success");
	}
	#endif
		eat_trace("*********eat_fs_interface_yml222:end*************");

}



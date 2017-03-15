#ifndef _APP_UTILITY_H_
#define _APP_UTILITY_H_

#define DBG(...) eat_trace(__VA_ARGS__)
#define DBG_E(...) eat_trace(__VA_ARGS__)

typedef enum
{
    APP_NVRAM_NO_ERROR = 1, //ok
    APP_NVRAM_PARAM_ERR = -1, //parameters 
    APP_NVRAM_READ_ERR = -2,   //write error
    APP_NVRAM_WRITE_ERR = -3, // read error
    APP_NVRAM_UNKNOW_ERR = -4,
    APP_NVRAM_ERR_TOTAL
}nvram_error_enum;

s32 app_nvram_read(u8 type, void* ptr, u16 len);
s32 app_nvram_save(u8 type, void* ptr, u16 length);

s32 app_fs_open(const u8 * filename, u32 Flag);
int app_fs_close(s32 FileHandle);
int app_fs_read(s32 FileHandle, void * DataPtr, u32 Length, u32* Read);
int app_fs_write(s32 FileHandle, void * DataPtr, u32 Length, u32 * Written);
int app_fs_seek(s32 FileHandle, int Offset, int Whence);
int app_fs_commit(s32 FileHandle);
int app_fs_abort(s32 ActionHandle);
int app_fs_get_file_size(s32 FileHandle, u32 * Size);
int  app_fs_del(const u8* filename);
int app_fs_create_dir(const u8* DirName);

void app_update(char* filename);

void upperstr(char*s, char*d);
s32 bytepos(const u8* pSrc, u16 nSrc, const char* pSub, u16 startPos);

#endif

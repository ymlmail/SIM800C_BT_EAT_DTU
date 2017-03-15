/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   app_demo_ftp.h
 * version       :   0.10
 * Description   :   
 * Authors       :   hanjun.liu
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2014-02-27, hanjun.liu, create originally.
 *
 *--------------------------------------------------------------------
  ********************************************************************/

#ifndef __APP_DEMP_FTP_H__
#define __APP_DEMP_FTP_H__
/********************************************************************
 * Include Files
 ********************************************************************/
#include "eat_interface.h"

 /********************************************************************
 * Macros
 ********************************************************************/
#define FTP_MAX_OUTPUT_BUFFER_SIZE 128
#define FTP_MAX_ATCMD_LENGTH 300
#define FTP_MAX_PARA_LENGTH 15
#define FTP_MAX_PARA_ARRAY_SIZE 30
#define FTP_MAX_OUTPUT_SIZE 1024
#define FTP_ONCE_PUT_SIZE 1000

//PARAS
#define STRING_FTP_CID "FTPCID"
#define STRING_FTP_SERV "FTPSERV"
#define STRING_FTP_UN "FTPUN"
#define STRING_FTP_PW "FTPPW"
#define STRING_FTP_GETNAME "FTPGETNAME"
#define STRING_FTP_GETPATH "FTPGETPATH"
#define STRING_FTP_PUTNAME "FTPPUTNAME"
#define STRING_FTP_PUTPATH "FTPPUTPATH"
#define STRING_FTP_PORT "FTPPORT"
#define STRING_FTP_MODE "FTPMODE"
#define STRING_FTP_TYPE "FTPTYPE"
#define STRING_FTP_PUTOPT "FTPPUTOPT"
#define STRING_FTP_REST "FTPREST"
//ASYNC CMDS
#define STRING_FTP_DELE "FTPDELE"
#define STRING_FTP_SIZE "FTPSIZE"
#define STRING_FTP_MKD "FTPMKD"
#define STRING_FTP_RMD "FTPRMD"
#define STRING_FTP_GETTOFS "FTPGETTOFS"

/********************************************************************
 * Types
 ********************************************************************/
typedef void (*GetDataHandle)(u8 *data, u32 len);
typedef void (*FtpAsyncHandle)(u32 res1, u32 res2, u32 res3);

/********************************************************************
* Local Function declaration
 ********************************************************************/
 //interface function
void eat_ftp_sync_cb_set(ResultNotifyCb cb);

void eat_ftp_async_cb_set(FtpAsyncHandle cb);

void eat_ftp_get_data_handle_set(GetDataHandle handle);

eat_bool eat_ftp_paras_set(u8 **paras, u8 **datas, u8 length, ResultNotifyCb syncCB);

eat_bool eat_ftpget(AtCmdRspCB sync, GetDataHandle dataHandle);

eat_bool eat_ftpput(u8 *pBufer, u32 bufSize, AtCmdRspCB sync);

eat_bool eat_ftp_run_async_cmd(u8 *cmd, u8 **paras, 
	u8 paraLength, ResultNotifyCb syncCB, FtpAsyncHandle asyncCB);

eat_bool eat_sapbr_init(AtCmdRspCB sync);

//for implement interface
static void eat_ftp_para_set_with_index(u8 index, u8 *value, AtCmdRspCB cmdCallBack);

static AtCmdRsp eat_ftp_final_para_set_rsp_hdlr(u8* pRspStr);

static void eat_ftp_para_set_default_cb(eat_bool result);

static AtCmdRsp eat_ftpget_start_rsp_hdlr(u8 *pRspStr);

static AtCmdRsp eat_ftpget_read_rsp_hdlr(u8 *pRspStr);

static AtCmdRsp eat_ftp_put_enable_rsp_hdlr(u8 *pRspStr);

static AtCmdRsp eat_ftp_put_running_rsp_hdlr(u8 *pRspStr);

static AtCmdRsp eat_ftp_put_last_rsp_hdlr(u8 *pRspStr);

static AtCmdRsp eat_ftp_put_end_hdlr(u8 *pRspStr);

static void eat_ftpput_send_data();

static AtCmdRsp eat_ftp_async_at_rsp_hdlr(u8 *pRspStr);

static void eat_ftp_async_at_urc_hdlr(u8* pRspStr, u16 len);

static AtCmdRsp eat_sapbr_init_rsp_hdlr(u8 *pRspStr);

//for test demo
eat_bool eat_module_test_ftp(u8 param1, u8 param2);

static void eat_ftp_test_paras_set();

static void eat_ftp_test_ftpput(u8 mode);

static void eat_ftp_para_set_test_cb(eat_bool result);

static void eat_ftpget_test_cb(eat_bool result);

static void eat_ftpget_test_data_handle(u8 *data, u32 len);

static void eat_ftpput_test_cb(eat_bool result);

static void eat_ftp_async_at_test_cb(eat_bool result);

static void eat_ftp_async_at_urc_test_cb(u32 res1, u32 res2, u32 res3);

static void eat_ftp_sapbr_test_cb(eat_bool result);

#endif //__APP_DEMP_FTP_H__

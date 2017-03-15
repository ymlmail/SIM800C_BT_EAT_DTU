/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   app_demo_ftp.c
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
 * File Description
 * AT+CEAT=parma1,param2
 * param1 param2 
 *   1      1    
 *   1      2    ftp params set(ftpcid, ftpun, ftppw...)
 *
 *   2      1    ftpget
 *   2      2    ftpput
 *   
 *   3      1    ftpdel
 *   3      2    ftpsize
 *   3      3    ftpmkd
 *   3      4    ftprmd
 *   3      5    ftpgettofs(full params)
 *   3      6    ftpgettofs
 *
 *--------------------------------------------------------------------
 ********************************************************************/
/********************************************************************
 * Include Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>
#include <string.h>

#include "eat_modem.h"
#include "eat_interface.h"
#include "eat_uart.h"

#include "app_at_cmd_envelope.h"
#include "app_demo_ftp.h"

/********************************************************************
 * Macros
 ********************************************************************/

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
static u8 *ftpParas[] = 
	{
		STRING_FTP_CID,//0
		STRING_FTP_SERV,
        STRING_FTP_UN,
		STRING_FTP_PW,
		STRING_FTP_GETNAME,
		STRING_FTP_GETPATH,//5
		STRING_FTP_PUTNAME,
		STRING_FTP_PUTPATH,
		STRING_FTP_PORT,
		STRING_FTP_MODE,
		STRING_FTP_TYPE,  //10
		STRING_FTP_PUTOPT,
		STRING_FTP_REST
	};

static u8 asyncAtCmd[FTP_MAX_PARA_LENGTH] = {0};

static ResultNotifyCb ftpSyncCB;  //for results return not too long
static FtpAsyncHandle ftpAsyncCB; //for result may return after a long time
static GetDataHandle ftpGetDataHandle;  //for data handle in ftpget

//for ftpput
static u8 *putBuffer = NULL;
static u32 putLength = 0;
static u32 putPoint = 0;

//for test put
static u8 tBuffer[2048] = {0};

/********************************************************************
* Local Function declaration
 ********************************************************************/

/********************************************************************
* Local Function
 ********************************************************************/
 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_module_test_ftp
 *  Description: test ftp function
 *        Input:
 			param1:
 			param2:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
eat_bool eat_module_test_ftp(u8 param1, u8 param2)
{
	eat_trace("eat_module_test_ftp: param1 = %d. param2 = %d", param1, param2);
	if(param1 == 1)
	{
		if(param2 == 1)
		{
			eat_sapbr_init(eat_ftp_sapbr_test_cb);
		}
		else if(param2 == 2)
		{
			eat_ftp_test_paras_set();
		}
	}
	else if(param1 == 2)
	{
		if(param2 == 1)
		{
			eat_ftpget(eat_ftpget_test_cb, eat_ftpget_test_data_handle);
		}
		else if(param2 == 2)
		{
			eat_ftp_test_ftpput(0);
		}
		else if(param2 == 3)
		{
			eat_ftp_test_ftpput(1);
		}
	}
	else if(param1 == 3)
	{
		if(param2 == 1)
		{
			eat_ftp_run_async_cmd(STRING_FTP_DELE, NULL, 0, 
				eat_ftp_async_at_test_cb, eat_ftp_async_at_urc_test_cb);
		}
		else if(param2 == 2)
		{
			eat_ftp_run_async_cmd(STRING_FTP_SIZE, NULL, 0, 
				eat_ftp_async_at_test_cb, eat_ftp_async_at_urc_test_cb);
		}
		else if(param2 == 3)
		{
			eat_ftp_run_async_cmd(STRING_FTP_MKD, NULL, 0, 
				eat_ftp_async_at_test_cb, eat_ftp_async_at_urc_test_cb);
		}
		else if(param2 == 4)
		{
			eat_ftp_run_async_cmd(STRING_FTP_RMD, NULL, 0, 
				eat_ftp_async_at_test_cb, eat_ftp_async_at_urc_test_cb);
		}
		else if(param2 == 5)
		{
			u8 *paras[] = 
				{
					"0",//save in nvram
					"1.txt",//file name, file will save at: c:\user\ftp\1.txt
					"0",//auto retry times, default is 3 times
					"0" //auto retry interveal, default is 5s
				};
			eat_ftp_run_async_cmd(STRING_FTP_GETTOFS, paras, 4, 
				eat_ftp_async_at_test_cb, eat_ftp_async_at_urc_test_cb);
		}
		else if(param2 == 6)
		{
			u8 *paras[] = 
				{
					"0",
					"2.txt",
				};
			
			eat_ftp_run_async_cmd(STRING_FTP_GETTOFS, paras, 2, 
				eat_ftp_async_at_test_cb, eat_ftp_async_at_urc_test_cb);
		}
	}
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_para_set_test_cb
 *  Description: callback function to output error msg in ftp para set in test!
 *        Input:
 			result:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static void eat_ftp_para_set_test_cb(eat_bool result)
{
	u8 buffer[FTP_MAX_OUTPUT_BUFFER_SIZE] = {0};

	eat_trace("eat_ftp_para_set_test_cb: result = %d", result);
	sprintf(buffer, "\r\neat_ftp_para_set_test_cb, result = %d\r\n", result);
	eat_uart_write(EAT_UART_1, buffer, strlen(buffer));
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_para_set_test_cb
 *  Description: callback function to output error msg in ftp para set in test!
 *        Input:
 			result:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static void eat_ftpget_test_cb(eat_bool result)
{
	u8 buffer[FTP_MAX_OUTPUT_BUFFER_SIZE] = {0};

	eat_trace("eat_ftpget_test_cb: result = %d", result);
	sprintf(buffer, "\r\neat_ftpget_test_cb, result = %d\r\n", result);
	eat_uart_write(EAT_UART_1, buffer, strlen(buffer));
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftpget_test_data_handle
 *  Description: test to handle data in FTPGET 
 *        Input:
 			data: data buffer
 			len: buffer size
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static void eat_ftpget_test_data_handle(u8 *data, u32 len)
{
	u32 outputLen = 0;
	u8 *p = data;

	eat_trace("eat_ftpget_test_data_handle: len = %d", len);
	while(len > 0)
	{
		outputLen = len < FTP_MAX_OUTPUT_SIZE ? len : FTP_MAX_OUTPUT_SIZE;
		//eat_trace("eat_ftpget_test_data_handle: %s", data);
		eat_uart_write(EAT_UART_1, p, outputLen);
		len -= outputLen;
		p += outputLen;
	}
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftpput_test_cb
 *  Description: 
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static void eat_ftpput_test_cb(eat_bool result)
{
	u8 buffer[FTP_MAX_OUTPUT_BUFFER_SIZE] = {0};

	eat_trace("eat_ftpput_test_cb: result = %d", result);
	sprintf(buffer, "\r\eat_ftpput_test_cb, result = %d\r\n", result);
	eat_uart_write(EAT_UART_1, buffer, strlen(buffer));
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftpput_test_cb
 *  Description: 
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static void eat_ftp_async_at_test_cb(eat_bool result)
{
	u8 buffer[FTP_MAX_OUTPUT_BUFFER_SIZE] = {0};

	eat_trace("eat_ftp_async_at_test_cb: at = %s, result = %d", asyncAtCmd, result);
	sprintf(buffer, "\r\eat_ftp_async_at_test_cb, result = %d\r\n", result);
	eat_uart_write(EAT_UART_1, buffer, strlen(buffer));
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_async_at_urc_test_cb
 *  Description: 
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static void eat_ftp_async_at_urc_test_cb(u32 res1, u32 res2, u32 res3)
{
	u8 buffer[FTP_MAX_OUTPUT_BUFFER_SIZE] = {0};

	eat_trace("eat_ftp_async_at_test_cb: at = %s, res1 = %d, res2 = %d, res3 = %d", 
		asyncAtCmd, res1, res2, res3);
	sprintf(buffer, "\r\eat_ftp_async_at_urc_test_cb, at = %s \
		res1 = %d, res2 = %d, res3 = %d\r\n", asyncAtCmd, res1, res2, res3);
	eat_uart_write(EAT_UART_1, buffer, strlen(buffer));
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_sapbr_test_cb
 *  Description: 
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static void eat_ftp_sapbr_test_cb(eat_bool result)
{
	u8 buffer[FTP_MAX_OUTPUT_BUFFER_SIZE] = {0};

	eat_trace("eat_ftp_sapbr_test_cb: result = %d", result);
	sprintf(buffer, "\r\eat_ftp_sapbr_test_cb, result = %d\r\n", result);
	eat_uart_write(EAT_UART_1, buffer, strlen(buffer));
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_sync_cb_set
 *  Description: 
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
void eat_ftp_sync_cb_set(ResultNotifyCb cb)
{
	ftpSyncCB = cb;
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_async_cb_set
 *  Description: 
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
void eat_ftp_async_cb_set(FtpAsyncHandle cb)
{
	ftpAsyncCB = cb;
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_get_data_handle_set
 *  Description: 
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static void eat_ftp_get_data_handle_set(GetDataHandle handle)
{
	ftpGetDataHandle = handle;
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_test_paras_set
 *  Description: test para set
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static void eat_ftp_test_paras_set()
{
	u8 *paras[] = 
		{
			STRING_FTP_CID,//0
			STRING_FTP_SERV,
	        STRING_FTP_UN,
			STRING_FTP_PW,
			STRING_FTP_GETNAME,
			STRING_FTP_GETPATH,//5
			STRING_FTP_PUTNAME,
			STRING_FTP_PUTPATH,
			STRING_FTP_PORT
		};
	u8 *datas[] =
		{
			"1",//0
			"116.228.221.52",
	        "sim.mkr",
			"hj135",
			"henryTest.txt",
			"/henry/",//5
			"henryTest.txt",
			"/henry/",
			"21"
		};
	u8 size = sizeof(paras)/sizeof(u8 *);
	eat_bool result = FALSE;

	result = eat_ftp_paras_set(paras, datas, size, eat_ftp_para_set_test_cb);
	eat_trace("eat_ftp_test_paras_set: result = %d", result);
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_test_ftpput
 *  Description: test ftpput
 *        Input:
 			mode: 0 small data, 1 large data
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static void eat_ftp_test_ftpput(u8 mode)
{
	u16 i = 0;

	eat_trace("eat_ftp_test_ftpput: mode = %d", mode);
	if(mode == 0)
	{
		for(i = 0; i < 10; i++)
		{
			tBuffer[i] = (i % 9) + 48;
		}
		eat_ftpput(tBuffer, 10, eat_ftpput_test_cb);
	}
	else
	{
		for(i = 0; i < 2048; i++)
		{
			tBuffer[i] = (i % 9) + 48;
		}
		eat_ftpput(tBuffer, 2048, eat_ftpput_test_cb);
	}
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_paras_set
 *  Description: set ftp paras, you could specify PARAS you want to set
 *        Input:
 			paras: string array, paras to set
 			datas: string array, data of paras
 			length: length of array
 			syncCB: you can handle result here
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
eat_bool eat_ftp_paras_set(u8 **paras, u8 **datas, u8 length, ResultNotifyCb syncCB)
{
	u8 i = 0, j = 0;
	u8 arraySize = sizeof(ftpParas)/sizeof(u8 *);
	u8 indexes[FTP_MAX_PARA_ARRAY_SIZE] = {0xFF};
	eat_bool result = FALSE;

	if(length > FTP_MAX_PARA_ARRAY_SIZE)
	{
		eat_trace("eat_ftp_paras_set: length too large! length = %d, FTP_MAX_PARA_ARRAY_SIZE = %d",
			length, FTP_MAX_PARA_ARRAY_SIZE);
		return FALSE;
	}
	
	for(i = 0; i < length; i++)
	{//convert to upper
		for(j = 0; j < FTP_MAX_PARA_LENGTH; j++)
		{
			if(paras[i][j] >= 97 && paras[i][j] <= 122)
			{
				paras[i][j] = paras[i][j] - 32;
			}
		}
	}
	for(i = 0; i < length; i++)
	{//validate para
		for(j = 0; j < arraySize; j++)
		{
			if(strcmp(paras[i], ftpParas[j]) == 0)
			{//store para index, then we needn't compare string again
				indexes[i] = j;
			}
		}
		if(indexes[i] == 0xFF)
		{//paras[i] not found in ftpParas, it's a wrong para
			eat_trace("eat_ftp_paras_set: wrong para! i = %d, p = %s", i, paras[i]);
			return FALSE;
		}
	}
	//set fun size
	result = simcom_atcmd_queue_fun_set(length, eat_ftp_para_set_default_cb);
	if(result != TRUE)
	{
		eat_trace("eat_ftp_paras_set: not enough mem!");
		return FALSE;
	}
	
	//default cb should set once. because it just deal error 
	//and fun will stop if an error occured
	eat_ftp_sync_cb_set(syncCB);
	for(i = 0; i < length - 1; i++)
	{//we set i < length - 1, due to the last ATCMD should set a special callback to output result
		eat_ftp_para_set_with_index(indexes[i], datas[i], NULL);
	}
	eat_ftp_para_set_with_index(indexes[length - 1], datas[length - 1], eat_ftp_final_para_set_rsp_hdlr);
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_para_set_with_index
 *  Description: set para with index
 *        Input:
 			index:
 			value: value of para
 			cmdCallBack: callback function you want to deal result, NULL means use default
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static void eat_ftp_para_set_with_index(u8 index, u8 *value, AtCmdRspCB cmdCallBack)
{
	u8 pAtCmd[FTP_MAX_ATCMD_LENGTH] = {0};
	AtCmdEntity atCmdInit = {0};
	
	eat_trace("eat_ftp_para_set_with_index: index = %d, value = %s, cb = %d", index, value, cmdCallBack);
	sprintf(pAtCmd,"AT+%s=%s%s", ftpParas[index], value, AT_CMD_END);
    atCmdInit.p_atCmdStr = pAtCmd;
    atCmdInit.cmdLen = strlen(pAtCmd);
	atCmdInit.p_atCmdCallBack = cmdCallBack;
	simcom_atcmd_queue_append(atCmdInit);
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_para_set_default_cb
 *  Description: callback function to output error msg in ftp para set!
 *        Input:
 			result:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static void eat_ftp_para_set_default_cb(eat_bool result)
{
	eat_trace("eat_ftp_para_set_default_cb: result = %d", result);
	if(ftpSyncCB != NULL)
	{
		ftpSyncCB(result);
		ftpSyncCB = NULL;
	}
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_final_para_set_rsp_hdlr
 *  Description: just hdlr final para
 *        Input:
 			index:
 			value: value of para
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static AtCmdRsp eat_ftp_final_para_set_rsp_hdlr(u8 *pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR","OK"};
    s16  rspType = -1;
    u8  i = 0;
    u8  cid = 0;
    u8  *p = pRspStr;
	
    while(p) 
	{
        /* ignore \r \n */
        while(AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }
        for(i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if(strncmp(rspStrTable[i], p, strlen(rspStrTable[i])) == 0)
            {
                rspType = i;
                break;
            }
        }
        p = (u8*)strchr(p, AT_CMD_LF);
    }

	eat_trace("eat_ftp_final_para_set_rsp_hdlr: rspType = %d", rspType);
    switch(rspType)
    {
        case 0:  /* ERROR */
		{
	        rspValue = AT_RSP_ERROR;
			if(ftpSyncCB != NULL)
			{
				ftpSyncCB(FALSE);
				ftpSyncCB = NULL;
			}
	        break;
        }
        case 1:  /* OK */
		{
	        rspValue = AT_RSP_FUN_OVER;
			if(ftpSyncCB != NULL)
			{
				ftpSyncCB(TRUE);
				ftpSyncCB = NULL;
			}
	        break;
        }
        default:
		{
	        rspValue = AT_RSP_WAIT;
	        break;
        }
    }
	
    return rspValue;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftpput
 *  Description: ftp put function.
 *        Input:
 			pBufer: put buffer
 			bufSize: size of buffer
 			sync: you can handle put result here
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
eat_bool eat_ftpput(u8 *pBufer, u32 bufSize, AtCmdRspCB sync)
{
	AtCmdEntity atCmdInit[] =
		{
	        {"AT+FTPPUT=1"AT_CMD_END, 0, eat_ftp_put_enable_rsp_hdlr},
			{"AT+FTPPUT=2,", 0, eat_ftp_put_running_rsp_hdlr},
			{"AT+FTPPUT=2,", 0, eat_ftp_put_last_rsp_hdlr},
			{"AT+FTPPUT=2,0"AT_CMD_END, 0, eat_ftp_put_end_hdlr}
		};
	u8 pAtCmd[50] = {0};
	u8 pAtCmdLast[50] = {0};
	u32 lastSize = 0;
	eat_bool result = FALSE;

	if(putBuffer == NULL)
	{
		putBuffer = pBufer;
		putLength = bufSize;
		putPoint = 0;
	}
	else
	{
		eat_trace("eat_ftpput: putBuffer != NULL, may still running");
		return FALSE;
	}
	//eat_trace("eat_ftpput: rspValue = %d", rspValue);
    atCmdInit[0].cmdLen = strlen(atCmdInit[0].p_atCmdStr);
	//put data 1000BYTE once
	sprintf(pAtCmd,"AT+FTPPUT=2,%d%s", FTP_ONCE_PUT_SIZE, AT_CMD_END);
	atCmdInit[1].p_atCmdStr = pAtCmd;
	atCmdInit[1].cmdLen = strlen(pAtCmd);
	//get last put size
	lastSize = bufSize % FTP_ONCE_PUT_SIZE;
	sprintf(pAtCmdLast,"AT+FTPPUT=2,%d%s", lastSize, AT_CMD_END);
	atCmdInit[2].p_atCmdStr = pAtCmdLast;
	atCmdInit[2].cmdLen = strlen(pAtCmdLast);
	//end
	atCmdInit[3].cmdLen = strlen(atCmdInit[3].p_atCmdStr);
	
	eat_ftp_sync_cb_set(sync);
	result = simcom_atcmd_queue_fun_append(atCmdInit, 4, NULL);
	eat_trace("eat_ftpput: result = %d", result);

	return TRUE;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftpget
 *  Description: 
 *        Input:
 			sync: you can handle result here
 			dataHandle: handle download data
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
eat_bool eat_ftpget(AtCmdRspCB sync, GetDataHandle dataHandle)
{
	AtCmdEntity atCmdInit[] =
		{
	        {"AT+FTPGET=1"AT_CMD_END, 0, eat_ftpget_start_rsp_hdlr},
			{"AT+FTPGET=2,1000"AT_CMD_END, 0, eat_ftpget_read_rsp_hdlr}
		};

	eat_trace("eat_ftpget: sync = %d, dataHandle = %d", sync, dataHandle);
	eat_ftp_sync_cb_set(sync);
	eat_ftp_get_data_handle_set(dataHandle);
    atCmdInit[0].cmdLen = strlen(atCmdInit[0].p_atCmdStr);
	atCmdInit[1].cmdLen = strlen(atCmdInit[1].p_atCmdStr);

	return simcom_atcmd_queue_fun_append(atCmdInit, 2, NULL);
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftpget_start_rsp_hdlr
 *  Description: 
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static AtCmdRsp eat_ftpget_start_rsp_hdlr(u8 *pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR", "+FTPGET: 1,"};
    s16  rspType = -1;
    u8  i = 0;
    u8  cid = 0;
    u8  *p = pRspStr;
	u8 buffer[FTP_MAX_OUTPUT_BUFFER_SIZE] = {0};
	u8 status = 0;
	
    while(p) 
	{
        /* ignore \r \n */
        while(AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }
        for(i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if(strncmp(rspStrTable[i], p, strlen(rspStrTable[i])) == 0)
            {
                rspType = i;
				if(rspType == 1)
				{
                    sscanf(p + strlen(rspStrTable[rspType]), "%d", &status);
                }
                break;
            }
        }
        p = (u8*)strchr(p, AT_CMD_LF);
    }

	eat_trace("eat_ftpget_start_rsp_hdlr: rspType = %d", rspType);
    switch (rspType)
    {
        case 0:  /* ERROR */
		{
	        rspValue = AT_RSP_ERROR;
			if(ftpSyncCB)
			{
				ftpSyncCB(FALSE);
				ftpSyncCB = NULL;
			}
	        break;
        }
        case 1:  /* OK */
		{
			if(status == 1)
			{
	        	rspValue = AT_RSP_CONTINUE;
			}
			else
			{
		        rspValue = AT_RSP_ERROR;
				if(ftpSyncCB)
				{
					ftpSyncCB(FALSE);
					ftpSyncCB = NULL;
				}
			}
	        break;
        }
        default:
		{
	        rspValue = AT_RSP_WAIT;
	        break;
        }
    }
	
    return rspValue;
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftpget_start_rsp_hdlr
 *  Description: 
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static AtCmdRsp eat_ftpget_read_rsp_hdlr(u8 *pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR", "+FTPGET: "};
    s16  rspType = -1;
    u8  i = 0;
    u8  cid = 0;
    u8  *p = pRspStr;
	u8 getMode = 0;
	u32 getSize = 0;
	
    while(p) 
	{
        /* ignore \r \n */
        while(AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }
        for(i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if(strncmp(rspStrTable[i], p, strlen(rspStrTable[i])) == 0)
            {
                rspType = i;
				if(rspType == 1)
				{
                    sscanf(p + strlen(rspStrTable[rspType]), "%d,%d", &getMode, &getSize);
                }
                break;
            }
        }
        p = (u8*)strchr(p, AT_CMD_LF);
		if(getMode == 2 && getSize > 0)
		{
			break;
		}
    }

	eat_trace("eat_ftpget_read_rsp_hdlr: rspType = %d, getMode = %d, getSize = %d", 
		rspType, getMode, getSize);
    switch (rspType)
    {
        case 0:  /* ERROR */
		{
	        rspValue = AT_RSP_ERROR;
			if(ftpSyncCB)
			{
				ftpSyncCB(FALSE);
				ftpSyncCB = NULL;
			}
	        break;
        }
        case 1:  /* +FTPGET:  */
		{
			if(getMode == 1 && getSize == 0)
			{//download finish
				if(ftpSyncCB)
				{
					ftpSyncCB(TRUE);
					ftpSyncCB = NULL;
				}
				rspValue = AT_RSP_FUN_OVER;
			}
			else if(getMode == 2 && getSize == 0)
			{//download buffer empty
				rspValue = AT_RSP_WAIT;
			}
			else if(getMode == 1 && getSize == 1)
			{//download buffer avaliable
				rspValue = AT_RSP_STEP;
			}
			else if(getMode == 2 && getSize != 0)
			{//continue read download buffer until it is empty
				if(ftpGetDataHandle)
				{
					p = (u8 *)strchr(p, AT_CMD_LF);
					p++;
					ftpGetDataHandle(p, getSize);
				}
				if(getSize >= 1000)
				{
					rspValue = AT_RSP_STEP;
				}
				else
				{
					rspValue = AT_RSP_WAIT;
				}
			}
			else
			{//error
				if(ftpSyncCB)
				{
					ftpSyncCB(FALSE);
					ftpSyncCB = NULL;
				}
				rspValue = AT_RSP_ERROR;
			}
	        break;
        }
        default:
		{
	        rspValue = AT_RSP_WAIT;
	        break;
        }
    }
	
    return rspValue;
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_put_enable_rsp_hdlr
 *  Description: handle rsp of AT+FTPPUT=1,1 
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static AtCmdRsp eat_ftp_put_enable_rsp_hdlr(u8 *pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR", "+FTPPUT: "};
    s16  rspType = -1;
    u8  i = 0;
    u8  cid = 0;
    u8  *p = pRspStr;
	u8 putOperation = 0;
	u32 putStatus = 0;
	
    while(p) 
	{
        /* ignore \r \n */
        while(AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }
        for(i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if(strncmp(rspStrTable[i], p, strlen(rspStrTable[i])) == 0)
            {
                rspType = i;
				if(rspType == 1)
				{
                    sscanf(p + strlen(rspStrTable[rspType]), "%d,%d", &putOperation, &putStatus);
                }
                break;
            }
        }
        p = (u8*)strchr(p, AT_CMD_LF);
    }

	eat_trace("eat_ftp_put_enable_rsp_hdlr: rspType = %d, putOperation = %d, putStatus = %d", 
		rspType, putOperation, putStatus);
    switch(rspType)
    {
        case 0:  /* ERROR */
		{
	        rspValue = AT_RSP_ERROR;
			if(ftpSyncCB)
			{
				ftpSyncCB(FALSE);
				ftpSyncCB = NULL;
				putBuffer = NULL;
			}
	        break;
        }
        case 1:  /* +FTPPUT: */
		{
			if(putOperation == 1)
			{
				if(putStatus == 1)
				{//first put will always larger than 1KB, do not mind size
					if(putLength <= FTP_ONCE_PUT_SIZE)
					{//just put once
						rspValue = AT_RSP_STEP + 2;
					}
					else
					{
						rspValue = AT_RSP_CONTINUE;
					}
				}
				else
				{
					rspValue = AT_RSP_ERROR;
					if(ftpSyncCB)
					{
						ftpSyncCB(FALSE);
						ftpSyncCB = NULL;
						putBuffer = NULL;
					}
				}
			}
			else
			{
				rspValue = AT_RSP_ERROR;
				if(ftpSyncCB)
				{
					ftpSyncCB(FALSE);
					ftpSyncCB = NULL;
					putBuffer = NULL;
				}
			}
	        break;
        }
        default:
		{
	        rspValue = AT_RSP_WAIT;
	        break;
        }
    }
	
    return rspValue;
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_put_running_rsp_hdlr
 *  Description: handle rsp of AT+FTPPUT=2,1000
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static AtCmdRsp eat_ftp_put_running_rsp_hdlr(u8 *pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR", "+FTPPUT: "};
    s16  rspType = -1;
    u8  i = 0;
    u8  cid = 0;
    u8  *p = pRspStr;
	u8 putOperation = 0;
	u32 putStatus = 0;
	u32 putSize = 0;
	u32 leftSize = 0;
	
    while(p) 
	{
        /* ignore \r \n */
        while(AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }
        for(i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if(strncmp(rspStrTable[i], p, strlen(rspStrTable[i])) == 0)
            {
                rspType = i;
				if(rspType == 1)
				{
                    sscanf(p + strlen(rspStrTable[rspType]), "%d,%d", &putOperation, &putStatus);
                }
				if(putStatus == 1 && putOperation == 1)
				{
					sscanf(p + strlen("+FTPPUT: 1,1,"), "%d", &putSize);
				}
                break;
            }
        }
        p = (u8*)strchr(p, AT_CMD_LF);
    }

	eat_trace("eat_ftp_put_running_rsp_hdlr: rspType = %d, putOperation = %d, \
		putStatus = %d, putSize = %d", rspType, putOperation, putStatus, putSize);
	leftSize = putLength - putPoint;
    switch (rspType)
    {
        case 0:  /* ERROR */
		{
	        rspValue = AT_RSP_ERROR;
			if(ftpSyncCB)
			{
				ftpSyncCB(FALSE);
				ftpSyncCB = NULL;
				putBuffer = NULL;
			}
	        break;
        }
        case 1:  /* +FTPPUT: */
		{
			if(putOperation == 1)
			{
				if(putStatus == 1)
				{
					if(leftSize > FTP_ONCE_PUT_SIZE && putSize > FTP_ONCE_PUT_SIZE)
					{//continue put
						rspValue = AT_RSP_STEP;
					}
					else if(leftSize <= FTP_ONCE_PUT_SIZE && putSize >= leftSize)
					{//go to last put
						rspValue = AT_RSP_CONTINUE;
					}
					else
					{//not enough put buffer, wait size larger
						rspValue = AT_RSP_WAIT;
					}
				}
				else
				{
					rspValue = AT_RSP_ERROR;
					if(ftpSyncCB)
					{
						ftpSyncCB(FALSE);
						ftpSyncCB = NULL;
						putBuffer = NULL;
					}
				}
			}
			else if(putOperation == 2)
			{//input data to module
				eat_ftpput_send_data();
				rspValue = AT_RSP_WAIT;
			}
			else
			{
				rspValue = AT_RSP_ERROR;
				if(ftpSyncCB)
				{
					ftpSyncCB(FALSE);
					ftpSyncCB = NULL;
					putBuffer = NULL;
				}
			}
	        break;
        }
        default:
		{
	        rspValue = AT_RSP_WAIT;
	        break;
        }
    }
	
    return rspValue;
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_put_last_rsp_hdlr
 *  Description: handle rsp of AT+FTPPUT=2,<last size>
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static AtCmdRsp eat_ftp_put_last_rsp_hdlr(u8 *pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR", "+FTPPUT: "};
    s16  rspType = -1;
    u8  i = 0;
    u8  cid = 0;
    u8  *p = pRspStr;
	u8 putOperation = 0;
	u32 putStatus = 0;
	
    while(p) 
	{
        /* ignore \r \n */
        while(AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }
        for(i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if(strncmp(rspStrTable[i], p, strlen(rspStrTable[i])) == 0)
            {
                rspType = i;
				if(rspType == 1)
				{
                    sscanf(p + strlen(rspStrTable[rspType]), "%d,%d", &putOperation, &putStatus);
                }
                break;
            }
        }
        p = (u8*)strchr(p, AT_CMD_LF);
    }

	eat_trace("eat_ftp_put_last_rsp_hdlr: rspType = %d, putOperation = %d, \
		putStatus = %d", rspType, putOperation, putStatus);
    switch (rspType)
    {
        case 0:  /* ERROR */
		{
	        rspValue = AT_RSP_ERROR;
			if(ftpSyncCB)
			{
				ftpSyncCB(FALSE);
				ftpSyncCB = NULL;
				putBuffer = NULL;
			}
	        break;
        }
        case 1:  /* +FTPPUT: */
		{
			if(putOperation == 1)
			{
				if(putStatus == 1)
				{//end put
					rspValue = AT_RSP_CONTINUE;
				}
				else
				{
					rspValue = AT_RSP_ERROR;
					if(ftpSyncCB)
					{
						ftpSyncCB(FALSE);
						ftpSyncCB = NULL;
						putBuffer = NULL;
					}
				}
			}
			else if(putOperation == 2)
			{//input data to module
				eat_ftpput_send_data();
				rspValue = AT_RSP_WAIT;
			}
			else
			{
				rspValue = AT_RSP_ERROR;
				if(ftpSyncCB)
				{
					ftpSyncCB(FALSE);
					ftpSyncCB = NULL;
					putBuffer = NULL;
				}
			}
	        break;
        }
        default:
		{
	        rspValue = AT_RSP_WAIT;
	        break;
        }
    }
	
    return rspValue;
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_put_end_hdlr
 *  Description: 
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static AtCmdRsp eat_ftp_put_end_hdlr(u8 *pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR", "+FTPPUT: "};
    s16  rspType = -1;
    u8  i = 0;
    u8  cid = 0;
    u8  *p = pRspStr;
	u8 putOperation = 0;
	u8 putStatus = 0;
	
    while(p) 
	{
        /* ignore \r \n */
        while(AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }
        for(i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if(strncmp(rspStrTable[i], p, strlen(rspStrTable[i])) == 0)
            {
                rspType = i;
				if(rspType == 1)
				{
                    sscanf(p + strlen(rspStrTable[rspType]), "%d,%d", &putOperation, &putStatus);
                }
                break;
            }
        }
        p = (u8*)strchr(p, AT_CMD_LF);
    }

	eat_trace("eat_ftp_put_end_hdlr: rspType = %d, putOperation = %d, \
		putStatus = %d", rspType, putOperation, putStatus);
    switch (rspType)
    {
        case 0:  /* ERROR */
		{
	        rspValue = AT_RSP_ERROR;
			if(ftpSyncCB)
			{
				ftpSyncCB(FALSE);
				ftpSyncCB = NULL;
				putBuffer = NULL;
			}
	        break;
        }
        case 1:  /* +FTPPUT: */
		{
			if(putOperation == 1)
			{
				if(putStatus == 0)
				{//end function
					rspValue = AT_RSP_FUN_OVER;
					if(ftpSyncCB)
					{
						ftpSyncCB(TRUE);
						ftpSyncCB = NULL;
						putBuffer = NULL;
					}
				}
				else
				{
					rspValue = AT_RSP_ERROR;
					if(ftpSyncCB)
					{
						ftpSyncCB(FALSE);
						ftpSyncCB = NULL;
						putBuffer = NULL;
					}
				}
			}
			else
			{
				rspValue = AT_RSP_ERROR;
				if(ftpSyncCB)
				{
					ftpSyncCB(FALSE);
					ftpSyncCB = NULL;
					putBuffer = NULL;
				}
			}
	        break;
        }
        default:
		{
	        rspValue = AT_RSP_WAIT;
	        break;
        }
    }
	
    return rspValue;
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftpput_send_data
 *  Description: 
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static void eat_ftpput_send_data()
{
	u32 actLen = 0;
	u32 wantLen = FTP_ONCE_PUT_SIZE;
	
	if(putBuffer == NULL)
	{
		eat_trace("eat_ftpput_send_data: putBuffer == NULL");
		return;
	}
	eat_trace("eat_ftpput_send_data: putLength = %d, putPoint = %d, FTP_ONCE_PUT_SIZE = %d",
		putLength, putPoint, FTP_ONCE_PUT_SIZE);
	if(putLength - putPoint < FTP_ONCE_PUT_SIZE)
	{
		wantLen = putLength - putPoint;
	}

	actLen = eat_modem_write(putBuffer + putPoint, wantLen);
	eat_trace("eat_ftpput_send_data: wantLen = %d, actLen = %d", wantLen, actLen);
	putPoint += actLen;
	if(putPoint >= putLength)
	{
		putBuffer = NULL;
		putPoint = 0;
		putLength= 0;
	}
}

 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_run_async_cmd
 *  Description: run FTP CMDS which return results async
 *        Input:
 			cmd:
 			paras:
 			paraLength:
 			asyncCB:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
eat_bool eat_ftp_run_async_cmd(u8 *cmd, u8 **paras, 
	u8 paraLength, ResultNotifyCb syncCB, FtpAsyncHandle asyncCB)
{
	u8 pAtCmd[FTP_MAX_ATCMD_LENGTH] = {0};
	u8 *cmds[] = 
		{
			STRING_FTP_DELE, //0
			STRING_FTP_SIZE,
			STRING_FTP_MKD,
			STRING_FTP_RMD,
			STRING_FTP_GETTOFS
		};
	AtCmdEntity atCmdInit = {0};
	u8 i = 0;
	u8 length = sizeof(cmds)/sizeof(u8 *);
	u8 index = 0xFF;
	UrcEntity urc = {0};

	for(i = 0; i < strlen(cmd); i++)
	{
		if(cmd[i] >= 97 && cmd[i] <= 122)
		{
			cmd[i] = cmd[i] - 32;
		}
	}
	eat_trace("eat_ftp_run_async_cmd: para = %s", cmd);
	for(i = 0; i < length; i++)
	{
		if(strcmp(cmds[i], cmd) == 0)
		{
			index = i;
			break;
		}
	}
	if(index == 0xFF)
	{
		eat_trace("eat_ftp_run_async_cmd: cmd not found");
		return FALSE;
	}
	if(i < 4)
	{
		sprintf(pAtCmd,"AT+%s%s", cmd, AT_CMD_END);
	}
	else if(i == 4)
	{//get to fs
		if(paraLength == 2)
		{
			sprintf(pAtCmd,"AT+%s=%s,%s%s", cmd, 
				paras[0], paras[1], AT_CMD_END);
		}
		else if(paraLength == 4)
		{
			sprintf(pAtCmd,"AT+%s=%s,%s,%s,%s%s", cmd, 
				paras[0], paras[1], paras[2], paras[3], AT_CMD_END);
		}
		else
		{
			eat_trace("eat_ftp_run_async_cmd: wrong param");
			return FALSE;
		}
	}
	if(asyncCB != NULL)
	{
		sprintf(asyncAtCmd, "+%s: ", cmd);
		urc.p_urcStr = asyncAtCmd;
		urc.p_urcCallBack = eat_ftp_async_at_urc_hdlr;
		simcom_urc_queue_append(urc);
		eat_ftp_async_cb_set(asyncCB);
	}
	eat_ftp_sync_cb_set(syncCB);
	atCmdInit.p_atCmdStr = pAtCmd;
	atCmdInit.cmdLen = strlen(pAtCmd);
	eat_trace("eat_ftp_run_async_cmd: cmd = %s, len = %d", pAtCmd, atCmdInit.cmdLen);
	atCmdInit.p_atCmdCallBack = eat_ftp_async_at_rsp_hdlr;
	simcom_atcmd_queue_fun_set(1, NULL);
	simcom_atcmd_queue_append(atCmdInit);

	return TRUE;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_async_at_rsp_hdlr
 *  Description: handle async at rsp
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static AtCmdRsp eat_ftp_async_at_rsp_hdlr(u8 *pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR", "OK"};
    s16  rspType = -1;
    u8  i = 0;
    u8  cid = 0;
    u8  *p = pRspStr;
	
    while(p) 
	{
        /* ignore \r \n */
        while(AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }
        for(i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if(strncmp(rspStrTable[i], p, strlen(rspStrTable[i])) == 0)
            {
                rspType = i;
                break;
            }
        }
        p = (u8*)strchr(p, AT_CMD_LF);
    }

	eat_trace("eat_ftp_async_at_rsp_hdlr: rspType = %d", rspType);
    switch(rspType)
    {
        case 0:  /* ERROR */
		{
	        rspValue = AT_RSP_ERROR;
			if(ftpSyncCB != NULL)
			{
				ftpSyncCB(FALSE);
				ftpSyncCB = NULL;
			}
	        break;
        }
        case 1:  /* OK */
		{
	        rspValue = AT_RSP_FUN_OVER;
			if(ftpSyncCB != NULL)
			{
				ftpSyncCB(TRUE);
				ftpSyncCB = NULL;
			}
	        break;
        }
        default:
		{
	        rspValue = AT_RSP_WAIT;
	        break;
        }
    }
	
    return rspValue;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_ftp_async_at_urc_hdlr
 *  Description: handle async at urc
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static void eat_ftp_async_at_urc_hdlr(u8* pRspStr, u16 len)
{
	eat_bool result = EAT_FALSE;
	eat_bool find = EAT_FALSE;
    u8  *p = pRspStr;
	u32 res1, res2, res3 = 0;
	UrcEntity urc = {0};

	eat_trace("eat_ftp_async_at_urc_hdlr: begin");
    while(p) 
	{
        /* ignore \r \n */
        while(AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }

        if(!strncmp(asyncAtCmd, p, strlen(asyncAtCmd)))
        {
        	find = EAT_TRUE;
			if(strstr(asyncAtCmd, STRING_FTP_GETTOFS) != 0)
			{
				sscanf(p + strlen(asyncAtCmd), "%d", &res1);
				if(res1 == 0)
				{
					sscanf(p + strlen(asyncAtCmd), "%d,%d", &res1, &res2);
				}
			}
			else
			{
				sscanf(p + strlen(asyncAtCmd), "%d,%d", &res1, &res2);
				if(res1 == 1 && res2 == 0 && strstr(asyncAtCmd, STRING_FTP_SIZE) != 0)
				{
					sscanf(p + strlen(asyncAtCmd), "%d,%d,%d", &res1, &res2, &res3);
				}
			}
            break;
        }

        p = (u8*)strchr(p,0x0a);
    }

	eat_trace("eat_ftp_async_at_urc_hdlr: res1 = %d, res2 = %d, res3 = %d", res1, res2, res3);
	if(find == EAT_TRUE)
	{
		if(ftpAsyncCB != NULL)
		{
			ftpAsyncCB(res1, res2, res3);
			ftpAsyncCB = NULL;
		}
		urc.p_urcStr = asyncAtCmd;
		urc.p_urcCallBack = eat_ftp_async_at_urc_hdlr;
		simcom_urc_queue_remove(urc);
	}
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_sapbr_init
 *  Description: 
 *        Input:

 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
eat_bool eat_sapbr_init(AtCmdRspCB sync)
{
    eat_bool result = FALSE;
    u8 count = 1;
	AtCmdEntity atCmdInit = {"AT+SAPBR=1,1"AT_CMD_END, 14, eat_sapbr_init_rsp_hdlr};
		
    /* set at cmd func's range,must first set */
    result = simcom_atcmd_queue_fun_set(1, NULL);
	if(result == FALSE)
	{
		eat_trace("eat_sapbr_init: simcom_atcmd_queue_fun_set fail");
		return FALSE;
	}
	eat_ftp_sync_cb_set(sync);
    simcom_atcmd_queue_append(atCmdInit);
	
    return TRUE;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_sapbr_init_rsp_hdlr
 *  Description: handle async at rsp
 *        Input:
 *       Output:
 *       Return:
 *       author: hanjun.liu
 * =====================================================================================
 */
static AtCmdRsp eat_sapbr_init_rsp_hdlr(u8 *pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR", "OK"};
    s16  rspType = -1;
    u8  i = 0;
    u8  *p = pRspStr;
	
    while(p) 
	{
        /* ignore \r \n */
        while(AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }
        for(i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
        {
            if(strncmp(rspStrTable[i], p, strlen(rspStrTable[i])) == 0)
            {
                rspType = i;
                break;
            }
        }
        p = (u8*)strchr(p, AT_CMD_LF);
    }

	eat_trace("eat_sapbr_init_rsp_hdlr: rspType = %d", rspType);
    switch(rspType)
    {
        case 0:  /* ERROR */
		{
	        rspValue = AT_RSP_ERROR;
			if(ftpSyncCB != NULL)
			{
				ftpSyncCB(FALSE);
				ftpSyncCB = NULL;
			}
	        break;
        }
        case 1:  /* OK */
		{
	        rspValue = AT_RSP_FUN_OVER;
			if(ftpSyncCB != NULL)
			{
				ftpSyncCB(TRUE);
				ftpSyncCB = NULL;
			}
	        break;
        }
        default:
		{
	        rspValue = AT_RSP_WAIT;
	        break;
        }
    }
	
    return rspValue;
}
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
#include "app_demo_http.h"

/********************************************************************
 * Macros
 ********************************************************************/

/********************************************************************
 * Types
 ********************************************************************/
typedef void (*app_user_func)(void*);
typedef void (*HttpReadNotifyCb)(eat_bool result,u8* strData, u32 len);


/********************************************************************
 * Extern Variables (Extern /Global)
 ********************************************************************/
 
/********************************************************************
 * Local Variables:  STATIC
 ********************************************************************/
static ResultNotifyCb s_httpSyncCB;  //for results return not too long
static ResultNotifyCb s_httpInitCB = NULL;
static ResultNotifyCb s_httpParaSetCB = NULL;
static ResultNotifyCb s_httpActionCB = NULL;
static HttpReadNotifyCb s_httpReadCB = NULL;
static ResultNotifyCb s_httpDataCB = NULL;
static ResultNotifyCb s_httpTermCB = NULL;
static u8 s_strDemoData[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};//excute AT+HTTPDATA=26,10000 command,input demo data.


/********************************************************************
* Local Function declaration
 ********************************************************************/
 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: UrcCb_http_modem_data_parse
 *  Description: uart data parse
 *        Input:
 			pRspStr:
 			len:
 *       Output:
 *       Return:
 *       author: dingfen.zhu
 * =====================================================================================
 */
void UrcCb_http_modem_data_parse(u8* pRspStr, u16 len)
{
    u8* buf_ptr = NULL;
    u8 param1 = 0;
    u8 param2 = 0;

    eat_trace("data_parse:%s",pRspStr);
    /*param:%d,extern_param:%d*/
     buf_ptr = (u8*)strstr((const char *)pRspStr,"param");
    if( buf_ptr != NULL)
    {
        sscanf((const char *)buf_ptr, "param:%d,extern_param:%d",(int*)&param1, (int*)&param2);
        eat_trace("data parse param1:%d param2:%d",param1, param2);
        eat_module_test_http(param1, param2);
    }
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: UrcCb_http_action_indicator
 *  Description: report after excuting HTTPACTION command information
 *        Input:
 			pRspStr:
 			len:
 *       Output:
 *       Return:
 *       author: dingfen.zhu
 * =====================================================================================
 */
void UrcCb_http_action_indicator(u8* pRspStr, u16 len)
{
	AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"+HTTPACTION: "};
    s16  rspType = -1;
    u8   szBuffer[20]= {0};
    u8  length = 0;
    u8  i = 0;
    u8  *p = pRspStr;
    u8 *pTemp = NULL;
    u16 code = 0;

    eat_trace("UrcCb_http_action_indicator(),%s",p);
    
    while(p) {
        /* ignore \r \n */
        while ( AT_CMD_CR == *p || AT_CMD_LF == *p)
        {
            p++;
        }

        if (!strncmp(rspStrTable[0], p, strlen(rspStrTable[0]))){
            rspType =0;
            if (rspType == 0){
                eat_trace("UrcCb_http_action_indicator: urc (%s)",p);
                p = (u8*)strchr(p,',');
                p = p + 1;
                eat_trace("UrcCb_http_action_indicator: urc1 (%s)",p);
                pTemp = (u8*)strchr(p,',');
                length = pTemp - p;
                strncpy(szBuffer,p,length);
                code= atoi(szBuffer); 

                eat_trace("UrcCb_http_action_indicator: code(%d)",code);
            }
            break;
        }
    }
}


/********************************************************************
* Local Function
 ********************************************************************/
static AtCmdRsp eat_http_init_rsp_hdlr(u8 *pRspStr)
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

    eat_trace("eat_http_init_rsp_hdlr: rspType = %d", rspType);
    switch(rspType)
    {
        case 0:  /* ERROR */
		{
	        rspValue = AT_RSP_ERROR;
			if(s_httpInitCB != NULL)
			{
				s_httpInitCB(FALSE);
				s_httpInitCB = NULL;
			}
	        break;
        }
        case 1:  /* OK */
		{
	        rspValue = AT_RSP_FUN_OVER;
			if(s_httpInitCB != NULL)
			{
				s_httpInitCB(TRUE);
				s_httpInitCB = NULL;
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

static eat_bool eat_http_init( ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    u8 count = 1;
    AtCmdEntity atCmdInit = {"AT+HTTPINIT"AT_CMD_END, 13, eat_http_init_rsp_hdlr};
		
    /* set at cmd func's range,must first set */
    result = simcom_atcmd_queue_fun_set(1, NULL);
    if(result == FALSE)
    {
    	eat_trace("eat_http_init: eat http init fail");
    	return FALSE;
    }
    
    s_httpInitCB = pResultCb;
    simcom_atcmd_queue_append(atCmdInit);
	
    return TRUE;
}

static AtCmdRsp eat_http_para_set_rsp_hdlr(u8 *pRspStr)
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

    eat_trace("eat_http_init_rsp_hdlr: rspType = %d", rspType);
    switch(rspType)
    {
        case 0:  /* ERROR */
		{
	        rspValue = AT_RSP_ERROR;
			if(s_httpParaSetCB != NULL)
			{
				s_httpParaSetCB(FALSE);
				s_httpParaSetCB = NULL;
			}
	        break;
        }
        case 1:  /* OK */
		{
	        rspValue = AT_RSP_FUN_OVER;
			if(s_httpParaSetCB != NULL)
			{
				s_httpParaSetCB(TRUE);
				s_httpParaSetCB = NULL;
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

static eat_bool eat_http_parameter_set(u8* strParaType,u8* strContent, ResultNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    u8 count = 0 ;
    u8 pAtCpin[550] = {0};
    AtCmdEntity atCmdInit={
        "AT+HTTPPARA=", 0, eat_http_para_set_rsp_hdlr /* 0  */
    };

    eat_trace("eat_http_parameter_set");
    /* set at cmd func's range,must first set */
    
    sprintf(pAtCpin,"%s\"%s\",\"%s\"%s",atCmdInit.p_atCmdStr,strParaType,strContent,AT_CMD_END);
    eat_trace("eat_http_parameter_set,%s",pAtCpin);
    atCmdInit.p_atCmdStr = pAtCpin;
    atCmdInit.cmdLen = strlen(pAtCpin);

    result = simcom_atcmd_queue_fun_set(1, NULL);

    result = simcom_atcmd_queue_append(atCmdInit);
    eat_trace("eat_http_parameter_set,result=%d",result);
    if(result&&(NULL != pResultCb)){
        s_httpParaSetCB = pResultCb;
    } 
    return result;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_http_sapbr_test_cb
 *  Description: 
 *        Input:
 *       Output:
 *       Return:
 *       author: zhudingfen
 * =====================================================================================
 */
static void eat_http_sapbr_test_cb(eat_bool result)
{
	u8 buffer[FTP_MAX_OUTPUT_BUFFER_SIZE] = {0};

	eat_trace("eat_http_sapbr_test_cb: result = %d", result);
}

static void eat_http_init_test_cb(eat_bool result)
{
	u8 buffer[FTP_MAX_OUTPUT_BUFFER_SIZE] = {0};

	eat_trace("eat_http_init_test_cb: result = %d", result);
}

static void eat_http_para_set_test_cb(eat_bool result)
{
	u8 buffer[FTP_MAX_OUTPUT_BUFFER_SIZE] = {0};

	eat_trace("eat_http_para_set_test_cb: result = %d", result);
}

 static void eat_http_action_test_cb(eat_bool result)
 {
     u8 buffer[FTP_MAX_OUTPUT_BUFFER_SIZE] = {0};
 
     eat_trace("eat_http_action_test_cb: result = %d", result);
 }

 static void eat_http_read_test_cb(eat_bool result,u8* strData, u32 len)
{
    u8 buffer[FTP_MAX_OUTPUT_BUFFER_SIZE] = {0};
    u16 i = 0;
    u8* p = strData;
    u32 outPutLen = 0;

    eat_trace("eat_http_read_test_cb: result = %d,len = %d", result,len);
    if ((TRUE== result)&& (len > 0) )
    {
        for (i = 0; i < len /FTP_MAX_OUTPUT_BUFFER_SIZE; i++)
        {
            strncpy(buffer,p+outPutLen,FTP_MAX_OUTPUT_BUFFER_SIZE);
            outPutLen = i*FTP_MAX_OUTPUT_BUFFER_SIZE+FTP_MAX_OUTPUT_BUFFER_SIZE;
            eat_trace("%s", buffer);
            memset(buffer,0,FTP_MAX_OUTPUT_BUFFER_SIZE);
        }

        if (outPutLen < len)
        {
            strncpy(buffer,p+outPutLen,len - outPutLen);
            eat_trace("%s", buffer);
        }
    }
}

 
 static void eat_http_data_test_cb(eat_bool result)
 {
     u8 buffer[FTP_MAX_OUTPUT_BUFFER_SIZE] = {0};
 
     eat_trace("eat_http_data_test_cb: result = %d", result);
 }

 static void eat_http_term_test_cb(eat_bool result)
 {
     u8 buffer[FTP_MAX_OUTPUT_BUFFER_SIZE] = {0};
 
     eat_trace("eat_http_term_test_cb: result = %d", result);
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
    s_httpSyncCB = sync;
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
 *       author: zhudingfen
 * =====================================================================================
 */
static AtCmdRsp eat_sapbr_init_rsp_hdlr(u8 *pRspStr)
{
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"ERROR", "OK"};
    s16  rspType = -1;
    u8  i = 0;
    u8  *p = pRspStr;

    eat_trace("eat_sapbr_init_rsp_hdlr: pRspStr = %s", pRspStr);
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
			if(s_httpSyncCB != NULL)
			{
				s_httpSyncCB(FALSE);
				s_httpSyncCB = NULL;
			}
	        break;
        }
        case 1:  /* OK */
		{
	        rspValue = AT_RSP_FUN_OVER;
			if(s_httpSyncCB != NULL)
			{
				s_httpSyncCB(TRUE);
				s_httpSyncCB = NULL;
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

 static AtCmdRsp eat_http_action_rsp_hdlr(u8 *pRspStr)
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
 
     eat_trace("eat_http_action_rsp_hdlr: rspType = %d", rspType);
     switch(rspType)
     {
         case 0:  /* ERROR */
         {
             rspValue = AT_RSP_ERROR;
             if(s_httpActionCB != NULL)
             {
                 s_httpActionCB(FALSE);
                 s_httpActionCB = NULL;
             }
             break;
         }
         case 1:  /* OK */
         {
             rspValue = AT_RSP_FUN_OVER;
             if(s_httpActionCB != NULL)
             {
                 s_httpActionCB(TRUE);
                 s_httpActionCB = NULL;
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


 static eat_bool eat_http_action(u8 mode, ResultNotifyCb pResultCb)
 {
     eat_bool result = FALSE;
     u8 count = 0 ;
     u8 pAtCpin[50] = {0};
     AtCmdEntity atCmdInit={
         "AT+HTTPACTION=", 0, eat_http_action_rsp_hdlr /* 0  */
     };
 
     eat_trace("eat_http_action");
     /* set at cmd func's range,must first set */
     
     sprintf(pAtCpin,"%s%d%s",atCmdInit.p_atCmdStr,mode,AT_CMD_END);
     eat_trace("eat_http_action,%s",pAtCpin);
     atCmdInit.p_atCmdStr = pAtCpin;
     atCmdInit.cmdLen = strlen(pAtCpin);
 
     result = simcom_atcmd_queue_fun_set(1, NULL);
 
     result = simcom_atcmd_queue_append(atCmdInit);
     eat_trace("eat_http_action,result=%d",result);
     if(result&&(NULL != pResultCb)){
         s_httpActionCB = pResultCb;
     } 
     return result;
 }

 static AtCmdRsp eat_http_read_rsp_hdlr(u8 *pRspStr)
 {
     AtCmdRsp  rspValue = AT_RSP_WAIT;
     u8 *rspStrTable[ ] = {"ERROR", "OK"};
     s16  rspType = -1;
     u8  i = 0;
     u8  *p = pRspStr;
     u8 * pTemp = NULL;
     u8 buffer[20] = {0};
     u32 len = 0;
     u8* pBuffer = NULL;

     eat_trace("eat_http_read_rsp_hdlr: pRspStr = %s", pRspStr);
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
 
     eat_trace("eat_http_read_rsp_hdlr: rspType = %d", rspType);
     switch(rspType)
     {
         case 0:  /* ERROR */
         {
             rspValue = AT_RSP_ERROR;
             if(s_httpReadCB != NULL)
             {
                 s_httpReadCB(FALSE,NULL,0);
                 s_httpReadCB = NULL;
             }
             break;
         }
         case 1:  /* OK */
         {
             rspValue = AT_RSP_FUN_OVER;
             p = pRspStr;
             pTemp = (u8*)strstr(pRspStr, "+HTTPREAD: ");

             if (NULL != pTemp)
            {
                pTemp += strlen("+HTTPREAD: ");
                p = pTemp;
                while((AT_CMD_CR != *p) && ( AT_CMD_LF != *p))
                {
                    p++;
                }
                len = p - pTemp;
                strncpy(buffer, pTemp, len);
                len = atoi(buffer);
                if (len > 0)
                {
                    p += 2;
                    pBuffer = (u8*)eat_mem_alloc(len);
                    if (NULL != pBuffer)
                    {
                        strncpy(pBuffer,p,len);
                    }
                }
            }
             
             if(s_httpReadCB != NULL)
             {
                 s_httpReadCB(TRUE,pBuffer,len);
                 s_httpReadCB = NULL;
                 if (NULL != pBuffer)
                {
                    eat_mem_free(pBuffer);
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

 
 static eat_bool eat_http_read(HttpReadNotifyCb pResultCb)
{
    eat_bool result = FALSE;
    u8 count = 1;
    AtCmdEntity atCmdInit = {"AT+HTTPREAD"AT_CMD_END, 13, eat_http_read_rsp_hdlr};
    	
    /* set at cmd func's range,must first set */
    result = simcom_atcmd_queue_fun_set(1, NULL);
    if(result == FALSE)
    {
    	eat_trace("eat_http_read: eat http read fail");
    	return FALSE;
    }

    s_httpReadCB = pResultCb;
    simcom_atcmd_queue_append(atCmdInit);

    return TRUE;
}

static AtCmdRsp eat_http_data_rsp_hdlr(u8 *pRspStr)
 {
     AtCmdRsp  rspValue = AT_RSP_WAIT;
     u8 *rspStrTable[ ] = {"ERROR", "DOWNLOAD"};
     s16  rspType = -1;
     u32  i = 0;
     u8  *p = pRspStr;

     eat_trace("eat_http_data_rsp_hdlr: pRspStr = %s", pRspStr);
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
 
     eat_trace("eat_http_data_rsp_hdlr: rspType = %d", rspType);
     switch(rspType)
     {
         case 0:  /* ERROR */
         {
             rspValue = AT_RSP_ERROR;
             if(s_httpActionCB != NULL)
             {
                 s_httpActionCB(FALSE);
                 s_httpActionCB = NULL;
             }
             break;
         }
         case 1:  /* OK */
         {
             rspValue = AT_RSP_FUN_OVER;
             
             eat_modem_write(s_strDemoData,strlen(s_strDemoData));
             if(s_httpDataCB != NULL)
             {
                 s_httpDataCB(TRUE);
                 s_httpDataCB = NULL;
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


static eat_bool eat_http_data(u32 u32DataSize,u32 u32Time,HttpReadNotifyCb pResultCb)
{
     eat_bool result = FALSE;
     u8 count = 0 ;
     u8 pAtCpin[50] = {0};
     AtCmdEntity atCmdInit={
         "AT+HTTPDATA=", 0, eat_http_data_rsp_hdlr /* 0  */
     };
 
     eat_trace("eat_http_data");
     /* set at cmd func's range,must first set */

     sprintf(pAtCpin,"%s%d,%d%s",atCmdInit.p_atCmdStr,u32DataSize,u32Time,AT_CMD_END);
     eat_trace("eat_http_data,%s",pAtCpin);
     atCmdInit.p_atCmdStr = pAtCpin;
     atCmdInit.cmdLen = strlen(pAtCpin);
 
     result = simcom_atcmd_queue_fun_set(1, NULL);
 
     result = simcom_atcmd_queue_append(atCmdInit);
     eat_trace("eat_http_data,result=%d",result);
     if(result&&(NULL != pResultCb)){
         s_httpDataCB = pResultCb;
     } 
     return result;
 }



  static AtCmdRsp eat_http_term_rsp_hdlr(u8 *pRspStr)
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
  
      eat_trace("eat_http_term_rsp_hdlr: rspType = %d", rspType);
      switch(rspType)
      {
          case 0:  /* ERROR */
          {
              rspValue = AT_RSP_ERROR;
              if(s_httpTermCB != NULL)
              {
                  s_httpTermCB(FALSE);
                  s_httpTermCB = NULL;
              }
              break;
          }
          case 1:  /* OK */
          {
              rspValue = AT_RSP_FUN_OVER;
              if(s_httpTermCB != NULL)
              {
                  s_httpTermCB(TRUE);
                  s_httpTermCB = NULL;
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
 
  
 static eat_bool eat_http_term(ResultNotifyCb pResultCb)
{
     eat_bool result = FALSE;
     u8 count = 1;
     AtCmdEntity atCmdInit = {"AT+HTTPTERM"AT_CMD_END, 13, eat_http_term_rsp_hdlr};
         
     /* set at cmd func's range,must first set */
     result = simcom_atcmd_queue_fun_set(1, NULL);
     if(result == FALSE)
     {
         eat_trace("eat_http_term: eat http term fail");
         return FALSE;
     }

     s_httpTermCB = pResultCb;
     simcom_atcmd_queue_append(atCmdInit);

     return TRUE;
}


 /* 
 * ===  FUNCTION  ======================================================================
 *         Name: eat_module_test_http
 *  Description: test http function
 *        Input:
 			param1:
 			param2:
 *       Output:
 *       Return:
 *       author: dingfen.zhu
 * =====================================================================================
 */
eat_bool eat_module_test_http(u8 param1, u8 param2)
{
    eat_trace("eat_module_test_http: param1 = %d. param2 = %d", param1, param2);
    if(param1 == 1)//AT+SAPBR=1,1
    {
        if(param2 == 1)
        {
            eat_sapbr_init(eat_http_sapbr_test_cb);
        }
    }
    else if(param1 == 2)//AT+HTTPINIT
    {
        if(param2 == 1)
        {
            eat_http_init(eat_http_init_test_cb);
        }
    }
    else if(param1 == 3)//AT+HTTPPARA="URL","www.sim.com"
    {
        if(param2 == 1)
        {
            eat_http_parameter_set("URL","www.sim.com",eat_http_para_set_test_cb);
        }
    }
    else if (param1 == 4)//AT+HTTPACTION=0
    {
        if (1 == param2)
        {
            eat_http_action(0,eat_http_action_test_cb);
        }
    }
    else if (param1 == 5)//AT+HTTPREAD
    {
        if (1 == param2)
        {
            eat_http_read(eat_http_read_test_cb);
        }
    }
    else if (param1 == 6)//AT+HTTPDATA=26,10000
    {
        if (1 == param2)
        {
            eat_http_data(26,10000,eat_http_data_test_cb);
        }
    }
    else if (param1 == 7)//AT+HTTPTREM
    {
        if (1 == param2)
        {
            eat_http_term(eat_http_term_test_cb);
        }
    }
}


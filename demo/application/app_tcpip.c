/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName     :   app_tcpip.c
 * version       :   0.10
 * Description  :   
 * Authors      :   heweijiang
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *
 *0.10  2014-06-11, heweijiang, create originally.
 *---------------------------------------------------------------------
 *
 *About socket APT useage,please refer to the API description in eat_socket.h
 *
 *
 ********************************************************************/
/********************************************************************
 * Include Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>
#include "eat_type.h"
#include "eat_socket.h"
#include "eat_interface.h"
#include "app_custom.h"
#include "app_at_cmd_envelope.h"

#include "App_include.h"
/********************************************************************
* Macros
 ********************************************************************/
extern u8 socket_id;
extern 	ResultNotifyCb p_gprsConnect;
extern 	ResultNotifyCb p_gprsSend;



/********************************************************************
* Types
 ********************************************************************/



/********************************************************************
* Extern Variables (Extern /Global)
 ********************************************************************/
 
/********************************************************************
* Local Variables:  STATIC
 ********************************************************************/
u8 *SOC_EVENT[]={
    "SOC_READ",
    "SOC_WRITE",  
    "SOC_ACCEPT", 
    "SOC_CONNECT",
    "SOC_CLOSE", 
    "SOC_ACKED"
};

u8 *BEARER_STATE[]={
    "DEACTIVATED",
    "ACTIVATING",
    "ACTIVATED",
    "DEACTIVATING",
    "CSD_AUTO_DISC_TIMEOUT",
    "GPRS_AUTO_DISC_TIMEOUT",
    "NWK_NEG_QOS_MODIFY",
    "CBM_WIFI_STA_INFO_MODIF",
};

/********************************************************************
* External Functions declaration
 ********************************************************************/
/********************************************************************
* Local Function declaration
 ********************************************************************/
 #if UseATCOMMON
s8 simcom_connect_server(sockaddr_struct *addr,ResultNotifyCb pResultCb);
#else
s8 simcom_connect_server(sockaddr_struct *addr);
#endif
s32 simcom_send_to_server(s8 sid, const void *buf, s32 len);
void cmd_IPconnect_cb(eat_bool result);

extern AtCmdRsp AtCmdCbCIFSR(u8* pRspStr);
extern AtCmdRsp AtCmdCbCIPSTART(u8* pRspStr);
extern AtCmdRsp AtCmdCbCIPSHUT(u8* pRspStr);
extern AtCmdRsp AtCmdCb_cips(u8* pRspStr);

/********************************************************************
* Local Function
 ********************************************************************/ 

/*****************************************************************************
* Function :   soc_notify_cb
* Description: call back function of soc_create()
* Parameters :
*     s - the socket ID
*     event - the notify event about current socket, please refer to soc_event_enum
*     result - if event is SOC_CONNECT, this paramter indicate the result of connect
*     ack_size - if event is  SOC_ACKED, this parameter is valid, it indicate the acked 
* Returns:
*****************************************************************************/
//u8 arryreport1[]={0x78,0x78,0x11,0x01,0x03,0x53,0x41,0x90,0x30,0x09,0x96,0x21,0x10,0x06,0x32,0x01,0x00,0x01,0x37,0x6C,0x0D,0x0A,0x1A};

void soc_notify_cb(s8 s,soc_event_enum event,eat_bool result, u16 ack_size)
{
    u8 buffer[128] = {0};
    u8 id = 0;    
    static u8 connect_count = 0;
	u8 i=0;

    eat_trace("YML soc_notify_cb() sss s=%d",s);
	socket_id=s;	//服务器会改变socket_id?? 造成无响应。
    
    if(event & SOC_READ)
    {
        s16 len = 0;
        u8  buf[2000]={0};
        id = 0;

        while(1)
        {
            len = eat_soc_recv(s,buf,sizeof(buf));
            if (len>0)
            {            
			eat_trace("yml Recv data len=%d, %s",len, buf);
			for(i=0;i<SeverDataMax;i++)
				{
				if(i<len)
					g_RecvServerData[i]=buf[i];
				else
					{
					g_RecvServerData[i]=0;
					}
				}
			
			g_RecvServerDataLen=len;
			//flag.recvSeverData=1;  move to logic.c
			eat_send_msg_to_user(EAT_USER_1, EAT_USER_0, EAT_FALSE, 14, "REC_SEVER_DATA", EAT_NULL);
				//eat_trace("flag.recvSeverData=0x%x",flag.recvSeverData);
			#if 0//DebugMsgOnOff
            for(i=0;i<len;i++)
            	{
                //Read from Server.TODO something
				eat_trace("R111 data 0x=%x",g_RecvServerData[i]);
            	}
			#endif
			break;
            }
            else
            	{
            	eat_trace("break 1111\n");
                break;
            	}
        }
    }
    else if (event&SOC_WRITE) id = 1;
    else if (event&SOC_ACCEPT) id = 2;
    else if (event&SOC_CONNECT)
    {
        id = 3;

        if (EAT_TRUE == result)
        {
            //Connect Server Successful,TODO something
            eat_trace("yml 2015-7-24test Conncet Successful");
           // simcom_send_to_server(s,"Hello", 6);  2015-7-24
           
		   eat_send_msg_to_user(EAT_USER_1, EAT_USER_0, EAT_FALSE, 12, "SEVER_CONNECT", EAT_NULL);
		  // LoginMsgPackage();//登录信息封装
          // simcom_send_to_server(s,logoinInfo.logoinArry, sizeof(logoinInfo.logoinArry)); 
        }
        else
        {
            eat_trace("Socket connect error,result=%d",result);
			#if 0
            if (connect_count < 3)
            {   //EAT_TIMER_6
            	//eat_timer_start(EAT_TIMER_6,20*60*1000);
                eat_trace("YML 100000connect again %d", connect_count);
                eat_soc_close(s);
				eat_trace("YML eat_soc_close s=%d",s);
                simcom_connect_server(&g_server_address);
                connect_count++;
            }
			#else
			eat_soc_close(s);
			#endif
        }
    }
    else if (event&SOC_CLOSE){ 
        id = 4;
        //eat_soc_close(s);        
        eat_trace("----SOC_CLOSE---");
		eat_send_msg_to_user(EAT_USER_1, EAT_USER_0, EAT_FALSE, 12, "SEVER_CLOSE", EAT_NULL);
        //simcom_connect_server(&g_server_address);
    }
    else if (event&SOC_ACKED) id = 5;

    if (id == 5)
        sprintf(buffer,"SOC_NOTIFY:%d,%s,%d\r\n",s,SOC_EVENT[id],ack_size);
    else 
        sprintf(buffer,"SOC_NOTIFY:%d,%s,%d\r\n",s,SOC_EVENT[id],result);
    eat_trace(buffer,strlen(buffer));

    if(SOC_ACCEPT==event){
        u8 val = 0;
        s8 ret = 0;
        sockaddr_struct clientAddr={0};
        s8 newsocket = eat_soc_accept(s,&clientAddr);
        if (newsocket < 0){
            eat_trace("eat_soc_accept return error :%d",newsocket);
        }
        else{
            sprintf(buffer,"client accept:%s,%d:%d:%d:%d\r\n",clientAddr.addr[0],clientAddr.addr[1],clientAddr.addr[2],clientAddr.addr[3]);
        }

        val = TRUE;
        ret = eat_soc_setsockopt(s, SOC_NODELAY, &val, sizeof(val));
        if (ret != SOC_SUCCESS)
            eat_trace("eat_soc_setsockopt SOC_NODELAY return error :%d",ret);
    }
}

/*****************************************************************************
* Function :   bear_notify_cb
* Description: The callback function of eat_gprs_bearer_open
* Parameters :
*     state   - the state of bearer open 
*     ip_addr - 
* Returns:
*     
*****************************************************************************/
void bear_notify_cb(cbm_bearer_state_enum state,u8 ip_addr[4])
{
    u8 buffer[128] = {0};
    u8 id = 0;
    u8 sid;

    eat_trace("enter bear_notify_cb()");

    if (state & CBM_DEACTIVATED) id = 0;
    else if (state & CBM_ACTIVATING) id = 1;
    else if (state & CBM_ACTIVATED) id = 2;
    else if (state & CBM_DEACTIVATING) id = 3;
    else if (state & CBM_CSD_AUTO_DISC_TIMEOUT) id = 4;
    else if (state & CBM_GPRS_AUTO_DISC_TIMEOUT) id = 5;
    else if (state & CBM_NWK_NEG_QOS_MODIFY) id = 6;
    else if (state & CBM_WIFI_STA_INFO_MODIFY) id = 7;

    if (id == 2)
    {
        sprintf(buffer,"BEAR_NOTIFY:%s,%d:%d:%d:%d\r\n",BEARER_STATE[id],ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);
        //eat_uart_write(EAT_UART_2,buffer,strlen(buffer));
		eat_trace("bear_notify_cb,buffer=%s",buffer);
    }
    else
    {
        sprintf(buffer,"BEAR_NOTIFY:%s\r\n",BEARER_STATE[id]);
    }

    eat_trace("buffer=%s;len=%d",buffer,strlen(buffer));

    if (id == 2){
        //Connect Successful, TODO something.
        
 #if UseATCOMMON
		//s8 simcom_connect_server(sockaddr_struct *addr,ResultNotifyCb pResultCb);

#else
sid = simcom_connect_server(&g_server_address);
if(sid < 0)
	eat_trace("simcom_connect_server return error :%d",sid);

#endif
   }

}

void simcom_gprs_start(u8* apn,u8* userName,u8* password)
{
   eat_gprs_bearer_open(apn, userName, password,bear_notify_cb);
}

/*****************************************************************************
* Function :   simcom_connect_server
* Description: Connect to server
* Parameters :
*     addr -  pointer of server address 
* Returns:
*     s - socket id
*****************************************************************************/
	typedef struct 
	{
		char* cmd;
		char* ok;
	}eat_at_st;

eat_at_st init_at[]=
	{
		{"ATE0\r\n","OK"},
    #ifdef EAT_TCP_MULIT_CHANNEL
		{"AT+CIPMUX=1\r\n","OK"},
    #else
		{"AT+CIPMUX=0\r\n","OK"},
		{"AT+CIPHEAD=1\r\n","OK"},
    #endif
		{"AT+CSTT=\"CMNET\"\r\n","OK"},
		{"AT+CIICR\r\n","OK"},
		{"AT+CGREG?\r\n","OK"},
		{"AT+CIFSR\r\n",""},
    #ifdef EAT_TCP_MULIT_CHANNEL
	//	{"AT+CIPSTART=0,\"TCP\",\"180.166.164.118\",5107\r\n","OK"}
	//	{"AT+CIPSTART=0,\"TCP\",\"222.79.65.146\",8000\r\n","OK"}
		{"AT+CIPSTART=0,\"TCP\",\"222.211.65.62\",8888\r\n","OK"}
    #else
		{"AT+CIPSTART=\"TCP\",\"222.211.65.62\",8888\r\n","OK"}
    #endif
	};
#if UseATCOMMON
s8 simcom_connect_server(sockaddr_struct *addr,ResultNotifyCb pResultCb)
{
	eat_bool result = FALSE;
	AtCmdEntity atCmdInit[]={
		{"ATE1"AT_CMD_END,6,NULL},		//关闭AT回显
		{"AT+CIPHEAD=1"AT_CMD_END,14,NULL},	//接收DATA 时增加IPHEAD,单通道为:+IPD,
		//{"AT+CIPCLOSE"AT_CMD_END,13,NULL},
		{"AT+CIPSHUT"AT_CMD_END,12,AtCmdCbCIPSHUT},
		{"AT+CSTT"AT_CMD_END,9,NULL},
		{"AT+CIICR"AT_CMD_END,10,NULL},
		{"AT+CIFSR"AT_CMD_END,10,AtCmdCbCIFSR},
		{"AT+CIPSTART",0,AtCmdCbCIPSTART}
		//{AT_CMD_DELAY"3000",10,NULL},
	};
	u8 pAtCipstart[50] = {"AT+CIPSTART=\"TCP\",\"222.211.65.62\",8888\x0d\x0a"};
	//sprintf(pAtCipstart,"AT+CIPSTART=\"TCP\",\"222.211.65.62\",\"8888\"%s",AT_CMD_END);
	atCmdInit[6].p_atCmdStr = pAtCipstart;
	atCmdInit[6].cmdLen = strlen(pAtCipstart);
	TRACE_DEBUG("pAtCipstart=%s;len=%d",atCmdInit[6].p_atCmdStr,atCmdInit[6].cmdLen);
	p_gprsConnect= pResultCb;
	
	result = simcom_atcmd_queue_fun_append(atCmdInit,sizeof(atCmdInit) / sizeof(atCmdInit[0]),p_gprsConnect);

	return result;
}

#else
s8 simcom_connect_server(sockaddr_struct *addr)
{
    u8 val = 0;
    s8 ret = 0;
    s8 sid;
    eat_soc_notify_register(soc_notify_cb); //register socket callback
    
    sid = eat_soc_create(SOC_SOCK_STREAM,0);
	socket_id=sid;//把创建的id号给socket_id
	socket_id_bak=socket_id;//创建好了之后把ID备份起来。
	
	eat_trace("eat_soc_create return 1212129999 :%d",sid);
    if(sid < 0)
        eat_trace("eat_soc_create return error :%d",sid);
    
    val = (SOC_READ | SOC_WRITE | SOC_CLOSE | SOC_CONNECT|SOC_ACCEPT);
    ret = eat_soc_setsockopt(sid,SOC_ASYNC,&val,sizeof(val));
    if (ret != SOC_SUCCESS)
        eat_trace("eat_soc_setsockopt 1 return error :%d",ret);
    
    val = TRUE;
    ret = eat_soc_setsockopt(sid, SOC_NBIO, &val, sizeof(val));
    if (ret != SOC_SUCCESS)
        eat_trace("eat_soc_setsockopt 2 return error :%d",ret);
    
    val = TRUE;
    ret = eat_soc_setsockopt(sid, SOC_NODELAY, &val, sizeof(val));
    if (ret != SOC_SUCCESS)
        eat_trace("eat_soc_setsockopt 3 return error :%d",ret);
        
    ret = eat_soc_connect(sid,addr); 
    if(ret >= 0){
        eat_trace("NEW Connection ID is :%d",ret);
    }
    else if (ret == SOC_WOULDBLOCK) {
        eat_trace("Connection is in progressing");
    }
    else {
        eat_trace("Connect return error:%d",ret);
    }

    return sid;
}
#endif
/*****************************************************************************
* Function :   simcom_send_to_server
* Description: Send data to server,please refer to the eat_soc_send() description in eat_socket.h
* Parameters :
*     sid  - socket id
*     buf - buffer for sending data
*     len -  buffer size
* Returns:
*     >=0 : SUCCESS
*     SOC_INVALID_SOCKET : invalid socket id
*     SOC_INVAL : buf is NULL or len equals to zero
*     SOC_WOULDBLOCK : buffer not available or bearer is establishing
*     SOC_BEARER_FAIL : bearer broken
*     SOC_NOTCONN : socket is not connected in case of TCP
*     SOC_PIPE : socket is already been shutdown
*     SOC_MSGSIZE : message is too long
*     SOC_ERROR : unspecified error
*     SOC_NOTBIND : in case of sending ICMP Echo Request, shall bind before
*****************************************************************************/
#if UseATCOMMON
char* itoa(int num,char*str,int radix)
{/*索引表*/
char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
unsigned unum;/*中间变量*/
int i=0,j,k;
char temp;
/*确定unum的值*/
if(radix==10&&num<0)/*十进制负数*/
{
unum=(unsigned)-num;
str[i++]='-';
}
else unum=(unsigned)num;/*其他情况*/
/*转换*/
do{
str[i++]=index[unum%(unsigned)radix];
unum/=radix;
}while(unum);
str[i]='\0';
/*逆序*/
if(str[0]=='-')k=1;/*十进制负数*/
else
	k=0;
for(j=k;j<=(i-1)/2;j++)
{
temp=str[j];
str[j]=str[i-1+k-j];
str[i-1+k-j]=temp;
}
return str;
}

//s32 simcom_send_to_server(s8 sid, const void *buf, s32 len,ResultNotifyCb pResultCb)
s32 simcom_send_to_server(s8 sid, const void *buf, s32 len)
{
	eat_bool result = FALSE;
	u8 i = 0;
    u8 count = 0 ;
	AtCmdEntity atCmdInit[]={
		{"AT"AT_CMD_END,4,NULL},
		{"AT+CIPSEND=",0,AtCmdCb_cips},
		{""AT_CMD_CTRL_Z,0,AtCmdCb_cips},
		//{"",0,AtCmdCb_cips}
	};
	u8 pAtCmdMsg[100] ={0};
	u8 tempLen[10]=0;
	#if 0
	AppMemSet(tempLen,0x00,10);
	AppMemSet(pAtCmdMsg,0x00,100);//清空para	
	//sprintf(pAtCmdMsg,"%s%s%s",atCmdInit[1].p_atCmdStr,itoa(len,tempLen,10),AT_CMD_END);	
	sprintf(pAtCmdMsg,"%s%s%s","AT+CIPSEND=","23",AT_CMD_END);
	atCmdInit[0].p_atCmdStr = pAtCmdMsg;
	//atCmdInit[1].cmdLen = strlen(pAtCmdMsg);	
	TRACE_DEBUG("data len=%d",len);
	TRACE_DEBUG("cmd 111=%s",atCmdInit[1].p_atCmdStr);
	TRACE_DEBUG("send_to_server len1=%d",atCmdInit[1].cmdLen);
	AppMemSet(pAtCmdMsg,0x00,100);//清空para
	//sprintf(pAtCmdMsg,"%s%s",buf,AT_CMD_CTRL_Z);	
	//atCmdInit[2].p_atCmdStr = pAtCmdMsg;
	//atCmdInit[2].cmdLen = strlen(pAtCmdMsg);
	TRACE_DEBUG("cmd 222=%s",atCmdInit[2].p_atCmdStr);
	TRACE_DEBUG("send_to_server len2=%d",atCmdInit[2].cmdLen);
	///p_gprsSend= pResultCb;
	result = simcom_atcmd_queue_fun_append(atCmdInit,sizeof(atCmdInit) / sizeof(atCmdInit[0]),NULL);
	return result;
#endif
    /* set at cmd func's range,must first set */
    count = sizeof(atCmdInit) / sizeof(atCmdInit[0]);
    result = simcom_atcmd_queue_fun_set(count, NULL);

    if(!result)                                 /* the space of queue is poor */
        return FALSE;

   // p_get2fsCb = pResultCb;
   // p_get2fsFinalUserCb = finalCb;
   TRACE_DEBUG("count=%d",count);

    for (i=0 ;i< count ;i++ ){
		AppMemSet(tempLen,0x00,10);
		AppMemSet(pAtCmdMsg,0x00,100);//清空para
        switch ( i ) {
            case 1:	
				sprintf(pAtCmdMsg,"%s%s%s",atCmdInit[i].p_atCmdStr,itoa(len,tempLen,10),AT_CMD_END);
				atCmdInit[i].p_atCmdStr = pAtCmdMsg;
				atCmdInit[i].cmdLen = strlen(pAtCmdMsg);
                break;

            case 2:	
				sprintf(pAtCmdMsg,"%s%s",buf,AT_CMD_CTRL_Z);	
				atCmdInit[i].p_atCmdStr = pAtCmdMsg;
				atCmdInit[i].cmdLen = len;
                break;
            default:	
                break;
        }				/* -----  end switch  ----- */
		
        simcom_atcmd_queue_append(atCmdInit[i]);
    }
	
	return result;
}

#else
s32 simcom_send_to_server(s8 sid, const void *buf, s32 len)
{
    s32 ret = 0;
    ret = eat_soc_send(sid,buf,len);
    if (ret < 0)
        eat_trace("eat_soc_send return error :%d",ret);
    else
    	{
        eat_trace("eat_soc_send success :%d",ret);

    	}

    return ret;
}
#endif
/*****************************************************************************
* Function :   simcom_recv_from_server
* Description: Receive data from server,please refer to the eat_soc_recv() description in eat_socket.h
* Parameters :
*     sid  - socket id
*     buf - buffer for receiving data
*     len -  buffer size
* Returns:
*     0 :                   receive the FIN from the server
*     SOC_INVALID_SOCKET :  invalid socket id
*     SOC_INVAL :           buf is NULL or len equals to zero
*     SOC_WOULDBLOCK :      no data available
*     SOC_BEARER_FAIL :     bearer broken
*     SOC_NOTCONN :         socket is not connected in case of TCP
*     SOC_PIPE :            socket is already been shutdown
*     SOC_ERROR :           unspecified error
*****************************************************************************/
s32 simcom_recv_from_server(s8 sid, void *buf, s32 len)
{
    s32 ret = 0;

    ret = eat_soc_recv(sid,buf,len);
    if(ret == SOC_WOULDBLOCK){
        eat_trace("eat_soc_recv no data available");
    }
    else if(ret > 0) {
        eat_trace("eat_soc_recv data:%s",buf);
    }
    else{
        eat_trace("eat_soc_recv return error:%d",ret);
    }
}

/*****************************************************************************
* Function :   hostname_notify_cb
* Description: The callback function of simcom_gethostbyname 
* Parameters :
*     request_id - it set by  eat_soc_gethostbyname
*     result - the result about gethostname.
*     ip_addr[4] - if the result is TRUE, this parameter indicate the IP address
*                            of hostname.
* Returns:
*     EAT_FALSE or EAT_TRUE
*****************************************************************************/
void hostname_notify_cb(u32 request_id,eat_bool result,u8 ip_addr[4])
{
    //u8 buffer[128] = {0};
    //sprintf(buffer,"HOSTNAME_NOTIFY:%d,%d,%d:%d:%d:%d\r\n",request_id,result,ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);
    //eat_uart_write(EAT_UART_2,buffer,strlen(buffer));
    eat_trace("hostname_notify_cb");
    
}

/*****************************************************************************
* Function :   simcom_gethostbyname
* Description: This function gets the IP of the given domain name. 
* Parameters :
*     domain_name   - domain name
* Returns:
*     EAT_FALSE or EAT_TRUE
*****************************************************************************/
eat_bool simcom_gethostbyname(const char *domain_name)
{
    u8 len;
    u8 ipaddr[4];    
    s32 result = 0;
    eat_bool ret = EAT_FALSE;     
    
    eat_soc_gethost_notify_register(hostname_notify_cb);
    
    result = eat_soc_gethostbyname(domain_name,ipaddr,&len,1234);
    if (SOC_SUCCESS == result){
        //u8 buffer[128] = {0};
        eat_trace("eat_soc_gethostbyname success");
        //sprintf(buffer,"HOSTNAME:%d,%d:%d:%d:%d\r\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
        //eat_uart_write(EAT_UART_1,buffer,strlen(buffer));
        ret = EAT_TRUE;
    } else if(SOC_WOULDBLOCK == result){
        eat_trace("eat_soc_gethostbyname wait callback function");
    } else
        eat_trace("eat_soc_gethostbyname error");

    return ret;
}

/*****************************************************************************
* Function :   simcom_tcpip_test
* Description: socket connect and app update usage exemple
* Parameters :
* Returns:
*****************************************************************************/
void simcom_tcpip_test(void)
{
	s8 result=2;
    s8 sid;
    //socket usage example entry
  //  simcom_gprs_start(g_modemConfig.apnName,g_modemConfig.apnUserName,g_modemConfig.apnPassword);
	eat_gprs_bearer_open(g_modemConfig.apnName,g_modemConfig.apnUserName,g_modemConfig.apnPassword,NULL);
	
#if UseATCOMMON
	result = simcom_connect_server(&g_server_address,cmd_IPconnect_cb);
	TRACE_DEBUG("add connect function resule=%d",result);
#else
	sid = simcom_connect_server(&g_server_address);
	TRACE_DEBUG("simcom_tcpip_test +++sid=%d",sid);
#endif


}
void App_connect_server(void)
{	s8 result=2;
    s8 sid;
	//simcom_tcpip_test();
    ///simcom_gprs_start(g_modemConfig.apnName,g_modemConfig.apnUserName,g_modemConfig.apnPassword);
	#if UseATCOMMON
    eat_soc_notify_register(soc_notify_cb); //register socket callback
	result = simcom_connect_server(&g_server_address,cmd_IPconnect_cb);
	TRACE_DEBUG("add connect function resule=%d",result);
	#else
	if(socket_id!=socket_id_bak)
		{
		result=eat_soc_close(socket_id_bak);
		socket_id_bak=socket_id;
		TRACE_DEBUG("close  socket_id_bak=%d,result=%d\n",socket_id_bak,result);
		}
	result=eat_soc_close(socket_id);
	TRACE_DEBUG("close  socket_id=%d,result=%d\n",socket_id,result);
	simcom_gprs_start(g_modemConfig.apnName,g_modemConfig.apnUserName,g_modemConfig.apnPassword);
	//sid = simcom_connect_server(&g_server_address);
	//TRACE_DEBUG("App_connect_server sid=%d",sid);
	#endif
}

void cmd_IPconnect_cb(eat_bool result)
{
    eat_trace("cmd_IPconnect_cb result = %d", result);
	if(result)
		{
		eat_send_msg_to_user(EAT_USER_0, EAT_USER_0, EAT_FALSE, 12, "SEVER_CONNECT", EAT_NULL);
		}
}





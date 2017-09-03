#ifndef _APP_RAM_C
#define _APP_RAM_C
#include "App_ram.h"

T_FLAG  flag;
T_DELAY delay;
T_KEY   key;

u32 g_CounterSec=0;


u8 g_imei_sn[16]={0};
u8 g_RecvServerData[100]={0};
u8 g_RecvServerDataLen=0;
u8 g_ServerCmdGPSONTimer=0;
//Info_LBS LBSmsg;
Info_Logoin logoinInfo;
EatRtc_st rtc = {0};
EatRtc_st GpsTime = {0};

u8 g_N1=0,g_N2=0,g_N3=0,g_N4=0;		//
u8 g_W1=0,g_W2=0,g_W3=0,g_W4=0;

// latitude纬度 and // latitude纬度
u8 g_GpsSpeed=0;
u16 g_GpsMoveDir=0;

u8 g_GpsLat_1=0;// latitude纬度
u8 g_GpsLat_2=0;
u8 g_GpsLat_3=0;
u8 g_GpsLat_4=0;

u8 g_Gpslong_1=0;// // latitude纬度
u8 g_Gpslong_2=0;
u8 g_Gpslong_3=0;
u8 g_Gpslong_4=0;


LAC_TYPE LocationAreaCode;//LAC
CI_TYPE CellID;				//CI
u8	g_MobileNetworkCode=0;

u16 g_loginMsgSn=0;
u8 g_sendDataArry[100];
u8 g_sendDataArryLen=0;
u8 g_serverMark4byte[4];

u8 StateIndex=0;
u8 PrevStateIndex=0;

u8 g_reConnectSeverCounter=0;
u8 socket_id=0; 	   //yml 2015-7-28
u8 socket_id_bak=0;

u8 g_SendDatAbnormalCnt=0;
VoltageLevelType g_VoltageValue=V_LEVEL_6;
GsmSignalLevelType g_GsmSignalValue=GSM_LEVEL_0;
u8 g_cgatt = 0;
u8 g_creg=0;
u8 g_KeypadButton=0;
u8 NoUsedPin=0;
CallMsg Call;
u8 g_KeyValue=0;

sockaddr_struct g_server_address =		//默认设置，开机时会读取短信设置的服务器
{
    SOC_SOCK_STREAM,
    4,
   // 8888,              //server port
  //  {222,211,65,62}  //server IP  宝星科技服务器IP
    8080,              //server port
    {119,23,74,240}  //server IP  yumComServer IP
};
u8 SosNumber[4][12]=
{
	{"NULL"},
	{"NULL"},
	{"NULL"},
	{"NULL"}
};
void AppMemSet(u8 *mem,u8 data,u32 len)
{
u32 i=0;
for(i=0;i<len;i++)
{
*mem++=data;
}
}
#endif

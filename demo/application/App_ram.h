#ifndef _APP_RAM_H
#define _APP_RAM_H
//结构体定义
#include "Platform.h"


#define MobileCountryCode   460U//十六进制为0X01CC

//结构体定义

typedef union T_UNION
{
    u8 Byte;
    struct
    {
        u8 B0:1;
        u8 B1:1;
        u8 B2:1;
        u8 B3:1;
        u8 B4:1;
        u8 B5:1;
        u8 B6:1;
        u8 B7:1;
    } Bit;
} T_UNION;

typedef struct
{
    u8 SendLoginMsg_fst:1;
    u8 recvSeverData:1;
    u8 LoginOK:1;
    u8 sms_ready:1;
    u8 SeverConnecting_fst:1;
    u8 reportLBSmsgOK:1;
    u8 LBSmsgReady:1;
    u8 askLBSmsg:1;

	u8 reportLBSmsg_fst:1;
	u8 reportStatemsgOK:1;
	u8 reportStatemsg_fst:1;
	u8 enterLogoinedState_fst:1;
	u8 StandbyState_fst:1;
	u8 :1;
	u8 reportGPSmsg_fst:1;
	u8 reportGPSmsgOK:1;

	u8 :1;
	u8 reConnectSever:1;
	u8 :1;
	u8 :1;
	u8 SeverConnected:1;
	u8 connectAbnormal:1;
	u8 GPSisOk:1;
	u8 GSMisOk:1;


	u8 timer1up:1;
	u8 timer2up:1;
	u8 timer3up:1;
	u8 timer4up:1;
	u8 timer5up:1;
	u8 timer41up:1;
	u8 :1;
	u8 timer8up:1;
	
	u8 timer9up:1;
	u8 timer10up:1;
	u8 timer11up:1;
	u8 timer12up:1;
	u8 timer13up:1;
	u8 timer14up:1;
	u8 timer15up:1;
	u8 timer16up:1;	//用于按键扫描的定时
	
	u8 reconnectTP:1;//使用timer6,定时1分钟
	u8 reBootTP_20:1;		//使用timer7,数字代表分钟数
	u8 reBootTP_10:1;		//使用timer8,数字代表分钟数
	u8 readADC:1;
	u8 key_down:1;
	u8 keyscan:1;
	u8 GPS_TIMER_UP:1;
	u8 :1;
	// latitude纬度 and longitude经度

	u8 GpsLatitude:1;	//北纬还是南纬1 or 0  N  or  S
	u8 GpsLongitude:1;	 //东经还是西经1 or 0  E or W
	u8 CLCCin:1;
	u8 CallinLine:1;
	u8 CalloutLine:1;
	u8 :1;
	u8 sosIsNull:1;
	u8 keycome:1;
	
	u8 ServerCmdSeeSOS:1;
	u8 ServerCmdGPSON:1;
	u8 :1;
	u8 :1;
	u8 :1;
	u8 :1;
	u8 :1;
	u8 :1;
}T_FLAG;


typedef struct{

    u8 phonetime:1;
    u8 :1;
    u8 :1;
    u8 :1;
    u8 :1;
    u8 :1;
    u8 :1;
    u8 :1;

}T_DELAY;

typedef struct
{
	u8  key1 :1;
	u8  key2 :1;
	u8  key3 :1;
	u8  key4 :1;
	u8  key5 :1;
	u8  key6 :1;
	u8  key7 :1;
	u8  key8 :1;
	
	u8  key1_hold :1;
	u8  key2_hold :1;
	u8  key3_hold :1;
	u8  key4_hold :1;
	u8  key5_hold :1;
	u8  key6_hold :1;
	u8  key7_hold :1;
	u8  key8_hold :1;

	u8	key1_fst :1;
	u8	key2_fst :1;
	u8	key3_fst :1;
	u8	key4_fst :1;
	u8	key5_fst :1;
	u8	key6_fst :1;
	u8	key7_fst :1;
	u8	key8_fst :1;

	u8	key1_stateBak :1;
	u8	key2_stateBak :1;
	u8	key3_stateBak :1;
	u8	key4_stateBak :1;
	u8	key5_stateBak :1;
	u8	key6_stateBak :1;
	u8	key7_stateBak :1;
	u8	key8_stateBak :1;


}T_KEY;
typedef enum
{
	LoginMsgNUM=0x01,
	GPSMsgNUM=0x10,
	LBSMsgNUM=0x11,
	StateMsgNUM=0x13,
	GPS_LBS_STATE_MsgNUM=0x16,
	LBS_TelPhoneNumber_NUM=0x17,
	LBS_ExpandMsgNUM=0x18,
	LBS_StateMsgNUM=0x19,
	GPS_TelPhoneNumber_NUM=0x1A,
	ServerEleFenceElarmAddrNUM=0x1B,
	TimerSyncMsgNUM=0x1F,
	ServerToTerminalCMD=0x80,
	TerminalToServer=0x81,
	
}ProtocolNumberType;
typedef enum
{
  PowerOnState=1,//	1	开机
  StandbyState,//	2待机
  SeverConnectingState,//	3连接服务器
  SeverconnectedState,	// 4 已经连接到服务器，等待登录
  SeverLogoinState,		// 5 服务器登录成功,循环发送LBS信息、GPS信息、state信息。
  
  reportLBSmsgState,// 4 上报LBS信息
  reportStatemsgState,// 5应答

  TakeUpPhoneState,//6 摘机准备拨号

  ImageDialState,//7 头像拨号中

  RecorderPlayState,// 8 答录机播放
  RecorderMenuState,//9 答录设置菜单
  MainMenuState,//	10
  IncomeCallState,			//	11
  OutCallState,			//	12
  AddPhoneNumState,		//	15
  RingSetState,//	18
  SOSState,//		19
  PreSOSState,//	20
  TimeSetState,// 21


  MaxState
} StateType;
typedef enum
{
	GSM_LEVEL_0=0,
	GSM_LEVEL_1=1,
	GSM_LEVEL_2=2,
	GSM_LEVEL_3=3,
	GSM_LEVEL_4=4,
	GSM_LEVEL_MAX=4
}GsmSignalLevelType;
typedef enum
{
	V_LEVEL_0=0,
	V_LEVEL_1=1,
	V_LEVEL_2=2,
	V_LEVEL_3=3,
	V_LEVEL_4=4,
	V_LEVEL_5=5,
	V_LEVEL_6=6,
	V_LEVEL_MAX=6	
}VoltageLevelType;

typedef union {	
	u8 logoinArry[22];
 struct __PACKED
{
    u16 start;			//起始位
    u8 len;  			//长度
    
    u8 ProtocolNumber;	//协议号
	u8 terminal_ID[8];	//终端号
	u16 TypeCode;		//识别码，终端
	u16 expand;			//扩展码
	
	u16 MsgSN;			//序列号
	u16 CRC;			//CRC校验
	u16 end;			//结束位
 }pkg;
}Info_Logoin;

typedef union {	
	u8 LBSArry[26];
 struct __PACKEDlbs
{
    u16 start;			//起始位
    u8 len; 			//长度
    u8 ProtocolNumber;	//协议号
	u8 year;	//年
	u8 month;		//月
	u8 day;			//日
	u8 hour;			//时
	u8 minute;			//分
	u8 second;			//秒
	u16 MCC;			//中国移动国家号
	u8 MNC;// 移动网络号mobile network code
	u8 LAC0;			// 位置区码
	u8 LAC1;
	u8 CI0;
	u8 CI1;
	u8 CI2;
	u8 expand0;
	u8 expand1;
	//u16 expand;			//扩展码
	u8 MsgSN0;			//序列号
	u8 MsgSN1;			//序列号
	u16 CRC;			//CRC校验
	u16 end;			//结束位
 }pkg;
}Info_LBS;
typedef enum
{
ACTIVE=0,
HELD,
DIALING,
ALERTING,
INCOMING,
WAITING,
DISCONNECT
}CallMsgStatType;

typedef struct __PACKEDCallMsg
{
	u8 idx;		//0-7  来电序号
	u8 dir;		//0-1 方向, MO,MT
	u8 stat;	//0-6状态，Active,Held,Dialing,Alerting,Incoming,Waiting,Disconnect
	u8 mode;	//0-2 voice data Fax  
	u8 mpty;	//0-1
	u8 phoneNum[12];
	u8 phoneNubCnt;
	u8 isSOS;
}CallMsg;
/*
u8 idx=0xFF;		//0-7  来电序号
u8 dir=0xFF;		//0-1 方向, MO,MT
u8 stat=0xFF;	//0-6状态，Active,Hold,Dialing,Alerting,Incoming,Waiting,Disconnect
u8 mode=0xFF;	//0-2 voice data Fax  
u8 mpty=0xFF;	//0-1
u8 phoneNum[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
u8 phoneNubCnt=0;
u8 result=0xFF;
*/
	//typedef struct MsgEntityTag


typedef union {	//LAC0 是高位
	u16 LAC;
 struct 
{
	u8 LAC1;
	u8 LAC0;
 }pkg;
}LAC_TYPE;

typedef union {	//CI0 是高位
	u32 CI;
 struct 
{
	u8 CI3;
	u8 CI2;
	u8 CI1;
	u8 CI0;
 }pkg;
}CI_TYPE;
//yml 2015-8-17定义msg的回调函数队列
typedef void (*MsgCB)(u8 *pMsgStr, u16 len);
typedef struct MsgEntityTag
{
    u8* p_msgStr;
    MsgCB p_msgCallBack;

}MsgEntity;

typedef struct MsgEntityQueueTag
{
    MsgEntity msgEntityArray[10]; /*10个 AT命令队列 */
    u8 count;                                 /* 当前执行的index */
}MsgEntityQueue;

typedef enum
{
	SERVER_ADDR=0,
	SOS_NUM_1_ADDR=1,
	SOS_NUM_2_ADDR=2,
	SOS_NUM_3_ADDR=3,
	SOS_NUM_4_ADDR=4,

	NVRAM_ADDR_MAX=101
}NVRAMType;

typedef enum
{
  BTN_Num1=1,
  BTN_Num2,
  BTN_Num3,
  BTN_Num4,
  BTN_Num5,  
  BTN_Num6,
  
  BTN_Num1_L,
  BTN_Num2_L,
  BTN_Num3_L,
  BTN_Num4_L,
  BTN_Num5_L,  
  BTN_Num6_L,

  BTN_Repeat,//7
  BTN_Nothing,
  BTN_Max
} ButtonType;


extern u16 GetCrc16(const u8* pData, int nLength);
extern Info_Logoin logoinInfo;

extern T_FLAG  flag;
extern T_DELAY delay;
extern T_KEY   key;
extern u8 g_imei_sn[16];
extern u8 g_RecvServerData[100];
extern u8 g_RecvServerDataLen;
extern u8 g_ServerCmdGPSONTimer;

extern u32 g_CounterSec;

//extern Info_LBS LBSmsg;
extern Info_Logoin logoinInfo;
extern EatRtc_st rtc;
extern EatRtc_st GpsTime;



extern LAC_TYPE LocationAreaCode;
extern CI_TYPE CellID;
extern u8	g_MobileNetworkCode;
extern u16 g_loginMsgSn;
extern u8 g_sendDataArry[100];
extern u8 g_sendDataArryLen;
extern u8 g_serverMark4byte[4];
extern u8 StateIndex;
extern u8 PrevStateIndex;
extern u8 g_reConnectSeverCounter;
extern u8 socket_id;
extern u8 socket_id_bak;
extern u8 g_SendDatAbnormalCnt;
extern VoltageLevelType g_VoltageValue;
extern GsmSignalLevelType g_GsmSignalValue;
extern u8 g_cgatt;
extern u8 g_creg;
extern u8 g_KeypadButton;
extern sockaddr_struct g_server_address;
extern u8 SosNumber[4][12];


extern u8 g_N1;
extern u8 g_N2;
extern u8 g_N3;
extern u8 g_N4;

extern u8 g_W1;
extern u8 g_W2;
extern u8 g_W3;
extern u8 g_W4;

extern u8 g_GpsSpeed;
extern u16 g_GpsMoveDir;
extern u8 g_GpsLat_1;
extern u8 g_GpsLat_2;
extern u8 g_GpsLat_3;
extern u8 g_GpsLat_4;

extern u8 g_Gpslong_1;
extern u8 g_Gpslong_2;
extern u8 g_Gpslong_3;
extern u8 g_Gpslong_4;
extern CallMsg Call;
extern void AppMemSet(u8 *mem,u8 data,u32 len);
extern u8 g_KeyValue;
#endif


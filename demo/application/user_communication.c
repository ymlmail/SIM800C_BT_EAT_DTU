/********************************************************************
 *                Copyright                    *
 *---------------------------------------------------------------------
 * FileName      :   communication.c
 * version        :   0.10
 * Description   :   
 * Authors       :   yemaolin
 * Notes          :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2015-07-27, yemaolin, create originally.
 *
 *--------------------------------------------------------------------
 * File Description
 * 
 * 
 *   
 *--------------------------------------------------------------------
 ********************************************************************/
 
/********************************************************************
 * Include Files
 ********************************************************************/
//#include "platform.h"
#include "user_communication.h"
#include "App_include.h"

/********************************************************************
 * Macros
 ********************************************************************/

/********************************************************************
 * Types
 ********************************************************************/
/*typedef struct 
{
    u16 w;  //write offset
    u16 r;  //read offset
    u8  buf[EAT_UART_RX_BUF_LEN_MAX];
}app_buf_st;

typedef void (*app_user_func)(void*);
*/
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
/*new sms message callback function*/
 static const u16 crctab16[] ={
	 0X0000, 0X1189, 0X2312, 0X329B, 0X4624, 0X57AD, 0X6536, 0X74BF,
	 0X8C48, 0X9DC1, 0XAF5A, 0XBED3, 0XCA6C, 0XDBE5, 0XE97E, 0XF8F7,
	 0X1081, 0X0108, 0X3393, 0X221A, 0X56A5, 0X472C, 0X75B7, 0X643E,
	 0X9CC9, 0X8D40, 0XBFDB, 0XAE52, 0XDAED, 0XCB64, 0XF9FF, 0XE876,
	 0X2102, 0X308B, 0X0210, 0X1399, 0X6726, 0X76AF, 0X4434, 0X55BD,
	 0XAD4A, 0XBCC3, 0X8E58, 0X9FD1, 0XEB6E, 0XFAE7, 0XC87C, 0XD9F5,
	 0X3183, 0X200A, 0X1291, 0X0318, 0X77A7, 0X662E, 0X54B5, 0X453C,
	 0XBDCB, 0XAC42, 0X9ED9, 0X8F50, 0XFBEF, 0XEA66, 0XD8FD, 0XC974,
	 0X4204, 0X538D, 0X6116, 0X709F, 0X0420, 0X15A9, 0X2732, 0X36BB,
	 0XCE4C, 0XDFC5, 0XED5E, 0XFCD7, 0X8868, 0X99E1, 0XAB7A, 0XBAF3,
	 0X5285, 0X430C, 0X7197, 0X601E, 0X14A1, 0X0528, 0X37B3, 0X263A,
	 0XDECD, 0XCF44, 0XFDDF, 0XEC56, 0X98E9, 0X8960, 0XBBFB, 0XAA72,
	 0X6306, 0X728F, 0X4014, 0X519D, 0X2522, 0X34AB, 0X0630, 0X17B9,
	 0XEF4E, 0XFEC7, 0XCC5C, 0XDDD5, 0XA96A, 0XB8E3, 0X8A78, 0X9BF1,
	 0X7387, 0X620E, 0X5095, 0X411C, 0X35A3, 0X242A, 0X16B1, 0X0738,
	 0XFFCF, 0XEE46, 0XDCDD, 0XCD54, 0XB9EB, 0XA862, 0X9AF9, 0X8B70,
	 0X8408, 0X9581, 0XA71A, 0XB693, 0XC22C, 0XD3A5, 0XE13E, 0XF0B7,
	 0X0840, 0X19C9, 0X2B52, 0X3ADB, 0X4E64, 0X5FED, 0X6D76, 0X7CFF,
	 0X9489, 0X8500, 0XB79B, 0XA612, 0XD2AD, 0XC324, 0XF1BF, 0XE036,
	 0X18C1, 0X0948, 0X3BD3, 0X2A5A, 0X5EE5, 0X4F6C, 0X7DF7, 0X6C7E,
	 0XA50A, 0XB483, 0X8618, 0X9791, 0XE32E, 0XF2A7, 0XC03C, 0XD1B5,
	 0X2942, 0X38CB, 0X0A50, 0X1BD9, 0X6F66, 0X7EEF, 0X4C74, 0X5DFD,
	 0XB58B, 0XA402, 0X9699, 0X8710, 0XF3AF, 0XE226, 0XD0BD, 0XC134,
	 0X39C3, 0X284A, 0X1AD1, 0X0B58, 0X7FE7, 0X6E6E, 0X5CF5, 0X4D7C,
	 0XC60C, 0XD785, 0XE51E, 0XF497, 0X8028, 0X91A1, 0XA33A, 0XB2B3,
	 0X4A44, 0X5BCD, 0X6956, 0X78DF, 0X0C60, 0X1DE9, 0X2F72, 0X3EFB,
	 0XD68D, 0XC704, 0XF59F, 0XE416, 0X90A9, 0X8120, 0XB3BB, 0XA232,
	 0X5AC5, 0X4B4C, 0X79D7, 0X685E, 0X1CE1, 0X0D68, 0X3FF3, 0X2E7A,
	 0XE70E, 0XF687, 0XC41C, 0XD595, 0XA12A, 0XB0A3, 0X8238, 0X93B1,
	 0X6B46, 0X7ACF, 0X4854, 0X59DD, 0X2D62, 0X3CEB, 0X0E70, 0X1FF9,
	 0XF78F, 0XE606, 0XD49D, 0XC514, 0XB1AB, 0XA022, 0X92B9, 0X8330,
	 0X7BC7, 0X6A4E, 0X58D5, 0X495C, 0X3DE3, 0X2C6A, 0X1EF1, 0X0F78,
 };
 //  计算给定长度数据的 16 位 CRC。
 u16 GetCrc16(const u8* pData, int nLength)
 {
	 u16  fcs  =  0xffff;		  //  初始化
	 while(nLength>0){
		 fcs = (fcs >> 8) ^ crctab16[(fcs ^ *pData) & 0xff];
		 nLength--;
		 pData++;
	 }
	 return  ~fcs;		   //  取反
 }

#if 0//test crc
void testGetCrcFunc(void)
{
u8 arry1[]={0x78,0x78,0x11,0x01,0x03,0x53,0x41,0x90,0x30,0x09,0x96,0x21,0x10,0x06,0x32,0x01,0x00,0x01,0x37,0x6C,0x0D,0x0A};
u16 arry1Crc=0x0000;
u8 arry2[]={0x78,0x78,0x05,0x01,0x00,0x01,0xd9,0xdc,0x0D,0x0A};
u16 arry2Crc=0x0000;
TRACE_DEBUG("yml test crc start\n");
//arry1Crc=GetCrc16((const u8*)(arry1[2]),strlen(arry1)-2);
//arry2Crc=GetCrc16((const u8*)(arry2[2]),strlen(arry2)-2);
arry1Crc=GetCrc16((const u8*)arry1+2,16);
arry2Crc=GetCrc16((const u8*)arry2+2,4);

TRACE_DEBUG("11arry1Crc=0x%x\n",arry1Crc);
TRACE_DEBUG("11arry2Crc=0x%x\n",arry2Crc);

arry1Crc=GetCrc16((const u8*)arry1+2,sizeof(arry1)-6);//这个正确
arry2Crc=GetCrc16((const u8*)arry2+2,sizeof(arry2)-6);//这个正确
TRACE_DEBUG("22arry1Crc=0x%x\n",arry1Crc);
TRACE_DEBUG("22arry2Crc=0x%x\n",arry2Crc);
arry1Crc=GetCrc16((const u8*)arry1[2],sizeof(arry1)-6);//这个错误
arry2Crc=GetCrc16((const u8*)arry2[2],sizeof(arry2)-6);//这个错误
TRACE_DEBUG("33arry1Crc=0x%x\n",arry1Crc);
TRACE_DEBUG("33arry2Crc=0x%x\n",arry2Crc);

TRACE_DEBUG("strlen(arry2)=%d\n",strlen(arry2));
TRACE_DEBUG("strlen(arry1)=%d\n",strlen(arry1));
TRACE_DEBUG("sizeof(arry1)=%d\n",sizeof(arry1));
TRACE_DEBUG("sizeof(arry2)=%d\n",sizeof(arry2));

TRACE_DEBUG("yml test crc end~~~~\n");

}


#endif

#define sw16(x) \
    ((short)(\
       (((short)(x)&(short)0x00ffU)<< 8) |\
       (((short)(x)&(short)0xff00U)>> 8)))
u8 ImeiSN8[8];

void getIMEI(void)
{
u8 imei_temp[8];
u8 temp1,temp2;
u8 i=0;

	//logoinInfo.pkg.terminal_ID[0]=0x03;
	//logoinInfo.pkg.terminal_ID[1]=0x53;
	//logoinInfo.pkg.terminal_ID[2]=0x41;
	//logoinInfo.pkg.terminal_ID[3]=0x90;
	//logoinInfo.pkg.terminal_ID[4]=0x30;
	//logoinInfo.pkg.terminal_ID[5]=0x09;
	//logoinInfo.pkg.terminal_ID[6]=0x96;
	//logoinInfo.pkg.terminal_ID[7]=0x21;
for(i=0;i<8;i++)
{	if(0==i)
	{
	imei_temp[i]=g_imei_sn[0]&0x0F;	//对高四位清零，保留后四位。
	}
	else
	{
	temp1=g_imei_sn[2*i-1]&0x0F;
	temp2=g_imei_sn[2*i]&0x0F;
	imei_temp[i]=temp1<<4|temp2;
	}
	
	TRACE_DEBUG("0x%2x,",imei_temp[i]);

}
for(i=0;i<8;i++)
{
	logoinInfo.pkg.terminal_ID[i]=imei_temp[i];//把处理好的数值给logoinInfo
	ImeiSN8[i]=imei_temp[i];
}
}

void LoginMsgPackage(void)//登录信息封装
{
eat_bool result = EAT_TRUE;
u8 i=0;
u16 CRC;
u8 Loginarry[22];

Loginarry[0]=0x78;
Loginarry[1]=0x78;
Loginarry[2]=0x11;
Loginarry[3]=LoginMsgNUM;		//协议号
getIMEI();
for(i=0;i<8;i++)
{
	Loginarry[i+4]=ImeiSN8[i];//4,5,6,7,8,9,10,11
}
//logoinInfo.pkg.TypeCode=0x0000;
Loginarry[12]=0x10;
Loginarry[13]=0x06;
	//getIMEI();
Loginarry[14]=0x32;
Loginarry[15]=0x01;

Loginarry[16]=0x01;
	//787811 01 03534190 30099621 1006 3201 0001 376C0D0A
Loginarry[17]=0x01;
CRC=GetCrc16((const u8*)Loginarry+2,sizeof(Loginarry)-6);
Loginarry[19]=(u8)CRC;		//
Loginarry[18]=(u8)(CRC>>8);		//

Loginarry[20]=0x0d;
Loginarry[21]=0x0a;
#if DebugOn&&PrintPackageData
TRACE_DEBUG("555 Loginarry is\n");
#endif
g_sendDataArryLen=22;
for(i=0;i<23;i++)
{
	if(i<22)
		{
		g_sendDataArry[i]=Loginarry[i];
}
	else
		{
		g_sendDataArry[i]=0x00;
		}
#if DebugOn&&PrintPackageData
	TRACE_DEBUG("g_sendDataArry[%d]=0x%2x,",i,g_sendDataArry[i]);
#endif
}
}
u16 sw16hl(u16 value)
{
u16 temp;
temp=value;
value=((u16)(((u16)(value)&(u16)0x00ffU)<< 8)|\
       (((u16)(temp)&(u16)0xff00U)>> 8));
return value;
}

void LBSPackage(void)//上报LBS信息
{
eat_bool result = EAT_TRUE;
u8 i=0;
u16 CRC;
u8 LBSarry[26];

//flag.GPSisOk=1;

if(1)//(flag.GPSisOk)
{
result = eat_get_rtc(&rtc);
		if( result )
		{
			eat_trace("LBSPackage get rtc value:%d/%d/%d %d:%d:%d",
					rtc.year,rtc.mon,rtc.day,rtc.hour,rtc.min,rtc.sec);
		}else
		{
			eat_trace("LBSPackage get rtc fail");
		}
}
else
{
	rtc.year=0;
	rtc.mon=0;
	rtc.day=0;
	rtc.hour=0;
	rtc.min=0;
	rtc.sec=0;
	
	eat_trace("LBSPackage get rtc value:%d/%d/%d %d:%d:%d",
			rtc.year,rtc.mon,rtc.day,rtc.hour,rtc.min,rtc.sec);
} 
LBSarry[0]=0x78;
LBSarry[1]=0x78;
LBSarry[2]=21;
LBSarry[3]=LBSMsgNUM;		//协议号

LBSarry[4]=rtc.year;		//年
LBSarry[5]=rtc.mon;		//月
LBSarry[6]=rtc.day;		//日

LBSarry[7]=rtc.hour;		//时
LBSarry[8]=rtc.min;		//分钟
LBSarry[9]=rtc.sec;		//秒钟

LBSarry[10]=0x01;		//中国移动号0X01CC
LBSarry[11]=0xCC;		//

LBSarry[12]=0x00;		//网络号，移动

LBSarry[13]=LocationAreaCode.pkg.LAC0;//0x31;		//LAC0 是高位
LBSarry[14]=LocationAreaCode.pkg.LAC1;//0x82;		//LAC1

LBSarry[15]=0;//0;			//CI0
LBSarry[16]=CellID.pkg.CI2;//0x5C;		//CI1   是高位
LBSarry[17]=CellID.pkg.CI3;//0x83;		//CI2 是低位

LBSarry[18]=0x00;		//expand0
LBSarry[19]=0x00;		//expand1

LBSarry[20]=0x03;		//MsgSN0
LBSarry[21]=0xf2;		//MsgSN1

CRC=GetCrc16((const u8*)LBSarry+2,sizeof(LBSarry)-6);
//CRC=sw16hl(CRC);
LBSarry[23]=(u8)CRC;		//
LBSarry[22]=(u8)(CRC>>8);		//

LBSarry[24]=0x0d;
LBSarry[25]=0x0a;
#if DebugOn&&PrintPackageData
TRACE_DEBUG("555 LBSArry is\n");
#endif
g_sendDataArryLen=26;
for(i=0;i<28;i++)
{
	if(i<26)
		{
		g_sendDataArry[i]=LBSarry[i];
		}
	else
		{
		g_sendDataArry[i]=0x00;
		}
	
#if DebugOn&&PrintPackageData
	TRACE_DEBUG("g_sendDataArry[%d]=0x%2x,",i,g_sendDataArry[i]);
#endif
}
}

void StatePackage(void)//上报LBS信息
{
eat_bool result = EAT_TRUE;
u8 i=0;
u16 CRC;
u8 StateArry[15];


StateArry[0]=0x78;
StateArry[1]=0x78;
StateArry[2]=0x0a;	//长度
StateArry[3]=StateMsgNUM;		//协议号

StateArry[4]=0x00;		//终端信息未定义，暂时为0
StateArry[5]=g_VoltageValue;		//电压等级(0-6)七档
StateArry[6]=g_GsmSignalValue;		//GSM信号(0x00-0x04)五档

StateArry[7]=0x00;		//expand0
StateArry[8]=0x00;		//expand1

StateArry[9]=0x03;		//MsgSN0
StateArry[10]=0xf3;		//MsgSN1

CRC=GetCrc16((const u8*)StateArry+2,sizeof(StateArry)-6);
//CRC=sw16hl(CRC);
StateArry[12]=(u8)CRC;		//这个位置是故意颠倒的
StateArry[11]=(u8)(CRC>>8);		//

StateArry[13]=0x0d;
StateArry[14]=0x0a;

#if DebugOn&&PrintPackageData
TRACE_DEBUG("666 LBSArry is\n");
#endif
g_sendDataArryLen=15;
for(i=0;i<17;i++)
{
	if(i<15)
		{
		g_sendDataArry[i]=StateArry[i];
		}
	else
		{
		g_sendDataArry[i]=0x00;
		}
	#if DebugOn&&PrintPackageData
	TRACE_DEBUG("StatePackage[%d]=0x%2x,",i,g_sendDataArry[i]);
	#endif
}

}
void GPSPackage(void)//上报GPS信息
{
eat_bool result = EAT_TRUE;
u8 i=0;
u16 CRC;
u8 GPSArry[30];
u8 	hangxiang1=0;
u16	hangxiang2=0;


//78 78 19 10 
//0E 09 03 0E 0A 26 
//C5 02 6C 19 96 
//0C 38 D1 20 00 14 
//00 00 01 00 1C 
//10 C6 0D 0A
GPSArry[0]=0x78;
GPSArry[1]=0x78;
GPSArry[2]=0x19;			//长度
GPSArry[3]=GPSMsgNUM;		//协议号

GPSArry[4]=GpsTime.year;	//年
GPSArry[5]=GpsTime.mon;		//月
GPSArry[6]=GpsTime.day;		//日

GPSArry[7]=GpsTime.hour;	//时
GPSArry[8]=GpsTime.min;		//分钟
GPSArry[9]=GpsTime.sec;		//秒钟

GPSArry[10]=0xCC;	//长度和卫星个数

GPSArry[11]=g_GpsLat_1;	//纬度1
GPSArry[12]=g_GpsLat_2;	//纬度2
GPSArry[13]=g_GpsLat_3;	//纬度3
GPSArry[14]=g_GpsLat_4;	//纬度4
//0C 38 D1 20 00 14 
//00 00 01 00 1C 
//10 C6 0D 0A

GPSArry[15]=g_Gpslong_1;	//经度1
GPSArry[16]=g_Gpslong_2;	//经度2
GPSArry[17]=g_Gpslong_3;	//经度3
GPSArry[18]=g_Gpslong_4;	//经度4
#if EnableGpsDebuge
eat_trace("GpsTime dingwei value:%d/%d/%d %d:%d:%d",
	GpsTime.year,GpsTime.mon,GpsTime.day,GpsTime.hour,GpsTime.min,GpsTime.sec);
#endif
GPSArry[19]=g_GpsSpeed;	//速度
if(flag.GpsLatitude)
{
	hangxiang1|=BIT2;
}
else
{
	hangxiang1&=~BIT2;
}
if(flag.GpsLongitude)
{
	hangxiang1|=BIT3;
}
else
{
	hangxiang1&=~BIT3;
}
if(flag.GPSisOk)
{
hangxiang1|=BIT4;//BIT4  设置为1，GPS已经定位
}

hangxiang2=g_GpsMoveDir;
hangxiang1|=(u8)hangxiang2>>8;
GPSArry[20]=hangxiang1;	//状态航向
GPSArry[21]=(u8)g_GpsMoveDir;	//状态航向

GPSArry[22]=0x00;	//预留扩展位
GPSArry[23]=0x01;	//预留扩展位

GPSArry[24]=0x00;		//MsgSN0
GPSArry[25]=0x01;		//MsgSN1

CRC=GetCrc16((const u8*)GPSArry+2,sizeof(GPSArry)-6);
//CRC=sw16hl(CRC);
GPSArry[27]=(u8)CRC;		//这个位置是故意颠倒的
GPSArry[26]=(u8)(CRC>>8);		//

GPSArry[28]=0x0d;
GPSArry[29]=0x0a;

#if DebugOn&&PrintPackageData
TRACE_DEBUG("777 GPS Arry is\n");
#endif
g_sendDataArryLen=30;
for(i=0;i<31;i++)
{
	if(i<30)
		{
		g_sendDataArry[i]=GPSArry[i];
		}
	else
		{
		g_sendDataArry[i]=0x00;
		}
	#if DebugOn&&PrintPackageData
	TRACE_DEBUG("GPSArry[%d]=0x%2x,",i,g_sendDataArry[i]);
	#endif
}

}

u8 add(u8 a[],u8 b[],u8 aNbit,u8 bLen){
	//向a数组里添加b数组，bLen代表b数组的长度，u8 len;代表从第几个元素开始添加
   
	u8 i;
    for(i=0; i<bLen; ++i){
		//if(b[i]<'0'||b[i]>'9') ///如果不是数字就直接return 返回，
			//{
			//return aNbit;
			//}
		//else
			//{
	        a[aNbit]=b[i];
	        aNbit++;
			//}
    }
	return aNbit;		//返回新的A[]数组的个数
}

u8 GetValidData(u8 a[],u8 bLen)// 判断数组a的头部，有几个有效数据，制定a的总长度为blen
{
u8 i=0;
u8 retValue=0;
	for(i=0;i<bLen;i++)
	{
		if((a[i]>='0')&&(a[i]<='9'))//为数字0到9
			{
			retValue++;
			}
		else
			{
			return retValue;
			}
	}
	return retValue;

}
void SeeSosCmdPackage(void)//上报sos号码信息
{
eat_bool result = EAT_TRUE;
u16 i=0;
u16 CRC;
u8 Arry[100];
u8 cmdLenth=0;
u8 totalLenth=0;
u8 para[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
AppMemSet(Arry,0x00,100);//清空para

Arry[0]=0x78;
Arry[1]=0x78;
Arry[2]=0x00;			//长度
Arry[3]=0x81;			//协议号

Arry[4]=0xFF;			//指令的长度

Arry[5]=g_serverMark4byte[0];			//服务器标志位
Arry[6]=g_serverMark4byte[1];			//服务器标志位
Arry[7]=g_serverMark4byte[2];			//服务器标志位
Arry[8]=g_serverMark4byte[3];			//服务器标志位
//strcat(Arry,"SEESOS:");
Arry[ 9]='S';			
Arry[10]='E';
Arry[11]='E';
Arry[12]='S';
Arry[13]='O';
Arry[14]='S';
Arry[15]=':';

totalLenth=16;
cmdLenth=11;

//41
for(i=0;i<4;i++)
	{
	AppMemSet(para,0x00,20);//清空para
	result=app_nvram_read(SOS_NUM_1_ADDR+i, para,11);

	//需要得到para的有效数据个数
	result=GetValidData(para,result);
	eat_trace("eat_fs_Read():Read File Success!ret=%d,%s",result,para);
	if((para[0]>='0')&&(para[0]<='9'))
		{
		if(result>=1)//电话 至少有一个数值
		{
		TRACE_DEBUG("server SosNumber[%d]=%s",i,para);
		totalLenth=add(Arry,para,totalLenth,result); //得到新数组的总个数
		cmdLenth += result;
		
		totalLenth=add(Arry,",",totalLenth,1);
		cmdLenth++;
		TRACE_DEBUG("len if %d=%d,",i,cmdLenth);
		}
		else
		{
		totalLenth=add(Arry,",",totalLenth,1);
		cmdLenth++;		
		TRACE_DEBUG("len else %d=%d,",i,cmdLenth);
		}
		}
	else
		{
		TRACE_DEBUG("server SosNumber[%d]=NULL",i);
		if(i<3)
			{
			totalLenth=add(Arry,",",totalLenth,1);			
			cmdLenth++;
			}
		}		
	}
totalLenth=add(Arry,"#",totalLenth,1);
TRACE_DEBUG("totalLenth 11=%d,\n",totalLenth);

cmdLenth=cmdLenth+1;
totalLenth = cmdLenth+5;
Arry[4]=cmdLenth;			//指令的长度

Arry[(totalLenth++)]=0x00;	//两个预留位
Arry[(totalLenth++)]=0x01;	//两个预留位

Arry[(totalLenth++)]=0x00;	//信息序列号
Arry[(totalLenth++)]=0x55;	//信息序列号

Arry[2]=totalLenth-1;			//长度重新赋值，在计算CRC之前。数值刚好就是totalLenth

CRC=GetCrc16((const u8*)Arry+2,totalLenth-2);
totalLenth++;
Arry[totalLenth]=(u8)CRC;		//这个位置是故意颠倒的
Arry[totalLenth-1]=(u8)(CRC>>8);		//
totalLenth++;

Arry[(totalLenth++)]=0x0d;
Arry[(totalLenth++)]=0x0a;

TRACE_DEBUG("len 2222=%d,\n",totalLenth);
for(i=0;i<totalLenth;i++)
{
	TRACE_DEBUG("Arry total[%d]=0x%2x",i,Arry[i]);
}

g_sendDataArryLen=totalLenth;
for(i=0;i<totalLenth+1;i++)
{
	if(i<totalLenth)
		{
		g_sendDataArry[i]=Arry[i];
		}
	else
		{
		g_sendDataArry[i]=0x00;
		}
}
}


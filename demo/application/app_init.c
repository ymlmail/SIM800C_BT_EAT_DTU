//yemaolin creat 2015-7-29
#include "App_init.h"
#include "App_include.h"
#include "App_include.h"//yml add 2015-8-5

void ReadNvramData(void);


void App_initTimer(void)
{
	eat_timer_start(EAT_TIMER_1, 1000);//开启定时器，时间到了之后需要重新开
}
static void State_InitVariable(void)
	{
		StateIndex=StandbyState;
		PrevStateIndex=PowerOnState;
	}

void GlobalVariableInit(void)
{
	key.key1=0;
	key.key2=0;
	key.key3=0;
	key.key4=0;
	key.key5=0;
	key.key6=0;
	key.key7=0;
	key.key8=0;

	key.key1_hold=0;
	key.key2_hold=0;
	key.key3_hold=0;
	key.key4_hold=0;
	key.key5_hold=0;
	key.key6_hold=0;
	key.key7_hold=0;
	key.key8_hold=0;

	key.key1_fst=0;
	key.key2_fst=0;
	key.key3_fst=0;
	key.key4_fst=0;
	key.key5_fst=0;
	key.key6_fst=0;
	key.key7_fst=0;
	key.key8_fst=0;

	flag.timer1up=0;
	flag.timer2up=0;
	flag.timer3up=0;
	flag.timer4up=0;
	flag.timer41up=0;
	flag.timer5up=0;
	flag.timer8up=0;
	
	flag.timer16up=0;
	
    flag.recvSeverData=0;
    flag.LoginOK=0;
	flag.sms_ready=0;
	flag.GSMisOk=0;

	g_CounterSec =0;
	flag.reBootTP_20=0;
	flag.reBootTP_10=0;
	flag.SeverConnected=0;
	flag.GPS_TIMER_UP=0;

	flag.askLBSmsg=0;
	flag.LBSmsgReady=0;
	State_InitVariable();

	g_SendDatAbnormalCnt=0;
	socket_id_bak=0;
	socket_id=0;
	g_VoltageValue=V_LEVEL_6;	//开机默认为最高电压
	g_GsmSignalValue=GSM_LEVEL_0;	//默认为信号低

	
	g_GpsSpeed=0;
	g_GpsMoveDir=0;
	g_GpsLat_1=0;
	g_GpsLat_2=0;
	g_GpsLat_3=0;
	g_GpsLat_4=0;
	
	g_Gpslong_1=0;
	g_Gpslong_2=0;
	g_Gpslong_3=0;
	g_Gpslong_4=0;
	flag.GpsLatitude=1;	// 1表示表示为北纬,0为南纬
	flag.GpsLongitude=1;	// 1 表示为东经,0为西经
	flag.GPSisOk=0;
	clear_CallArry();
	flag.sosIsNull=1;

	flag.ServerCmdSeeSOS=0;
	flag.ServerCmdGPSON=0;
	g_ServerCmdGPSONTimer = 0;

	
	ReadNvramData();
	
}

void ReadNvramData(void)
{
	sockaddr_struct Netaddr={0};
	static char readNvramBuf[50]="";
	u8 i,j;
	s32 ret;
	u8 *ptr1="13320984483,";
	u8 *p;
	u8 num=0;
	//如果是第一次初始化存储空间
	for(i=0;i<50;i++)
		readNvramBuf[i]=0;
	ret=app_nvram_read(NVRAM_ADDR_MAX, readNvramBuf,10);
	eat_trace("eat_fs_Read():Read File Success!ret=%d,%s",ret,readNvramBuf);
	if(0!=strcmp(readNvramBuf,"0x55.0xAA#"))
		{
		eat_trace("init nvram files\n");
		app_nvram_save(NVRAM_ADDR_MAX, "0x55.0xAA#",10);
		app_nvram_save(SERVER_ADDR, "222.211.65.62,8888",18);
		
		//app_nvram_save(SOS_NUM_1_ADDR, "13320984483",11);
		//app_nvram_save(SOS_NUM_2_ADDR, "15008464603",11);
		
		app_nvram_save(SOS_NUM_1_ADDR, "NULL\0\0\0\0\0\0\0",11);
		app_nvram_save(SOS_NUM_2_ADDR, "NULL\0\0\0\0\0\0\0",11);
		app_nvram_save(SOS_NUM_3_ADDR, "NULL\0\0\0\0\0\0\0",11);
		app_nvram_save(SOS_NUM_4_ADDR, "NULL\0\0\0\0\0\0\0",11);
		eat_sleep(100);
		}
	else
		{}
	for(i=0;i<50;i++)
		readNvramBuf[i]=0;
	ret=app_nvram_read(SERVER_ADDR, readNvramBuf,20);
	eat_trace("eat_fs_Read():Read File Success!ret=%d,%s",ret,readNvramBuf);
		p=readNvramBuf;
	   ///sscanf(readNvramBuf,"%15[^,]",Netaddr.addr);//遇到符号','后停止输出
	   g_server_address.addr[0]=atoi(p);
	   p=strchr(p,'.')+1;
	   g_server_address.addr[1]=atoi(p);
	   p=strchr(p,'.')+1;
	   g_server_address.addr[2]=atoi(p);
	   p=strchr(p,'.')+1;
	   g_server_address.addr[3]=atoi(p);
	   
	   p=strchr(p,',')+1;
	g_server_address.port=atoi(p);//将P所指的字符串变为数字。
	eat_trace("g_server_address.addr1 =%d,%d,%d,%d",g_server_address.addr[0],g_server_address.addr[1],g_server_address.addr[2],g_server_address.addr[3]);
	eat_trace("g_server_address.port1 =%d",g_server_address.port);
for(j=0;j<4;j++)
{
	for(i=0;i<50;i++)
		readNvramBuf[i]=0;
	ret=app_nvram_read(SOS_NUM_1_ADDR+j, readNvramBuf,11);
	eat_trace("eat_fs_Read():Read File Success!ret=%d,%s",ret,readNvramBuf);
	strcpy(SosNumber[j],readNvramBuf);
	if(SosNumber[j][0]!='N')//如果有一个不为空，则把SOS为空的标志位值0
		{
		flag.sosIsNull=0;
		}
	eat_trace("SosNumber[%d]=%s",j,SosNumber[j]);
}
if(!flag.sosIsNull)
{
	eat_trace("SosNumber is no null");
}
else
{
	eat_trace("SosNumber is  null");
}
}

void intGpsModule(void)
{
	TRACE_DEBUG("++++++++++++++++Start GPS SET+++++++++++++++++++");
	eat_uart_write(eat_uart_GPS, "$CCRMO,,3,*4F\r\n", 15);//关闭所有输出语句
	eat_sleep(1000);//
	eat_uart_write(eat_uart_GPS, "$CCSIR,3,0*4B\r\n", 15);//
	eat_sleep(1000);
	eat_uart_write(eat_uart_GPS, "$CCRMO,GGA,2,1.0*20\r\n", 21);//开启GGA语句输出
	eat_sleep(1000);
	eat_uart_write(eat_uart_GPS, "$CCRMO,RMC,2,1.0*3D\r\n", 21);//开启RMC语句输出
	eat_sleep(1000);//	
	//eat_uart_write(eat_uart_GPS, "$PCAS02,4000*2B\r\n", 17);//4S输出一次
	eat_uart_write(eat_uart_GPS, "$PCAS02,10000*1E\r\n", 18);//10S输出一次
	eat_sleep(1000);//
	eat_uart_write(eat_uart_GPS, "$CCSIR,3,1*4A\r\n", 15);//BD2/GPS双模（冷启动）
	TRACE_DEBUG("++++++++++++++++END GPS SET+++++++++++++++++++");
}



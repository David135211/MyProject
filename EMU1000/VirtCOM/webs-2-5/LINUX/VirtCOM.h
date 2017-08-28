#ifndef _VIRTCOM_H_
#define _VIRTCOM_H_

#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<string.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<sys/wait.h>
#include	<signal.h>
#include	<netdb.h>
#include 	<stdio.h>
#include 	<stdlib.h>


#define ERROR  -1

#define NONE 0
#define EVEN 1
#define ODD 2
#define TCP 1
#define UDP 2
#define RS485 0
#define RS232 1
#define RS422 2
#define GET_CONFIG	0x01
#define SET_CONFIG	0x02
#define	SEND_CONFIG	0x03
#define	GET_STATUS	0x04
#define SET_DEFAULTCONFIG	0x05

#define	SERIAL_CFG_INTERVAL_IN_SECOND  3

#define	SERIAL_DEFAULT_BAUD  9600

//used for remote zhaohuan, remote send broadcast udp packet to this port
#define	NETPORT_ZHAOHUAN  6799
#define  MAX_CONN_PER_ZHAOHUAN_PORT   1024
//used for remote manage serial
#define	NETPORT_REMOTEMANAGE  6100
#define  MAX_CONN_PER_REMOTEMANAGE_PORT   1

#define  MIN_PORT_FOR_SERIAL 4000
#define  MAX_PORT_FOR_SERIAL 60000



#define  VIRTCOM_VERSION  "V19"
#define	MAX_SERIAL_PORTS_NUM  64
#define	MAX_NET_PORTS_NUM  64
#define	MAX_RECORDS  16
//#define	MAX_CONN_PER_SERIAL_PORT  5
#define  MAX_LEN_ITEM    2048 //every process item len

/*lel*/
#if 1
#define FILE_NAME_GPIO "/dev/gpio_drv"
#else
#define FILE_NAME_GPIO "/dev/gpio_drv.ko"
#endif
/*end*/
#define FILE_NAME_NETNUM "/disk/.NetNum"
#define FILE_NAME_COMNUM "/disk/.COMNum"
#define FILE_NAME_PRINTFILE  "/tmp/VirtCOM"
#define FILE_NAME_CONFIGFILE  "/myapp/DevConfig.cfg"
#define FILE_NAME_CONFIGFILE_BIN  "/myapp/DevConfig"


typedef struct {
	int rate;		/*115200,...*/
	int databits;		/*8,7,6,5*/
	int parity;		/* 'N' = NONE,'E'= Even,'O' = Odd */
	int stopbit;		/*1,15, 2*/

	int net_port;		/*4000,4001,...*/
	int socket_type;	/*1=tcp;2=udp*/
	int ClientMode; /* 0= work in server mode., 1= work in client mode */
	int ServerIP; /* remote server ip when work in client mode */
	int Interface; /* 0=RS485, 1= RS232, 2= RS422*/
}DEV_COMCONFIG;

typedef struct {
	unsigned int IPAddr;
	unsigned int NetMask;
	unsigned int MACAddr1;
	unsigned int MACAddr2;
	unsigned int Gateway;
	unsigned int DNS;
	unsigned int SecondDNS;
}DEV_NETCONFIG;


typedef struct{
	int cmd;
	unsigned char DevName[128];
	int ValidNetNum; //valid network port number
	int ValidCOMNum; //valid com port number
	DEV_NETCONFIG IPConfig[MAX_NET_PORTS_NUM];
	DEV_COMCONFIG COMConfig[MAX_SERIAL_PORTS_NUM];	
}DEV_CONFIG;

typedef struct {
	int Index; 
	//int COMNum; /* 1=COM1,2=COM2,...*/	
	int ServerPort;
	unsigned char COMDevName[128];
	int Baud; //115200,38400,....
	int DataBit; //8,7,6,5,,,
	int Parity; //'N' = NONE,'E'= Even,'O' = Odd
	int StopBit; // 1 = 1,15=1.5, 2=2
	int Interface; /* 0=RS485, 1= RS232, 2= RS422*/
}NETCOM_TABLE_ITEM;

typedef struct {
	int ValidLen;
	unsigned char Buffer[MAX_LEN_ITEM];
}SendBUF_ITEM;


typedef struct  {
		char portindex[3];		/*索引*/
		char rate[7];		/*波特率*/
		char databits[2];	/*数据位*/
		char parity[2]; 	/*奇偶校验*/
		char stopbit[3];	/*停止位*/
		
		char net_port[6];		/*端口号*/
		char socket_type[4];	/*网络协议类型*/
		char ClientMode[7]; 	/*工作模式*/
		char ServerIP[16]; 		/* 服务器IP地址*/
		char Interface[6]; 		/* 串口协议*/
} PORT_ATTRIBUTE; 

typedef struct {
	unsigned char SoftVer[64];
	int ValidSerial; 
	int ValidNet;
	DEV_COMCONFIG Serial[MAX_SERIAL_PORTS_NUM];
	DEV_NETCONFIG Net[MAX_NET_PORTS_NUM];
	unsigned char CRC;
	int ConfigErr;
	unsigned char ErrStatus[512];
}WEB_CONFIG;

typedef struct {
	unsigned char COMStatus[16];
	unsigned int ClientSendByte;
	unsigned int ClientRecvByte;
	int SerialRecvByte;
	int SerialSendByte;
}VCOM_STATUS;


typedef struct {
	int COMIndex;
	int COMValid;


	//Run time Param
	int COMFd;
	int NetServerFD; //the Server Socket in Server Mode
	//int NetClientFD; //the Server Socket in Server Mode
	volatile int NetDataFD;
	int ClientPort;
	unsigned int ClientIP;
	volatile unsigned int ClientSendByte;
	volatile unsigned int ClientRecvByte;
	//int ClientMAC[MAX_CONN_PER_SERIAL_PORT][2]; // maybe not known
	struct	sockaddr_in NetClientAddr;
	pthread_t ThreadID_NetClient;
	//SendBUF_ITEM SerialSendBuf[MAX_RECORDS];
	//int SerialSendBuf_WriteIndex;
	//sem_t Sem_SerialSend;
	int SerialSendReady;
	volatile int SerialRecvByte;
	volatile int SerialSendByte;

	//run state
	volatile int NetDataConnectOK;
	int TCPConnectStartFlag;

	pthread_t ThreadID_NetConsole;
	volatile int NetConsoleConnectOK;
	volatile int NetConsoleFD;
	int ClientConsolePort;
	unsigned int ClientConsoleIP;
	struct	sockaddr_in NetConsoleClientAddr;
	unsigned char NetConsoleSendBuffer[1024];

	pthread_mutex_t COMParamLock; //used to lock the setting while setting.
	//int COMParamLock; //used to lock the setting while setting.
	time_t COMLastCfgTime;

	pthread_t ThreadID_Net; //TCP Server Thread ID
	//pthread_t ThreadID_COMRecv;
	//pthread_t ThreadID_COMSend;


	//Net Param
	int ServerPort;
	int WorkInClientMode; /* 1 = work in Client Mode */
	int ServerIP; //used when work in client mode
	int TCPConnect;


	// Serial Param
	unsigned char COMDevName[128];
	unsigned char COMStatus[16]; //OK?Failed?
	int Baud; //115200,38400,....
	int DataBit; //8,7,6,5,,,
	int Parity; //'N' = NONE,'E'= Even,'O' = Odd
	int StopBit; // 1 = 1,15=1.5, 2=2
	int Interface; /* 0=RS485, 1= RS232, 2= RS422*/
}NETCOM_COM_PARAM_STATE;

typedef struct {
	int  COMNum;
	int NetNum;

	int LEDStatus_Alarm;

	pthread_t ThreadID_WebServer;


	int NetManageFD;
	int NetManagePort;
	int NetManageClientFD[MAX_CONN_PER_REMOTEMANAGE_PORT]; // 0 = invalid
	int NetManageClientIP[MAX_CONN_PER_REMOTEMANAGE_PORT]; //
	int NetManageClientPort[MAX_CONN_PER_REMOTEMANAGE_PORT]; //
	int NetManageLastPort[MAX_CONN_PER_REMOTEMANAGE_PORT]; //last managed serial data port
	pthread_t ThreadID_NetManageClient[MAX_CONN_PER_REMOTEMANAGE_PORT];

	//zhaohuan port in UDP
	int NetMZhaoHuanFD;
	int NetZhaoHuanPort;
	int NetZhaoHuanClientFD[MAX_CONN_PER_ZHAOHUAN_PORT];// 0 = invalid
	pthread_t ThreadID_NetZhaoHuanClient[MAX_CONN_PER_REMOTEMANAGE_PORT];
	pthread_t ThreadID_NetZhaoHuan;

	//PrintFile
	pthread_t ThreadID_PrintFile;
	pthread_t ThreadID_ClientTCPManage;
	pthread_t ThreadID_ProcessSerial;
	pthread_t ThreadID_ProcessNet;

	volatile NETCOM_COM_PARAM_STATE COMState[MAX_SERIAL_PORTS_NUM];
}VIRT_COM_DEVSTATE;

typedef struct __attribute__ ((packed))  { 
	unsigned int  UserTag;
	unsigned char Cmd;
	unsigned int  NetPort;
	unsigned char Baudrate;
	unsigned char Pariority;
	unsigned char DataBit;
	unsigned char StopBit;
	unsigned short Pad;
}REMOTE_SET_SERIAL_PARAM;

typedef struct __attribute__ ((packed)) _MUSTER_TELE { 
	//消息ID，默认为4个0。 
	unsigned int  xid;                 
	//消息类型，0-上位机发出，用于召唤设备，1-设备响应召唤（设备返回），3-上位机向设备发送配置数据 
	unsigned char msg_type;           
	//muster版本，一直为1。 
	unsigned char muster_ver;        
	//主机名，msg_type=0时置0，msg_type=1时为设备返回的主机名，msg_type=3时指定设备的新主机名。 
	unsigned char net_hostname[12];      
	//网络物理地址，msg_type=0时置0，msg_type=1时为设备返回的MAC地址， 
//msg_type=3时，指定为要修改设备的Mac地址。 
unsigned char net_mac[6];         
//IP地址，msg_type=0时置0，msg_type=1时为设备返回的IP地址，msg_type=3时设备修改IP为该地址。 
unsigned int net_ip_addr;        
//子网络掩码，msg_type=0时置0，msg_type=1时为设备返回的掩码，msg_type=3时设备修改掩码为该掩码。 
unsigned int net_ip_mask;        
//网关，保留参数，尚未使用。 
unsigned int net_gateway;        
//产品型号,尚未使用。 
unsigned int devmodel;          
//工作模式,0-无效，1-Server模式,2-Client模式,10-UDP模式 
//msg_type=0时置0，msg_type=1时为设备返回的工作模式，msg_type=3时设备忽略该参数。 
unsigned int workmodel;          
//设备的端口数，msg_type=0时置0，msg_type=1时为设备返回的端口数量，msg_type=3时设备忽略该参数。 
unsigned int portnum;     
//第一个网络监听，保留参数，尚未使用 
unsigned int firstport;          
//文本形式的固件版本说明， 
//msg_type=0时置0，msg_type=1时为设备返回的固件版本说明，msg_type=3时设备忽略该参数。 
char firmware[24];      
//文本形式的口令。 
//msg_type=0时置0，msg_type=1设备忽略该参数，msg_type=3时设备将检查该口令，如果合法修改自身参数。 
unsigned char cfgpwd[12]; 
//msg_type=0时置0，msg_type=1设备返回当前的监听IP，msg_type=3时设备忽略该参数。 
//映射IP，与mapport联合使用，mapip和mapport数组的第1个元素为一个可使用的TCP/IP连接。 
//如：mapip[0]与mapport[0]指出设备第一个监听的IP和Port。可以直接使用这个参数与设备建立TCP/IP连接。 
    unsigned int mapip[32];          
//映射端口 
//msg_type=0时置0，msg_type=1设备返回当前的监听Port，msg_type=3时设备忽略该参数。 
   unsigned int mapport[32];       
    
} MUSTER_TELE;


typedef struct __attribute__ ((packed))  { 
//消息ID，默认为4个0。 
unsigned int  xid;                 
//消息类型，0-上位机发出，用于召唤设备，1-设备响应召唤（设备返回），3-上位机向设备发送配置数据 
unsigned char msg_type;           
//muster版本，一直为1。 
unsigned char muster_ver;        
//主机名，msg_type=0时置0，msg_type=1时为设备返回的主机名，msg_type=3时指定设备的新主机名。 
unsigned char net_hostname[12];      
//网络物理地址，msg_type=0时置0，msg_type=1时为设备返回的MAC地址， 
//msg_type=3时，指定为要修改设备的Mac地址。 
unsigned char net_mac[6];         
//IP地址，msg_type=0时置0，msg_type=1时为设备返回的IP地址，msg_type=3时设备修改IP为该地址。 
unsigned int net_ip_addr;        
//子网络掩码，msg_type=0时置0，msg_type=1时为设备返回的掩码，msg_type=3时设备修改掩码为该掩码。 
unsigned int net_ip_mask;        
//网关，保留参数，尚未使用。 
unsigned int net_gateway;        
//产品型号,尚未使用。 
unsigned int devmodel;          
//工作模式,0-无效，1-Server模式,2-Client模式,10-UDP模式 
//msg_type=0时置0，msg_type=1时为设备返回的工作模式，msg_type=3时设备忽略该参数。 
unsigned int workmodel;          
//设备的端口数，msg_type=0时置0，msg_type=1时为设备返回的端口数量，msg_type=3时设备忽略该参数。 
unsigned int portnum;     
//第一个网络监听，保留参数，尚未使用 
unsigned int firstport;          
//文本形式的固件版本说明， 
//msg_type=0时置0，msg_type=1时为设备返回的固件版本说明，msg_type=3时设备忽略该参数。 
char firmware[24];      
//文本形式的口令。 
//msg_type=0时置0，msg_type=1设备忽略该参数，msg_type=3时设备将检查该口令，如果合法修改自身参数。 
unsigned char cfgpwd[12]; 
//msg_type=0时置0，msg_type=1设备返回当前的监听IP，msg_type=3时设备忽略该参数。 
//映射IP，与mapport联合使用，mapip和mapport数组的第1个元素为一个可使用的TCP/IP连接。 
//如：mapip[0]与mapport[0]指出设备第一个监听的IP和Port。可以直接使用这个参数与设备建立TCP/IP连接。 
 //   unsigned int mapip[32];          
//映射端口 
//msg_type=0时置0，msg_type=1设备返回当前的监听Port，msg_type=3时设备忽略该参数。 
//   unsigned int mapport[32];       
    
} MUSTER_TELE_TEST;

typedef struct {
	unsigned int Index;
	NETCOM_COM_PARAM_STATE *pCOMParam;
}STRUCT_FOR_NEW_NETCLIENT;

int Serial_Send(NETCOM_COM_PARAM_STATE *pCOMParam,unsigned char *pData, int Len);
void VirtCOM_PrintBuf(unsigned char *Buffer,int Len);

void NetServer_NewClient_UDP(NETCOM_COM_PARAM_STATE *pCOMParam);
void NetServer_NewClient_TCP(STRUCT_FOR_NEW_NETCLIENT *pNetClientParam);
int SwapChar(unsigned char *pAddr32,float Data);
int main_Serial(NETCOM_COM_PARAM_STATE *pCOMParam);
void Manage_NewClient_TCP(unsigned int  *pIndex);
int Serial_ChangeParam(NETCOM_COM_PARAM_STATE *pCOMParam);
unsigned int SwapChar32(unsigned int  pSrc32);
void VirtCOM_PrintBufInASCII(unsigned char *Buffer,int Len);

int net_SendBuffer_Console(NETCOM_COM_PARAM_STATE *pCOMParam,unsigned char *pBuffer,int Len,int DataFromSerial);

#endif //_VIRTCOM_H_

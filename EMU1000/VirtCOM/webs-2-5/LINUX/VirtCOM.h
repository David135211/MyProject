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
		char portindex[3];		/*����*/
		char rate[7];		/*������*/
		char databits[2];	/*����λ*/
		char parity[2]; 	/*��żУ��*/
		char stopbit[3];	/*ֹͣλ*/
		
		char net_port[6];		/*�˿ں�*/
		char socket_type[4];	/*����Э������*/
		char ClientMode[7]; 	/*����ģʽ*/
		char ServerIP[16]; 		/* ������IP��ַ*/
		char Interface[6]; 		/* ����Э��*/
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
	//��ϢID��Ĭ��Ϊ4��0�� 
	unsigned int  xid;                 
	//��Ϣ���ͣ�0-��λ�������������ٻ��豸��1-�豸��Ӧ�ٻ����豸���أ���3-��λ�����豸������������ 
	unsigned char msg_type;           
	//muster�汾��һֱΪ1�� 
	unsigned char muster_ver;        
	//��������msg_type=0ʱ��0��msg_type=1ʱΪ�豸���ص���������msg_type=3ʱָ���豸������������ 
	unsigned char net_hostname[12];      
	//���������ַ��msg_type=0ʱ��0��msg_type=1ʱΪ�豸���ص�MAC��ַ�� 
//msg_type=3ʱ��ָ��ΪҪ�޸��豸��Mac��ַ�� 
unsigned char net_mac[6];         
//IP��ַ��msg_type=0ʱ��0��msg_type=1ʱΪ�豸���ص�IP��ַ��msg_type=3ʱ�豸�޸�IPΪ�õ�ַ�� 
unsigned int net_ip_addr;        
//���������룬msg_type=0ʱ��0��msg_type=1ʱΪ�豸���ص����룬msg_type=3ʱ�豸�޸�����Ϊ�����롣 
unsigned int net_ip_mask;        
//���أ�������������δʹ�á� 
unsigned int net_gateway;        
//��Ʒ�ͺ�,��δʹ�á� 
unsigned int devmodel;          
//����ģʽ,0-��Ч��1-Serverģʽ,2-Clientģʽ,10-UDPģʽ 
//msg_type=0ʱ��0��msg_type=1ʱΪ�豸���صĹ���ģʽ��msg_type=3ʱ�豸���Ըò����� 
unsigned int workmodel;          
//�豸�Ķ˿�����msg_type=0ʱ��0��msg_type=1ʱΪ�豸���صĶ˿�������msg_type=3ʱ�豸���Ըò����� 
unsigned int portnum;     
//��һ�����������������������δʹ�� 
unsigned int firstport;          
//�ı���ʽ�Ĺ̼��汾˵���� 
//msg_type=0ʱ��0��msg_type=1ʱΪ�豸���صĹ̼��汾˵����msg_type=3ʱ�豸���Ըò����� 
char firmware[24];      
//�ı���ʽ�Ŀ�� 
//msg_type=0ʱ��0��msg_type=1�豸���Ըò�����msg_type=3ʱ�豸�����ÿ������Ϸ��޸���������� 
unsigned char cfgpwd[12]; 
//msg_type=0ʱ��0��msg_type=1�豸���ص�ǰ�ļ���IP��msg_type=3ʱ�豸���Ըò����� 
//ӳ��IP����mapport����ʹ�ã�mapip��mapport����ĵ�1��Ԫ��Ϊһ����ʹ�õ�TCP/IP���ӡ� 
//�磺mapip[0]��mapport[0]ָ���豸��һ��������IP��Port������ֱ��ʹ������������豸����TCP/IP���ӡ� 
    unsigned int mapip[32];          
//ӳ��˿� 
//msg_type=0ʱ��0��msg_type=1�豸���ص�ǰ�ļ���Port��msg_type=3ʱ�豸���Ըò����� 
   unsigned int mapport[32];       
    
} MUSTER_TELE;


typedef struct __attribute__ ((packed))  { 
//��ϢID��Ĭ��Ϊ4��0�� 
unsigned int  xid;                 
//��Ϣ���ͣ�0-��λ�������������ٻ��豸��1-�豸��Ӧ�ٻ����豸���أ���3-��λ�����豸������������ 
unsigned char msg_type;           
//muster�汾��һֱΪ1�� 
unsigned char muster_ver;        
//��������msg_type=0ʱ��0��msg_type=1ʱΪ�豸���ص���������msg_type=3ʱָ���豸������������ 
unsigned char net_hostname[12];      
//���������ַ��msg_type=0ʱ��0��msg_type=1ʱΪ�豸���ص�MAC��ַ�� 
//msg_type=3ʱ��ָ��ΪҪ�޸��豸��Mac��ַ�� 
unsigned char net_mac[6];         
//IP��ַ��msg_type=0ʱ��0��msg_type=1ʱΪ�豸���ص�IP��ַ��msg_type=3ʱ�豸�޸�IPΪ�õ�ַ�� 
unsigned int net_ip_addr;        
//���������룬msg_type=0ʱ��0��msg_type=1ʱΪ�豸���ص����룬msg_type=3ʱ�豸�޸�����Ϊ�����롣 
unsigned int net_ip_mask;        
//���أ�������������δʹ�á� 
unsigned int net_gateway;        
//��Ʒ�ͺ�,��δʹ�á� 
unsigned int devmodel;          
//����ģʽ,0-��Ч��1-Serverģʽ,2-Clientģʽ,10-UDPģʽ 
//msg_type=0ʱ��0��msg_type=1ʱΪ�豸���صĹ���ģʽ��msg_type=3ʱ�豸���Ըò����� 
unsigned int workmodel;          
//�豸�Ķ˿�����msg_type=0ʱ��0��msg_type=1ʱΪ�豸���صĶ˿�������msg_type=3ʱ�豸���Ըò����� 
unsigned int portnum;     
//��һ�����������������������δʹ�� 
unsigned int firstport;          
//�ı���ʽ�Ĺ̼��汾˵���� 
//msg_type=0ʱ��0��msg_type=1ʱΪ�豸���صĹ̼��汾˵����msg_type=3ʱ�豸���Ըò����� 
char firmware[24];      
//�ı���ʽ�Ŀ�� 
//msg_type=0ʱ��0��msg_type=1�豸���Ըò�����msg_type=3ʱ�豸�����ÿ������Ϸ��޸���������� 
unsigned char cfgpwd[12]; 
//msg_type=0ʱ��0��msg_type=1�豸���ص�ǰ�ļ���IP��msg_type=3ʱ�豸���Ըò����� 
//ӳ��IP����mapport����ʹ�ã�mapip��mapport����ĵ�1��Ԫ��Ϊһ����ʹ�õ�TCP/IP���ӡ� 
//�磺mapip[0]��mapport[0]ָ���豸��һ��������IP��Port������ֱ��ʹ������������豸����TCP/IP���ӡ� 
 //   unsigned int mapip[32];          
//ӳ��˿� 
//msg_type=0ʱ��0��msg_type=1�豸���ص�ǰ�ļ���Port��msg_type=3ʱ�豸���Ըò����� 
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

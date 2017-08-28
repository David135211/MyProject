#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <netdb.h>



#include	"VirtCOM.h"
//#include "webconfig.h"

#define FALSE  -1
#define TRUE   0

VIRT_COM_DEVSTATE VirtCOM_DevState;
DEV_CONFIG gDevConfig;


int  COMNum;
static int  NetNum;


int	main_net_server(int Port);
int  main_net_client_tcp(void *pParam);
int	main_net_server_tcp(NETCOM_COM_PARAM_STATE *pCOMParam);
int	main_net_server_udp(NETCOM_COM_PARAM_STATE *pCOMParam);
int	main_NetManage(void);
int	main_PrintFile(void);
int main_webserver(void);

int get_data_from_file(char *data);
void system_execute(DEV_CONFIG *exe);
int	main_ZhaoHuan(void);

void InitConfigServer(void);

void VirtCOM_PrintBuf(unsigned char *Buffer, int Len)
{
	int i;

	printf("========= %d  ============\n",Len);
	for(i = 0; i < Len; i++){
		if(i && ((i % 8) == 0)) printf("\n");
		//printf("%d(0x%02x)(%c) ",Buffer[i],Buffer[i],Buffer[i]);
		//printf("%03d(0x%02x)(%02c) ",Buffer[i],Buffer[i],Buffer[i]);
		printf("0x%02x   ", Buffer[i]);
	}
	printf("\n");
}
void VirtCOM_PrintBufInASCII(unsigned char *Buffer, int Len)
{
	int i;

	printf("========= %d  ============\n",Len);
	for(i = 0; i < Len; i++){
		printf("%c  ", Buffer[i]);
	}
	printf("\n");
}

int WMConfig_GetLocalNetMask(unsigned int *pMask, unsigned char *pName)
{
	int sock_netmask;
	char netmask_addr[50];
	struct ifreq ifr_mask;
	struct sockaddr_in *net_mask;

	sock_netmask = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_netmask <= 0){
		perror("socket error.\n");
		printf("%s->Line %d, socket error.\n", __FUNCTION__, __LINE__);
		return -1;
	}

	//printf("%s->Line %d,\n",__FUNCTION__,__LINE__,*pMask);

	memset(&ifr_mask, 0, sizeof(ifr_mask));
	strncpy(ifr_mask.ifr_name, pName, sizeof(ifr_mask.ifr_name) - 1);
	if((ioctl(sock_netmask, SIOCGIFNETMASK, &ifr_mask)) < 0){
		perror("socket SIOCGIFNETMASK error.\n");
		printf("%s->Line %d, SIOCGIFNETMASK error.\n", __FUNCTION__, __LINE__);
		return -2;
	}
	net_mask = (struct sockaddr_in *) & (ifr_mask.ifr_netmask);
	*pMask = (unsigned int)(net_mask->sin_addr.s_addr);

	close(sock_netmask);

	printf("%s->Line %d, net mask 0x%08x.\n", __FUNCTION__, __LINE__, *pMask);

	return 0;
}


int  VirtCOM_InitCOMParamDefault(NETCOM_COM_PARAM_STATE *pParam)
{

	int Index = pParam->COMIndex ;

	memset(pParam, 0, sizeof(NETCOM_COM_PARAM_STATE));
	pParam->COMIndex = Index;
	pParam->COMValid = 1;

	//sprintf(pParam->COMDevName,"/dev/ttyS%d",pParam->COMIndex);
	/*lel*/
#if 0
	sprintf(pParam->COMDevName, "/dev/ttySub%d", pParam->COMIndex);
#else
	sprintf(pParam->COMDevName, "/dev/ttyCH%d", pParam->COMIndex);
#endif
	/*end*/
	//serial and net config use dev config

	return 0;
}

int  VirtCOM_PrintCOMParam(NETCOM_COM_PARAM_STATE *pParam)
{
	struct in_addr MyIP;

	printf("\n\n\n========== Index: %d ================\n", pParam->COMIndex);
	printf("COM Device Name: %s\n", pParam->COMDevName);
	printf("Server Port:%d\n", pParam->ServerPort);
	printf("Net Connect Type: %s\n", pParam->TCPConnect ? "TCP" : "UDP");
	printf("COM Baudrate: %d\n", pParam->Baud);
	printf("COM DataBit: %d\n", pParam->DataBit);
	printf("COM Parity: %s\n", pParam->Parity == 'N' ? "None":
			(pParam->Parity == 'E' ? "Even" : "Odd"));
	printf("COM StopBit: %s\n", pParam->StopBit == 1 ? "1":
			(pParam->StopBit == 15 ? "1.5" : "2"));
	printf("COM Net Mode: %s\n", pParam->WorkInClientMode == 1 ? "CLIENT" : "SERVER");
	if(pParam->WorkInClientMode){
		MyIP.s_addr = pParam->ServerIP;
		printf("Server IP: %s\n", inet_ntoa(MyIP));
	}
	printf("COM Interface: %s\n", pParam->Interface == 0 ? "RS485" : (pParam->Interface == 1) ? "RS232" : "RS422");

	return 0 ;
}

int VirtCOM_InitCOMParam(NETCOM_COM_PARAM_STATE *pParam)
{
	VirtCOM_InitCOMParamDefault(pParam);

	//printf("%s->Line %d,Index:%d,Parity:%d.\n",__FUNCTION__,__LINE__,pParam->COMIndex,
	//	gDevConfig.COMConfig[pParam->COMIndex].parity);

	pParam->Baud    = gDevConfig.COMConfig[pParam->COMIndex].rate;
	pParam->DataBit = gDevConfig.COMConfig[pParam->COMIndex].databits;
	pParam->Parity  = gDevConfig.COMConfig[pParam->COMIndex].parity;
	pParam->StopBit = gDevConfig.COMConfig[pParam->COMIndex].stopbit;

	pParam->ServerPort       = gDevConfig.COMConfig[pParam->COMIndex].net_port;
	pParam->TCPConnect       = gDevConfig.COMConfig[pParam->COMIndex].socket_type;
	pParam->WorkInClientMode = gDevConfig.COMConfig[pParam->COMIndex].ClientMode;
	if(pParam->WorkInClientMode){
		pParam->ServerIP = gDevConfig.COMConfig[pParam->COMIndex].ServerIP;
	}
	pParam->Interface = gDevConfig.COMConfig[pParam->COMIndex].Interface;

	VirtCOM_PrintCOMParam(pParam);
	return 0;
}

void DevConfig_PrintConfig(DEV_CONFIG *OK)
{
	int i=0;

#if 0
	printf("servername\t = %s\n\n", OK->DevName);
	printf("ValidNetNum \t = %d\n\n", OK->ValidNetNum);
	for(i=0;i<OK->ValidNetNum;i++){

		printf("==========  eth%d  ==================\n",i);
		printf("IP\t = 0x%x\n",OK->IPConfig[i].IPAddr);
		printf("NET MASK\t = 0x%x\n", OK->IPConfig[i].NetMask);
		printf("MAC Addr1\t = 0x%x\n", OK->IPConfig[i].MACAddr1);
		printf("MAC Addr2\t = 0x%x\n", OK->IPConfig[i].MACAddr2);
		printf("Gateway\t = 0x%x\n", OK->IPConfig[i].Gateway);
		printf("DNS\t = 0x%x\n", OK->IPConfig[i].DNS);
		printf("DNS_Second\t = 0x%x\n\n", OK->IPConfig[i].SecondDNS);
	}
#endif
	printf("ValidCOMNum \t = %d\n\n", OK->ValidCOMNum);
	for(i = 0; i < OK->ValidCOMNum; i++){
		/*lel*/
#if 0
		printf("==========  ttySub%d  ==================\n",i);
#else
		printf("==========  ttyCH%d  ==================\n", i);
#endif
		/*end*/
		printf("rate\t = %d\n", OK->COMConfig[i].rate);
		printf("datas\t = %d\n", OK->COMConfig[i].databits);
		printf("par\t = %c\n", OK->COMConfig[i].parity);
		printf("stop\t = %d\n", OK->COMConfig[i].stopbit);
		printf("net_p\t = %d\n", OK->COMConfig[i].net_port);
		printf("socket type\t = %s\n\n", (OK->COMConfig[i].socket_type == 1) ? "TCP" : "UDP");
	}
	return;

}


int  VirtCOM_InitDevParamDefault(DEV_CONFIG *OK)
{

	int i ;

	printf("***************** USE Default Dev Config *************************\n\n");

	sprintf(OK->DevName, "ServerName");
	//OK->ValidNetNum                = NetNum;
	for(i = 0; i < OK->ValidNetNum; i++){
		OK->IPConfig[i].IPAddr    = inet_addr("192.168.1.177") + (i << 16);
		OK->IPConfig[i].NetMask   = inet_addr("255.255.255.0");
		OK->IPConfig[i].MACAddr1  = 0x0001;
		OK->IPConfig[i].MACAddr2  = 0x02030405 + i;
		OK->IPConfig[i].Gateway   = inet_addr("192.168.1.1") + (i << 16);
		OK->IPConfig[i].DNS       = inet_addr("192.168.1.1");
		OK->IPConfig[i].SecondDNS = inet_addr("192.168.1.1");
	}

	OK->ValidCOMNum = COMNum;
	for(i = 0; i < OK->ValidCOMNum; i++){
		OK->COMConfig[i].rate        = SERIAL_DEFAULT_BAUD;
		OK->COMConfig[i].databits    = 8;
		OK->COMConfig[i].parity      = 'N';
		OK->COMConfig[i].stopbit     = 1;
		OK->COMConfig[i].net_port    = 4000 + i;
		OK->COMConfig[i].socket_type = 1; // TCP
		OK->COMConfig[i].ClientMode  = 0; // work in server mode
		OK->COMConfig[i].Interface   = 0; // 0                        = RS485
		OK->COMConfig[i].ServerIP    = inet_addr("192.168.1.250"); //
	}

	return 0;
}

int  VirtCOM_CorrectConfig(DEV_CONFIG *OK)
{
	int i;

	for(i = 0; i < OK->ValidCOMNum; i++){
		if((i == 0) || (i == 1) || (i == 3) || (i == 7))
			continue;
		if(0 != OK->COMConfig[i].Interface){
			printf("Serial %d [INTERFACE] is not Correct, we correct it to RS485.", i + 1);
			OK->COMConfig[i].Interface = 0; // 0=RS485
		}
	}
	return 0;
}

int  VirtCOM_InitDevParam(DEV_CONFIG *pParam)
{
	int ret;


	memset(pParam, 0, sizeof(DEV_CONFIG));
	//printf("%s->Line %d.\n",__FUNCTION__,__LINE__);
	pParam->ValidCOMNum = COMNum;
	pParam->ValidNetNum = NetNum;

	//printf("%s->Line %d.\n",__FUNCTION__,__LINE__);
	VirtCOM_InitDevParamDefault(pParam);

	//	printf("%s->Line %d.\n",__FUNCTION__,__LINE__);
	//	ret = WMLConfigFile(FILE_NAME_CONFIGFILE,pParam);
	/*read system config*/
	/*lel*/
#if 1
	ret = WMLConfigFile_BIN(pParam);
	if(ret){
		printf("Parse Config File Failed. ret = %d\n",ret);
		return -1;
	}
#endif
	/*end*/
	/*
	   when read binary config file from config file,may overwrite COMNum/NetNum
	   re set the COMNum/NetNum
	   */
	pParam->ValidCOMNum = COMNum;
	pParam->ValidNetNum = NetNum;

	VirtCOM_CorrectConfig(pParam);

//	DevConfig_PrintConfig(pParam);
	return 0;
}



#define CMD_SET_UT1_RS485		11
#define CMD_SET_UT1_NON_RS485	12
#define CMD_SET_UT2_RS485		13
#define CMD_SET_UT2_NON_RS485	14
#define CMD_SET_UT3_RS485		15
#define CMD_SET_UT3_NON_RS485	16
#define CMD_SET_UT4_RS485		17
#define CMD_SET_UT4_NON_RS485	18

/*
   0 = RS485, 1= RS232, 2= RS422
   */
int VirtCOM_SetSerialInterfaceAbout(DEV_CONFIG *pConfig)
{
	int i;
	int fd_GPIO, Data32, cmd;

	fd_GPIO = open(FILE_NAME_GPIO, O_RDWR);
	if (fd_GPIO < 0)
	{
		printf("open device gpio %s failed\n", FILE_NAME_GPIO);
		perror("error :\n");
		return (-1);
	}

	if(pConfig->COMConfig[0].Interface == 1){
		printf("Set Channel 1 to RS232 .\n");
		cmd = CMD_SET_UT1_NON_RS485;
	}else{
		printf("Set Channel 1 to RS485 .\n");
		cmd = CMD_SET_UT1_RS485;
	}
	ioctl(fd_GPIO, cmd, &Data32);

	if(pConfig->COMConfig[1].Interface == 1){
		printf("Set Channel 2 to RS232 .\n");
		cmd = CMD_SET_UT2_NON_RS485;
	}else{
		printf("Set Channel 2 to RS485 .\n");
		cmd = CMD_SET_UT2_RS485;
	}
	ioctl(fd_GPIO, cmd, &Data32);

	if(pConfig->COMConfig[3].Interface == 2){
		printf("Set Channel 4 to RS422 .\n");
		cmd = CMD_SET_UT4_NON_RS485;
	}else{
		printf("Set Channel 4 to RS485 .\n");
		cmd = CMD_SET_UT4_RS485;
	}
	ioctl(fd_GPIO, cmd, &Data32);

	if(pConfig->COMConfig[7].Interface == 2){
		printf("Set Channel 8 to RS422 .\n");
		cmd = CMD_SET_UT3_NON_RS485;
	}else{
		printf("Set Channel 8 to RS485 .\n");
		cmd = CMD_SET_UT3_RS485;
	}
	ioctl(fd_GPIO, cmd, &Data32);


	close(fd_GPIO);

	return 0;
}

int VirtCOM_SetIPAbout(DEV_CONFIG *pConfig)
{
	int i;
	unsigned char Command[128];

	for(i = 0; i < pConfig->ValidNetNum; i++){
		sprintf(Command, "ifconfig eth%d %d.%d.%d.%d", i, (pConfig->IPConfig[i].IPAddr>>0) & 0xff, (pConfig->IPConfig[i].IPAddr>>8) & 0xff, (pConfig->IPConfig[i].IPAddr>>16) & 0xff, (pConfig->IPConfig[i].IPAddr>>24) & 0xff);
		system(Command);
		printf("%s %d Command = %s\n", __func__, __LINE__, Command);
	//	printf("Net MAC Addr is not completed.***********************************\n");
	//	printf("Net Gateway Addr is not completed.***********************************\n");
	//	printf("Net DNS Addr is not completed.***********************************\n");
	//	printf("Net DNS Second Addr is not completed.***********************************\n");
	}
	sleep(1);
	main_TellIP();

	return 0;
}

int VirtCOM_Config2Param(DEV_CONFIG *pConfig, NETCOM_COM_PARAM_STATE *pParam)
{
	int i;

	for(i = 0; i < pConfig->ValidCOMNum; i++){

		pParam[i].Baud    = pConfig->COMConfig[i].rate;
		pParam[i].DataBit = pConfig->COMConfig[i].databits;
		pParam[i].Parity  = (pConfig->COMConfig[i].parity == 0)?'N':
			((pConfig->COMConfig[i].parity == 1) ? 'E' : 'O');
		pParam[i].StopBit    = pConfig->COMConfig[i].stopbit;
		pParam[i].ServerPort = pConfig->COMConfig[i].net_port;
		pParam[i].TCPConnect = (pConfig->COMConfig[i].socket_type == 1) ? 1 : 0;
	}
}

unsigned int SwapChar32(unsigned int pSrc32)
{
	unsigned char buf[4];
	unsigned char *pDstbuf;
	unsigned int Data32;

	//printf("%s->Input 0x%08x\n",__FUNCTION__,pSrc32);
	pDstbuf = (unsigned char *)&Data32;
	memcpy(buf, &pSrc32, 4);
	pDstbuf[0] = buf[3];
	pDstbuf[1] = buf[2];
	pDstbuf[2] = buf[1];
	pDstbuf[3] = buf[0];

	return Data32;
}

int SwapChar(unsigned char *pAddr32, float Data)
{
	unsigned char buf[4];
	float *pFloat = (float *)pAddr32;

	//printf("Data:%f\n",Data);
	*pFloat = Data;
	memcpy(buf, pAddr32, 4);
	pAddr32[0] = buf[2];
	pAddr32[1] = buf[3];
	pAddr32[2] = buf[0];
	pAddr32[3] = buf[1];

	return 0;
}

void  Process_ClientTCPManage(void)
{
	int i;
	NETCOM_COM_PARAM_STATE *pCOMParam;

	while(1){
		for(i = 0;i < VirtCOM_DevState.COMNum; i++){
			if((VirtCOM_DevState.COMState[i].COMValid) &&
					(VirtCOM_DevState.COMState[i].WorkInClientMode)){ //Serial Valid and Work In Client Mode
				if(0 == VirtCOM_DevState.COMState[i].NetDataConnectOK){
					pCOMParam = &VirtCOM_DevState.COMState[i];
					main_net_client_tcp(pCOMParam);
				}
			}
		}
		//return;
		sleep(1);
	}
	return;
}

void  Process_SerialRecv(void)
{
	int i;
	NETCOM_COM_PARAM_STATE *pCOMParam;
	int ReadNum;
	unsigned char ComReadBuffer[2048];
	fd_set rfds;
	struct timeval tv;
	int MaxFD = 0,COMFd;
	int ret;


	//	return;
	while(1){

		tv.tv_sec  = 0;
		tv.tv_usec = 1000;
		MaxFD      = 0;
		FD_ZERO(&rfds);
		for(i = 0;i < VirtCOM_DevState.COMNum; i++){
			if(0 == VirtCOM_DevState.COMState[i].COMValid) continue;
			if(VirtCOM_DevState.COMState[i].COMFd <= 0) continue;

			pCOMParam = &VirtCOM_DevState.COMState[i];
			COMFd = pCOMParam->COMFd;
			if(MaxFD < COMFd)
				MaxFD = COMFd;
			FD_SET(COMFd, &rfds);
		}

		ret = select(MaxFD+1, &rfds, NULL, NULL, &tv); //the last NULL is wait forever
		if(ret == 0) continue; //timeout
		if(ret < 0) {
			printf("%s->Line %d, select ERROR, errno=%d.\n", __FUNCTION__, __LINE__, errno);
			continue; //ERROR
		}

		for(i = 0; i < VirtCOM_DevState.COMNum; i++){
			if(0 == VirtCOM_DevState.COMState[i].COMValid) continue;
			if(VirtCOM_DevState.COMState[i].COMFd <= 0 ) continue;

			pCOMParam = &VirtCOM_DevState.COMState[i];
			COMFd = pCOMParam->COMFd;

			if(FD_ISSET(COMFd, &rfds) == 0){
				continue;
			}

			ReadNum = read(pCOMParam->COMFd, ComReadBuffer, sizeof(ComReadBuffer)); 
			if(ReadNum <= 0){
				//printf("no input \n");
				continue;
			}
			tcflush(pCOMParam->COMFd, TCIFLUSH);

			pCOMParam->SerialRecvByte += ReadNum;
			//printf("%s->Recv %d bytes\n",pCOMParam->COMDevName,ReadNum);
			//VirtCOM_PrintBuf(ComReadBuffer,ReadNum);

			net_SendBuffer_Console(pCOMParam, ComReadBuffer, ReadNum, 1);

			if(pCOMParam->TCPConnect){
				if(pCOMParam->WorkInClientMode)
					net_SendBuffer_TCPClient(pCOMParam, ComReadBuffer, ReadNum);
				else
					net_SendBuffer_TCP(pCOMParam, ComReadBuffer, ReadNum);
			}
			else{
				net_SendBuffer_UDP(pCOMParam, ComReadBuffer, ReadNum);
			}
		}
	}

	return ;
}

void  Process_NetRecv(void)
{
	int i;
	NETCOM_COM_PARAM_STATE *pCOMParam;
	int ReadNum;
	int NetFd;
	unsigned char NetRecvBuffer[2048];
	int Err;
	fd_set rfds;
	struct timeval tv;
	int MaxFD = 0;
	int ret;


	//return ;
	while(1){
		//printf("%s->Line %d, Net Recv Again\n",__FUNCTION__,__LINE__);

		tv.tv_sec  = 0;
		tv.tv_usec = 10000;
		MaxFD      = 0;
		FD_ZERO(&rfds);
		for(i = 0; i < VirtCOM_DevState.COMNum; i++){
			if(0 == VirtCOM_DevState.COMState[i].NetDataConnectOK) continue;
			if(VirtCOM_DevState.COMState[i].COMFd <= 0) continue;

			pCOMParam = &VirtCOM_DevState.COMState[i];
			NetFd = pCOMParam->NetDataFD;
			if(MaxFD < NetFd)
				MaxFD = NetFd;
			FD_SET(NetFd, &rfds);
			//printf("%s->Line %d, add set %d\n",__FUNCTION__,__LINE__,NetFd);
		}

		//printf("%s->Line %d, After reset fdsets\n",__FUNCTION__,__LINE__);

		ret = select(MaxFD + 1, &rfds, NULL, NULL, &tv); //the last NULL is wait forever
		//printf("%s->Line %d, After select,ret:%d\n",__FUNCTION__,__LINE__,ret);
		//ret>0:OK, =0:timeout; -1:ERROR
		if(ret == 0) continue; //timeout
		if(ret < 0) {
			printf("%s->Line %d, select ERROR, errno=%d.\n", __FUNCTION__, __LINE__, errno);
			continue; //ERROR
		}

		//printf("%s->Line %d, any fd recv data\n",__FUNCTION__,__LINE__);
		for(i = 0; i < VirtCOM_DevState.COMNum; i++){
			if(0 == VirtCOM_DevState.COMState[i].NetDataConnectOK) continue;
			if(VirtCOM_DevState.COMState[i].COMFd <= 0 ) continue;

			pCOMParam = &VirtCOM_DevState.COMState[i];

			NetFd=pCOMParam->NetDataFD;
			if(FD_ISSET(NetFd, &rfds) == 0){
				continue;
			}
			ReadNum = recv(NetFd, NetRecvBuffer, sizeof(NetRecvBuffer) - 1, 0);

			//if(ReadNum =0) continue;
			if(ReadNum <= 0)
			{ // TCP Connect fail
				perror("net recv 0,return");
				printf("%s->recv <=0,continue, port:%d,return:%d, error = %d\n", __FUNCTION__,
						pCOMParam->ServerPort, ReadNum, errno);
				Err = errno;
				if((Err != EAGAIN) &&
						(Err != EWOULDBLOCK) &&
					/*lel 当串口连接手动断开时，errno是0，导致程序一直重复打印上一条printf，不知道当时为什么加 Err!= 0 我个人注释它。*/
					//	(Err != 0) &&
					/*end*/
					//	(Err != ESPIPE) &&
						(Err != EINTR))
				{ // TCP Connect fail
					perror("net recv");
					printf("%s->recv error id %d,Port:%d\n", __FUNCTION__, Err, pCOMParam->ServerPort);
					pCOMParam->NetDataConnectOK = 0;
				}
				continue;
			}
			pCOMParam->ClientRecvByte += ReadNum;
#if 0
			printf("net rec %d:\n",ReadNum);

			VirtCOM_PrintBuf(NetRecvBuffer,ReadNum);
			printf("\n");
#endif

			net_SendBuffer_Console(pCOMParam, NetRecvBuffer, ReadNum, 0);
			Serial_Send(pCOMParam, NetRecvBuffer, ReadNum);
		}
	}

	return ;
}

/*
   one COM use one main_Process thread to process data
   */
void  main_Process(void *Param)
{
	int COMIndex = *(int *)Param;
	int ret;
	NETCOM_COM_PARAM_STATE *pCOMParam;

	//printf("%s-> Process Index %d Enter.\n",__FUNCTION__,COMIndex);
	pCOMParam = &VirtCOM_DevState.COMState[COMIndex];

	main_SerialInit(pCOMParam);

	//return ;
	//printf("%s->  TCPConnect %d .\n",__FUNCTION__,pCOMParam->TCPConnect);
	printf("%s -> WorkInClientMode = %d\n", __FUNCTION__, pCOMParam->WorkInClientMode);
	//if(pCOMParam->TCPConnect){
	if(1){
		ret = main_net_server_tcp(pCOMParam);
		usleep(100000);
	}else{
		ret = main_net_server_udp(pCOMParam);
	}

	return ;
}

static int GetNetNum(void)
{

	FILE *fp;
	int ret,Num;
	unsigned char Buffer[128];

	if((fp = fopen(FILE_NAME_NETNUM, "rb")) == NULL){//a : No file create, writing at end of file
		printf("Open file %s wrong \n", FILE_NAME_NETNUM);
		return -1;
	}
	ret = fread(Buffer, sizeof(Buffer), 1, fp);
	printf("%s->ret =%d \n", __FUNCTION__, ret);
	fclose(fp);

	//Buffer[ret] = 0;
	Num = atoi(Buffer);
	printf("%s->Buffer %s, Num=%d \n", __FUNCTION__, Buffer, Num);
	return Num;

}


static int GetCOMNum(void)
{

	FILE *fp;
	int ret,Num;
	unsigned char Buffer[128];

	if((fp = fopen(FILE_NAME_COMNUM, "rb")) == NULL){//a : No file create, writing at end of file
		printf("Open file %s wrong \n", FILE_NAME_COMNUM);
		return -1;
	}
	ret = fread(Buffer, sizeof(Buffer), 1, fp);
	printf("%s->ret =%d \n", __FUNCTION__, ret);
	fclose(fp);

	//Buffer[ret] = 0;
	Num = atoi(Buffer);

	printf("%s->Buffer %s, Num=%d \n", __FUNCTION__, Buffer, Num);

	return Num;
}

/*
   prog [serialnum]
   */
int main_VirtCOM(int *pCOMNum)
{
	int ret;
	int ServerPort ;
	int i, j;
	float *pFloatData;
	NETCOM_TABLE_ITEM *pCovtTableItem;


	printf("==============================\n");
	printf("VirtCOM Compile time %s   %s\n", __DATE__, __TIME__);
	printf("==============================\n");

	/* Init DataBuffer */
	signal(SIGPIPE, SIG_IGN);

	COMNum = *pCOMNum;

	if((COMNum <= 0) || (COMNum > 64)){
		COMNum = 4; //we have 4 ports
		printf("Get ComNum Error,Set to Default %d.\n", COMNum);
	}
	printf("Serial Port Number: %d.\n", COMNum);
#if 0
	system("rm -f /ourapp/ramdisk.img");
	system("killall webs");
	system("cp /myapp/webs /bin/ -f");
	system("chmod +x /bin/webs");
	sleep(1);
	system("webs &");
	sleep(1);
#endif

	NetNum = 4;
#if 0
	NetNum = GetNetNum();

	if((NetNum<=0)||(NetNum>12)){
		NetNum = 2; //we have 2 ports
		printf("Get NetNum Error,Set to Default %d.\n",NetNum);
	}
#endif


	VirtCOM_InitDevParam(&gDevConfig);
	//printf("after VirtCOM_InitDevParam.\n");
	VirtCOM_SetIPAbout(&gDevConfig);
	//printf("after VirtCOM_SetIPAbout.\n");

	WMConfig_GetLocalNetMask(&(gDevConfig.IPConfig[0].NetMask), "eth0");
	WMConfig_GetLocalNetMask(&(gDevConfig.IPConfig[1].NetMask), "eth1");
	WMConfig_GetLocalNetMask(&(gDevConfig.IPConfig[2].NetMask), "eth2");
	WMConfig_GetLocalNetMask(&(gDevConfig.IPConfig[3].NetMask), "eth3");

	/*lel*/
#if 0
	VirtCOM_SetSerialInterfaceAbout(&gDevConfig);
#endif
	/*end*/

	memset(&VirtCOM_DevState, 0, sizeof(VirtCOM_DevState));
	VirtCOM_DevState.COMNum = COMNum;
	VirtCOM_DevState.NetManagePort = NETPORT_REMOTEMANAGE;
	VirtCOM_DevState.NetZhaoHuanPort = NETPORT_ZHAOHUAN;
	for(i = 0; i < COMNum; i++){
		VirtCOM_DevState.COMState[i].COMIndex = i;
		VirtCOM_InitCOMParam(&(VirtCOM_DevState.COMState[i]));
	}

	//printf("after VirtCOM_InitCOMParam.\n");

	for(; i < MAX_SERIAL_PORTS_NUM; i++){
		VirtCOM_DevState.COMState[i].COMIndex = i;
		VirtCOM_DevState.COMState[i].COMValid = 0;
	}

#if 0
	for(i=0;i<MAX_SERIAL_PORTS_NUM;i++){
		if(VirtCOM_DevState.COMState[i].COMValid == 0) continue;
		printf("COM %d, Conn type:%s\n",i,
				VirtCOM_DevState.COMState[i].TCPConnect?"TCP":"UDP");
	}
#endif
	for(i = 0; i < COMNum; i++){
		if(VirtCOM_DevState.COMState[i].COMValid == 0) continue;
		VirtCOM_DevState.COMState[i].COMFd = 0;
		ret = pthread_create(&VirtCOM_DevState.COMState[i].ThreadID_Net, NULL, (void *)main_Process, &i);
		if(ret != 0)
		{
			printf ("Create pthread Serial %d  error!return %d\n", i, ret);
			return -1;
		}
		usleep(100000);
	}

	ret = pthread_create(&VirtCOM_DevState.ThreadID_ClientTCPManage, NULL, (void *)Process_ClientTCPManage, NULL);
	if(ret != 0)
	{
		printf ("Create pthread Process_ClientTCPManage  error!return %d\n", i, ret);
	}

	ret = pthread_create(&VirtCOM_DevState.ThreadID_ProcessSerial, NULL, (void *)Process_SerialRecv, NULL);
	if(ret != 0)
	{
		printf ("Create pthread Process_Serial  error!return %d\n", i, ret);
	}

	ret = pthread_create(&VirtCOM_DevState.ThreadID_ProcessNet, NULL, (void *)Process_NetRecv, NULL);
	if(ret != 0)
	{
		printf ("Create pthread Process_Serial  error!return %d\n", i, ret);
	}

	//ret = pthread_create(&VirtCOM_DevState.ThreadID_NetManage,NULL,(void *)main_NetManage,NULL);
	//if(ret!=0)
	//{
	//	printf ("Create pthread NetManage  error!return %d\n",i,ret);
	//}

#if 0
	ret = pthread_create(&VirtCOM_DevState.ThreadID_NetZhaoHuan,NULL,(void *)main_ZhaoHuan,NULL);
	if(ret!=0)
	{
		printf ("Create pthread NetZhaoHuan  error!return %d\n",i,ret);
		return -1;
	}
#endif
	/*lel*/
#if 1
	ret = pthread_create(&VirtCOM_DevState.ThreadID_PrintFile, NULL, (void *)main_PrintFile, NULL);
	if(ret != 0)
	{
		printf ("Create pthread ThreadID_PrintFile  error!return %d\n", i, ret);
		return -1;
	}
#endif
	/*end*/
	/*
	   ret = pthread_create(&pNetCOMParam[0].ThreadID_WebServer,NULL,(void *)main_webserver,pNetCOMParam);
	   if(ret!=0)
	   {
	   printf ("Create pthread NetManage  error!return %d\n",i,ret);
	   return -1;
	   }
	   */
	//main_NetManage();
	//printf("Start WebServer.\n");
	//InitConfigServer();

	while(1) sleep(1);

	return TRUE;
} 

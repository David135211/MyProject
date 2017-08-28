#include "VirtCOM.h"

#define	LOCAL_PORT	10000
#define	REMOTE_PORT	8000

struct sockaddr_in remote_addr; 
struct sockaddr_in local_addr;

int WML_GetStatus(VIRT_COM_DEVSTATE *pStatus);
int WML_SetConfig(DEV_CONFIG *pNewConfig);
int WML_GetConfig(DEV_CONFIG *pNewConfig);
int WML_SetDefaultConfig(void);
void VirtCOM_PrintBuf(unsigned char *Buffer,int Len);
int WMConfigIsOK(DEV_CONFIG *pNewConfig,unsigned char *pError,unsigned int MaxLen);

/*Added by zql  2015.3.12*/
static WEB_CONFIG WebConfig;	/*�������ݴ洢*/
static VCOM_STATUS VcomStatus[MAX_SERIAL_PORTS_NUM];

static unsigned char ConfigErrStatus[512];
static int LastConfigErr;

PORT_ATTRIBUTE portAttr[64];/*��������*/
static char configBuf[100000];/*1�ֽڵ����2761�ֽڵ�WEB_CONFIG���ݣ�����1λ  |cmd| |config data....| */
	/*���ǻ�ȡ״̬��1�ֽڵ�����,����1λ��2777�ֽڵ�״̬���� |cmd| |status data....| */




int WebServer_SetDefaultConfig()
{
	
	system("rm -f /myapp/DevConfig");
	sleep(1);
	system("reboot");
}



/*��ȡ��ҳ�������������ݣ������浽���������ļ���*/
int WebServer_SetConfig(WEB_CONFIG *pConfig)
{
	char * ptr =NULL;
	DEV_CONFIG NewDevConfig;
	int ret;

	printf("%s->Line %d, Enter\n",__FUNCTION__,__LINE__);

	memcpy(&WebConfig,pConfig,sizeof(WebConfig));
	
	WML_GetConfig(&NewDevConfig);
	memcpy(&NewDevConfig.COMConfig[0],&WebConfig.Serial[0],sizeof(DEV_COMCONFIG)*MAX_SERIAL_PORTS_NUM);
	memcpy(&NewDevConfig.IPConfig[0],&WebConfig.Net[0],sizeof(DEV_NETCONFIG)*MAX_NET_PORTS_NUM);
	
	printf("%s->Line %d, before WML_SetConfig\n",__FUNCTION__,__LINE__);
	LastConfigErr = WMConfigIsOK(&NewDevConfig,ConfigErrStatus,sizeof(ConfigErrStatus));
	if(LastConfigErr != 0){
		
		printf("%s->Line %d, WMConfigIsOK return Error %d.\n",__FUNCTION__,__LINE__,LastConfigErr);
		 return -1;
	}
	printf("%s->Line %d, WMConfigIsOK return OK\n",__FUNCTION__,__LINE__);
	WML_SetConfig(&NewDevConfig);
	
	system("sync");
	sleep(2);
	system("reboot");
}


/*��ȡ�����������ݣ������͵�web������*/
void WebServer_GetConfig(WEB_CONFIG * pConfig)
{
	char * ptr =NULL;
	DEV_CONFIG NewDevConfig;
	int i;
	
	/*1���ӱ����ļ��ж�ȡ���ã��ŵ�WebConfig��*/

	strcpy(WebConfig.SoftVer,VIRTCOM_VERSION);
	if(LastConfigErr){
		printf("%s->Line %d, LastConfigErr Error %d.\n",__FUNCTION__,__LINE__,LastConfigErr);
		WebConfig.ConfigErr = LastConfigErr;
		strcpy(WebConfig.ErrStatus,ConfigErrStatus);
		//the WebConfig is not changed after last setconfig.so the rest segment do not init.
	}else{
		WebConfig.ConfigErr = 0;
		memset(WebConfig.ErrStatus,0,sizeof(WebConfig.ErrStatus));

		WML_GetConfig(&NewDevConfig);
		memcpy(&WebConfig.Serial[0],&NewDevConfig.COMConfig[0],
			sizeof(DEV_COMCONFIG)*MAX_SERIAL_PORTS_NUM);
		memcpy(&WebConfig.Net[0],&NewDevConfig.IPConfig[0],sizeof(DEV_NETCONFIG)*MAX_NET_PORTS_NUM);

		WebConfig.ValidNet = NewDevConfig.ValidNetNum;
		WebConfig.ValidSerial = NewDevConfig.ValidCOMNum;
		//printf("%s->Line %d, ValidNet= %d,ValidSerial= %d\n",__FUNCTION__,__LINE__,
		//	WebConfig.ValidNet,WebConfig.ValidSerial);
	}

	/*2�������������ݵ���ҳ������*/

	memcpy(pConfig,&WebConfig,sizeof(WebConfig));

}


/*��ȡ����״̬���ݣ������͵���ҳ������*/
void WebServer_GetStatus(VCOM_STATUS *pStatus)
{
	VIRT_COM_DEVSTATE NewDevStatus;
	int i;

	/*��ȡ��ǰ��״̬����*/
	WML_GetStatus(&NewDevStatus);

	/*��״̬���ݷ��͵���ҳ������*/
	for(i=0;i<MAX_SERIAL_PORTS_NUM;i++){
		VcomStatus[i].ClientRecvByte = NewDevStatus.COMState[i].ClientRecvByte;
		VcomStatus[i].ClientSendByte = NewDevStatus.COMState[i].ClientSendByte;
		VcomStatus[i].SerialRecvByte= NewDevStatus.COMState[i].SerialRecvByte;		
		VcomStatus[i].SerialSendByte= NewDevStatus.COMState[i].SerialSendByte;
		strcpy(VcomStatus[i].COMStatus,NewDevStatus.COMState[i].COMStatus);
#if 0
		printf("%s->LIne %d, ClientRecvByte= %d,ClientSendByte = %d,SerialRecvByte = %d,SerialSendByte = %d\n",__FUNCTION__,__LINE__,
				VcomStatus[i].ClientRecvByte,
				VcomStatus[i].ClientSendByte,
				VcomStatus[i].SerialRecvByte,
				VcomStatus[i].SerialSendByte);
#endif
	}

	memcpy(pStatus,&VcomStatus,sizeof(VcomStatus));
	return 0;

}


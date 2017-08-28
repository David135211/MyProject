#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<errno.h>
#include	<string.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<sys/wait.h>
#include	<signal.h>
#include	<netdb.h>
#include	<netinet/tcp.h>

#include <sys/param.h> 
#include <sys/ioctl.h> 
#include <net/if.h> 
#include <net/if_arp.h> 
#ifdef SOLARIS 
#include <sys/sockio.h> 
#endif 


#include	"VirtCOM.h"

#define	BACKLOG	10

int main_net_server_tcp(void *pParam)
{
	NETCOM_COM_PARAM_STATE *pCOMParam;
	int	sockfd,sin_size,yes=1,new_fd;
	int	ServerPort ;
	struct	sockaddr_in	server_addr;
	struct	sockaddr_in	clients_addr;
	int ClientIP;
	struct	sigaction	sa;
	NETCOM_TABLE_ITEM * pNewItem;
	int ret;
	STRUCT_FOR_NEW_NETCLIENT NewClient;
	int i;
	int KeepAlive    = 1;	//open keepalive opt, default is close.
	int KeepIdle     = 1;	//if no data in KeepIdle second,start detect, default is 7200s
	int KeepInterval = 20;	//detect interval, default is 75s
	int KeepCount    = 10;	//detect retry times, all are failed then act as link failed.
	//	char buf[MAXBUFLEN];

	pCOMParam = (NETCOM_COM_PARAM_STATE *)pParam;
	ServerPort = pCOMParam->ServerPort;
	printf("%s -> Start to listen Port %d\n", __FUNCTION__, ServerPort);
	//VirtCOM_PrintCOMParam(pCOMParam);
	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		return -1;
	}
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
	{
		perror("setsocopt");
		return -1;
	}
	pCOMParam->NetServerFD= sockfd;

	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(ServerPort);
	server_addr.sin_addr.s_addr=INADDR_ANY;
	memset(&(server_addr.sin_zero),'\0',8);

	if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("bind");
		return -1;
	}

	//change by zxz . 130718-0957
	if(listen(sockfd,BACKLOG) == -1)
	{
		perror("listen");
		return -1;
	}

	while(1){
		printf("Start a new connection.\n");
		sin_size=sizeof(struct sockaddr_in);
		if((new_fd = accept(sockfd, (struct sockaddr *)&clients_addr, &sin_size)) == -1)
		{
			perror("accept");
			continue;
		}

		ClientIP = clients_addr.sin_addr.s_addr;

		if((pCOMParam->NetDataConnectOK == 0) && (0 == pCOMParam->WorkInClientMode)){
			pCOMParam->NetDataConnectOK = 1;
			pCOMParam->NetDataFD = new_fd;
			pCOMParam->ClientIP = ClientIP;
			pCOMParam->ClientPort = ntohs(clients_addr.sin_port);
			memcpy(&pCOMParam->NetClientAddr,&clients_addr,sizeof(clients_addr));
			NewClient.Index = 0;
			NewClient.pCOMParam = pCOMParam;

			printf("Server Get Data TCP Connection From %s, Port:%d\n",
					inet_ntoa(clients_addr.sin_addr),
					ntohs(clients_addr.sin_port)
				  );

			setsockopt(new_fd,SOL_SOCKET,SO_KEEPALIVE,(void *)&KeepAlive,sizeof(KeepAlive));
			setsockopt(new_fd,SOL_TCP,TCP_KEEPIDLE,(void *)&KeepIdle,sizeof(KeepIdle));
			setsockopt(new_fd,SOL_TCP,TCP_KEEPINTVL,(void *)&KeepInterval,sizeof(KeepInterval));
			setsockopt(new_fd,SOL_TCP,TCP_KEEPCNT,(void *)&KeepCount,sizeof(KeepCount));

			//i=1;

			//ret = ioctl(new_fd,FIONBIO,&i);
			//if(ret){
			//	printf("set FIONBIO failed,ret=%d.\n",ret);
			//}

		}
		else{ //net console
			pCOMParam->NetConsoleFD= new_fd;
			pCOMParam->ClientConsoleIP = ClientIP;
			pCOMParam->ClientConsolePort= ntohs(clients_addr.sin_port);
			memcpy(&pCOMParam->NetConsoleClientAddr,&clients_addr,sizeof(clients_addr));
			pCOMParam->NetConsoleConnectOK = 1;

			printf("Server Get Console Connection From %s, Port:%d\n",
					inet_ntoa(clients_addr.sin_addr),
					ntohs(clients_addr.sin_port)
				  );
		}

		printf("%s %d NetConsoleConnectOK = %d NetDataConnectOK = %d WorkInClientMode = %d\n", __func__, __LINE__, pCOMParam->NetConsoleConnectOK, pCOMParam->NetDataConnectOK, pCOMParam->WorkInClientMode);
		while(1){ //now net data connect and net console OK.
			if(pCOMParam->NetConsoleConnectOK == 0)
				break;
			if( (0 == pCOMParam->NetDataConnectOK) &&
					(0 == pCOMParam->WorkInClientMode))
				break;
			usleep(100000);
		}
	}
	return 0;
}


int net_SendBuffer_TCP(NETCOM_COM_PARAM_STATE *pCOMParam,unsigned char *pBuffer,int Len)
{
	if(pCOMParam->ClientPort == 0) return 0;
	if(pCOMParam->NetDataFD == 0) return 0;

	if(send(pCOMParam->NetDataFD,pBuffer,Len,0)==-1)
	{
		perror("send");
		printf("%s,Port %d->send error,return\n",__FUNCTION__,pCOMParam->ServerPort);
		printf("%s->Client IP:%s\n",__FUNCTION__,inet_ntoa(pCOMParam->NetClientAddr.sin_addr));
		close(pCOMParam->NetDataFD);
		pCOMParam->NetDataFD = 0;
		pCOMParam->ClientPort = 0;
	}else{
		pCOMParam->ClientSendByte += Len;
#if 0
		printf("net_SendBuffer_TCP->send  %d bytes\n",Len);
		VirtCOM_PrintBuf(pBuffer,Len);
#endif
	}

	return Len;
}


/*
   DataFromSerial = 0: data is from network
else : Data is from Serial
*/
int net_SendBuffer_Console(NETCOM_COM_PARAM_STATE *pCOMParam,unsigned char *pBuffer,int Len,int DataFromSerial)
{
	int i;
	int SendLen;

	if((0 == pCOMParam->NetConsoleFD) || 
			(0 == pCOMParam->NetConsoleConnectOK)       )
		return 0;

#if 0
	printf("net_SendBuffer_TCP->send  %d bytes\n",Len);
	VirtCOM_PrintBuf(pBuffer,Len);
#endif
	for(i=0;i<Len;i++){
		SendLen = 0;
		if(i%8 ==0) {// print \n
			if(i){
				pCOMParam->NetConsoleSendBuffer[0]= '\r';
				pCOMParam->NetConsoleSendBuffer[1]= '\n';
				SendLen = 2;
			}
			SendLen += sprintf(&pCOMParam->NetConsoleSendBuffer[SendLen],"%s,%s->",VIRTCOM_VERSION,DataFromSerial?"Serial":"Net");
		}
		SendLen += sprintf(&pCOMParam->NetConsoleSendBuffer[SendLen],"0x%02x ",pBuffer[i]) ;
#if 0
		printf("%s->send  %d bytes\n",__FUNCTION__,SendLen);
		VirtCOM_PrintBuf(pCOMParam->NetConsoleSendBuffer,SendLen);
#endif

		if(send(pCOMParam->NetConsoleFD,pCOMParam->NetConsoleSendBuffer,SendLen,0)==-1)
		{
			perror("send");
			printf("%s->send error,return\n",__FUNCTION__);
			printf("%s->Client IP:%s\n",__FUNCTION__,inet_ntoa(pCOMParam->NetConsoleClientAddr.sin_addr));
			close(pCOMParam->NetConsoleFD);
			pCOMParam->NetConsoleFD = 0;
			pCOMParam->NetConsoleConnectOK = 0;
			return 0;
		}
	}

	sprintf(pCOMParam->NetConsoleSendBuffer,"\n");

	if(send(pCOMParam->NetConsoleFD, pCOMParam->NetConsoleSendBuffer, strlen(pCOMParam->NetConsoleSendBuffer), 0) == -1)
	{
		perror("send");
		printf("%s->send error,return\n", __FUNCTION__);
		printf("%s->Client IP:%s\n", __FUNCTION__, inet_ntoa(pCOMParam->NetConsoleClientAddr.sin_addr));
		close(pCOMParam->NetConsoleFD);
		pCOMParam->NetConsoleFD = 0;
		pCOMParam->NetConsoleConnectOK = 0;
		return 0;
	}

	return Len;
}


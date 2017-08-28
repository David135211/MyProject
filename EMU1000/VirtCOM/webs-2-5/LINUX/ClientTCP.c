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
#include	<netinet/tcp.h>
#include	<netdb.h>

#include <sys/param.h> 
#include <sys/ioctl.h> 
#include <net/if.h> 
#include <net/if_arp.h> 

#include	<fcntl.h>

#include	"VirtCOM.h"

#define	BACKLOG	10

int main_net_client_tcp(void *pParam)
{
	NETCOM_COM_PARAM_STATE *pCOMParam;
	int	sockfd,sin_size,yes=1;
	int	ServerPort ;
	struct in_addr MyIP;
	struct	sockaddr_in	server_addr;
	int ClientIP;
	struct	sigaction	sa;
	NETCOM_TABLE_ITEM * pNewItem;
	STRUCT_FOR_NEW_NETCLIENT NewClient;
	int ret,NextIndex;
	int i;
	int KeepAlive = 1; //open keepalive opt, default is close.
	int KeepIdle = 1; //if no data in KeepIdle second,start detect, default is 7200s
	int KeepInterval = 20; //detect interval, default is 75s
	int KeepCount = 10;//detect retry times, all are failed then act as link failed.
	//	char	buf[MAXBUFLEN];
	int PipeSet = 1;
	struct timeval timeo = {0,100000};

	pCOMParam = (NETCOM_COM_PARAM_STATE *)pParam;
	ServerPort = pCOMParam->ServerPort;
	MyIP.s_addr = pCOMParam->ServerIP;
	//printf("%s->Start to Connect Port %d,Server IP:%s\n",__FUNCTION__,ServerPort,inet_ntoa(MyIP));
	//VirtCOM_PrintCOMParam(pCOMParam);
	sockfd = pCOMParam->NetDataFD;

	if(sockfd) {close(sockfd);pCOMParam->NetDataFD = 0;}
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket");
		return -1;
	}

	//setsockopt(sockfd,SOL_SOCKET,SO_NOSIGPIPE,(void *)&PipeSet,sizeof(PipeSet));
	setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,&timeo,sizeof(timeo));

	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(ServerPort);
	server_addr.sin_addr.s_addr=pCOMParam->ServerIP;

	ret = connect(sockfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr));

	if(ERROR == ret){
#if 0
		perror("Socket Connect\n");
		printf("Socket FD:%d\n",sockfd);
		printf("connet Failed., Server  IP: %s, Port:%d\n",
				inet_ntoa(MyIP),
				ntohs(server_addr.sin_port)
		      );
#endif
		close(sockfd);
		return -1;
	}

	setsockopt(sockfd,SOL_SOCKET,SO_KEEPALIVE,(void *)&KeepAlive,sizeof(KeepAlive));
	setsockopt(sockfd,SOL_TCP,TCP_KEEPIDLE,(void *)&KeepIdle,sizeof(KeepIdle));
	setsockopt(sockfd,SOL_TCP,TCP_KEEPINTVL,(void *)&KeepInterval,sizeof(KeepInterval));
	setsockopt(sockfd,SOL_TCP,TCP_KEEPCNT,(void *)&KeepCount,sizeof(KeepCount));

#if 0
	i=1;
	ret = ioctl(sockfd,FIONBIO,&i);
	if(ret){
		printf("set FIONBIO failed,ret=%d.\n",ret);
	}
#endif

#if 0
	i = fcntl(sockfd,F_GETFL,0);
	if(i == -1){
		printf("fcntl get error.\n");
		return -1;
	}

	i |= O_NONBLOCK;

	ret = fcntl(sockfd,F_SETFL,i);
	if(ret == -1){
		printf("fcntl set error.\n");
		return -1;
	}
#endif
	MyIP.s_addr = pCOMParam->ServerIP;

	pCOMParam->NetDataFD= sockfd;
	usleep(100000);
	pCOMParam->TCPConnectStartFlag = 1;
	pCOMParam->NetDataConnectOK = 1;
	printf("connet OK, Server Connection to %s, Port:%d\n",
			inet_ntoa(MyIP),
			ntohs(server_addr.sin_port)
	      );
	return 0;
}

int net_SendBuffer_TCPClient(NETCOM_COM_PARAM_STATE *pCOMParam,unsigned char *pBuffer,int Len)
{
	int ret ,Err;

	if(pCOMParam->NetDataFD == 0 ) return -100;
	if(pCOMParam->NetDataConnectOK == 0 ) return -200;

	//return 0;

	ret = send(pCOMParam->NetDataFD,pBuffer,Len,MSG_NOSIGNAL);
	if(ret >0){
		pCOMParam->ClientSendByte += Len;
#if 0
		printf("net_SendBuffer_TCP->send  %d bytes\n",Len);
		VirtCOM_PrintBuf(pBuffer,Len);
#endif
		return ret;
	}
	Err = errno;

	if((Err != EAGAIN) &&
			(Err != EWOULDBLOCK) &&
			(Err != EINTR)
	  )
	{
		perror("send");
		printf("%s,Port %d->send error,return\n",__FUNCTION__,pCOMParam->ServerPort);
		printf("%s,NetDataFD =%d\n",__FUNCTION__,pCOMParam->NetDataFD);
		pCOMParam->NetDataConnectOK = 0;
	}

	return ret;
}


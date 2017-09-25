/*++
***************************************************************************************************

(c) ��Ȩ����: 

ģ������:

	GroupBroad.c

ժҪ:

	�����ڲ����ߵĻ�����ʽ������

����:

	������--- 2014-04-30

�޸���ʷ:
	x��what---who---time

***************************************************************************************************  
--*/


#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#define MCAST_PORT 65529
#define MCAST_ADDR "224.0.0.100"    /*һ���ֲ����Ӷಥ��ַ��·����������ת��*/
#define BUFF_SIZE 1024


static int rcv_fd = -1;
static int sd_fd = -1;
static struct sockaddr_in r_addr;
static struct sockaddr_in local_addr;				/*���ص�ַ*/


#define	LOCAL_DATA_MAXLEN 1024			/*  */

unsigned short GroupBroadRecv(unsigned char* pBuf,int* pLen)
{
	int len = sizeof(local_addr);
	if(rcv_fd<=0)
	{
		return -1;
	}
 	*pLen = recvfrom (rcv_fd, 
			 pBuf, 
			LOCAL_DATA_MAXLEN, 
			0,
			(struct sockaddr *) &local_addr,
			 (socklen_t *)&len);

 	if (* pLen < 0 )
    {
		perror ("recvfrom");
        return (-1);
    }
    else
    {

    }
    //inet_ntoa_b (clientAddr.sin_addr, inetAddr);
 
 	return 0;	
}


static int GroupBroadSendEn(unsigned char* pBuf,int Len)
{

	if(sd_fd<=0)
	{
		return -1;
	}
	if(Len>LOCAL_DATA_MAXLEN) 
	{
		Len=LOCAL_DATA_MAXLEN;
		//printf("\n[MlGroupBroadSend]data overflow...");
	}

	
	if (sendto (sd_fd, pBuf, Len, 0, (struct sockaddr *)&r_addr, sizeof (r_addr)) < 0 )
	{

		return (-1);
	}
	else
	{
		return (1);
	}
}

static int MutexInited = 0;
static pthread_mutex_t  mutex;

int GroupBroadSend(unsigned char* pBuf,int Len)
{
	if(MutexInited == 0)
	{
		pthread_mutex_init (&mutex, NULL);
		MutexInited=1;
	}
	pthread_mutex_lock(&mutex);
	GroupBroadSendEn(pBuf, Len);
	pthread_mutex_unlock(&mutex);
	return 0;
}

static int GroupBroadSendInit(void)
{
    //int s;
    //struct sockaddr_in r_addr;     
    sd_fd = socket(AF_INET, SOCK_DGRAM, 0);         /*�����׽���*/
    if (sd_fd== -1)
    {
        perror("socket()");
        return -1;
    }
   
    memset(&r_addr, 0, sizeof(r_addr));/*��ʼ��IP�ಥ��ַΪ0*/
    r_addr.sin_family = AF_INET;                /*����Э��������ΪAF*/
    r_addr.sin_addr.s_addr = inet_addr(MCAST_ADDR);/*���öಥIP��ַ*/
    r_addr.sin_port = htons(MCAST_PORT);        /*���öಥ�˿�*/
   
                                                    /*��ಥ��ַ��������*/
	int	nVal = 1;
	int err = setsockopt(sd_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&nVal, sizeof(nVal));
	if(err < 0)
	{
		perror("setsockopt():SO_REUSEADDR");
	}
   
    return 0;
}


static int GroupBroadRecvInit(void)
{                                      

	int err = -1;
	rcv_fd = socket(AF_INET, SOCK_DGRAM, 0);     /*�����׽���*/
	if (sd_fd == -1)
	{
		perror("socket()");
		return -1;
	}  

	/* Set SO_REUSEADDR */
	int	nVal = 1;
	err = setsockopt(rcv_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&nVal, sizeof(nVal));
	if(err < 0)
	{
		perror("setsockopt():SO_REUSEADDR");
	}

	/*��ʼ����ַ*/
	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons(MCAST_PORT);

	/*���ûػ����*/
	int loop = 0;
	err = setsockopt(rcv_fd,IPPROTO_IP, IP_MULTICAST_LOOP,&loop, sizeof(loop));
	if(err < 0)
	{
		perror("setsockopt():IP_MULTICAST_LOOP");
		return -3;
	}

	struct ip_mreq mreq;                                    /*����ಥ��*/
	mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR); /*�ಥ��ַ*/
	mreq.imr_interface.s_addr = htonl(INADDR_ANY); /*����ӿ�ΪĬ��*/
	/*����������ಥ��*/
	err = setsockopt(rcv_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof
			(mreq));
	if (err < 0)
	{
		perror("setsockopt():IP_ADD_MEMBERSHIP");
		return -4;
	}

	/*��socket*/
	err = bind(rcv_fd,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;
	if(err < 0)
	{
		perror("bind()");
		return -2;
	}




	return 0;
}

int GroupBroadInit(void)
{
	if( rcv_fd >= 0 )
	{
		shutdown( rcv_fd, 2 );
		close( rcv_fd );
		rcv_fd = -1;
	}
	if( sd_fd >= 0 )
	{
		shutdown( sd_fd, 2 );
		close( sd_fd );
		sd_fd = -1;
	}

	GroupBroadSendInit();
	GroupBroadRecvInit();
	return 1;
}



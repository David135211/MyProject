// PintTest.cpp: implementation of the CPintTest class.
//
//////////////////////////////////////////////////////////////////////

#include<sys/time.h>  /*��Linuxϵͳ������ʱ��ͷ�ļ�*/  
#include<sys/socket.h>    /*��������socket��������*/  
#include<netdb.h> /*�������������йصĽṹ���������ͣ��꣬����������gethostbyname()��*/  
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "PintTest.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPintTest::CPintTest()
{
	InitPing( ) ;
	m_dataLen = DEFAULT_LEN ;
}

CPintTest::~CPintTest()
{

}

void CPintTest::InitPing( )
{
	m_socket = ::socket( AF_INET , SOCK_RAW , IPPROTO_ICMP ) ;
}

BOOL CPintTest::Ping( char * pDestIp )
{
	/*
	A.��ping����������⣬Ŀǰ���ã�û�е�������
	B.���¸��汾ʹ��ʱ���ٵ�������
	if( pDestIp == NULL )
		return FALSE ;
	
	//��ȡ����ICMP��
	if( !PackICMP( pDestIp ) )
		return FALSE ;
	
	//����ping��
	//SendICMP( pDestIp ) ;
	//����ping��
	RecvICMP( pDestIp ) ;
*/
	return TRUE ;
}

BOOL CPintTest::SendICMP( char * pDestIp )
{
// 	struct sockaddr_in dest ;
// 	dest.sin_family = AF_INET ;
// 	dest.sin_port = htons( 0 ) ;
// 	dest.sin_addr.s_addr = inet_addr( pDestIp ) ;
// 	
// 	sendto( m_socket , m_sbuf ,  , 0 , (struct sockaddr *)&dest,sizeof (dest) );
	return  TRUE ;
}

BOOL CPintTest::RecvICMP( char * pDestIp )
{
	if( pDestIp == NULL )
		return FALSE ;
	
	BYTE byRecv[1024] ;
	memset( byRecv , 0 , sizeof( byRecv ) ) ;
	struct sockaddr_in dest ;
	socklen_t len = 0 ;
	int iRet = 0 ;
	iRet = recvfrom( m_socket , byRecv , sizeof( byRecv ) , 0 , ( struct sockaddr * )&dest , &len ) ;
	
	if( iRet )
		return TRUE ;
	else
		return FALSE ;
}

BOOL CPintTest::PackICMP( char * pDestIP )
{
	if( pDestIP == NULL )
		return FALSE ;

	struct iphdr *ip_hdr;   /*iphdrΪIPͷ���ṹ��*/  
    struct icmphdr *icmp_hdr;   /*icmphdrΪICMPͷ���ṹ��*/  
    int len;  
    int len1; 

	/*ipͷ���ṹ�������ʼ��*/  
    ip_hdr=(struct iphdr *)m_sbuf; /*�ַ���ָ��*/     
    ip_hdr->hlen=sizeof(struct iphdr)>>2;  /*ͷ������*/  
    ip_hdr->ver=IPVERSION;   /*�汾*/  
    ip_hdr->tos=0;   /*��������*/  
    ip_hdr->tot_len=IP_HSIZE+ICMP_HSIZE+m_dataLen; /*����ͷ�������ݵ��ܳ���*/  
    ip_hdr->id=0;    /*��ʼ�����ı�ʶ*/  
    ip_hdr->frag_off=0;  /*����flag���Ϊ0*/  
    ip_hdr->protocol=IPPROTO_ICMP;/*���õ�Э��ΪICMPЭ��*/  
    ip_hdr->ttl=255; /*һ������������Ͽ��Դ���ʱ��*/  
    ip_hdr->daddr= inet_addr( pDestIP );  /*Ŀ�ĵ�ַ*/  
    len1=ip_hdr->hlen<<2;  /*ip���ݳ���*/  

    /*ICMPͷ���ṹ�������ʼ��*/  
    icmp_hdr=(struct icmphdr *)(m_sbuf+len1);  /*�ַ���ָ��*/  
    icmp_hdr->type=8;    /*��ʼ��ICMP��Ϣ����type*/  
    icmp_hdr->code=0;    /*��ʼ����Ϣ����code*/  
    icmp_hdr->wRequestID=getpid( );   /*�ѽ��̱�ʶ���ʼ��icmp_id*/  
    icmp_hdr->wSequence= 0 ;  /*���͵�ICMP��Ϣ��Ÿ�ֵ��icmp���*/      
    gettimeofday((struct timeval *)icmp_hdr->timeStamp,NULL); /* ��ȡ��ǰʱ��*/  
	
    len=ip_hdr->tot_len; /*�����ܳ��ȸ�ֵ��len����*/  
    icmp_hdr->checksum=0;    /*��ʼ��*/  
    icmp_hdr->checksum=checksum((BYTE *)icmp_hdr,len);  /*����У���*/  

	struct sockaddr_in dest ;
	dest.sin_family = AF_INET ;
	dest.sin_port = htons( 0 ) ;
	dest.sin_addr.s_addr = inet_addr( pDestIP ) ;
	
	sendto( m_socket , m_sbuf , len , 0 , (struct sockaddr *)&dest,sizeof (dest) );
	return TRUE ;
}

WORD CPintTest::checksum( BYTE *buf,int len)  
{  
    DWORD sum=0;  
    WORD *cbuf;  
	
    cbuf=(WORD *)buf;  
	
    while(len>1){  
		sum+=*cbuf++;  
		len-=2;  
    }  
	
    if(len)  
        sum+=*(BYTE *)cbuf;  
	
	sum=(sum>>16)+(sum & 0xffff);  
	sum+=(sum>>16);  
	
	return ~sum;  
}




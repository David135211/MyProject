// TcpClient.cpp: implementation of the CTcpClient class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "TcpClient.h"
#define MAX_TCP_SIZE  1280
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTcpClient::CTcpClient()
{
	m_hComm = ERROR ;
	m_bConnet = FALSE ;
}

CTcpClient::~CTcpClient()
{
	ClosePort() ;
}

BOOL CTcpClient::IsPortValid( void )
{
	if(m_hComm <= 0)
	{
		//	printf( "m_hComm Error!\n") ;
		return FALSE;
	}

	if(m_bConnet <= 0)
	{
		//	printf( "m_bConnet Error!\n" ) ;
		return FALSE ;
	}

	return TRUE;
}

BOOL CTcpClient::OpenPort( char* lpszError )
{
	int  nVal;
	char buf[ 24 ] ;

	/* Create socket id */
	// sprintf(m_szRemoteAddr, "%s", m_szAttrib);
	/*lel*/
	printf("%s %d TCP_CLIENT\n", __func__, __LINE__);
	/*end*/
	memcpy(m_szRemoteAddr, m_szAttrib, 24);
	memcpy( buf , m_szAttrib , 24 ) ;
	if( (m_hComm=socket(AF_INET, SOCK_STREAM, 0))==ERROR ) /*IPPROTO_TCP*/
	{
		if( lpszError )
			sprintf( lpszError, "CTcpPort: %s", "Create socket error!" );
		return FALSE;
	}

	int nAddrSize = sizeof(struct sockaddr_in);
	bzero((char*)&m_RemoteAddr, nAddrSize);
	m_RemoteAddr.sin_family = AF_INET;
	m_RemoteAddr.sin_port = htons( m_uThePort );
	m_RemoteAddr.sin_addr.s_addr = inet_addr( m_szAttrib );

	/* Set SO_REUSEADDR */
	nVal = 1;
	setsockopt(m_hComm, SOL_SOCKET, SO_REUSEADDR, (char*)&nVal, sizeof(nVal));

	/* set up receive buffer sizes */
	nVal = MAX_TCP_SIZE;
	setsockopt(m_hComm, SOL_SOCKET, SO_RCVBUF, (char*)&nVal, sizeof(nVal));

	//初始先连接一次服务器，以后每次扫描连接是否断开
	Connect();

	return TRUE ;
}

BOOL CTcpClient::Connect( )
{
	if( m_hComm <= 0 ) return FALSE;
	if( connect( m_hComm,
				(struct sockaddr*)&m_RemoteAddr,
				sizeof(m_RemoteAddr) ) == ERROR )
	{
		return FALSE;
	}
	m_bConnet = TRUE ;
	return TRUE;
}

void CTcpClient::ClosePort( void )
{
	if(m_hComm==ERROR) return;
	shutdown(m_hComm, 2);
	close(m_hComm);
	m_hComm = ERROR;
	m_bConnet = 0;
}

int	CTcpClient::ReadPort( BYTE *pBuf, int nRead )
{
	int  nBytes = 0;

	if( !IsPortValid() || nRead<=0 ) return -1;
	nBytes = recv( m_hComm, (char*)pBuf, nRead, 0 );
	if( nBytes == ERROR )
	{
		m_bConnet = FALSE ;
		return -2;
	}

	return nBytes;
}

int	 CTcpClient::WritePort( BYTE *pBuf, int nWrite )
{
	if( !IsPortValid() || nWrite<=0 ) return -1;
	//MSG_NOSIGNAL 禁止向系统发送异常信息
	int nBytes = send( m_hComm, (char*)pBuf, nWrite, MSG_NOSIGNAL );
	if( nBytes == ERROR )
	{
		perror( "send" ) ;
		m_bConnet = 0;
		return -2;
	}
	return nBytes;
}

int	 CTcpClient::AsyReadData( BYTE *pBuf, int nRead )
{
	int  nBytes = 0;
	fd_set rfds;
	struct timeval tv;

	if( !IsPortValid() || nRead<=0 ) return -1;
	FD_ZERO(&rfds);
	FD_SET(m_hComm, &rfds);
	tv.tv_sec  = 0;
	tv.tv_usec = 10000;
	switch( select(m_hComm+1, &rfds, NULL, NULL, &tv) )
	{
	case ERROR:
		m_bConnet = 0;
		return -2;
	case 0:
		break;
	case 1:
		if(FD_ISSET(m_hComm, &rfds))
		{
			nBytes = recv( m_hComm, (char*)pBuf, nRead, 0 );
			if( nBytes == ERROR )
				return 0;
			else if( nBytes == 0 )
				ClosePort() ;
		}
		break;
	}
	return nBytes;
}

int  CTcpClient::AsySendData( BYTE *pBuf, int nWrite )
{
	return 0 ;
}

BOOL CTcpClient::Ping( char * cIp )
{
	if( cIp == NULL )
		return FALSE ;

	m_Ping.Ping( cIp ) ;

	return TRUE ;
}


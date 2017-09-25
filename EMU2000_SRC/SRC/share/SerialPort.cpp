/******************************************************************************
 *
 *  SerialPort.cpp: implementation of the serial port class.
 *  2010/12/01 Create by HouPeng for Linux 
 *
 ******************************************************************************/
#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>  /* Standard libarary definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>

#include "SerialPort.h"

/*****************************************************************************/
#if 0
#define MAX_COMM_NUM  6
char *SERIAL_NAME[MAX_COMM_NUM] =
{
	"/dev/tts/1",
	"/dev/tts/2",
	"/dev/ttyASYNC00",
	"/dev/ttyASYNC01",
	"/dev/ttyASYNC02",
	"/dev/ttyASYNC03"
};
#else
#define MAX_COMM_NUM  16

#if 1				//for ppc platform!
char *SERIAL_NAME[MAX_COMM_NUM] =
{/*{{{*/
	(char *)"/dev/ttySub0",
	(char *)"/dev/ttySub1",
	(char *)"/dev/ttySub2",
	(char *)"/dev/ttySub3",
	(char *)"/dev/ttySub4",
	(char *)"/dev/ttySub5",
	(char *)"/dev/ttySub6",
	(char *)"/dev/ttySub7",
	(char *)"/dev/ttySub8",
	(char *)"/dev/ttySub9",
	(char *)"/dev/ttySub10",
	(char *)"/dev/ttySub11",
	(char *)"/dev/ttySub12",
	(char *)"/dev/ttySub13",
	(char *)"/dev/ttySub14",
	(char *)"/dev/ttySub15",
};/*}}}*/
#endif

#endif
#if 0			//for arm platform!
char *SERIAL_NAME[MAX_COMM_NUM]={
	(char *)"/dev/ttyCH0",
	(char *)"/dev/ttyCH1",
	(char *)"/dev/ttyCH2",
	(char *)"/dev/ttyCH3",
	(char *)"/dev/ttyCH4",
	(char *)"/dev/ttyCH5",
	(char *)"/dev/ttyCH6",
	(char *)"/dev/ttyCH7",
	(char *)"/dev/ttyCH8",
	(char *)"/dev/ttyCH9",
	(char *)"/dev/ttyCH10",
	(char *)"/dev/ttyCH11",
	(char *)"/dev/ttyCH12",
	(char *)"/dev/ttyCH13",
	(char *)"/dev/ttyCH14",
	(char *)"/dev/ttyCH15"
};
#endif
#if 0		//for arm platform! 8串口
char *SERIAL_NAME[MAX_COMM_NUM]={
	(char *)"/dev/ttyCH0",
	(char *)"/dev/ttyCH1",
	(char *)"/dev/ttyCH2",
	(char *)"/dev/ttyCH3",
	(char *)"/dev/ttyCH4",
	(char *)"/dev/ttyCH5",
	(char *)"/dev/ttyCH6",
	(char *)"/dev/ttyCH7",
};
#endif

/*****************************************************************************/
CSerialPort::CSerialPort()
{/*{{{*/
	m_uThePort = 1;
	m_hComm = ERROR;
	m_nBaudRate = 1200;
}/*}}}*/

CSerialPort::~CSerialPort()
{ /*{{{*/
	ClosePort();
}/*}}}*/

BOOL CSerialPort::IsPortValid( void )
{/*{{{*/
	return (m_hComm > 0);
}/*}}}*/

void CSerialPort::ClosePort( void )
{/*{{{*/
	if( !IsPortValid() ) return;
	tcflush(m_hComm, TCIFLUSH);
	tcflush(m_hComm, TCOFLUSH);
	close(m_hComm);
}/*}}}*/

/*COM2:19200,N,8,1*/
BOOL CSerialPort::OpenPort( char* lpszError )
{/*{{{*/
	int	 i, k, nLen;
	char szName[32];
	struct termios  tio;
	const char* p = m_szAttrib;

	nLen = strlen(m_szAttrib);
	for( i=0, k=0; i<nLen; i++, p++ )
	{
		if( *p != ',' ) { szName[k++] = *p; }
		else { szName[k] = '\0'; break; }
	}
	switch( atoi(szName) )
	{
	case 300:    m_nBaudRate=B300; break;
	case 600:    m_nBaudRate=B600; break;
	case 1200:   m_nBaudRate=B1200; break;
	case 2400:   m_nBaudRate=B2400; break;
	case 4800:   m_nBaudRate=B4800; break;
	case 9600:   m_nBaudRate=B9600; break;
	case 19200:  m_nBaudRate=B19200; break;
	case 38400:  m_nBaudRate=B38400; break;
	case 57600:  m_nBaudRate=B57600; break;
	case 115200: m_nBaudRate=B115200; break;
	default:
				 if( lpszError )
					 sprintf( lpszError, "Baudrate(%s) error!", szName );
				 return FALSE;
	}
	if(m_uThePort>0 && m_uThePort<=MAX_COMM_NUM)
		sprintf( szName, "%s", SERIAL_NAME[m_uThePort-1] );
	else
	{
		if( lpszError )
			sprintf( lpszError, "portnum(%d) no exit!", m_uThePort );
		return FALSE;
	}
	m_hComm = open( szName, O_RDWR|O_NOCTTY|O_NDELAY, 0 );
	if( m_hComm < 0 )
	{
		if( lpszError )
			sprintf( lpszError, "open %s error!", szName );
		return FALSE;
	}
	fcntl( m_hComm, F_SETFL, 0 ); //FNDELAY

	//Get the current options for the port...
	tcgetattr( m_hComm, &tio );
	//Set the baud rates
	cfsetispeed( &tio, m_nBaudRate );
	cfsetospeed( &tio, m_nBaudRate );
	cfmakeraw( &tio );			//将终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理，在原始模式下，终端是不可回显的， 而且所有特定的终端输入/输出模式不可用!

	p++;
	/*	tio.c_cflag &= ~PARENB;
		tio.c_iflag &= ~INPCK;
		switch( *p )
		{
		case 'E':
		case 'e':
		tio.c_cflag |= PARENB;
		tio.c_cflag &= ~PARODD;
		tio.c_iflag |= INPCK;
		break;
		case 'O':
		case 'o':
		tio.c_cflag |= (PARENB|PARODD);
		tio.c_iflag |= INPCK;
		break;
		}
		tio.c_cflag &= ~CSTOPB;
		tio.c_cflag &= ~CSIZE; // Mask the character size bits 
		tio.c_cflag |= CS8;    // Select 8 data bits 
		*/
	//设置校验位
	switch ( *p )
	{  
	case 'n':
	case 'N': //无奇偶校验位。
		tio.c_cflag &= ~PARENB; 
		tio.c_iflag &= ~INPCK;    
		break; 
	case 'o':  
	case 'O'://设置为奇校验    
		tio.c_cflag |= (PARODD | PARENB); 
		tio.c_iflag |= INPCK;             
		break; 
	case 'e': 
	case 'E'://设置为偶校验  
		tio.c_cflag |= PARENB;       
		tio.c_cflag &= ~PARODD;       
		tio.c_iflag |= INPCK;       
		break;
	case 's':
	case 'S': //设置为空格 
		tio.c_cflag &= ~PARENB;
		tio.c_cflag &= ~CSTOPB;
		break; 
	default:  
		fprintf(stderr,"Unsupported parity/n");   
		return (FALSE); 
	}

	//设置数据位
	tio.c_cflag &= ~CSIZE; //屏蔽其他标志位
	switch ( p[2] )
	{  
	case '5' :
		tio.c_cflag |= CS5;
		break;
	case '6' :
		tio.c_cflag |= CS6;
		break;
	case '7' :    
		tio.c_cflag |= CS7;
		break;
	case '8':    
		tio.c_cflag |= CS8;
		break;  
	default:   
		fprintf(stderr,"Unsupported data size/n");
		return (FALSE); 
	}

	// 设置停止位 
	switch ( p[4] )
	{  
	case '1':   
		tio.c_cflag &= ~CSTOPB; 
		break; 
	case '2':   
		tio.c_cflag |= CSTOPB; 
		break;
	default:   
		fprintf(stderr,"Unsupported stop bits/n"); 
		return (FALSE);
	}


	tcsetattr( m_hComm, TCSANOW, &tio );
	tcflush( m_hComm, TCIOFLUSH );

	if( lpszError )
		sprintf( lpszError, "open %s(BAUDRATE=%d) ok!", szName, m_nBaudRate );
	return TRUE;
}/*}}}*/

BOOL CSerialPort::SetQueue( DWORD dwInQueueSize, DWORD dwOutQueueSize )
{/*{{{*/
	if( !IsPortValid() ) return FALSE; //TIOCINQ TIOCOUTQ
	//	ioctl(m_hComm, FIORBUFSET, (int)dwInQueueSize);
	//	ioctl(m_hComm, FIOWBUFSET, (int)dwOutQueueSize);
	return TRUE;
}/*}}}*/

int CSerialPort::GetInQueue( void )
{/*{{{*/
	int nBytes = 0;
	if( !IsPortValid() ) return -1;
	ioctl(m_hComm, FIONREAD, (int)&nBytes); 
	return nBytes;
}/*}}}*/

int CSerialPort::GetOutQueue( void )
{/*{{{*/
	int	nBytes = 0;
	if( !IsPortValid() ) return -1;
	//	ioctl(m_hComm, FIONWRITE, (int)&nBytes);
	return nBytes;
}/*}}}*/

int CSerialPort::ReadPort( BYTE *pBuf, int nRead )
{/*{{{*/
	if( !IsPortValid() || nRead<=0 ) return -1;
	return read(m_hComm, (char*)pBuf, nRead);
}/*}}}*/

int CSerialPort::WritePort( BYTE *pBuf, int nWrite )
{/*{{{*/
	if( !IsPortValid() || nWrite<=0 ) return -1;
	tcflush( m_hComm, TCOFLUSH );
	int iRtn = write(m_hComm, (char*)pBuf, nWrite);
	return iRtn;
}/*}}}*/

int CSerialPort::AsyReadData( BYTE *pBuf, int nRead )
{/*{{{*/
	int iRtn;
	/*
	   int nSize = 0;
	   int nCount = GetInQueue();
	   if( nCount <= 0 ) return 0;
	   nSize = min(nCount, nRead);
	   return read(m_hComm, (char*)pBuf, nSize);
	   */
	if( !IsPortValid() || nRead<=0 ) return -1;
	fd_set rfds;
	struct timeval tv;

	FD_ZERO(&rfds); 
	FD_SET(m_hComm, &rfds); 
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	switch( select(m_hComm+1, &rfds, NULL, NULL, &tv) ) 
	{
	case ERROR:
		return -2;
	case 0:
		break;
	case 1:
		if(FD_ISSET(m_hComm, &rfds)) 
		{
			iRtn = read(m_hComm, (char*)pBuf, nRead); 
			tcflush( m_hComm, TCIFLUSH );
			return iRtn;
		}

		break;
	}
	return 0;
}/*}}}*/

int CSerialPort::AsySendData( BYTE *pBuf, int nWrite )
{/*{{{*/
	if( !IsPortValid() || nWrite<=0 ) return -1;

	fd_set wfds;
	struct timeval tv;

	FD_ZERO(&wfds); 
	FD_SET(m_hComm, &wfds); 
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	switch( select(m_hComm+1, NULL, &wfds, NULL, &tv) ) 
	{
	case ERROR:
		return -2;
	case 0:
		break;
	case 1:
		if(FD_ISSET(m_hComm, &wfds)) 
			return write(m_hComm, (char*)pBuf, nWrite); 
		break;
	}
	return 0;
}/*}}}*/
/*****************************************************************************/

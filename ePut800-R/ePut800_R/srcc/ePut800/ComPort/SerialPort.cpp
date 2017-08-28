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
#include "structure.h"

#define MAX_COMM_NUM  8
const char *SERIAL_NAME[MAX_COMM_NUM] =
{
    (char *)"/dev/ttyS0",
    (char *)"/dev/ttyS1",
    (char *)"/dev/ttyS2",
    (char *)"/dev/ttyS3",
    (char *)"/dev/ttyS4",
    (char *)"/dev/ttyS5",
    (char *)"/dev/ttyS6",
    (char *)"/dev/ttyS7",
};


/*****************************************************************************/
CSerialPort::CSerialPort()
{
    m_uThePort = 1;
    m_hComm = ERROR;
    m_nBaudRate = 1200;
}

CSerialPort::~CSerialPort()
{ 
    ClosePort();
}

BOOL CSerialPort::IsPortValid( void )
{
    return (m_hComm > 0);
}

void CSerialPort::ClosePort( void )
{
    if( !IsPortValid() ) return;
	tcflush(m_hComm, TCIFLUSH);
	tcflush(m_hComm, TCOFLUSH);
    close(m_hComm);
}

/*COM2:19200,N,8,1*/
BOOL CSerialPort::OpenPort( char* lpszError )
{
    int	 i, k, nLen;
    char szName[32];
	struct termios  tio;
    const char* p = m_szAttrib;
//	printf("##### m_szAttrib = %s\n", m_szAttrib);

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
                {
                    sprintf( lpszError, "Baudrate(%s) error!", szName );
                    Q_ASSERT( FALSE ) ;
                }
		return FALSE;
	}
	if(m_uThePort>0 && m_uThePort<=MAX_COMM_NUM)
		sprintf( szName, "%s", SERIAL_NAME[m_uThePort-1] );
	else
	{
		if( lpszError )
                {
                    sprintf( lpszError, "portnum(%d) no exit!", m_uThePort );
                    Q_ASSERT( FALSE ) ;
                }
		return FALSE;
	}
	m_hComm = open( szName, O_RDWR|O_NOCTTY|O_NDELAY, 0 );
	if( m_hComm < 0 )
	{
		if( lpszError )
            {
                    sprintf( lpszError, "open %s error!", szName );
                    //Q_ASSERT( FALSE );
                }
		return FALSE;
	}
	fcntl( m_hComm, F_SETFL, 0 ); //FNDELAY

	//Get the current options for the port...
	tcgetattr( m_hComm, &tio );
	//Set the baud rates
	cfsetispeed( &tio, m_nBaudRate );
	cfsetospeed( &tio, m_nBaudRate );
        cfmakeraw( &tio );
        tio.c_oflag |= OPOST;
        tio.c_oflag &= ~ONLCR;
        p++;
        //设置校验位
//	printf("############# *p = %c\n", *p);
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
}

BOOL CSerialPort::SetQueue( DWORD dwInQueueSize, DWORD dwOutQueueSize )
{
	if( !IsPortValid() ) return FALSE; //TIOCINQ TIOCOUTQ
//	ioctl(m_hComm, FIORBUFSET, (int)dwInQueueSize);
//	ioctl(m_hComm, FIOWBUFSET, (int)dwOutQueueSize);
	return TRUE;
}

int CSerialPort::GetInQueue( void )
{
	int nBytes = 0;
	if( !IsPortValid() ) return -1;
	ioctl(m_hComm, FIONREAD, (int)&nBytes); 
	return nBytes;
}

int CSerialPort::GetOutQueue( void )
{
	int	nBytes = 0;
	if( !IsPortValid() ) return -1;
//	ioctl(m_hComm, FIONWRITE, (int)&nBytes);
	return nBytes;
}

int CSerialPort::ReadPort( BYTE *pBuf, int nRead )
{
	if( !IsPortValid() || nRead<=0 ) return -1;
	return read(m_hComm, (char*)pBuf, nRead);
}

int CSerialPort::WritePort( BYTE *pBuf, int nWrite )
{
	if( !IsPortValid() || nWrite<=0 ) return -1;
    tcflush( m_hComm, TCOFLUSH );
    int iRtn = write(m_hComm, (char*)pBuf, nWrite);
//	printf("###%s %d iRtn = %d\n", __func__, __LINE__, iRtn);
    return iRtn;
}

int CSerialPort::AsyReadData( BYTE *pBuf, int nRead )
{
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
}

int CSerialPort::AsySendData( BYTE *pBuf, int nWrite )
{
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
}

BOOL CSerialPort::SetPreOpenPara( Property * pProperty )
{
    if( !pProperty )
    {
       Q_ASSERT( FALSE ) ;
       return FALSE ;
    }

    if( pProperty->m_ComType != Property::COMRS485 )
    {
        Q_ASSERT( FALSE ) ;
        return FALSE ;
    }

    COM_Property * pComProperty = ( COM_Property *)pProperty ;
    //9600,n,8,1
    sprintf( m_szAttrib , "%ld,%c,%d,%d" , pComProperty->wBaud ,
             (char)pComProperty->byParity , pComProperty->byDataBits ,
             pComProperty->byStopBits ) ;

//	printf("### m_szAttrib = %s\n", m_szAttrib);
    m_uThePort = pComProperty->wPortNo ;
    return TRUE ;
}

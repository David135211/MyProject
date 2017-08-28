/******************************************************************************
*
*  TcpPort.h: interface for the TcpPort class for Linux
*  Copyright (C): 2010 by houpeng
*
******************************************************************************/
#ifndef _TCPPORT_H_
#define _TCPPORT_H_

#include <netinet/in.h>
#include "ccomport.h"

#define MAX_TCP_SIZE  1280

class CTcpPort : public CComPort
{
public:
    CTcpPort(void);
    virtual ~CTcpPort(void);
    WORD m_uThePort ;
    char m_szAttrib[ 100 ] ;

/* Attributes */
private:
    int m_nState;
    int m_nAddrSize;
    struct sockaddr_in m_RemoteAddr;
    int m_hComm ;
    char m_szRemoteAddr[24],m_szLocalAddr[24];

protected:

/* Implementation */
public:
	void Attach( int hSocket );
    BOOL Connect( /*int nTimeout*/ );
    virtual BOOL   IsPortValid( void );
    virtual BOOL   OpenPort( char* lpszError=NULL );
    virtual void   ClosePort( void );
    virtual BOOL   SetQueue( DWORD dwInQueueSize, DWORD dwOutQueueSize );
    virtual int	   GetInQueue( void );
    virtual int    GetOutQueue( void );
    virtual int	   ReadPort( BYTE *pBuf, int nRead );
    virtual int	   WritePort( BYTE *pBuf, int nWrite );
    virtual int	   AsyReadData( BYTE *pBuf, int nRead );
    virtual int    AsySendData( BYTE *pBuf, int nWrite );
};
#endif

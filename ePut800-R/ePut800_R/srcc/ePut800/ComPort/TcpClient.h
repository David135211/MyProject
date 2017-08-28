// TcpClient.h: interface for the CTcpClient class.
//
//////////////////////////////////////////////////////////////////////


#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_

#include <netinet/in.h>
//#include "BasePort.h"
#include "ccomport.h"

class CTcpClient  : public CComPort
{
public:
	CTcpClient();
	virtual ~CTcpClient();
	virtual BOOL   IsPortValid( void );
	virtual BOOL   OpenPort( char* lpszError=NULL );
	virtual void   ClosePort( void );
	virtual int	   ReadPort( BYTE *pBuf, int nRead );
	virtual int	   WritePort( BYTE *pBuf, int nWrite );
	virtual int	   AsyReadData( BYTE *pBuf, int nRead );
        virtual int    AsySendData( BYTE *pBuf, int nWrite );
	virtual BOOL Connect( );
        virtual BOOL SetPreOpenPara( Property * pProperty );
protected:
        int m_hComm ;
        UINT	m_uThePort;
        char    m_szAttrib[32];
        //char	m_szLocalAddr[24];
        char	m_szRemoteAddr[24];
	BOOL m_bConnet ;
	 struct sockaddr_in m_RemoteAddr;
	 BOOL Ping( char * cIp ) ;
};

#endif // !defined(AFX_TCPCLIENT_H__06DCF3A5_6F00_4CAD_8D07_3DDB75370453__INCLUDED_)

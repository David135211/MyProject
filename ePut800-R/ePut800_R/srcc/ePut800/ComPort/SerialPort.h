/******************************************************************************
*
*  SerialPort.h: interface for the SerialPort class.
*
******************************************************************************/
#ifndef _SERIALPORT_H_
#define _SERIALPORT_H_
#include "ccomport.h"


class CSerialPort : public CComPort
{
/*Constructors / Destructors*/
public:
	CSerialPort(void);
	virtual ~CSerialPort(void);

/* Attributes */
private:
	int	m_nBaudRate;
        WORD m_uThePort ;
        int m_hComm ;
        char m_szAttrib[ 100 ] ;
protected:

public:

/* Implementation */
public:
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
        virtual BOOL   SetPreOpenPara( Property * pProperty );

private:
	
};
/*****************************************************************************/
#endif /*_SERIALPORT_H_*/

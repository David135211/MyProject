// Protocol_UPS.h: interface for the CProtocol_UPS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTOCOL_UPS_H__DB4E4A83_510B_4232_A294_B1B4EE1AF4FD__INCLUDED_)
#define AFX_PROTOCOL_UPS_H__DB4E4A83_510B_4232_A294_B1B4EE1AF4FD__INCLUDED_


#include "../../share/CProtocol.h"
#include "../../share/CMethod.h"

class CProtocol_UPS  : public CProtocol
{
public:
	CProtocol_UPS();
	virtual ~CProtocol_UPS();

	virtual BOOL GetProtocolBuf( BYTE * buf , int &len , PBUSMSG pBusMsg = NULL ) ;
	virtual BOOL ProcessProtocolBuf( BYTE * buf , int len ) ;

	BOOL UPSQueryStatePack( BYTE * buf , int &len );

	BOOL UPSQueryStateDeal( BYTE * buf , int len );

	virtual BOOL Init( BYTE byLineNo  ) ;
	
	protected:
		BOOL GetDevData( ) ;
	protected:
		BOOL ProcessFileData( CProfile &profile );
		BOOL CreateModule( int iModule , int iSerialNo , WORD iAddr , char * sName , char * stplatePath ) ;

};

#endif // !defined(AFX_PROTOCOL_UPS_H__DB4E4A83_510B_4232_A294_B1B4EE1AF4FD__INCLUDED_)

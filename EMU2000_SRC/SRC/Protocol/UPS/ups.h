#if !defined(UPS_)
#define UPS_

#define UPSMSGFALSE (1)
#define UPSMSGTRUE (0)

//#include "../../share/CProtocol.h"
#include "Protocol_UPS.h"

class CUPS  : public CProtocol_UPS
{
public:
	CUPS();
	virtual ~CUPS();

	BYTE SendFlag;
	BOOL m_bLinkStatus;
	BYTE m_bySendCount;


	virtual BOOL GetProtocolBuf( BYTE * buf , int &len , PBUSMSG pBusMsg = NULL ) ;
	virtual BOOL ProcessProtocolBuf( BYTE * buf , int len ) ;
	virtual BOOL Init( BYTE byLineNo  ) ;
	
	virtual void TimerProc() ;
	//获得装置通讯状态
	virtual BOOL GetDevCommState( ) ;
	
	BOOL GetYKBuffer( BYTE * buf , int &len , PBUSMSG pBusMsg );
		
	BOOL UPSQueryStatePack( BYTE * buf , int &len );
	
	BOOL UPSQueryCellStatePack( BYTE * buf , int &len );

	BOOL UPSQueryG2StatePack( BYTE * buf , int &len );

	BOOL UPSQueryG3StatePack( BYTE * buf , int &len );

	BOOL UPSTestTenSecondPack( BYTE * buf , int &len );
	BOOL UPSTestundervoltagePack( BYTE * buf , int &len );
	BOOL UPSTestSomeMinutesPack( BYTE * buf , int &len );
	BOOL UPSControlBuzzerPack( BYTE * buf , int &len );
	BOOL UPSPowerOffPack( BYTE * buf , int &len );
	BOOL UPSPowerOffPowerOnPack( BYTE * buf , int &len );
	BOOL UPSCancelPowerOffPack( BYTE * buf , int &len );
	BOOL UPSCancelTestPack( BYTE * buf , int &len );

	BOOL UPSQueryStateDeal( BYTE * buf , int len );

	BOOL UPSQueryCellStateDeal( BYTE * buf , int len );

	BOOL UPSQueryG2StateDeal( BYTE * buf , int len );
	
	BOOL UPSQueryG3StateDeal( BYTE * buf , int len );
};

#endif // !defined(AFX_PROTOCOL_UPS_H__DB4E4A83_510B_4232_A294_B1B4EE1AF4FD__INCLUDED_)


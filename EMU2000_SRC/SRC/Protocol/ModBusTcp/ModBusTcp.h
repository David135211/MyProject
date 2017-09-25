#if !defined(MODBUSTCP_)
#define MODBUSTCP_



//#include "../../share/CProtocol.h"
#include "CProtocol_ModBusTcp.h"

#define MODBUSTCPMAX_AI_LEN (6800)
#define MODBUSTCPMAX_PI_LEN (1599)
#define MODBUSTCPMAX_DI_LEN (9999)

#define MODBUSTCPMAX_MSG_LEN (1024)

class ModBusTcp  : public CProtocol_ModBusTcp
{
public:
	ModBusTcp();
	virtual ~ModBusTcp();

	BYTE SendFlag;
 	float    m_wYCBuf[MODBUSTCPMAX_AI_LEN];
    DWORD   m_dwYMBuf[MODBUSTCPMAX_PI_LEN];
	BYTE	m_byYXbuf[MODBUSTCPMAX_DI_LEN ] ;

	BYTE MsgBuf[MODBUSTCPMAX_MSG_LEN];
	WORD MsgLen;
	BYTE MsgFlag;
	BYTE FunNum;
	BYTE ErrorFlag;
	int m_wErrorTimer;
	int m_byPortStatus;
	BYTE yk_bufSerial[2];//遥控报文的前两字节序号
	
	BYTE Yk_FunNum;		//保存遥控下发帧的功能码
	BOOL yk_recv_flag;		//接收到遥控下发帧置为真，遥控回复之后置为假，默认假
	UINT time_flag;			//保存收到遥控下发帧的时刻

	virtual BOOL GetProtocolBuf( BYTE * buf , int &len , PBUSMSG pBusMsg = NULL ) ;
	virtual BOOL ProcessProtocolBuf( BYTE * buf , int len ) ;
	virtual BOOL Init( BYTE byLineNo  ) ;
	virtual BOOL InitRtuBase();
	virtual BOOL WriteAIVal(WORD wSerialNo ,WORD wPnt, float wVal) ;
	virtual BOOL WriteDIVal(WORD wSerialNo ,WORD wPnt, WORD wVal) ;
	virtual BOOL WritePIVal(WORD wSerialNo ,WORD wPnt, DWORD dwVal) ;
	virtual BOOL WriteSOEInfo( WORD wSerialNo ,WORD wPnt, WORD wVal, LONG lTime, WORD wMiSecond) ;
	virtual  void RelayEchoProc(BYTE byCommand, WORD wIndex, BYTE byResult);

	//获得装置通讯状态 by	zhanghg 2014-9-23
	virtual BOOL GetDevCommState( ) ;
	virtual void TimerProc() ;

	BOOL YXPacket( BYTE * buf , int len ); 
	BOOL YcYmPacket( BYTE * buf , int len ); 
	BOOL YKMsg( BYTE * buf , int len ); 
	BOOL YKPacket( int yk_register , int val ); 

	BOOL DealBusMsgInfo( PBUSMSG pBusMsg ); 

	BOOL ErrorPacket( BYTE * buf , BYTE errorflag ); 
	
	
};

#endif // !defined(AFX_PROTOCOL_UPS_H__DB4E4A83_510B_4232_A294_B1B4EE1AF4FD__INCLUDED_)


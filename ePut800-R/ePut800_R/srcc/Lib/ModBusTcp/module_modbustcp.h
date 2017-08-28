#ifndef MODULE_MODBUSTCP_H
#define MODULE_MODBUSTCP_H

#include "modbustcp.h"
#include <vector>
#include <sys/time.h>

class CModule_ModBusTcp : public CModBusTcp
{
public:
    CModule_ModBusTcp( CMethod * m_pMethod , WORD wBusNo , WORD wAddr );

    BYTE SendFlag;

    virtual BOOL GetReadBuf( BYTE *pBuf , int &len , BYTE &byType, LPVOID pVoid  );
    virtual BOOL ProcessBuf( BYTE *pBuf , int len , BYTE byType , LPVOID pVoid ) ;
    virtual int GetErrorMsg( BYTE *pBuf , int len );
    virtual BOOL Init( );
    virtual WORD GetProtoAddr( ){ return m_wAddr ;}
    //virtual BOOL GetDevCommState( ) ;
    //virtual void TimerProc() ;


    WORD m_wBusNO ;

private:
    //char m_sTemplatePath[ 64 ] ;//模板名字
    BYTE MsgBuf[256];
    WORD MsgLen;
    BYTE MsgFlag;
    BYTE FunNum;
    BYTE ErrorFlag;
    int m_wErrorTimer;

    char ErrorInformation[300];

    BOOL YXPacket( BYTE * buf , int len );
    BOOL YcYmPacket( BYTE * buf , int len );
    BOOL YKMsg( BYTE * buf , int len );
    BOOL YKPacket( int yk_register , int val );
    BOOL DealBusMsgInfo( PYKST_MSG pBusMsg );

    BOOL ErrorPacket( BYTE errorflag );
};

#endif // MODULE_MODBUSTCP_H

#ifndef CPROTOCOL_H
#define CPROTOCOL_H
#include "cmethod.h"
#include <QVector>

class CProtocol ;
typedef QVector< CProtocol * > QPROTO_MODULE_ARRAY ;
typedef QPROTO_MODULE_ARRAY::iterator ITOR_PROTO ;

#define PROTO_NULL       1
#define PROTO_MODBUS     2
#define PROTO_MODBUSTCP  3

//PROTO_MODBUS
#define PROTO_MODULE_MODBUSRTU  1

//PROTO_MODBUSTCP
#define PROTO_MODULE_MODBUSTCP  1

//device real running status
#define COMM_NOMAL    0
#define COMM_ABNORMAL  1


#define YK_NULL         0x00
#define YK_OPEN         0x01
#define YK_CLOSE        0x02
#define YK_PREPARE      0x03
#define YK_PREPARE_RTN  0x04
#define YK_EXEC         0x05
#define YK_EXEC_RTN     0x06
#define YK_CANCEL       0x07
#define YK_CANCEL_RTN   0x08

#define YK_ERROR_FUN_01             0xFFFC
#define YK_ERROR_REGISTER_02        0xFFFD
#define YK_ERROR_DATA_03            0xFFFE
#define YK_ERROR_DEVSTATE_04        0xFFFF

#define BUS_GATHER  0x10
#define BUS_TRANS   0x11
#define BUS_PAUSE   0x12

class CProtocol
{
public:
    CMethod * m_pMethod ;
    WORD m_wProtoNo ;
    BYTE m_byProtoType ;
    WORD m_circulateCounter ;
    WORD m_wAddr ;
    char m_sTemplatePath[64];
    QPROTO_MODULE_ARRAY m_ProtoModule ;
public:
    CProtocol( CMethod * pMethod )
    {
        if(!pMethod)
            return ;
        m_pMethod = pMethod ;
        m_circulateCounter = 0 ;
    }

    virtual ~CProtocol(){ return ;}
    virtual BOOL GetReadBuf( BYTE *pBuf , int &len , BYTE &byType, LPVOID pVoid  ){ return FALSE ;}
    virtual BOOL ProcessBuf( BYTE *pBuf , int len , BYTE byType , LPVOID pVoid ){ return FALSE ;}
    virtual WORD GetCRC(BYTE *pBuf,WORD nLen){ return FALSE ;}
    virtual BOOL Init( ) { return FALSE ;}
    virtual int GetErrorMsg( BYTE *pBuf , int len ){ return 0 ;}

    virtual BOOL AddModule( WORD wBusNo , WORD wAddr , WORD wModule , char * stemplate){ return FALSE ;}
    virtual BOOL IsProtoValid() = 0 ;
    virtual WORD GetProtoNo () = 0 ; // proto logical no
    virtual BYTE GetProtoType() = 0 ;  //proto type gather or transmit
    virtual WORD GetProtoAddr( ) = 0 ; // proto Addr
};

#endif // CPROTOCOL_H

#ifndef MODBUSRTU_H
#define MODBUSRTU_H
#include "../../ePut800/structure.h"
#include "../../ePut800/Proto/cprotocol.h"
#include "../../ePut800/Proto/cmethod.h"

#include "ModBusRtu_global.h"
#include <stdio.h>
#define MODBUSRTUPREFIXFILENAME	"./config/ModBus/template/"
class MODBUSRTUSHARED_EXPORT CModBusRtu : public CProtocol
{
public:
    CModBusRtu( CMethod * pMethod ) ;
    virtual WORD GetCRC(BYTE *pBuf,WORD nLen);

    virtual BOOL Init( );
    virtual BOOL AddModule(WORD wBusNo , WORD wAddr , WORD wModule, char * stemplate);
    virtual BOOL IsProtoValid() { return TRUE ;}
    virtual WORD GetProtoNo () { return PROTO_MODBUS ; } // proto serialno
    virtual BYTE GetProtoType(){ return BUS_GATHER ; }  //proto type gather or transmit
    virtual WORD GetProtoAddr( ){ return m_wAddr ;}
};



#endif // MODBUSRTU_H

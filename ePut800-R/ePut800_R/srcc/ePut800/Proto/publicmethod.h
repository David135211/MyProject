#ifndef PUBLICMETHOD_H
#define PUBLICMETHOD_H

#include "cmethod.h"

class CPublicMethod : public CMethod
{
public:
    CPublicMethod();
    virtual BOOL SetYc( WORD wBusNo , WORD wAddr , WORD wPnt , float fVal );
    virtual BOOL GetYc( WORD wBusNo , WORD wAddr , WORD wPnt , float &fVal );
    virtual BOOL SetYx( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE bVal );
    virtual BOOL GetYx( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE &bVal );
    virtual BOOL SetYm( WORD wBusNo , WORD wAddr , WORD wPnt , DWORD dwVal );
    virtual BOOL GetYm( WORD wBusNo , WORD wAddr , WORD wPnt , DWORD &dwVal );

    virtual BOOL SetDevCommState( WORD wBusNo , WORD wAddr, BYTE byState ) ;
    virtual BOOL GetDevCommState( WORD wBusNo , WORD wAddr, BYTE &byState ) ;
    virtual BOOL SengYK( WORD wBusNoDest , WORD wAddrDest , WORD nPntDest ,
                            WORD wBusNoSrc , WORD wAddrSrc , WORD nPntSrc ,
                            BYTE byAction , WORD wVal );
    virtual BOOL SetSoe( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE byVal ,void * pRealTime);
    virtual BOOL GetSysTime(void *pRealTime);
};

#endif // PUBLICMETHOD_H

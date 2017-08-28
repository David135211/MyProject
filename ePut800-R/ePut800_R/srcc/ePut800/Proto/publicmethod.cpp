#include "publicmethod.h"
#include "cglobal.h"
CPublicMethod::CPublicMethod()
{
}

BOOL CPublicMethod::SetYc( WORD wBusNo , WORD wAddr , WORD wPnt , float fVal )
{
    return AfxSetYc( wBusNo , wAddr , wPnt , fVal ) ;
}

BOOL CPublicMethod::GetYc( WORD wBusNo , WORD wAddr , WORD wPnt , float &fVal )
{
    PYCST pYc = AfxGetYc( wBusNo , wAddr , wPnt ) ;
    if( !pYc )
        return FALSE ;

    fVal = pYc->fVal_1Time ;
    return TRUE ;
}

BOOL CPublicMethod::SetYx( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE bVal )
{
    return AfxSetYx( wBusNo , wAddr , wPnt , bVal ) ;
}

BOOL CPublicMethod::GetYx( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE &byVal )
{
    PYXST pYx = AfxGetYx( wBusNo , wAddr , wPnt ) ;
    if( !pYx )
        return FALSE ;

    byVal = pYx->byValue ;
    return TRUE ;
}

BOOL CPublicMethod::SetYm( WORD wBusNo , WORD wAddr , WORD wPnt , DWORD dwVal )
{
    return AfxSetYm( wBusNo , wAddr , wPnt , dwVal ) ;
}

BOOL CPublicMethod::GetYm( WORD wBusNo , WORD wAddr , WORD wPnt , DWORD &dwVal )
{
    PYMST pYm = AfxGetYm( wBusNo , wAddr , wPnt );
    if( !pYm )
        return FALSE ;

    dwVal = pYm->dVal_1Time ;

    return TRUE ;
}

BOOL CPublicMethod::SetDevCommState( WORD wBusNo , WORD wAddr ,BYTE byState )
{
    if( byState != COMM_ABNORMAL && byState != COMM_NOMAL )
    {
        Q_ASSERT( FALSE );
        return FALSE ;
    }

    PDEVST pDev = AfxGetDev( wBusNo ,wAddr ) ;
    if( !pDev )
    {
        Q_ASSERT( FALSE ) ;
        return FALSE ;
    }

    pDev->wDevState = byState ;
    return TRUE ;
}


BOOL CPublicMethod::GetDevCommState( WORD wBusNo , WORD wAddr , BYTE &byState )
{

    PDEVST pDev = AfxGetDev( wBusNo ,wAddr ) ;
    if( !pDev )
    {
        Q_ASSERT( FALSE ) ;
        return FALSE ;
    }

    byState = pDev->wDevState ;
    return TRUE ;
}

BOOL CPublicMethod::SengYK( WORD wBusNoDest , WORD wAddrDest , WORD nPntDest ,
                            WORD wBusNoSrc , WORD wAddrSrc , WORD nPntSrc ,
                            BYTE byAction , WORD wVal )
{
    return AfxSengYK( wBusNoDest , wAddrDest , nPntDest ,
                      wBusNoSrc , wAddrSrc , nPntSrc ,
                      byAction , wVal );
}

BOOL CPublicMethod::SetSoe( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE byVal ,void * pRealTime)
{
    REALTIME * tmpRealTime = (REALTIME *)pRealTime;

    return AfxSetSoe( wBusNo , wAddr , wPnt , byVal , tmpRealTime );
}

BOOL CPublicMethod::GetSysTime(void *pRealTime)
{
    REALTIME * tmpRealTime = (REALTIME *)pRealTime;

    return AfxGetSysTime(tmpRealTime);
}

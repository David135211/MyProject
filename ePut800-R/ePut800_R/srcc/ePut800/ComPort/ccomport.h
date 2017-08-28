#ifndef CCOMPORT_H
#define CCOMPORT_H
#include "../Typedef.h"
#define ERROR      -1

struct Property ;

class CComPort
{
public:
    CComPort();
    virtual ~CComPort();

    virtual BOOL   IsPortValid( void ) ;
    virtual BOOL   OpenPort( char* lpszError=NULL )  ;
    virtual void   ClosePort( void )  ;
    virtual int	   ReadPort( BYTE *pBuf , int nRead );
    virtual int	   WritePort( BYTE *pBuf  , int nWrite  );
    virtual int	   AsyReadData( BYTE *pBuf  , int nRead  ) ;
    virtual int    AsySendData( BYTE *pBuf , int nWrite );
    virtual BOOL   SetPreOpenPara( Property * pProperty );
};

#endif // CCOMPORT_H

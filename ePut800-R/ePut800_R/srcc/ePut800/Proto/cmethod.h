#ifndef CMETHOD_H
#define CMETHOD_H

//定义数据类型
typedef unsigned char BYTE ;
typedef unsigned short int WORD ;
typedef unsigned long DWORD ;
typedef bool          BOOL ;
typedef int           HANDLE ;
typedef void *        LPVOID ;

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD)(b)) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)(((DWORD)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD)(w)) >> 8) & 0xff))
#define UINT8         BYTE
#define UINT16        WORD
#define FALSE         false
#define TRUE          true

class CMethod
{
public:
    CMethod();
    virtual BOOL SetYc( WORD wBusNo , WORD wAddr , WORD wPnt , float fVal ){ return FALSE ;}
    virtual BOOL GetYc( WORD wBusNo , WORD wAddr , WORD wPnt , float &fVal ){ return FALSE ;}

    virtual BOOL SetYx( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE bVal ){ return FALSE ;}
    virtual BOOL GetYx( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE &bVal ){ return FALSE ;}
    virtual BOOL SetYm( WORD wBusNo , WORD wAddr , WORD wPnt , DWORD dwVal ){ return FALSE ;}
    virtual BOOL GetYm( WORD wBusNo , WORD wAddr , WORD wPnt , DWORD &dwVal ){ return FALSE ;}

    virtual BOOL SetDevCommState( WORD wBusNo , WORD wAddr, BYTE byState ){ return FALSE ;}
    virtual BOOL GetDevCommState( WORD wBusNo , WORD wAddr,BYTE &byState ){ return FALSE ;}
    virtual BOOL SengYK( WORD wBusNoDest , WORD wAddrDest , WORD nPntDest ,
                            WORD wBusNoSrc , WORD wAddrSrc , WORD nPntSrc ,
                            BYTE byAction , WORD wVal ){ return FALSE ;}
    virtual BOOL SetSoe( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE byVal ,void * pRealTime){ return FALSE ;}
    virtual BOOL GetSysTime(void *pRealTime){ return FALSE ;}

};

#endif // CMETHOD_H

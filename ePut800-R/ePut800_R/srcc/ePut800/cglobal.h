#ifndef CGLOBAL_H
#define CGLOBAL_H
#include <QObject>

#include <stdlib.h>
#include <linux/input.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Typedef.h"
#include "widget.h"
#include "structure.h"


#define  UTF8( str ) QString::fromUtf8( str )
#define  UTF8_LEN( str , len ) QString::fromUtf8( str , len );

#define PASUE       	"PAUSE"
#define COMRS_485	"COMRS485_"
#define LAN_TCP     	"TCP_"
#define LAN_TCP_CLIENT	"TCPCLIENT_"

#define	GPIO_GETMU	_IOWR('G', 0x3A, int)
#define	GPIO_GETOS	_IOWR('G', 0x3B, int)
#define	GPIO_GETBF	_IOWR('G', 0x3C, int)
#define	GPIO_GETBU	_IOWR('G', 0x3D, int)
#define	GPIO_GETMW	_IOWR('G', 0x3E, int)
#define	GPIO_GETBW	_IOWR('G', 0x3F, int)

#define GPIN "/dev/gpin"
#define GPOUT "/dev/gpout"

#define GPIOON 0
#define GPIOOFF 1



class Widget ;			//原来是widget	of mine!
class CPage ;
class CGlobal ;

CPage * AfxGetSpecifyPage( BYTE byPageIndex ) ;
BOOL AfxSetMainWidet( Widget * pWidget );
Widget * AfxGetWidget( );
BYTE AfxGetCurPageIndex( );
BOOL AfxSetCurPageIndex( BYTE byIndex );
void AfxRefreshScreen( );
BOOL AfxGetSysTime( PREALTIME pTime );
BOOL AfxSetSysTime(REALTIME *pRealTime);
QString AfxFormatStr( const char * pString , ... );
QString AfxGetSysTimeStr( );
CGlobal * AfxGetGlobal( ) ;
void AfxAsleep( DWORD dwMilliSecd );
BYTE AfxGetLoginState( );
void AfxSetLoginState( BYTE byLoginState );
 void AfxRefreshLoginState( ) ;
CPage * AfxGetCurPagePointer( );
BOOL AfxSendMsg( CMsg * pMsg ,LPVOID pVoid );
void AfxUpdateTimeoutTime( );

BOOL AfxReadPwd( );
BOOL AfxWritePwd( PUSRPWD VevisePwd );
BOOL AfxResetPwd( );

BOOL AfxReadNetParamFile( );
BOOL AfxWriteNetParamFile( PNETPARAM VeviseNetParam );
BOOL AfxSetNetParam ( PNETPARAM VeviseNetParam );

QBUS_ARRAY * AfxGetBusArray() ;
PBUS_LINEST AfxGetBus( WORD wBusNo );
PDEVST AfxGetDev( WORD wBusNo , WORD wAddr ) ;
PYCST AfxGetYc( WORD wBusNo , WORD wAddr , WORD wPnt ) ;
PYXST AfxGetYx( WORD wBusNo , WORD wAddr , WORD wPnt ) ;
PYMST AfxGetYm( WORD wBusNo , WORD wAddr , WORD wPnt ) ;
template< class DTYPE >
DTYPE * AfxGetData( QDEV_DATA_ARRAY * pArray , WORD wPnt );
PDataType AfxGetData( BYTE byDataType , WORD wBusNo , WORD wAddr , WORD wPnt );
const QCAB_ARRAY &AfxGetCabinetArray( ) ;
const QSCIRCUIT_ARRAY &AfxGetSCircuitArray( );
const QDEV_DATA_ARRAY* AfxGetCircuitData( WORD wCircuitNo , DATETYPE byType );
const QDEV_DATA_ARRAY* AfxGetCabCircuitData( WORD wCab , WORD wCircuitNo , DATETYPE byType ) ;
PSINGLECIRCUIT AfxGetCabSingleCircuit( WORD wCab , WORD wCircuitNo );
PSINGLECIRCUIT AfxGetSingleCircuit( WORD wCircuitNo );
PCABINET AfxGetCabinet( WORD wCabNo );
BOOL AfxComPortProcess( PBUS_LINEST pBus , PSMSG pMsg = NULL );
CProtocol * AfxGetDevProto( CProtocol *pBusProto ) ;
BOOL AfxSetYc( WORD wBusNo , WORD wAddr , WORD wPnt , float fVal );
BOOL AfxSetYx( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE byVal );
BOOL AfxSetYm( WORD wBusNo , WORD wAddr , WORD wPnt , DWORD dwVal );
BOOL AfxGetYc( WORD wBusNo , WORD wAddr , WORD wPnt , float &fVal );
BOOL AfxGetYx( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE &byVal );
BOOL AfxGetYm( WORD wBusNo , WORD wAddr , WORD wPnt , double &dVal );
BOOL AfxSetBusCommState( WORD wBusNo , WORD wState );
BOOL AfxGetBusCommState( WORD wBusNo , WORD &wState );
BOOL AfxSetDevCommState( WORD wBusNo , WORD wAddr , WORD wState );
BOOL AfxGetDevCommState( WORD wBusNo , WORD wAddr , WORD &wState );
void AfxDisplayMsg( WORD wBusNo , WORD wAddr , BYTE byType , BYTE * pBuf , int len );
PYKST AfxGetYk( WORD wBusNo , WORD wAddr , WORD wPnt );
BOOL AfxSengYK( WORD wBusNo , WORD wAddr , WORD nPnt , BYTE byAction , WORD wVal );
BOOL AfxSengYK( WORD wBusNoDest , WORD wAddrDest , WORD nPntDest ,
                WORD wBusNoSrc , WORD wAddrSrc , WORD nPntSrc ,
                BYTE byAction , WORD wVal );
BOOL AfxSetSoe( WORD wBusNo , WORD wAddr , WORD wPnt , BYTE byVal ,REALTIME * pRealTime);
BOOL AfxComPortSpecify( PSMSG pMsg );
BOOL AfxProcessYk( PYKST_MSG pYk_msg );
CProtocol * AfxGetSpecifyProto( PSMSG _pMsg );

//refresh Home page Time
BOOL AfxRefreshHomePageTime( );

//refresh Bus And Dev State
BOOL AfxRefreshDiagnosticPageBusAndDevState( );

BOOL AfxSetBusTransClientState( PBUS_LINEST pBus );

void AfxGetCurNetPara( NETPARAM * pNetParam );

//thread assosiated func
PSTHREAD_PROC AfxGetThreadInfo( pthread_t tid ) ;


void AfxGetProjectName( QString & str ) ;
// 获取数据库
CDBaseManager *AfxGetDBase();

//thread function
void * ThreadRealProc( void * pParam ) ;
void * ThreadComProc( void * pParam );

void ltrim(char *s);
void rtrim(char *s);
BOOL AfxGetSysNetParamAndSetDefaultNetParam( PNETPARAM VeviseNetParam );

void m_VectorTcpClientPush_back( CComPort * pPort ) ;
void ScanServerOnLine( );
void* ThreadScanSever(void *pProtObj );
//刷新指示灯状态
void AfxRefreshGpioState();
//数据查询刷新实时状态
BOOL AfxRefreshDataQueryState();
//屏幕刷新实时状态
BOOL AfxRefreshScreenState();
void AfxSetStandby( BOOL bstate);

void AfxRefreshYkTime();
//刷新动态着色
BOOL AfxRefreshPicColor(PST_SYSPAGE CurPagest ,CElement * FElement);

BOOL InsertYxAlarmDbase( PYXST pYxData );

class CGlobal : public QObject
{
    Q_OBJECT
public:
    CGlobal();

public:
    enum{ ELAPSE_TIMEOUT = 10 * 60 } ;//unit sec
    enum{ SEND_MSG = 0x21 ,RECV_MSG , ERROR_MSG } ;
    enum{ BUFFERSIZE = 1024 * 3 };
    enum{ FAULT=2,WARN,MAIN_POWER,WORKING,BACKUP_POWER,GPIO_ERROR };

    Widget * m_pWidget ;
    BYTE m_byCurPage ;
    QString m_strRealTime ;
    QString GetRealTimeStr( ) ;
    void SetRealTimeStr( QString str ) ;
    BOOL GetThreadRunFlag( ) ;
    BOOL SetThreadRunFlag( BOOL bFlag ) ;
    BYTE GetLoginState( );
    void SetLoginState( BYTE byState ) ;
    time_t GetCountdownTime( ) ;
    void   SetCountdownTime( time_t timeCount ) ;
    char UsrPwdPath[ 32 ];
    USRPWD DefaultUsrPwd;
    USRPWD CurrentUsrPwd;
    char NetParamPath[ 32 ];
    NETPARAM DefaultNetParam;
    NETPARAM CurrentNetParam;
    char SysDnsPath[ 32 ];
    QVector <CComPort*> m_VectorTcpClient;
    int m_GpinFd;
    int m_GpoutFd;
    int GpinCmd[6];
    BYTE GpioState[7];
    BOOL s_bstate; // static 的待机状态
    int keys_fd;
    struct input_event t;
    unsigned int gs_uisum_usec;  // 等待的最长时间
    unsigned int gs_uiadd_usec;                 // 每次增加时间在此变量上
    unsigned int gs_uisleep_usec;      // 睡眠时间

    time_t YkBeginTime;

    void RefreshGpioPowerState();
    void RefreshGpioFalutState();
    void RefreshGpioWorkingState();
    void RefreshGpioWarnState();
    void AfxSetGpioState( BYTE index , BYTE State);
    void SetStandby( BOOL bstate );
    void RefreshScreenState();

    void EmitSignal_YkRtnInRealThread( WORD wBusNoDest , WORD wAddrDest , WORD nPntDest ,
                                       WORD wBusNoSrc , WORD wAddrSrc , WORD nPntSrc ,
                                       BYTE byAction , WORD wVal );
    void EmitSignal_YkRtnTimeOut();
    void EmitSignal_YxAlarm( char* chName, char byValue );

protected:
    BOOL m_bThreadRun ;
    BYTE m_byLogState ;
    time_t m_wCountdownTime ;

signals:
    void LoginStateChanged( BYTE byState );
    void Signal_YkRtnInRealThread(unsigned short wBusNoDest, unsigned short wAddrDest, unsigned short nPntDest, unsigned short wBusNoSrc, unsigned short wAddrSrc, unsigned short nPntSrc, unsigned char byAction, unsigned short wVal);
    void Signal_YkRtnTimeOut();
    void Signal_YxAlarm( char* chName, char byValue );
};

#endif // CGLOBAL_H

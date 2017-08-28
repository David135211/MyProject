#ifndef CPAGE_DIAGNOSTIC_H
#define CPAGE_DIAGNOSTIC_H
#include "cpage.h"

typedef struct DevAddrDataOfBus
{
    WORD BusNo;
    WORD DevNum;
    WORD DevAddr[32];
    DevAddrDataOfBus()
    {
        BusNo = 0;
        DevNum = 0;
        memset(DevAddr,0,sizeof(DevAddr));
    }
}DEVADDRDATAOFBUS,*PDEVADDRDATAOFBUS;

typedef struct BusNoData
{
    WORD BusNum;
    WORD BusTransNum;
    WORD BusTransNo[22];
    DEVADDRDATAOFBUS DevAddrOfBus[22];
    BusNoData()
    {
        BusNum = 0;
        BusTransNum = 0;
        memset(DevAddrOfBus,0,sizeof(BusTransNo));
        memset(DevAddrOfBus,0,sizeof(DevAddrOfBus));
    }
}BUSNODATA,*PBUSNODATA;


class CPage_Diagnostic : public CPage
{
    Q_OBJECT
public:
    CPage_Diagnostic( BYTE byType );
    virtual BOOL AddData( LPVOID pVoid ) ;
    virtual BOOL DrawPage( QPainter * pPainter );
    virtual BOOL mousePressEvent(QMouseEvent *pEvent );
    BOOL DisplayMsg( WORD wBusNo , WORD wAddr , BYTE byType , BYTE * pBuf , int len ) ;

    QRect DevStateRect;
    QRect RxTxRect;
    BYTE DiagnosticOption;
    enum{DATAGRAM_PAGE,COMMUNINICATION_STATE_PAGE};
private:
    QTextEdit * MessageTextEdit;
    QComboBox * BusNoComboBox;
    QComboBox * DevAddrComboBox;
    BUSNODATA sBusNoData;

    time_t TimeBegin;
    QString m_str;


    enum{DATAGRAM_RECT,COMMUNINICATION_STATE_RECT,CHANNEL_TEXT_RECT,CHANNEL_RECT,
        DEVADDR_TEXT_RECT,DEVADDR_RECT,CHECK_RECT,HEX_OR_ASCII,START_OR_END_RECT,
        TX_RECT,RX_RECT};
    enum{HEX_FOEMAT,ASCII_FOEMAT};

    char DatagramReleasePath[64];
    char DatagramPressPath[64];
    char CommunStateReleasePath[64];
    char CommunStatePressPath[64];

    BYTE ChannelNoAllNum;
    BYTE DevAddressAllNum;

    BOOL CheckState;
    BOOL MessageShow;
    BYTE MessageFormat;
    BYTE ChannelNo;
    BYTE DevAddress;
    QRect DiagnosticPageRect[11];
    BOOL ChannelNoRectShow;
    BOOL DevAddressRectShow;
    BOOL CheckStateRectShow;
    BOOL MessageShowRectShow;
    BOOL MessageFormatRectShow;

    BOOL getBusNoAndDevAddr();
    BOOL DrawDiagnosticPage( QPainter * pPainter );
    BOOL DrawDiagnosticOption( QPainter * pPainter );
    BOOL DrawDatagram( QPainter * pPainter );
    BOOL DrawCommunState( QPainter * pPainter );

    BOOL MousePressChoicePage( QMouseEvent *pEvent );
    BOOL MousePressDatagram( QMouseEvent *pEvent );
    BOOL MouseCommunState( QMouseEvent *pEvent );

signals:
    void signal_MessageShow(QString str);

private slots:
    void slots_MessageShow(QString str);
    void slots_BusNoChange(int index);
    void slots_DevAddrChange(int index);

};

#endif // CPAGE_DIAGNOSTIC_H

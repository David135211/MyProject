#ifndef CPAGE_SYSPARAMETER_H
#define CPAGE_SYSPARAMETER_H
#include "cpage.h"

class CPage_SysParameter : public CPage
{
    Q_OBJECT
public:
    CPage_SysParameter(BYTE byType );
    virtual BOOL AddData( LPVOID pVoid ) ;
    virtual BOOL DrawPage( QPainter * pPainter );
    virtual BOOL mousePressEvent(QMouseEvent *pEvent );

    enum{CURRENT_PAGENAME_RECT,NET_PARAM_RECT,SYS_TIME_RECT,LOGIN_PWD_RECT,
         IP_INPUT_RECT,MASK_INPUT_RECT,GATEWAY_INPUT_RECT,DNS_INPUT_RECT,
         YEAR_INPUT_RECT,MONTH_INPUT_RECT,DAY_INPUT_RECT,HOUR_INPUT_RECT,
         MINUTE_INPUT_RECT,SECOND_INPUT_RECT,OLD_PWD_INPUT_RECT,NEW_PWD_INPUT_RECT,
         CONFIRM_PWD_INPUT_RECT,YES_RECT,NO_RECT};
    enum{NET_PARAM,SYS_TIME,LOGIN_PWD};

    BYTE SysParamOption;
    BYTE Is_Refresh_SysParamTime;
    QRect SysParamPageRect[19];
    QString SysParamPageRectText[19];

private:


    QLineEdit * KeyLineEdit;

    BOOL DrawSysParamPage( QPainter * pPainter );
    BOOL DrawSysParamOption( QPainter * pPainter );
    BOOL DrawNetParam( QPainter * pPainter );
    BOOL DrawSysTime( QPainter * pPainter );
    BOOL DrawLoginPwd( QPainter * pPainter );

    BOOL MousePressChoicePage( QMouseEvent *pEvent );
    BOOL MousePressNetParam( QMouseEvent *pEvent );
    BOOL MousePressSysTime( QMouseEvent *pEvent );
    BOOL MousePressLoginPwd( QMouseEvent *pEvent );

    BOOL is_valid_ip(const char *ip);
    BOOL netIpIsValid(DWORD IP);
    BOOL netMaskIsValid(DWORD mask);
    BOOL netMaskAndIpIsValid(DWORD IP, DWORD mask);
    BOOL Is_valid_NetParam(QString ip, QString subIP, QString mask , QString dns);
    BOOL Is_valid_SysTime(QString Year, QString Month, QString Day ,
                          QString Hour, QString Minute, QString Second);
    BOOL Is_valid_LoginPwd(QString OldPwd, QString NewPwd, QString ConfirmPwd);

    int NumBoardFocusRect;
    int LastNumBoardFocusRect;

    QString SysParamPageRectPreviousText[19];
    char NetParamReleasePath[64];
    char NetParamPressPath[64];
    char SysTimeReleasePath[64];
    char SysTimePressPath[64];
    char LoginPwdReleasePath[64];
    char LoginPwdPressPath[64];

    BYTE LoginState_SysParam;
public slots:
    void TextChangeFocusRect( QString text );
    void LoginStateChange_SysParam( BYTE byState );
    void NumBoardHide();
    void NumBoardShow();
};

#endif // CPAGE_SYSPARAMETER_H

#include "cpage_sysparameter.h"
#include "cglobal.h"

#include <arpa/inet.h>

CPage_SysParameter::CPage_SysParameter( BYTE byType ):CPage( byType )
{
    m_strPic += "ePut800-SysParam.jpg";
    SysParamOption = NET_PARAM;
    memset( NetParamReleasePath , 0 , sizeof(NetParamReleasePath) );
    memcpy( NetParamReleasePath , "/usr/share/ePut800/Pics/灰底-按钮白-网络参数.jpg" , sizeof("/usr/share/ePut800/Pics/灰底-按钮白-网络参数.jpg"));
    memset( NetParamPressPath , 0 , sizeof(NetParamPressPath) );
    memcpy( NetParamPressPath , "/usr/share/ePut800/Pics/灰底-按钮蓝-网络参数.jpg" , sizeof("/usr/share/ePut800/Pics/灰底-按钮蓝-网络参数.jpg"));

    memset( SysTimeReleasePath , 0 , sizeof(SysTimeReleasePath) );
    memcpy( SysTimeReleasePath , "/usr/share/ePut800/Pics/灰底-按钮白-系统时间.jpg" , sizeof("/usr/share/ePut800/Pics/灰底-按钮白-系统时间.jpg"));
    memset( SysTimePressPath , 0 , sizeof(SysTimePressPath) );
    memcpy( SysTimePressPath , "/usr/share/ePut800/Pics/灰底-按钮蓝-系统时间.jpg" , sizeof("/usr/share/ePut800/Pics/灰底-按钮蓝-系统时间.jpg"));

    memset( LoginPwdReleasePath , 0 , sizeof(LoginPwdReleasePath) );
    memcpy( LoginPwdReleasePath , "/usr/share/ePut800/Pics/灰底-按钮白-登录密码.jpg" , sizeof("/usr/share/ePut800/Pics/灰底-按钮白-登录密码.jpg"));
    memset( LoginPwdPressPath , 0 , sizeof(LoginPwdPressPath) );
    memcpy( LoginPwdPressPath , "/usr/share/ePut800/Pics/灰底-按钮蓝-登录密码.jpg" , sizeof("/usr/share/ePut800/Pics/灰底-按钮蓝-登录密码.jpg"));

    SysParamPageRect[CURRENT_PAGENAME_RECT].setRect(150,30, 100, 30);
    SysParamPageRect[NET_PARAM_RECT].setRect(40 , 113 , 106 , 51);
    SysParamPageRect[SYS_TIME_RECT].setRect(40 , 183 , 106 , 51);
    SysParamPageRect[LOGIN_PWD_RECT].setRect(40 , 253 , 106 , 51);
    SysParamPageRect[IP_INPUT_RECT].setRect(360,160, 300, 40);
    SysParamPageRect[MASK_INPUT_RECT].setRect(360,225, 300, 40);
    SysParamPageRect[GATEWAY_INPUT_RECT].setRect(360,290, 300, 40);
    SysParamPageRect[DNS_INPUT_RECT].setRect(360,355, 300, 40);
    SysParamPageRect[YES_RECT].setRect(280,420, 95, 45);
    SysParamPageRect[NO_RECT].setRect(545,420, 95, 45);

    SysParamPageRect[YEAR_INPUT_RECT].setRect(230 , 190 , 130 , 45);
    SysParamPageRect[MONTH_INPUT_RECT].setRect(390 , 190 , 130 , 45);
    SysParamPageRect[DAY_INPUT_RECT].setRect(550 , 190 , 130 , 45);
    SysParamPageRect[HOUR_INPUT_RECT].setRect(230 , 300 , 130 , 45);
    SysParamPageRect[MINUTE_INPUT_RECT].setRect(390 , 300 , 130 , 45);
    SysParamPageRect[SECOND_INPUT_RECT].setRect(550 , 300 , 130 , 45);

    SysParamPageRect[OLD_PWD_INPUT_RECT].setRect(360,170, 300, 40);
    SysParamPageRect[NEW_PWD_INPUT_RECT].setRect(360,250, 300, 40);
    SysParamPageRect[CONFIRM_PWD_INPUT_RECT].setRect(360,330, 300, 40);

    SysParamPageRectText[IP_INPUT_RECT] =  QString(QLatin1String(AfxGetGlobal()->CurrentNetParam.ip));
    SysParamPageRectText[MASK_INPUT_RECT] = QString(QLatin1String(AfxGetGlobal()->CurrentNetParam.mask));
    SysParamPageRectText[GATEWAY_INPUT_RECT] = QString(QLatin1String(AfxGetGlobal()->CurrentNetParam.gateway));
    SysParamPageRectText[DNS_INPUT_RECT] = QString(QLatin1String(AfxGetGlobal()->CurrentNetParam.dns));
    SysParamPageRectPreviousText[IP_INPUT_RECT] =  QString(QLatin1String(AfxGetGlobal()->CurrentNetParam.ip));
    SysParamPageRectPreviousText[MASK_INPUT_RECT] = QString(QLatin1String(AfxGetGlobal()->CurrentNetParam.mask));
    SysParamPageRectPreviousText[GATEWAY_INPUT_RECT] = QString(QLatin1String(AfxGetGlobal()->CurrentNetParam.gateway));
    SysParamPageRectPreviousText[DNS_INPUT_RECT] = QString(QLatin1String(AfxGetGlobal()->CurrentNetParam.dns));

    KeyLineEdit = new QLineEdit;
    NumBoardFocusRect = -1;
    LastNumBoardFocusRect = -1;
    Is_Refresh_SysParamTime = TRUE;
    LoginState_SysParam = AfxGetLoginState();
    connect(KeyLineEdit , SIGNAL(textChanged(QString)) , this , SLOT(TextChangeFocusRect(QString)));
    connect(AfxGetGlobal() , SIGNAL(LoginStateChanged(BYTE)) , this , SLOT(LoginStateChange_SysParam(BYTE)));
}

BOOL CPage_SysParameter::AddData( LPVOID pVoid )
{
    connect(AfxGetWidget()->Numkeyboard , SIGNAL(hide_signal()) , this , SLOT(NumBoardHide()));
    connect(AfxGetWidget()->Numkeyboard , SIGNAL(show_signal()) , this , SLOT(NumBoardShow()));
    return FALSE ;
}

BOOL CPage_SysParameter::DrawPage( QPainter * pPainter )
{
    CPage::DrawPage( pPainter ) ;

    //connect(AfxGetWidget()->Numkeyboard , SIGNAL(hide_signal()) , this , SLOT(NumBoardHide()));
    //connect(AfxGetWidget()->Numkeyboard , SIGNAL(show_signal()) , this , SLOT(NumBoardShow()));
    DrawSysParamPage( pPainter ) ;

    return FALSE ;
}

BOOL CPage_SysParameter::DrawSysParamPage( QPainter * pPainter )
{
    BOOL Ret = FALSE;
    DrawSysParamOption(pPainter);
    if( LoginState_SysParam == CPage::LOGIN )
    {
        switch( SysParamOption )
        {
        case NET_PARAM:
            Ret = DrawNetParam(pPainter);
        break;
        case SYS_TIME:
            Ret = DrawSysTime(pPainter);
        break;
        case LOGIN_PWD:
            Ret = DrawLoginPwd(pPainter);
        break;
        default:
            Ret = DrawNetParam(pPainter);
        break;
        }
    }
    else
    {
        pPainter->save();
        QPen pen = pPainter->pen();
        pen.setColor(QColor(255,255,255,255));
        pPainter->setPen(pen);
        QFont font = pPainter->font();
        font.setPixelSize(22);
        pPainter->setFont(font);
        pPainter->drawText(QRect(250,200,400,200),Qt::AlignCenter,UTF8("尚未登录，无权限操作该页面！！"));
        pPainter->restore();
    }
    return Ret;
}

BOOL CPage_SysParameter::DrawSysParamOption( QPainter * pPainter )
{
    pPainter->save();
    QPen pen = pPainter->pen();
    pen.setColor(QColor(255,255,255,255));
    pPainter->setPen(pen);
    QFont font = pPainter->font();
    font.setPixelSize(22);
    pPainter->setFont(font);
    switch( SysParamOption )
    {
    case NET_PARAM:
        pPainter->drawText(SysParamPageRect[CURRENT_PAGENAME_RECT] ,UTF8("网络参数"));
        pPainter->drawPixmap( SysParamPageRect[NET_PARAM_RECT] , UTF8(NetParamPressPath) );
        pPainter->drawPixmap( SysParamPageRect[SYS_TIME_RECT] , UTF8(SysTimeReleasePath) );
        pPainter->drawPixmap( SysParamPageRect[LOGIN_PWD_RECT] , UTF8(LoginPwdReleasePath) );
    break;
    case SYS_TIME:
        pPainter->drawText(SysParamPageRect[CURRENT_PAGENAME_RECT] ,UTF8("系统时间"));
        pPainter->drawPixmap( SysParamPageRect[NET_PARAM_RECT] , UTF8(NetParamReleasePath) );
        pPainter->drawPixmap( SysParamPageRect[SYS_TIME_RECT] , UTF8(SysTimePressPath) );
        pPainter->drawPixmap( SysParamPageRect[LOGIN_PWD_RECT] , UTF8(LoginPwdReleasePath) );
    break;
    case LOGIN_PWD:
        pPainter->drawText(SysParamPageRect[CURRENT_PAGENAME_RECT] ,UTF8("登录密码"));
        pPainter->drawPixmap( SysParamPageRect[NET_PARAM_RECT] , UTF8(NetParamReleasePath) );
        pPainter->drawPixmap( SysParamPageRect[SYS_TIME_RECT] , UTF8(SysTimeReleasePath) );
        pPainter->drawPixmap( SysParamPageRect[LOGIN_PWD_RECT] , UTF8(LoginPwdPressPath) );
    break;
    default:
        pPainter->drawText(SysParamPageRect[CURRENT_PAGENAME_RECT] ,UTF8("网络参数"));
        pPainter->drawPixmap( SysParamPageRect[NET_PARAM_RECT] , UTF8(NetParamPressPath) );
        pPainter->drawPixmap( SysParamPageRect[SYS_TIME_RECT] , UTF8(SysTimeReleasePath) );
        pPainter->drawPixmap( SysParamPageRect[LOGIN_PWD_RECT] , UTF8(LoginPwdReleasePath) );
    break;
    }
    pPainter->restore();
    return TRUE;
}

BOOL CPage_SysParameter::DrawNetParam( QPainter * pPainter )
{
    pPainter->save();

    QPen pen = pPainter->pen();
    pen.setColor(QColor(255,255,255,255));
    pPainter->setPen(pen);
    QFont font = pPainter->font();
    font.setPixelSize(22);
    pPainter->setFont(font);
    pPainter->drawText(SysParamPageRect[CURRENT_PAGENAME_RECT] ,UTF8("网络参数"));

    pPainter->drawPixmap( SysParamPageRect[NET_PARAM_RECT] , UTF8(NetParamPressPath) );
    pPainter->drawPixmap( SysParamPageRect[SYS_TIME_RECT] , UTF8(SysTimeReleasePath) );
    pPainter->drawPixmap( SysParamPageRect[LOGIN_PWD_RECT] , UTF8(LoginPwdReleasePath) );

    pPainter->drawText(QRect(250,160, 108, 40) ,Qt::AlignCenter,UTF8("IP地址："));
    pPainter->drawText(QRect(250,225, 108, 40) ,Qt::AlignCenter,UTF8("子网掩码:"));
    pPainter->drawText(QRect(250,290, 108, 40) ,Qt::AlignCenter,UTF8("网关："));
    pPainter->drawText(QRect(250,355, 108, 40) ,Qt::AlignCenter,UTF8("DNS："));

    pen.setColor(QColor(0x3c,0xb3,0x71,255));
    pen.setWidth(3);
    pPainter->setPen(pen);

    pPainter->setBrush(Qt::white);
    pPainter->drawRect(SysParamPageRect[IP_INPUT_RECT]);
    pPainter->drawText(SysParamPageRect[IP_INPUT_RECT] ,Qt::AlignCenter,SysParamPageRectText[IP_INPUT_RECT]);
    pPainter->drawRect(SysParamPageRect[MASK_INPUT_RECT]);
    pPainter->drawText(SysParamPageRect[MASK_INPUT_RECT] ,Qt::AlignCenter,SysParamPageRectText[MASK_INPUT_RECT]);
    pPainter->drawRect(SysParamPageRect[GATEWAY_INPUT_RECT]);
    pPainter->drawText(SysParamPageRect[GATEWAY_INPUT_RECT] ,Qt::AlignCenter,SysParamPageRectText[GATEWAY_INPUT_RECT]);
    pPainter->drawRect(SysParamPageRect[DNS_INPUT_RECT]);
    pPainter->drawText(SysParamPageRect[DNS_INPUT_RECT] ,Qt::AlignCenter,SysParamPageRectText[DNS_INPUT_RECT]);

    pPainter->setBrush(Qt::black);
    pPainter->drawRoundedRect(SysParamPageRect[YES_RECT],5,5);
    pPainter->drawRoundedRect(SysParamPageRect[NO_RECT],5,5);

    //pen.setColor(QColor(0xCD,0x68,0x39,255));
    pen.setColor(QColor(0xCD,0x66,0x00,255));
    pPainter->setPen(pen);
    pPainter->drawText(SysParamPageRect[YES_RECT] ,Qt::AlignCenter,UTF8("确定"));
    pPainter->drawText(SysParamPageRect[NO_RECT] ,Qt::AlignCenter,UTF8("取消"));

    if( NumBoardFocusRect != -1 )
    {
        pPainter->setBrush(Qt::white);
        pen.setColor(QColor(255,0,0,255));
        pen.setWidth(3);
        pPainter->setPen(pen);
        pPainter->drawRect(SysParamPageRect[NumBoardFocusRect]);
        pPainter->drawText(SysParamPageRect[NumBoardFocusRect] ,Qt::AlignCenter,SysParamPageRectText[NumBoardFocusRect]);
    }
    if( LastNumBoardFocusRect != -1 )
    {
        pPainter->setBrush(Qt::white);
        pen.setColor(QColor(0x3c,0xb3,0x71,255));
        pen.setWidth(3);
        pPainter->setPen(pen);
        pPainter->drawRect(SysParamPageRect[LastNumBoardFocusRect]);
        pPainter->drawText(SysParamPageRect[LastNumBoardFocusRect] ,Qt::AlignCenter,SysParamPageRectText[LastNumBoardFocusRect]);
    }
    pPainter->restore();
    return TRUE;
}

BOOL CPage_SysParameter::DrawSysTime( QPainter * pPainter )
{
    pPainter->save();

    QPen pen = pPainter->pen();
    pen.setColor(QColor(255,255,255,255));
    pPainter->setPen(pen);
    QFont font = pPainter->font();
    font.setPixelSize(22);
    pPainter->setFont(font);
    pPainter->drawText(SysParamPageRect[CURRENT_PAGENAME_RECT] ,UTF8("系统时间"));

    pPainter->drawPixmap( SysParamPageRect[NET_PARAM_RECT] , UTF8(NetParamReleasePath) );
    pPainter->drawPixmap( SysParamPageRect[SYS_TIME_RECT] , UTF8(SysTimePressPath) );
    pPainter->drawPixmap( SysParamPageRect[LOGIN_PWD_RECT] , UTF8(LoginPwdReleasePath) );

    pen.setColor(QColor(0x3c,0xb3,0x71,255));
    pen.setWidth(3);
    pPainter->setPen(pen);

    pPainter->drawText(QRect(230 , 145 , 130 , 45) ,Qt::AlignCenter,UTF8("年"));
    pPainter->drawText(QRect(390 , 145 , 130 , 45) ,Qt::AlignCenter,UTF8("月"));
    pPainter->drawText(QRect(550 , 145 , 130 , 45) ,Qt::AlignCenter,UTF8("日"));
    pPainter->drawText(QRect(230 , 255 , 130 , 45) ,Qt::AlignCenter,UTF8("时"));
    pPainter->drawText(QRect(390 , 255 , 130 , 45) ,Qt::AlignCenter,UTF8("分"));
    pPainter->drawText(QRect(550 , 255 , 130 , 45) ,Qt::AlignCenter,UTF8("秒"));

    pPainter->setBrush(Qt::white);
    pPainter->drawRect(SysParamPageRect[YEAR_INPUT_RECT]);
    pPainter->drawText(SysParamPageRect[YEAR_INPUT_RECT] ,Qt::AlignCenter,SysParamPageRectText[YEAR_INPUT_RECT]);
    pPainter->drawRect(SysParamPageRect[MONTH_INPUT_RECT]);
    pPainter->drawText(SysParamPageRect[MONTH_INPUT_RECT] ,Qt::AlignCenter,SysParamPageRectText[MONTH_INPUT_RECT]);
    pPainter->drawRect(SysParamPageRect[DAY_INPUT_RECT]);
    pPainter->drawText(SysParamPageRect[DAY_INPUT_RECT] ,Qt::AlignCenter,SysParamPageRectText[DAY_INPUT_RECT]);
    pPainter->drawRect(SysParamPageRect[HOUR_INPUT_RECT]);
    pPainter->drawText(SysParamPageRect[HOUR_INPUT_RECT] ,Qt::AlignCenter,SysParamPageRectText[HOUR_INPUT_RECT]);
    pPainter->drawRect(SysParamPageRect[MINUTE_INPUT_RECT]);
    pPainter->drawText(SysParamPageRect[MINUTE_INPUT_RECT] ,Qt::AlignCenter,SysParamPageRectText[MINUTE_INPUT_RECT]);
    pPainter->drawRect(SysParamPageRect[SECOND_INPUT_RECT]);
    pPainter->drawText(SysParamPageRect[SECOND_INPUT_RECT] ,Qt::AlignCenter,SysParamPageRectText[SECOND_INPUT_RECT]);

    pPainter->setBrush(Qt::black);
    pPainter->drawRoundedRect(SysParamPageRect[YES_RECT],5,5);
    pPainter->drawRoundedRect(SysParamPageRect[NO_RECT],5,5);

    //pen.setColor(QColor(0xCD,0x68,0x39,255));
    pen.setColor(QColor(0xCD,0x66,0x00,255));
    pPainter->setPen(pen);
    pPainter->drawText(SysParamPageRect[YES_RECT] ,Qt::AlignCenter,UTF8("确定"));
    pPainter->drawText(SysParamPageRect[NO_RECT] ,Qt::AlignCenter,UTF8("取消"));

    if( NumBoardFocusRect != -1 )
    {
        pPainter->setBrush(Qt::white);
        pen.setColor(QColor(255,0,0,255));
        pen.setWidth(3);
        pPainter->setPen(pen);
        pPainter->drawRect(SysParamPageRect[NumBoardFocusRect]);
        pPainter->drawText(SysParamPageRect[NumBoardFocusRect] ,Qt::AlignCenter,SysParamPageRectText[NumBoardFocusRect]);
    }
    if( LastNumBoardFocusRect != -1 )
    {
        pPainter->setBrush(Qt::white);
        pen.setColor(QColor(0x3c,0xb3,0x71,255));
        pen.setWidth(3);
        pPainter->setPen(pen);
        pPainter->drawRect(SysParamPageRect[LastNumBoardFocusRect]);
        pPainter->drawText(SysParamPageRect[LastNumBoardFocusRect] ,Qt::AlignCenter,SysParamPageRectText[LastNumBoardFocusRect]);
    }
    pPainter->restore();
    return TRUE;
}

BOOL CPage_SysParameter::DrawLoginPwd( QPainter * pPainter )
{
    pPainter->save();
    QString tmpPassWord;
    int i = 0;

    QPen pen = pPainter->pen();
    pen.setColor(QColor(255,255,255,255));
    pPainter->setPen(pen);
    QFont font = pPainter->font();
    font.setPixelSize(22);
    pPainter->setFont(font);
    pPainter->drawText(SysParamPageRect[CURRENT_PAGENAME_RECT] ,UTF8("登录密码"));

    pPainter->drawPixmap( SysParamPageRect[NET_PARAM_RECT] , UTF8(NetParamReleasePath) );
    pPainter->drawPixmap( SysParamPageRect[SYS_TIME_RECT] , UTF8(SysTimeReleasePath) );
    pPainter->drawPixmap( SysParamPageRect[LOGIN_PWD_RECT] , UTF8(LoginPwdPressPath) );

    pPainter->drawText(QRect(250,170, 108, 40) ,Qt::AlignCenter,UTF8("原密码："));
    pPainter->drawText(QRect(250,250, 108, 40) ,Qt::AlignCenter,UTF8("新密码："));
    pPainter->drawText(QRect(250,330, 108, 40) ,Qt::AlignCenter,UTF8("确认密码："));

    pen.setColor(QColor(0x3c,0xb3,0x71,255));
    pen.setWidth(3);
    pPainter->setPen(pen);

    pPainter->setBrush(Qt::white);
    pPainter->drawRect(SysParamPageRect[OLD_PWD_INPUT_RECT]);
    if( SysParamPageRectText[OLD_PWD_INPUT_RECT].length() > 0 )
    {
        tmpPassWord = "";
        for( i=0; i<SysParamPageRectText[OLD_PWD_INPUT_RECT].length(); i++ )
            tmpPassWord += '*';
        pPainter->drawText(SysParamPageRect[OLD_PWD_INPUT_RECT] ,Qt::AlignCenter,tmpPassWord);
    }
    pPainter->drawRect(SysParamPageRect[NEW_PWD_INPUT_RECT]);
    if( SysParamPageRectText[NEW_PWD_INPUT_RECT].length() > 0 )
    {
        tmpPassWord = "";
        for( i=0; i<SysParamPageRectText[NEW_PWD_INPUT_RECT].length(); i++ )
            tmpPassWord += '*';
        pPainter->drawText(SysParamPageRect[NEW_PWD_INPUT_RECT] ,Qt::AlignCenter,tmpPassWord);
    }
    pPainter->drawRect(SysParamPageRect[CONFIRM_PWD_INPUT_RECT]);
    if( SysParamPageRectText[CONFIRM_PWD_INPUT_RECT].length() > 0 )
    {
        tmpPassWord = "";
        for( i=0; i<SysParamPageRectText[CONFIRM_PWD_INPUT_RECT].length(); i++ )
            tmpPassWord += '*';
        pPainter->drawText(SysParamPageRect[CONFIRM_PWD_INPUT_RECT] ,Qt::AlignCenter,tmpPassWord);
    }

    pPainter->setBrush(Qt::black);
    pPainter->drawRoundedRect(SysParamPageRect[YES_RECT],5,5);
    pPainter->drawRoundedRect(SysParamPageRect[NO_RECT],5,5);

    pen.setColor(QColor(0xCD,0x66,0x00,255));
    pPainter->setPen(pen);
    pPainter->drawText(SysParamPageRect[YES_RECT] ,Qt::AlignCenter,UTF8("确定"));
    pPainter->drawText(SysParamPageRect[NO_RECT] ,Qt::AlignCenter,UTF8("取消"));

    if( NumBoardFocusRect != -1 )
    {
        pPainter->setBrush(Qt::white);
        pen.setColor(QColor(255,0,0,255));
        pen.setWidth(3);
        pPainter->setPen(pen);
        pPainter->drawRect(SysParamPageRect[NumBoardFocusRect]);
        pPainter->drawText(SysParamPageRect[NumBoardFocusRect] ,Qt::AlignCenter,SysParamPageRectText[NumBoardFocusRect]);
    }
    if( LastNumBoardFocusRect != -1 )
    {
        pPainter->setBrush(Qt::white);
        pen.setColor(QColor(0x3c,0xb3,0x71,255));
        pen.setWidth(3);
        pPainter->setPen(pen);
        pPainter->drawRect(SysParamPageRect[LastNumBoardFocusRect]);
        if( SysParamPageRectText[LastNumBoardFocusRect].length() > 0 )
        {
            tmpPassWord = "";
            for( i=0; i<SysParamPageRectText[LastNumBoardFocusRect].length(); i++ )
                tmpPassWord += '*';
            pPainter->drawText(SysParamPageRect[LastNumBoardFocusRect] ,Qt::AlignCenter,tmpPassWord);
        }
    }
    pPainter->restore();
    return TRUE;
}

BOOL CPage_SysParameter::mousePressEvent(QMouseEvent *pEvent )
{
    if( CPage::mousePressEvent( pEvent ) )
    {
        if( Widget::PAGE_SYS_PARA != AfxGetCurPageIndex() )
        {
            SysParamOption = NET_PARAM;
            AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
            NumBoardFocusRect = -1;
            LastNumBoardFocusRect = -1;
            Is_Refresh_SysParamTime = TRUE;
            SysParamPageRectText[IP_INPUT_RECT] = SysParamPageRectPreviousText[IP_INPUT_RECT];
            SysParamPageRectText[MASK_INPUT_RECT] = SysParamPageRectPreviousText[MASK_INPUT_RECT];
            SysParamPageRectText[GATEWAY_INPUT_RECT] = SysParamPageRectPreviousText[GATEWAY_INPUT_RECT];
            SysParamPageRectText[DNS_INPUT_RECT] = SysParamPageRectPreviousText[DNS_INPUT_RECT];
            SysParamPageRectText[OLD_PWD_INPUT_RECT] = SysParamPageRectPreviousText[OLD_PWD_INPUT_RECT];
            SysParamPageRectText[NEW_PWD_INPUT_RECT] = SysParamPageRectPreviousText[NEW_PWD_INPUT_RECT];
            SysParamPageRectText[CONFIRM_PWD_INPUT_RECT] = SysParamPageRectPreviousText[CONFIRM_PWD_INPUT_RECT];
        }
        AfxRefreshScreen();
        return TRUE ;
    }

    if(pEvent->button() == Qt::LeftButton)
    {
        BOOL Ret = FALSE;

        if( MousePressChoicePage(pEvent) )
            return TRUE;
        if( LoginState_SysParam == CPage::LOGIN )
        {
            switch( SysParamOption )
            {
            case NET_PARAM:
                Ret = MousePressNetParam(pEvent);
            break;
            case SYS_TIME:
                Ret = MousePressSysTime(pEvent);
            break;
            case LOGIN_PWD:
                Ret = MousePressLoginPwd(pEvent);
            break;
            default:
                Ret = MousePressNetParam(pEvent);
            break;
            }
            return Ret;
        }
    }

    AfxRefreshScreen( );
    return  FALSE ;
}

BOOL CPage_SysParameter::MousePressChoicePage( QMouseEvent *pEvent )
{
    if( SysParamPageRect[ NET_PARAM_RECT ].contains(pEvent->pos()) )
    {
        if( SysParamOption != NET_PARAM )
        {
            Is_Refresh_SysParamTime = TRUE;
            SysParamOption = NET_PARAM;
        }
    }
    else if( SysParamPageRect[ SYS_TIME_RECT ].contains(pEvent->pos()) )
    {
        if( SysParamOption != SYS_TIME )
        {
            SysParamOption = SYS_TIME;
        }
    }
    else if( SysParamPageRect[ LOGIN_PWD_RECT ].contains(pEvent->pos()) )
    {
        if( SysParamOption != LOGIN_PWD )
        {
            Is_Refresh_SysParamTime = TRUE;
            SysParamOption = LOGIN_PWD;
        }
    }
    else
    {
        return FALSE;
    }

    SysParamPageRectText[IP_INPUT_RECT] = SysParamPageRectPreviousText[IP_INPUT_RECT];
    SysParamPageRectText[MASK_INPUT_RECT] = SysParamPageRectPreviousText[MASK_INPUT_RECT];
    SysParamPageRectText[GATEWAY_INPUT_RECT] = SysParamPageRectPreviousText[GATEWAY_INPUT_RECT];
    SysParamPageRectText[DNS_INPUT_RECT] = SysParamPageRectPreviousText[DNS_INPUT_RECT];
    SysParamPageRectText[YEAR_INPUT_RECT] = SysParamPageRectPreviousText[YEAR_INPUT_RECT];
    SysParamPageRectText[MONTH_INPUT_RECT] = SysParamPageRectPreviousText[MONTH_INPUT_RECT];
    SysParamPageRectText[DAY_INPUT_RECT] = SysParamPageRectPreviousText[DAY_INPUT_RECT];
    SysParamPageRectText[HOUR_INPUT_RECT] = SysParamPageRectPreviousText[HOUR_INPUT_RECT];
    SysParamPageRectText[MINUTE_INPUT_RECT] = SysParamPageRectPreviousText[MINUTE_INPUT_RECT];
    SysParamPageRectText[SECOND_INPUT_RECT] = SysParamPageRectPreviousText[SECOND_INPUT_RECT];
    SysParamPageRectText[OLD_PWD_INPUT_RECT] = SysParamPageRectPreviousText[OLD_PWD_INPUT_RECT];
    SysParamPageRectText[NEW_PWD_INPUT_RECT] = SysParamPageRectPreviousText[NEW_PWD_INPUT_RECT];
    SysParamPageRectText[CONFIRM_PWD_INPUT_RECT] = SysParamPageRectPreviousText[CONFIRM_PWD_INPUT_RECT];

    AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
    NumBoardFocusRect = -1;
    LastNumBoardFocusRect = -1;
    AfxGetWidget()->update();

    return TRUE;
}

BOOL CPage_SysParameter::MousePressNetParam( QMouseEvent *pEvent )
{
    if( SysParamPageRect[ IP_INPUT_RECT ].contains(pEvent->pos()) )
    {
        if( NumBoardFocusRect == IP_INPUT_RECT && AfxGetWidget()->Numkeyboard->isVisible() &&
            AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_WIDGET )
        {
            AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit); 
        }
        else
        {
            LastNumBoardFocusRect = NumBoardFocusRect;
            NumBoardFocusRect = IP_INPUT_RECT;
            AfxGetWidget()->Numkeyboard->KeyBoardShow( KeyLineEdit );;
        }
    }
    else if(SysParamPageRect[ MASK_INPUT_RECT ].contains(pEvent->pos()) )
    {
        if( NumBoardFocusRect == MASK_INPUT_RECT && AfxGetWidget()->Numkeyboard->isVisible() &&
            AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_WIDGET )
        {
            AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        }
        else
        {
            LastNumBoardFocusRect = NumBoardFocusRect;
            NumBoardFocusRect = MASK_INPUT_RECT;
            AfxGetWidget()->Numkeyboard->KeyBoardShow( KeyLineEdit );
        }
    }
    else if(SysParamPageRect[ GATEWAY_INPUT_RECT ].contains(pEvent->pos()) )
    {
        if( NumBoardFocusRect == GATEWAY_INPUT_RECT && AfxGetWidget()->Numkeyboard->isVisible() &&
            AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_WIDGET )
        {
            AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        }
        else
        {
            LastNumBoardFocusRect = NumBoardFocusRect;
            NumBoardFocusRect = GATEWAY_INPUT_RECT;
            AfxGetWidget()->Numkeyboard->KeyBoardShow( KeyLineEdit );
        }
    }
    else if(SysParamPageRect[ DNS_INPUT_RECT ].contains(pEvent->pos()) )
    {
        if( NumBoardFocusRect == DNS_INPUT_RECT && AfxGetWidget()->Numkeyboard->isVisible() &&
            AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_WIDGET )
        {
            AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        }
        else
        {
            LastNumBoardFocusRect = NumBoardFocusRect;
            NumBoardFocusRect = DNS_INPUT_RECT;
            AfxGetWidget()->Numkeyboard->KeyBoardShow( KeyLineEdit );
        }
    }
    else if(SysParamPageRect[ YES_RECT ].contains(pEvent->pos()) )
    {
        AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        if( FALSE == Is_valid_NetParam(SysParamPageRectText[IP_INPUT_RECT],
                                SysParamPageRectText[GATEWAY_INPUT_RECT],
                                SysParamPageRectText[MASK_INPUT_RECT],
                                SysParamPageRectText[DNS_INPUT_RECT]) )
        {
            return FALSE;
        }
        QMessageBox::information(AfxGetWidget() , UTF8("确定") , UTF8("修改成功!!!") ,
                           QObject::tr("确定"));
        NETPARAM tmpNetParam ;
        memcpy(tmpNetParam.ip , SysParamPageRectText[IP_INPUT_RECT].toStdString().c_str() , sizeof(tmpNetParam.ip));
        memcpy(tmpNetParam.mask , SysParamPageRectText[MASK_INPUT_RECT].toStdString().c_str() , sizeof(tmpNetParam.mask));
        memcpy(tmpNetParam.gateway , SysParamPageRectText[GATEWAY_INPUT_RECT].toStdString().c_str() , sizeof(tmpNetParam.gateway));
        memcpy(tmpNetParam.dns , SysParamPageRectText[DNS_INPUT_RECT].toStdString().c_str() , sizeof(tmpNetParam.gateway));
        AfxWriteNetParamFile(&tmpNetParam);
        AfxSetNetParam(&tmpNetParam);
        SysParamPageRectPreviousText[IP_INPUT_RECT] = SysParamPageRectText[IP_INPUT_RECT];
        SysParamPageRectPreviousText[MASK_INPUT_RECT] = SysParamPageRectText[MASK_INPUT_RECT];
        SysParamPageRectPreviousText[GATEWAY_INPUT_RECT] = SysParamPageRectText[GATEWAY_INPUT_RECT];
        SysParamPageRectPreviousText[DNS_INPUT_RECT] = SysParamPageRectText[DNS_INPUT_RECT];
        AfxGetWidget()->update();
    }
    else if(SysParamPageRect[ NO_RECT ].contains(pEvent->pos()) )
    {
        AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        SysParamPageRectText[IP_INPUT_RECT] = SysParamPageRectPreviousText[IP_INPUT_RECT];
        SysParamPageRectText[MASK_INPUT_RECT] = SysParamPageRectPreviousText[MASK_INPUT_RECT];
        SysParamPageRectText[GATEWAY_INPUT_RECT] = SysParamPageRectPreviousText[GATEWAY_INPUT_RECT];
        SysParamPageRectText[DNS_INPUT_RECT] = SysParamPageRectPreviousText[DNS_INPUT_RECT];
        AfxGetWidget()->update();
    }
    return TRUE;
}

BOOL CPage_SysParameter::MousePressSysTime( QMouseEvent *pEvent )
{
    if( SysParamPageRect[ YEAR_INPUT_RECT ].contains(pEvent->pos()) )
    {
        if( NumBoardFocusRect == YEAR_INPUT_RECT && AfxGetWidget()->Numkeyboard->isVisible() &&
            AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_WIDGET )
        {
            AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        }
        else
        {
            Is_Refresh_SysParamTime = FALSE;
            LastNumBoardFocusRect = NumBoardFocusRect;
            NumBoardFocusRect = YEAR_INPUT_RECT;
            AfxGetWidget()->Numkeyboard->KeyBoardShow( KeyLineEdit );;
        }
    }
    else if(SysParamPageRect[ MONTH_INPUT_RECT ].contains(pEvent->pos()) )
    {
        if( NumBoardFocusRect == MONTH_INPUT_RECT && AfxGetWidget()->Numkeyboard->isVisible() &&
            AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_WIDGET )
        {
            AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        }
        else
        {
            Is_Refresh_SysParamTime = FALSE;
            LastNumBoardFocusRect = NumBoardFocusRect;
            NumBoardFocusRect = MONTH_INPUT_RECT;
            AfxGetWidget()->Numkeyboard->KeyBoardShow( KeyLineEdit );
        }
    }
    else if(SysParamPageRect[ DAY_INPUT_RECT ].contains(pEvent->pos()) )
    {
        if( NumBoardFocusRect == DAY_INPUT_RECT && AfxGetWidget()->Numkeyboard->isVisible() &&
            AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_WIDGET )
        {
            AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        }
        else
        {
            Is_Refresh_SysParamTime = FALSE;
            LastNumBoardFocusRect = NumBoardFocusRect;
            NumBoardFocusRect = DAY_INPUT_RECT;
            AfxGetWidget()->Numkeyboard->KeyBoardShow( KeyLineEdit );
        }
    }
    else if(SysParamPageRect[ HOUR_INPUT_RECT ].contains(pEvent->pos()) )
    {
        if( NumBoardFocusRect == HOUR_INPUT_RECT && AfxGetWidget()->Numkeyboard->isVisible() &&
            AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_WIDGET )
        {
            AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        }
        else
        {
            Is_Refresh_SysParamTime = FALSE;
            LastNumBoardFocusRect = NumBoardFocusRect;
            NumBoardFocusRect = HOUR_INPUT_RECT;
            AfxGetWidget()->Numkeyboard->KeyBoardShow( KeyLineEdit );
        }
    }
    else if(SysParamPageRect[ MINUTE_INPUT_RECT ].contains(pEvent->pos()) )
    {
        if( NumBoardFocusRect == MINUTE_INPUT_RECT && AfxGetWidget()->Numkeyboard->isVisible() &&
            AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_WIDGET )
        {
            AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        }
        else
        {
            Is_Refresh_SysParamTime = FALSE;
            LastNumBoardFocusRect = NumBoardFocusRect;
            NumBoardFocusRect = MINUTE_INPUT_RECT;
            AfxGetWidget()->Numkeyboard->KeyBoardShow( KeyLineEdit );
        }
    }
    else if(SysParamPageRect[ SECOND_INPUT_RECT ].contains(pEvent->pos()) )
    {
        if( NumBoardFocusRect == SECOND_INPUT_RECT && AfxGetWidget()->Numkeyboard->isVisible() &&
            AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_WIDGET )
        {
            AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        }
        else
        {
            Is_Refresh_SysParamTime = FALSE;
            LastNumBoardFocusRect = NumBoardFocusRect;
            NumBoardFocusRect = SECOND_INPUT_RECT;
            AfxGetWidget()->Numkeyboard->KeyBoardShow( KeyLineEdit );
        }
    }
    else if(SysParamPageRect[ YES_RECT ].contains(pEvent->pos()) )
    {
        AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        if( FALSE == Is_valid_SysTime(SysParamPageRectText[YEAR_INPUT_RECT],
                                SysParamPageRectText[MONTH_INPUT_RECT],
                                SysParamPageRectText[DAY_INPUT_RECT],
                                SysParamPageRectText[HOUR_INPUT_RECT],
                                SysParamPageRectText[MINUTE_INPUT_RECT],
                                SysParamPageRectText[SECOND_INPUT_RECT]) )
        {
            return FALSE;
        }
        QMessageBox::information(AfxGetWidget() , UTF8("确定") , UTF8("修改成功!!!") ,
                           QObject::tr("确定"));
        Is_Refresh_SysParamTime = TRUE;
/*        SysParamPageRectPreviousText[YEAR_INPUT_RECT] = SysParamPageRectText[YEAR_INPUT_RECT];
        SysParamPageRectPreviousText[MONTH_INPUT_RECT] = SysParamPageRectText[MONTH_INPUT_RECT];
        SysParamPageRectPreviousText[DAY_INPUT_RECT] = SysParamPageRectText[DAY_INPUT_RECT];
        SysParamPageRectPreviousText[HOUR_INPUT_RECT] = SysParamPageRectText[HOUR_INPUT_RECT];
        SysParamPageRectPreviousText[MINUTE_INPUT_RECT] = SysParamPageRectText[MINUTE_INPUT_RECT];
        SysParamPageRectPreviousText[SECOND_INPUT_RECT] = SysParamPageRectText[SECOND_INPUT_RECT];
        */
        AfxGetWidget()->update();
    }
    else if(SysParamPageRect[ NO_RECT ].contains(pEvent->pos()) )
    {
        AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        Is_Refresh_SysParamTime = TRUE;
        /*
        SysParamPageRectText[YEAR_INPUT_RECT] = SysParamPageRectPreviousText[YEAR_INPUT_RECT];
        SysParamPageRectText[MONTH_INPUT_RECT] = SysParamPageRectPreviousText[MONTH_INPUT_RECT];
        SysParamPageRectText[DAY_INPUT_RECT] = SysParamPageRectPreviousText[DAY_INPUT_RECT];
        SysParamPageRectText[HOUR_INPUT_RECT] = SysParamPageRectPreviousText[HOUR_INPUT_RECT];
        SysParamPageRectText[MINUTE_INPUT_RECT] = SysParamPageRectPreviousText[MINUTE_INPUT_RECT];
        SysParamPageRectText[SECOND_INPUT_RECT] = SysParamPageRectPreviousText[SECOND_INPUT_RECT];
        */
        AfxGetWidget()->update();
    }
    return TRUE;
}

BOOL CPage_SysParameter::MousePressLoginPwd( QMouseEvent *pEvent )
{
    if( SysParamPageRect[ OLD_PWD_INPUT_RECT ].contains(pEvent->pos()) )
    {
        if( NumBoardFocusRect == OLD_PWD_INPUT_RECT && AfxGetWidget()->Numkeyboard->isVisible() &&
            AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_WIDGET )
        {
            AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        }
        else
        {
            LastNumBoardFocusRect = NumBoardFocusRect;
            NumBoardFocusRect = OLD_PWD_INPUT_RECT;
            AfxGetWidget()->Numkeyboard->KeyBoardShow( KeyLineEdit );;
        }
    }
    else if(SysParamPageRect[ NEW_PWD_INPUT_RECT ].contains(pEvent->pos()) )
    {
        if( NumBoardFocusRect == NEW_PWD_INPUT_RECT && AfxGetWidget()->Numkeyboard->isVisible() &&
            AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_WIDGET )
        {
            AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        }
        else
        {
            LastNumBoardFocusRect = NumBoardFocusRect;
            NumBoardFocusRect = NEW_PWD_INPUT_RECT;
            AfxGetWidget()->Numkeyboard->KeyBoardShow( KeyLineEdit );
        }
    }
    else if(SysParamPageRect[ CONFIRM_PWD_INPUT_RECT ].contains(pEvent->pos()) )
    {
        if( NumBoardFocusRect == CONFIRM_PWD_INPUT_RECT && AfxGetWidget()->Numkeyboard->isVisible() &&
            AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_WIDGET )
        {
            AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        }
        else
        {
            LastNumBoardFocusRect = NumBoardFocusRect;
            NumBoardFocusRect = CONFIRM_PWD_INPUT_RECT;
            AfxGetWidget()->Numkeyboard->KeyBoardShow( KeyLineEdit );
        }
    }
    else if(SysParamPageRect[ YES_RECT ].contains(pEvent->pos()) )
    {
        AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        if( FALSE == Is_valid_LoginPwd(SysParamPageRectText[OLD_PWD_INPUT_RECT],
                                SysParamPageRectText[NEW_PWD_INPUT_RECT],
                                SysParamPageRectText[CONFIRM_PWD_INPUT_RECT]) )
        {
            return FALSE;
        }
        //QMessageBox::information(AfxGetWidget() , UTF8("确定") , UTF8("修改成功!!!") ,
                          // QObject::tr("确定"));
        SysParamPageRectPreviousText[OLD_PWD_INPUT_RECT] = "";
        SysParamPageRectPreviousText[NEW_PWD_INPUT_RECT] = "";
        SysParamPageRectPreviousText[CONFIRM_PWD_INPUT_RECT] = "";
        SysParamPageRectPreviousText[OLD_PWD_INPUT_RECT].clear();
        SysParamPageRectPreviousText[NEW_PWD_INPUT_RECT].clear();
        SysParamPageRectPreviousText[CONFIRM_PWD_INPUT_RECT].clear();
        SysParamPageRectText[OLD_PWD_INPUT_RECT].clear();
        SysParamPageRectText[NEW_PWD_INPUT_RECT].clear();
        SysParamPageRectText[CONFIRM_PWD_INPUT_RECT].clear();


        AfxGetWidget()->update();
    }
    else if(SysParamPageRect[ NO_RECT ].contains(pEvent->pos()) )
    {
        AfxGetWidget()->Numkeyboard->KeyBoardhide(KeyLineEdit);
        SysParamPageRectText[OLD_PWD_INPUT_RECT] = "";//SysParamPageRectPreviousText[OLD_PWD_INPUT_RECT];
        SysParamPageRectText[NEW_PWD_INPUT_RECT] = "";//SysParamPageRectPreviousText[NEW_PWD_INPUT_RECT];
        SysParamPageRectText[CONFIRM_PWD_INPUT_RECT] = "";//SysParamPageRectPreviousText[CONFIRM_PWD_INPUT_RECT];
        AfxGetWidget()->update();
    }
    return TRUE;
}

void CPage_SysParameter::TextChangeFocusRect(QString text)
{
    SysParamPageRectText[NumBoardFocusRect] = text;
    AfxGetWidget()->update( SysParamPageRect[ NumBoardFocusRect ] );
}

BOOL CPage_SysParameter::is_valid_ip(const char *ip)
{
    int section = 0; //每一节的十进制值
    int dot = 0; //几个点分隔符
    int last = -1; //每一节中上一个字符
    while(*ip)
    {
        if(*ip == '.')
        {
            dot++;
            if(dot > 3)
            {
                return FALSE;
            }
            if(section >= 0 && section <=255)
            {
                if( dot == 1 && section == 0 )
                    return FALSE;
                section = -1;
            }
            else
            {
                return FALSE;
            }
        }
        else if(*ip >= '0' && *ip <= '9')
        {
            if( section == -1 )
                section = 0;
            section = section * 10 + *ip - '0';
        }
        else
        {
            return FALSE;
        }
        last = *ip;
        ip++;
    }

    if(section >= 0 && section <=255)
    {
        if(3 == dot)
        {
            section = 0;
            //printf ("IP address success!\n");
            //printf ("%d\n",dot);
            return TRUE;
        }
    }
    return FALSE;
}

/*参数UINT32都为网络字节顺序。*/

/*MASK子网掩码是否合法, 合法返回TURE，失败返回FALSE*/
BOOL CPage_SysParameter::netMaskIsValid(DWORD mask)
{
    unsigned long ii;
    ii = ntohl(mask);
    if((ii|(ii-1))==0xffffffff)
    {
        return TRUE;
    }

    return FALSE;
}

/*MASK子网掩码是否合法, 合法返回TURE，失败返回FALSE*/
BOOL CPage_SysParameter::netMaskAndIpIsValid(DWORD IP, DWORD mask)
{
    DWORD a, b=0, c;
    a = IP&0x000000ff;
    b = ntohl(mask);

    /*首先与默认子网掩码比较*/
    if(a>0&&a<127)
    {
        if(mask<0x000000ff)
            return FALSE;
        if(mask>0x000000ff)
            b-=0xff000000;
    }
    if(a>=128&&a<=191)
    {
        if(mask<0x0000ffff)
            return FALSE;
        if(mask>0x0000ffff)
            b-=0xffff0000;
    }
    if(a>=192&&a<=223)
    {
        if(mask<0x00ffffff)
            return FALSE;
        if(mask>0x00ffffff)
            b-=0xffffff00;
    }

    /*每个子网段的第一个是网络地址,用来标志这个网络,最后一个是广播地址,用来代表这个网络上的所有主机.这两个IP地址被TCP/IP保留,不可分配给主机使用.*/
    c = ~ntohl(mask)&ntohl(IP);
    if(c==0||c==~ntohl(mask))
    return FALSE;

    /*RFC 1009中规定划分子网时，子网号不能全为0或1，会导致IP地址的二义性*/
    if(b>0)
    {
        c = b&(ntohl(IP));
        if(c==0||c==b)
            return FALSE;
    }

    return TRUE;
}

/*测试主网和子网是否匹配，也可测试两个主机IP是否在同一网段内*/
BOOL CPage_SysParameter::Is_valid_NetParam(QString ip, QString subIP, QString mask, QString dns)
{
    int i;
    int addr1, addr2;

    std::string strip = ip.toStdString();
    const char* chIp = strip.c_str();
    std::string strSubIP = subIP.toStdString();
    const char* chSubIP = strSubIP.c_str();
    std::string strMask = mask.toStdString();
    const char* chMask = strMask.c_str();
    std::string strDns = dns.toStdString();
    const char* chDns = strDns.c_str();

    char tmpDword[4] = {};
    DWORD DwIp,DwSubIP,DwMask;
    DWORD * PDwIp;
    DWORD * PDwSubIP;
    DWORD * PDwMask;
    bool ok;
    tmpDword[0] = ip.section('.',0,0).toInt(&ok,10);
    tmpDword[1] = ip.section('.',1,1).toInt(&ok,10);
    tmpDword[2] = ip.section('.',2,2).toInt(&ok,10);
    tmpDword[3] = ip.section('.',3,3).toInt(&ok,10);
    //DwIp = *(DWORD *)(tmpDword);
    PDwIp = (DWORD *)(tmpDword);
    DwIp = *PDwIp;
    tmpDword[0] = subIP.section('.',0,0).toInt(&ok,10);
    tmpDword[1] = subIP.section('.',1,1).toInt(&ok,10);
    tmpDword[2] = subIP.section('.',2,2).toInt(&ok,10);
    tmpDword[3] = subIP.section('.',3,3).toInt(&ok,10);
    //DwSubIP = *(DWORD *)(tmpDword);
    PDwSubIP = (DWORD *)(tmpDword);
    DwSubIP = *PDwSubIP;
    tmpDword[0] = mask.section('.',0,0).toInt(&ok,10);
    tmpDword[1] = mask.section('.',1,1).toInt(&ok,10);
    tmpDword[2] = mask.section('.',2,2).toInt(&ok,10);
    tmpDword[3] = mask.section('.',3,3).toInt(&ok,10);
    //DwMask = *(DWORD *)(tmpDword);
    PDwMask = (DWORD *)(tmpDword);
    DwMask = *PDwMask;


    if(!is_valid_ip(chIp))
    {
        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("IP错误!!!") ,
                           QObject::tr("确定"));
        return FALSE;
    }
    if(!is_valid_ip(chSubIP))
    {
        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("网关错误!!!") ,
                           QObject::tr("确定"));
        return FALSE;
    }
    if(!is_valid_ip(chMask) || !netMaskIsValid(DwMask))
    {
        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("子网掩码错误!!!") ,
                           QObject::tr("确定"));
        return FALSE;
    }
    if(!is_valid_ip(chDns))
    {
        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("Dns错误!!!") ,
                           QObject::tr("确定"));
        return FALSE;
    }

    i = netMaskAndIpIsValid(DwIp, DwMask);
    if(i!=TRUE)
    {
        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("IP和子网掩码对应错误!!!") ,
                         QObject::tr("确定"));
        return FALSE;
    }
    i = netMaskAndIpIsValid(DwSubIP, DwMask);
    if(i!=TRUE)
    {
        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("网关和子网掩码对应错误!!!") ,
                         QObject::tr("确定"));
        return FALSE;
    }

    addr1 = DwIp&DwMask;
    addr2 = DwSubIP&DwMask;

    if(addr1!=addr2)
    {
        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("IP和网关不在同一网段!!!") ,
                         QObject::tr("确定"));
        return FALSE;
    }

    return TRUE;
}

BOOL CPage_SysParameter::Is_valid_SysTime(QString Year, QString Month,
                                          QString Day, QString Hour, QString Minute, QString Second)
{
    QString tmpstr = ".";

    if( Year.isEmpty() > 0 || Month.isEmpty() > 0 || Day.isEmpty() > 0 ||
        Hour.isEmpty() > 0 || Minute.isEmpty() > 0 || Second.isEmpty() > 0 )
    {
        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("时间不能为空!!!") ,
                         QObject::tr("确定"));
        return FALSE;
    }
    if( Year.indexOf(tmpstr) > 0 || Month.indexOf(tmpstr) > 0 || Day.indexOf(tmpstr) > 0 ||
        Hour.indexOf(tmpstr) > 0 || Minute.indexOf(tmpstr) > 0 || Second.indexOf(tmpstr) > 0 )
    {
        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("时间不能含有符号“.”!!!") ,
                         QObject::tr("确定"));
        return FALSE;
    }

    int iYear,iMonth,iDay,iHour,iMinute,iSecond;
    iYear = Year.toInt();
    iMonth = Month.toInt();
    iDay = Day.toInt();
    iHour = Hour.toInt();
    iMinute = Minute.toInt();
    iSecond = Second.toInt();

    if( iYear<1970 || iYear>2037 )
    {
        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("Year只能设定在1970～2037之间!!!") ,
                         QObject::tr("确定"));
        return FALSE;
    }
    else
    {
        switch( iMonth )
        {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                if( iDay<1 || iDay > 31 )
                {
                    QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("Day超出范围!!!") ,
                                     QObject::tr("确定"));
                    return FALSE;
                }
                break;
            case 2:
                if((iYear%4==0&&iYear%100!=0)||(iYear%400==0))
                {
                    if( iDay<1 || iDay > 29 )
                    {
                        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("Day超出范围!!!") ,
                                         QObject::tr("确定"));
                        return FALSE;
                    }
                }
                else
                {
                    if( iDay<1 || iDay > 28 )
                    {
                        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("Day超出范围!!!") ,
                                         QObject::tr("确定"));
                        return FALSE;
                    }
                }
                break;
            case 4:
            case 6:
            case 9:
            case 11:
                if( iDay<1 || iDay > 30 )
                {
                    QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("Day超出范围!!!") ,
                                     QObject::tr("确定"));
                    return FALSE;
                }
                break;
             default:
                QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("Month超出范围!!!") ,
                                 QObject::tr("确定"));
                return FALSE;
        }
        if( iHour<0 || iHour > 23 )
        {
            QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("Hour超出范围!!!") ,
                             QObject::tr("确定"));
            return FALSE;
        }
        else
        {
            if( iMinute<0 || iMinute > 59 )
            {
                QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("Minute超出范围!!!") ,
                                 QObject::tr("确定"));
                return FALSE;
            }
            else
            {
                if( iSecond<0 || iSecond > 59 )
                {
                    QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("Second超出范围!!!") ,
                                     QObject::tr("确定"));
                    return FALSE;
                }
            }
        }
    }
    REALTIME SetTime;
    memset( &SetTime , 0 , sizeof( SetTime ) );
    SetTime.wYear = iYear;
    SetTime.wMonth = iMonth;
    SetTime.wDay = iDay;
    SetTime.wHour = iHour;
    SetTime.wMinute = iMinute;
    SetTime.wSecond = iSecond;
    AfxSetSysTime(&SetTime);
    return TRUE;
}

BOOL CPage_SysParameter::Is_valid_LoginPwd(QString OldPwd, QString NewPwd, QString ConfirmPwd)
{
    if( 0 != QString::compare(QString(AfxGetGlobal()->CurrentUsrPwd.Pwd) , OldPwd) )
    {
        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("原密码不正确!!!") ,
                         QObject::tr("确定"));
        return FALSE;
    }
    else
    {
        if( NewPwd.length() != 6 )
        {
            QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("请输入6位字符的新密码!!!") ,
                             QObject::tr("确定"));
            return FALSE;
        }
        else
        {
            if( 0 != QString::compare(NewPwd , ConfirmPwd) )
            {
                QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("确认密码与新密码不一致!!!") ,
                                 QObject::tr("确定"));
                return FALSE;
            }
            else
            {
                std::string strNewPwd = ConfirmPwd.toStdString();
                const char* chNewPwd = strNewPwd.c_str();

                USRPWD SetNewUsrPwd;
                CGlobal * tmpGlobal = AfxGetGlobal();
                memset( &SetNewUsrPwd , 0 , sizeof(USRPWD) );
                memcpy( SetNewUsrPwd.Usr , tmpGlobal->CurrentUsrPwd.Usr , sizeof(SetNewUsrPwd.Usr) );
                memcpy( SetNewUsrPwd.Pwd , chNewPwd , ConfirmPwd.length() );
                AfxWritePwd( &SetNewUsrPwd );
                QMessageBox::warning(AfxGetWidget() , UTF8("提示") , UTF8("新密码设置成功!!!") ,
                                 QObject::tr("确定"));
            }
        }
    }

    return  TRUE;
}

void CPage_SysParameter::NumBoardShow()
{
    AfxGetWidget()->NumkeyboardFocusWindow = Widget::BOARD_WIDGET;
    KeyLineEdit->setText(SysParamPageRectText[NumBoardFocusRect]);
    AfxGetWidget()->update(SysParamPageRect[ NumBoardFocusRect ]);
    AfxGetWidget()->update(SysParamPageRect[ LastNumBoardFocusRect ]);
}

void CPage_SysParameter::NumBoardHide()
{
    LastNumBoardFocusRect = NumBoardFocusRect;
    NumBoardFocusRect = -1;
    AfxGetWidget()->update(SysParamPageRect[ LastNumBoardFocusRect ]);
}

void CPage_SysParameter::LoginStateChange_SysParam(BYTE byState)
{
    LoginState_SysParam = byState;
    AfxGetWidget()->update();
}

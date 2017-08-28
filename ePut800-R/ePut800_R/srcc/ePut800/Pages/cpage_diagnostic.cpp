#include "cpage_diagnostic.h"

CPage_Diagnostic::CPage_Diagnostic( BYTE byType ):CPage( byType )
{
    m_strPic += "4通讯诊断-1.jpg";//"ePut800_Diagnosdic.jpg";

    MessageTextEdit = new QTextEdit(AfxGetWidget());
    MessageTextEdit->setWindowFlags(Qt::FramelessWindowHint );
    MessageTextEdit->move(153,87);
    MessageTextEdit->setFixedSize(615,431);
    QPalette pl = MessageTextEdit->palette();
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    MessageTextEdit->setPalette(pl);
    QFont font = MessageTextEdit->font();
    font.setPixelSize(20);
    MessageTextEdit->setFont(font);
    MessageTextEdit->setVisible(false);
    MessageTextEdit->setReadOnly(true);
    MessageTextEdit->document()->setMaximumBlockCount(2000);

    BusNoComboBox = new QComboBox(AfxGetWidget());
    BusNoComboBox->move(130+70,30);
    BusNoComboBox->setFixedSize(70,30);
    BusNoComboBox->setEditable(FALSE);
    BusNoComboBox->setMaxVisibleItems(5);

    BusNoComboBox->setStyleSheet("QComboBox{border:1px solid red;background-color:rgb(96, 96, 96);color:red;}"
                                 "QComboBox::drop-down{width:30px;}"
                                 "QComboBox QAbstractItemView::item{border:1px solid red;height:40px;width:300px;}"
                                 "QComboBox QAbstractItemView::item{background-color:rgb(96, 96, 96);;color:red;}"
                                );
    BusNoComboBox->setView(new QListView());
    BusNoComboBox->setVisible(false);

    DevAddrComboBox = new QComboBox(AfxGetWidget());
    DevAddrComboBox->move(130+210,30);
    DevAddrComboBox->setFixedSize(70,30);
    DevAddrComboBox->setEditable(FALSE);
    DevAddrComboBox->setMaxVisibleItems(5);
    DevAddrComboBox->setStyleSheet(BusNoComboBox->styleSheet());
    DevAddrComboBox->setView(new QListView());
    DevAddrComboBox->setVisible(false);

    connect(BusNoComboBox , SIGNAL(activated(int)) , this , SLOT(slots_BusNoChange(int)));
    connect(DevAddrComboBox , SIGNAL(activated(int)) , this , SLOT(slots_DevAddrChange(int)));
    connect(this , SIGNAL(signal_MessageShow(QString)) , this , SLOT(slots_MessageShow(QString)));

    DiagnosticOption = DATAGRAM_PAGE;
    CheckState = TRUE;
    MessageShow = TRUE;
    MessageFormat = HEX_FOEMAT;
    ChannelNoRectShow = FALSE;
    DevAddressRectShow = FALSE;
    CheckStateRectShow = FALSE;
    MessageShowRectShow = FALSE;
    MessageFormatRectShow = FALSE;
    ChannelNo = 9;
    DevAddress = 3;
    ChannelNoAllNum = 9;
    DevAddressAllNum = 3;
    memset( &sBusNoData , 0 , sizeof(sBusNoData) );

    memset( DatagramReleasePath , 0 , sizeof(DatagramReleasePath) );
    memcpy( DatagramReleasePath , "/usr/share/ePut800/Pics/灰底-按钮白-报文显示.jpg" , sizeof("/usr/share/ePut800/Pics/灰底-按钮白-报文显示.jpg"));
    memset( DatagramPressPath , 0 , sizeof(DatagramPressPath) );
    memcpy( DatagramPressPath , "/usr/share/ePut800/Pics/灰底-按钮蓝-报文显示.jpg" , sizeof("/usr/share/ePut800/Pics/灰底-按钮蓝-报文显示.jpg"));

    memset( CommunStateReleasePath , 0 , sizeof(CommunStateReleasePath) );
    memcpy( CommunStateReleasePath , "/usr/share/ePut800/Pics/灰底-按钮白-通讯状态.jpg" , sizeof("/usr/share/ePut800/Pics/灰底-按钮白-通讯状态.jpg"));
    memset( CommunStatePressPath , 0 , sizeof(CommunStatePressPath) );
    memcpy( CommunStatePressPath , "/usr/share/ePut800/Pics/灰底-按钮蓝-通讯状态.jpg" , sizeof("/usr/share/ePut800/Pics/灰底-按钮蓝-通讯状态.jpg"));

    DiagnosticPageRect[DATAGRAM_RECT].setRect(40 , 113 , 106 , 51);
    DiagnosticPageRect[COMMUNINICATION_STATE_RECT].setRect(40 , 183 , 106 , 51);

    DiagnosticPageRect[CHANNEL_TEXT_RECT].setRect(130,30, 70, 30);
    DiagnosticPageRect[CHANNEL_RECT].setRect(130+70,30, 70, 30);
    DiagnosticPageRect[DEVADDR_TEXT_RECT].setRect(130+140,30, 70, 30);
    DiagnosticPageRect[DEVADDR_RECT].setRect(130+210,30, 70, 30);
    DiagnosticPageRect[CHECK_RECT].setRect(135+280,30, 65, 60);
    DiagnosticPageRect[HEX_OR_ASCII].setRect(135+350,30, 65, 60);
    DiagnosticPageRect[START_OR_END_RECT].setRect(135+420,30, 65, 60);
    DiagnosticPageRect[TX_RECT].setRect(130+490,30, 75, 30);
    DiagnosticPageRect[RX_RECT].setRect(130+490,60, 75, 30);

    DevStateRect.setRect(153,87,615,431);
    RxTxRect.setRect(130+490,30,70,60);

    TimeBegin = 0;
    m_str.clear();
}

BOOL CPage_Diagnostic::AddData( LPVOID pVoid )
{
    getBusNoAndDevAddr();

    int i=0;
    for(i=0;i<sBusNoData.BusNum;i++)
    {
        char tmpbuf[16]="";
        sprintf(tmpbuf,"%d",sBusNoData.DevAddrOfBus[i].BusNo+1);

        BusNoComboBox->addItem(UTF8(tmpbuf));
    }
    BusNoComboBox->setCurrentIndex(0);
    slots_BusNoChange(0);

    return FALSE ;
}

BOOL CPage_Diagnostic::getBusNoAndDevAddr()
{
    QBUS_ARRAY * pBusArray = AfxGetBusArray() ;
    if( !pBusArray )
    {
        Q_ASSERT( FALSE ) ;
        return FALSE ;
    }

    int size = pBusArray->size() ;
    if( !size )
    {
        Q_ASSERT( FALSE ) ;
        return FALSE ;
    }

    int i=0;
    for( i=0;i<size;i++ )
    {
        QBUS_ARRAY::iterator itor =  pBusArray->find( i ) ;
        if( itor == pBusArray->end() )
            return NULL ;
        PBUS_LINEST tmpBus = itor.value() ;
        if( tmpBus->byBusType != BUS_PAUSE )
        {
            int DevSize = tmpBus->DevArray.size() ;
            if( !DevSize )
            {
                Q_ASSERT( FALSE ) ;
                return FALSE ;
            }
            int j=0;
            if( tmpBus->byBusType == BUS_TRANS )
            {
                sBusNoData.BusTransNo[sBusNoData.BusTransNum++] = tmpBus->wBusNo;
            }

            QDEV_ARRAY::iterator Begin_itor =  tmpBus->DevArray.begin() ;
            QDEV_ARRAY::iterator End_itor =  tmpBus->DevArray.end() ;

            for( ;Begin_itor != End_itor ; Begin_itor++ )
            {
                PDEVST pDev = Begin_itor.value() ;
                if( !pDev || pDev->wAddr != Begin_itor.key( ) )
                    return FALSE ;
                j++;
                PDEVST tmpDev = Begin_itor.value() ;
                sBusNoData.DevAddrOfBus[sBusNoData.BusNum].BusNo = tmpDev->wBusNo;
                sBusNoData.DevAddrOfBus[sBusNoData.BusNum].DevAddr[j-1] = tmpDev->wAddr;
                sBusNoData.DevAddrOfBus[sBusNoData.BusNum].DevNum++;


            }

            sBusNoData.BusNum++;

        }
    }
    return TRUE;
}

BOOL CPage_Diagnostic::DrawPage( QPainter * pPainter )
{

    CPage::DrawPage( pPainter ) ;
    DrawDiagnosticPage(pPainter);
    if( BusNoComboBox->isHidden() )
    {
        BusNoComboBox->setVisible(true);
    }
    if(  DiagnosticOption == DATAGRAM_PAGE )
    {
        if( DevAddrComboBox->isHidden() )
            DevAddrComboBox->setVisible(true);

        if( MessageTextEdit->isHidden() )
            MessageTextEdit->setVisible(true);
    }
    return FALSE ;
}

BOOL CPage_Diagnostic::mousePressEvent(QMouseEvent *pEvent )
{
    if( CPage::mousePressEvent( pEvent ) )
    {
        AfxRefreshScreen();
        if( Widget::PAGE_DIOGNOSTIC != AfxGetCurPageIndex() )
        {
            DevAddrComboBox->setVisible(false);
            BusNoComboBox->setVisible(false);
            MessageTextEdit->setVisible(false);
            MessageTextEdit->clear();
            return TRUE ;
        }
    }
    if(pEvent->button() == Qt::LeftButton)
    {
        //BOOL Ret = FALSE;
        if( MousePressChoicePage(pEvent) )
            return TRUE;
        else if( DiagnosticPageRect[ CHECK_RECT ].contains(pEvent->pos()) )
        {
            if( CheckStateRectShow  )
            {
                QRect tmpRect(DiagnosticPageRect[ CHECK_RECT ].x(),DiagnosticPageRect[ CHECK_RECT ].y(),
                              DiagnosticPageRect[ CHECK_RECT ].width(),
                              0.5 * DiagnosticPageRect[ CHECK_RECT ].height());
                if( tmpRect.contains(pEvent->pos()) )
                    ;
                else
                {
                    if( CheckState )
                        CheckState = FALSE;
                    else
                        CheckState = TRUE;
                }
                CheckStateRectShow = FALSE;
            }
            else
            {
                CheckStateRectShow = TRUE;
            }
            AfxGetWidget()->update(DiagnosticPageRect[ CHECK_RECT ]);
        }
        else if( DiagnosticPageRect[ HEX_OR_ASCII ].contains(pEvent->pos()) )
        {
            if( MessageFormatRectShow )
            {
                QRect tmpRect(DiagnosticPageRect[ HEX_OR_ASCII ].x(),DiagnosticPageRect[ HEX_OR_ASCII ].y(),
                              DiagnosticPageRect[ HEX_OR_ASCII ].width(),
                              0.5 * DiagnosticPageRect[ HEX_OR_ASCII ].height());
                if( tmpRect.contains(pEvent->pos()) )
                    ;
                else
                {
                    if( MessageFormat == HEX_FOEMAT )
                        MessageFormat = ASCII_FOEMAT;
                    else
                        MessageFormat = HEX_FOEMAT;
                }
                MessageFormatRectShow = FALSE;
            }
            else
            {
                MessageFormatRectShow = TRUE;
            }
            AfxGetWidget()->update(DiagnosticPageRect[ HEX_OR_ASCII ]);
        }
        else if( DiagnosticPageRect[ START_OR_END_RECT ].contains(pEvent->pos()) )
        {
            if( MessageShowRectShow )
            {
                QRect tmpRect(DiagnosticPageRect[ START_OR_END_RECT ].x(),DiagnosticPageRect[ START_OR_END_RECT ].y(),
                              DiagnosticPageRect[ START_OR_END_RECT ].width(),
                              0.5 * DiagnosticPageRect[ START_OR_END_RECT ].height());
                if( tmpRect.contains(pEvent->pos()) )
                    ;
                else
                {
                    if( MessageShow )
                        MessageShow = FALSE;
                    else
                        MessageShow = TRUE;
                }
                MessageShowRectShow = FALSE;
            }
            else
            {
                MessageShowRectShow = TRUE;
            }
            AfxGetWidget()->update(DiagnosticPageRect[ START_OR_END_RECT ]);
        }
        else
        {
            if( ChannelNoRectShow )
            {
                if( (pEvent->x() > DiagnosticPageRect[ CHANNEL_RECT ].x()) &&
                    (pEvent->x() < DiagnosticPageRect[ CHANNEL_RECT ].x() +
                     DiagnosticPageRect[ CHANNEL_RECT ].width()))
                {
                    int i=0;
                    for(i=0;i<sBusNoData.BusNum+2;i++)
                    {
                        if( (pEvent->y() > DiagnosticPageRect[ CHANNEL_RECT ].y()+
                            i * DiagnosticPageRect[ CHANNEL_RECT ].height()) &&
                            (pEvent->y() < DiagnosticPageRect[ CHANNEL_RECT ].y() +
                             (i+1) * DiagnosticPageRect[ CHANNEL_RECT ].height()))
                        {
                            if( i == 0 )
                                break;
                            else
                            {
                                ChannelNo = i-1;
                                DevAddress = sBusNoData.DevAddrOfBus[ChannelNo].DevNum;
                            }
                        }
                    }
                }
                ChannelNoRectShow = FALSE;
            }
            else
            {
                if( DiagnosticPageRect[ CHANNEL_RECT ].contains(pEvent->pos()) )
                {
                    ChannelNoRectShow = TRUE;
                }
            }

            if( DevAddressRectShow )
            {
                if( (pEvent->x() > DiagnosticPageRect[ DEVADDR_RECT ].x()) &&
                    (pEvent->x() < DiagnosticPageRect[ DEVADDR_RECT ].x() +
                     DiagnosticPageRect[ DEVADDR_RECT ].width()))
                {
                    int i=0;
                    for(i=0;i<sBusNoData.DevAddrOfBus[ChannelNo].DevNum+2;i++)
                    {
                        if( (pEvent->y() > DiagnosticPageRect[ DEVADDR_RECT ].y()+
                            i * DiagnosticPageRect[ DEVADDR_RECT ].height()) &&
                            (pEvent->y() < DiagnosticPageRect[ DEVADDR_RECT ].y() +
                             (i+1) * DiagnosticPageRect[ DEVADDR_RECT ].height()))
                        {
                            if( i == 0 )
                                break;
                            else
                            {
                                DevAddress = i - 1;
                            }
                        }
                    }
                }
                DevAddressRectShow = FALSE;
            }
            else
            {
                if( DiagnosticPageRect[ DEVADDR_RECT ].contains(pEvent->pos()) )
                {
                    DevAddressRectShow = TRUE;
                }
            }
        }
         AfxGetWidget()->update();
        return TRUE;
    }

    AfxRefreshScreen( );
    return  FALSE ;
}

BOOL CPage_Diagnostic::DrawDiagnosticPage( QPainter * pPainter )
{
    BOOL Ret = FALSE;
    DrawDiagnosticOption(pPainter);
    switch( DiagnosticOption )
    {
    case DATAGRAM_PAGE:
        Ret = DrawDatagram(pPainter);
    break;
    case COMMUNINICATION_STATE_PAGE:
        Ret = DrawCommunState(pPainter);
    break;
    default:
        Ret = DrawDatagram(pPainter);
    break;
    }
    return Ret;
}

BOOL CPage_Diagnostic::DrawDiagnosticOption( QPainter * pPainter )
{
    pPainter->save();

    QPen pen = pPainter->pen();
    pen.setColor(QColor(255,0,0,255));
    pPainter->setPen(pen);
    QFont font = pPainter->font();
    font.setPixelSize(16);
    pPainter->setFont(font);

    pPainter->drawText( DiagnosticPageRect[CHANNEL_TEXT_RECT] , Qt::AlignCenter , UTF8("通道号:") );
    QRect tmpRect;

    switch( DiagnosticOption )
    {
    case DATAGRAM_PAGE:
        pPainter->drawPixmap( DiagnosticPageRect[DATAGRAM_RECT] , UTF8(DatagramPressPath) );
        pPainter->drawPixmap( DiagnosticPageRect[COMMUNINICATION_STATE_RECT] , UTF8(CommunStateReleasePath) );
    break;
    case COMMUNINICATION_STATE_PAGE:
        pPainter->drawPixmap( DiagnosticPageRect[DATAGRAM_RECT] , UTF8(DatagramReleasePath) );
        pPainter->drawPixmap( DiagnosticPageRect[COMMUNINICATION_STATE_RECT] , UTF8(CommunStatePressPath) );
    break;
    default:
        pPainter->drawPixmap( DiagnosticPageRect[DATAGRAM_RECT] , UTF8(DatagramPressPath) );
        pPainter->drawPixmap( DiagnosticPageRect[COMMUNINICATION_STATE_RECT] , UTF8(CommunStateReleasePath) );
    break;
    }
    pPainter->restore();
    return TRUE;
}

BOOL CPage_Diagnostic::DrawDatagram( QPainter * pPainter )
{
    pPainter->save();

    QPen pen = pPainter->pen();
    pen.setColor(QColor(255,0,0,255));
    pPainter->setPen(pen);
    QFont font = pPainter->font();
    font.setPixelSize(16);
    pPainter->setFont(font);
    QBrush brush = pPainter->brush();
    brush.setColor(QColor(96, 96, 96,255));
    brush.setStyle(Qt::SolidPattern);
    pPainter->setBrush(brush);

    QRect tmpRect;
    char TmpBuf[10] = "";

    pPainter->drawText( DiagnosticPageRect[DEVADDR_TEXT_RECT] , Qt::AlignCenter , UTF8("设备地址:") );

    tmpRect.setRect(DiagnosticPageRect[ CHECK_RECT ].x(),DiagnosticPageRect[ CHECK_RECT ].y(),
                  DiagnosticPageRect[ CHECK_RECT ].width(),
                  0.5 * DiagnosticPageRect[ CHECK_RECT ].height());
    pPainter->drawRect( tmpRect );
    if( CheckState )
        pPainter->drawText(tmpRect,UTF8("校验"));
    else
        pPainter->drawText(tmpRect,UTF8("无校验"));
    if( CheckStateRectShow )
    {
        tmpRect.setRect(DiagnosticPageRect[ CHECK_RECT ].x(),
                     DiagnosticPageRect[ CHECK_RECT ].y() + 0.5 * DiagnosticPageRect[ CHECK_RECT ].height(),
                     DiagnosticPageRect[ CHECK_RECT ].width(),
                     0.5 * DiagnosticPageRect[ CHECK_RECT ].height());
        pPainter->drawRect( tmpRect );
        if( CheckState )
            pPainter->drawText(tmpRect,UTF8("无校验"));
        else
            pPainter->drawText(tmpRect,UTF8("校验"));
    }

    tmpRect.setRect(DiagnosticPageRect[ HEX_OR_ASCII ].x(),DiagnosticPageRect[ HEX_OR_ASCII ].y(),
                  DiagnosticPageRect[ HEX_OR_ASCII ].width(),
                  0.5 * DiagnosticPageRect[ HEX_OR_ASCII ].height());
    pPainter->drawRect( tmpRect );
    if( MessageFormat == HEX_FOEMAT )
        pPainter->drawText(tmpRect,UTF8("HEX"));
    else if( MessageFormat == ASCII_FOEMAT )
        pPainter->drawText(tmpRect,UTF8("ASCII"));
    if( MessageFormatRectShow )
    {
        tmpRect.setRect(DiagnosticPageRect[ HEX_OR_ASCII ].x(),
                     DiagnosticPageRect[ HEX_OR_ASCII ].y() + 0.5 * DiagnosticPageRect[ HEX_OR_ASCII ].height(),
                     DiagnosticPageRect[ HEX_OR_ASCII ].width(),
                     0.5 * DiagnosticPageRect[ HEX_OR_ASCII ].height());
        pPainter->drawRect( tmpRect );
        if( MessageFormat == HEX_FOEMAT )
            pPainter->drawText(tmpRect,UTF8("ASCII"));
        else
            pPainter->drawText(tmpRect,UTF8("HEX"));
    }

    tmpRect.setRect(DiagnosticPageRect[ START_OR_END_RECT ].x(),DiagnosticPageRect[ START_OR_END_RECT ].y(),
                  DiagnosticPageRect[ START_OR_END_RECT ].width(),
                  0.5 * DiagnosticPageRect[ START_OR_END_RECT ].height());
    pPainter->drawRect( tmpRect );
    if( MessageShow )
        pPainter->drawText(tmpRect,UTF8("显示"));
    else
        pPainter->drawText(tmpRect,UTF8("停止"));
    if( MessageShowRectShow )
    {
        tmpRect.setRect(DiagnosticPageRect[ START_OR_END_RECT ].x(),
                     DiagnosticPageRect[ START_OR_END_RECT ].y() + 0.5 * DiagnosticPageRect[ START_OR_END_RECT ].height(),
                     DiagnosticPageRect[ START_OR_END_RECT ].width(),
                     0.5 * DiagnosticPageRect[ START_OR_END_RECT ].height());
        pPainter->drawRect( tmpRect );
        if( MessageShow )
            pPainter->drawText(tmpRect,UTF8("停止"));
        else
            pPainter->drawText(tmpRect,UTF8("显示"));
    }



    QFont tmpsavefont = pPainter->font();
    font.setPixelSize(14);
    pPainter->setFont(font);
    pPainter->drawRect( DiagnosticPageRect[ TX_RECT ] );
    memset(TmpBuf,0,sizeof(TmpBuf));
    sprintf(TmpBuf,"T:%u",(uint)(AfxGetBus(sBusNoData.DevAddrOfBus[ChannelNo].BusNo)->dwSendNo));
    pPainter->drawText(DiagnosticPageRect[ TX_RECT ],UTF8(TmpBuf));
    pPainter->drawRect( DiagnosticPageRect[ RX_RECT ] );
    memset(TmpBuf,0,sizeof(TmpBuf));
    sprintf(TmpBuf,"R:%u",(uint)(AfxGetBus(sBusNoData.DevAddrOfBus[ChannelNo].BusNo)->dwRecvNo));
    pPainter->drawText(DiagnosticPageRect[ RX_RECT ],UTF8(TmpBuf));
    pPainter->setFont(tmpsavefont);

    pPainter->restore();
    return TRUE;
}

BOOL CPage_Diagnostic::DrawCommunState( QPainter * pPainter )
{
    char TmpBuf[32] = "";
    WORD BusState=COMM_ABNORMAL;
    WORD DevState=COMM_ABNORMAL;
    pPainter->save();
    QBrush brush = pPainter->brush();
    brush.setColor(QColor(255, 0, 0,255));
    brush.setStyle(Qt::SolidPattern);
    pPainter->setBrush(brush);
    QRect tmpRect(183,87,45,25);

    if( AfxGetBusCommState(sBusNoData.DevAddrOfBus[ChannelNo].BusNo,BusState) )
    {
        if( BusState == COMM_NOMAL )
            brush.setColor(QColor(0, 255, 0,255));
        else if( BusState == COMM_ABNORMAL )
            brush.setColor(QColor(255, 0, 0,255));
        pPainter->setBrush(brush);
    }
    tmpRect.setRect(tmpRect.x(),tmpRect.y(),tmpRect.width(),tmpRect.height()+25);
    pPainter->drawRect( tmpRect );
    memset(TmpBuf,0,sizeof(TmpBuf));
    sprintf(TmpBuf,"通讯行:%d",sBusNoData.DevAddrOfBus[ChannelNo].BusNo+1);
    pPainter->drawText(tmpRect,UTF8(TmpBuf));
    tmpRect.setRect(tmpRect.x(),tmpRect.y(),tmpRect.width(),tmpRect.height()-25);

    int i=0;
    for( i=0;i<sBusNoData.DevAddrOfBus[ChannelNo].DevNum;i++ )
    {

        if( i!=0 && i%8==0 )
        {
            pPainter->drawLine(tmpRect.x()+tmpRect.width(),tmpRect.y()+tmpRect.height()/2,
                               tmpRect.x()+tmpRect.width()+20,tmpRect.y()+tmpRect.height()/2);
            tmpRect.setRect(tmpRect.x()-7*tmpRect.width()-7*20,tmpRect.y()+75,tmpRect.width(),tmpRect.height());
        }
        else
        {
            pPainter->drawLine(tmpRect.x()+tmpRect.width(),tmpRect.y()+tmpRect.height()/2,
                               tmpRect.x()+tmpRect.width()+20,tmpRect.y()+tmpRect.height()/2);
            tmpRect.setRect(tmpRect.x()+tmpRect.width()+20,tmpRect.y(),tmpRect.width(),tmpRect.height());
        }
        if( AfxGetDevCommState(sBusNoData.DevAddrOfBus[ChannelNo].BusNo,
                               sBusNoData.DevAddrOfBus[ChannelNo].DevAddr[i],
                               DevState) )
        {
            if( DevState == COMM_NOMAL )
                brush.setColor(QColor(0, 255, 0,255));
            else if( DevState == COMM_ABNORMAL )
                brush.setColor(QColor(255, 0, 0,255));
            pPainter->setBrush(brush);
        }
        pPainter->drawRect( tmpRect );
        memset(TmpBuf,0,sizeof(TmpBuf));
        sprintf(TmpBuf,"%d",sBusNoData.DevAddrOfBus[ChannelNo].DevAddr[i]);
        pPainter->drawText(tmpRect,UTF8(TmpBuf));
    }

    pPainter->restore();
    return TRUE;
}

BOOL CPage_Diagnostic::MousePressChoicePage( QMouseEvent *pEvent )
{
    if( DiagnosticPageRect[ DATAGRAM_RECT ].contains(pEvent->pos()) )
    {
        if( DiagnosticOption != DATAGRAM_PAGE )
        {
            DiagnosticOption = DATAGRAM_PAGE;
            MessageTextEdit->setVisible(true);       
            DevAddrComboBox->setVisible(true);
        }
    }
    else if( DiagnosticPageRect[ COMMUNINICATION_STATE_RECT ].contains(pEvent->pos()) )
    {
        if( DiagnosticOption != COMMUNINICATION_STATE_PAGE )
        {
            DiagnosticOption = COMMUNINICATION_STATE_PAGE;
            MessageTextEdit->setVisible(false);
            DevAddrComboBox->setVisible(false);
            MessageTextEdit->clear();
        }
    }
    else
    {
        return FALSE;
    }
    AfxGetWidget()->update();
    return TRUE;
}

BOOL CPage_Diagnostic::DisplayMsg( WORD wBusNo , WORD wAddr , BYTE byType , BYTE * pBuf , int len )
{
    if( !len || !pBuf )
        return FALSE ;
    if( byType != CGlobal::SEND_MSG && byType != CGlobal::RECV_MSG
        && byType != CGlobal::ERROR_MSG )
        return FALSE ;
    if( DiagnosticOption != DATAGRAM_PAGE )
        return FALSE;

    if( !MessageShow )
        return FALSE;

    if( wBusNo != sBusNoData.DevAddrOfBus[ChannelNo].BusNo )
    {
        if( ChannelNo != sBusNoData.BusNum )
            return FALSE;
    }

    if( wAddr != sBusNoData.DevAddrOfBus[ChannelNo].DevAddr[DevAddress] )
    {
        if( DevAddress != sBusNoData.DevAddrOfBus[ChannelNo].DevNum )
            return FALSE;
    }

    QString tmpstr;

    switch(byType)
    {
    case CGlobal::SEND_MSG:
        tmpstr = "TX:";
        break;
    case CGlobal::RECV_MSG:
        tmpstr = "RX:";
        break;
    case CGlobal::ERROR_MSG:
        tmpstr = UTF8("说明");
        break;
    default:
        return FALSE;
    }

    if( byType == CGlobal::ERROR_MSG )
    {
        if( CheckState )
        {
            tmpstr = UTF8_LEN((const char *)pBuf,len);
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if( MessageFormat == HEX_FOEMAT )
        {
            if( len > 1024 )
            {
                len = 1024;
            }
            char HexBuf[1024 * 3 + 1] ;
            memset( HexBuf , 0 , sizeof(HexBuf ) );
            char tChar[3] ;
            int i=0;
            for(i=0;i<len;i++)
            {
                sprintf(&HexBuf[i*3], "%02x ", pBuf[i]);
               // char t = pBuf[ i ] ;
               // sprintf( tChar ,"%02x ", t );
               // tChar[2] = '\0';
               // strcat( HexBuf + i * 2 , tChar ) ;
            }

            tmpstr += UTF8_LEN( HexBuf , i * 3 );
        }
        else if( MessageFormat == ASCII_FOEMAT )
        {
            tmpstr = UTF8_LEN((const char *)pBuf,len);
        }
    }
    emit signal_MessageShow(tmpstr);

    return TRUE ;
}


void CPage_Diagnostic::slots_MessageShow(QString str)
{
    time_t cur_time;
    time(&cur_time);
    str+='\n';
    m_str += str;
    if( abs(cur_time - TimeBegin) >= 1 )
    {
        MessageTextEdit->append(m_str);
        m_str.clear();
        TimeBegin = cur_time;
    }
    else
        m_str+='\n';
}

void CPage_Diagnostic::slots_BusNoChange(int index)
{
    ChannelNo = index;
    MessageTextEdit->clear();
    DevAddrComboBox->clear();
    int i=0;
    for(i=0;i<sBusNoData.DevAddrOfBus[index].DevNum;i++)
    {
        char tmpbuf[16]="";
        sprintf(tmpbuf,"%d",sBusNoData.DevAddrOfBus[index].DevAddr[i]);

        DevAddrComboBox->addItem(UTF8(tmpbuf));
    }
    DevAddrComboBox->addItem(UTF8("all"));
    DevAddrComboBox->setCurrentIndex(sBusNoData.DevAddrOfBus[index].DevNum);
    slots_DevAddrChange(sBusNoData.DevAddrOfBus[index].DevNum);
}

void CPage_Diagnostic::slots_DevAddrChange(int index)
{
    MessageTextEdit->clear();
    DevAddress = index;
}

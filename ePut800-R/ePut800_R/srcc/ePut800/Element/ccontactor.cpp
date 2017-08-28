#include "ccontactor.h"

CContactor::CContactor()
{
    m_wObjectType = GENERATRIX ;
    m_rcDraw = QRect( 0 , 0 , 0 , 0 );
    m_byOpen = BREAKER_OPEN;
    m_byLeftTopPower = POWER_OFF;
    m_byRightBottomPower = POWER_OFF;
    m_IsDirection = HORZ_DRAW;

    CreateMenu();
}

BOOL CContactor::DrawElemnt( QPainter *pPainter )
{
    if( !pPainter)
    {
        Q_ASSERT( FALSE ) ;
        return FALSE ;
    }

    pPainter->save();


    if( m_IsDirection == VER_DRAW )
    {
      //垂直方向画图
      DrawVertical( pPainter ) ;
    }
    else
    {
        //水平方向画图
        DrawHorizontal(pPainter);
    }
    DrawIsChoice(pPainter);

    //pPainter->drawRect( m_rcDraw );
    pPainter->restore();
    return TRUE ;

}

BOOL CContactor::InitData( )
{
    m_IsDirection = m_wDirect;
    InitMenu();
    InitYkData();
    return TRUE ;
}

BOOL CContactor::Create( QRect rc )
{
    m_rcDraw = rc ;
    return TRUE ;
}

void CContactor::CreateMenu()
{
    m_Menu = new QMenu(AfxGetWidget());
    QAction * YkSet_action = m_Menu->addAction(UTF8("遥控执行"));
    QAction * Yx_action = m_Menu->addAction(UTF8("遥信数据"));
    m_YxTab = new QTableWidget();
    m_YxTab->setWindowFlags((m_YxTab->windowFlags()|Qt::WindowStaysOnTopHint| Qt::WindowCloseButtonHint) &~Qt::WindowMinMaxButtonsHint);
    m_YxTab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_YxTab->setFixedSize(400,300);
    m_YxTab->move(200,150);
    m_YxTab->setColumnCount(3);
    m_YxTab->setRowCount(20);
    m_YxTab->setColumnWidth(0,200);
    m_YxTab->setColumnWidth(1,100);
    m_YxTab->setColumnWidth(2,100);
    QStringList Yx_header;
    Yx_header<<UTF8("名称")<<UTF8("点号")<<UTF8("值");
    m_YxTab->setHorizontalHeaderLabels(Yx_header);

    QAction * Yc_action = m_Menu->addAction(UTF8("遥测数据"));
    m_YcTab = new QTableWidget();
    m_YcTab->setWindowFlags((m_YcTab->windowFlags()|Qt::WindowStaysOnTopHint| Qt::WindowCloseButtonHint) &~Qt::WindowMinMaxButtonsHint);
    m_YcTab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_YcTab->setFixedSize(400,300);
    m_YcTab->move(200,150);
    m_YcTab->setColumnCount(3);
    m_YcTab->setRowCount(20);
    m_YcTab->setColumnWidth(0,200);
    m_YcTab->setColumnWidth(1,100);
    m_YcTab->setColumnWidth(2,100);
    QStringList Yc_header;
    Yc_header<<UTF8("名称")<<UTF8("点号")<<UTF8("值");
    m_YcTab->setHorizontalHeaderLabels(Yc_header);

    QAction * Yk_action = m_Menu->addAction(UTF8("遥控数据"));
    m_YkTab = new QTableWidget();
    m_YkTab->setWindowFlags((m_YkTab->windowFlags()|Qt::WindowStaysOnTopHint| Qt::WindowCloseButtonHint) &~Qt::WindowMinMaxButtonsHint);
    m_YkTab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_YkTab->setFixedSize(400,300);
    m_YkTab->move(200,150);
    m_YkTab->setColumnCount(2);
    m_YkTab->setRowCount(20);
    m_YkTab->setColumnWidth(0,250);
    m_YkTab->setColumnWidth(1,100);
    QStringList Yk_header;
    Yk_header<<UTF8("名称")<<UTF8("点号");
    m_YkTab->setHorizontalHeaderLabels(Yk_header);

    connect(m_Menu, SIGNAL(triggered(QAction*)), this, SLOT(slots_QMenuAction(QAction*)));
}

void CContactor::InitMenu( )
{
    char BusAddrPnt[15] = "";

    m_YxTab->setRowCount(Yx_ElementAttributeArray.size());
    int Yx_AttributeDataSize = Yx_ElementAttributeArray.size();

    int Yx_AttributeDataNo = 0;
    for( Yx_AttributeDataNo = 0; Yx_AttributeDataNo < Yx_AttributeDataSize; Yx_AttributeDataNo++ )
    {
        PPIC_ATTRIBUTE_DATA_ELEMENT tmpData = Yx_ElementAttributeArray.at(Yx_AttributeDataNo);
        switch( tmpData->byDataType )
        {
        case PIC_ATTRIBUTE_DATA_ELEMENT::YX_DTYPE:
            tmpData->dbVal = 0;
            m_YxTab->setItem(Yx_AttributeDataNo,0,new QTableWidgetItem(tmpData->name));
            memset(BusAddrPnt,0,sizeof(BusAddrPnt));
            sprintf(BusAddrPnt,"P%d,%d,%d",tmpData->wBusNo,tmpData->wAddr,tmpData->wPntNo);
            m_YxTab->setItem(Yx_AttributeDataNo,1,new QTableWidgetItem(BusAddrPnt));
            memset(BusAddrPnt,0,sizeof(BusAddrPnt));
            sprintf(BusAddrPnt,"%u",(BYTE)tmpData->dbVal);
            m_YxTab->setItem(Yx_AttributeDataNo,2,new QTableWidgetItem(BusAddrPnt));
            break;
        default:
            continue;
        }
    }

    m_YcTab->setRowCount(Yc_ElementAttributeArray.size());
    int Yc_AttributeDataSize = Yc_ElementAttributeArray.size();

    int Yc_AttributeDataNo = 0;
    for( Yc_AttributeDataNo = 0; Yc_AttributeDataNo < Yc_AttributeDataSize; Yc_AttributeDataNo++ )
    {
        PPIC_ATTRIBUTE_DATA_ELEMENT tmpData = Yc_ElementAttributeArray.at(Yc_AttributeDataNo);
        switch( tmpData->byDataType )
        {
        case PIC_ATTRIBUTE_DATA_ELEMENT::YC_DTYPE:
            tmpData->dbVal = 0;
            m_YcTab->setItem(Yc_AttributeDataNo,0,new QTableWidgetItem(tmpData->name));
            memset(BusAddrPnt,0,sizeof(BusAddrPnt));
            sprintf(BusAddrPnt,"P%d,%d,%d",tmpData->wBusNo,tmpData->wAddr,tmpData->wPntNo);
            m_YcTab->setItem(Yc_AttributeDataNo,1,new QTableWidgetItem(BusAddrPnt));
            m_YcTab->setItem(Yc_AttributeDataNo,2,new QTableWidgetItem(tmpData->dbVal));
            break;
        default:
            continue;
        }
    }

    m_YkTab->setRowCount(Yk_ElementAttributeArray.size());
    int Yk_AttributeDataSize = Yk_ElementAttributeArray.size();

    int Yk_AttributeDataNo = 0;
    for( Yk_AttributeDataNo = 0; Yk_AttributeDataNo < Yk_AttributeDataSize; Yk_AttributeDataNo++ )
    {
       PPIC_ATTRIBUTE_DATA_ELEMENT tmpData = Yk_ElementAttributeArray.at(Yk_AttributeDataNo);
       switch( tmpData->byDataType )
       {
       case PIC_ATTRIBUTE_DATA_ELEMENT::YK_DTYPE:
           tmpData->dbVal = 0;
           m_YkTab->setItem(Yk_AttributeDataNo,0,new QTableWidgetItem(tmpData->name));
           memset(BusAddrPnt,0,sizeof(BusAddrPnt));
           sprintf(BusAddrPnt,"P%d,%d,%d",tmpData->wBusNo,tmpData->wAddr,tmpData->wPntNo);
           m_YkTab->setItem(Yk_AttributeDataNo,1,new QTableWidgetItem(BusAddrPnt));
       default:
           continue;
       }
    }

}


void CContactor::DrawVertical( QPainter *pPainter )
{
    if( !pPainter || m_rcDraw.isNull() )
    {
        Q_ASSERT( FALSE ) ;
        return  ;
    }

    if( m_rcDraw.isEmpty() )
        return  ;
    //draw Breaker
    DrawVerticalTop( pPainter ) ;
    DrawVerticalBottom( pPainter ) ;
}

void CContactor::DrawVerticalTop( QPainter * pPainter )
{
    if( !pPainter || m_rcDraw.isNull() )
    {
        Q_ASSERT( FALSE ) ;
        return  ;
    }

    if( m_rcDraw.isEmpty() )
        return  ;

    pPainter->save();

    QPen pen ;
    pen.setStyle( Qt::SolidLine );
    if( m_byLeftTopPower == POWER_ON )
        pen.setColor( QColor( 255 , 0 , 0 , 255 ));
    else
        pen.setColor( QColor( 0 , 255 , 0 , 255 ) );

    pPainter->setPen( pen );

    QPainterPath BeginPath;
    BeginPath.moveTo(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y());
    BeginPath.lineTo(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()/3);
    BeginPath.cubicTo(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()/3,
                 m_rcDraw.x()+m_rcDraw.width()/3,m_rcDraw.y()+m_rcDraw.height()*5/18,
                 m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()*2/9);

    pPainter->drawPath(BeginPath);
    pPainter->restore();
}

void CContactor::DrawVerticalBottom( QPainter * pPainter )
{
    if( !pPainter || m_rcDraw.isNull() )
    {
        Q_ASSERT( FALSE ) ;
        return  ;
    }

    if( m_rcDraw.isEmpty() )
        return  ;

    pPainter->save();

    QPen pen ;
    pen.setStyle( Qt::SolidLine );
    if( m_byRightBottomPower == POWER_ON )
        pen.setColor( QColor( 255 , 0 , 0 , 255 ));
    else
        pen.setColor( QColor( 0 , 255 , 0 , 255 ) );

    pPainter->setPen( pen );

    WORD wHight = m_rcDraw.height();
    WORD wWidth = m_rcDraw.width() ;
     WORD cy = m_rcDraw.top();
    WORD wXMoveTo = 0 ;
    WORD wYMoveTo = 0 ;
    WORD wXLineTo = 0 ;
    WORD wYLineTo = 0 ;

    if( BREAKER_OPEN == m_byOpen )
    {
       wXMoveTo = m_rcDraw.left() + wWidth * 1 / 8 ;
       wYMoveTo = cy + wHight / 3 ;
       wXLineTo = m_rcDraw.left() + wWidth * 4 / 8 ;
       wYLineTo = cy + wHight * 2 / 3 ;
   }
   else if( BREAKER_CLOSE == m_byOpen )
   {
       wXMoveTo = m_rcDraw.center().x();
       wYMoveTo = cy + wHight / 3 ;
       wXLineTo = m_rcDraw.center().x() ;
       wYLineTo = cy + wHight * 2 / 3 ;
   }
   else
       Q_ASSERT( FALSE ) ;

    pPainter->drawLine(QPoint(wXMoveTo , wYMoveTo ) , QPoint( wXLineTo , wYLineTo ) );

    wXMoveTo = wXLineTo  ;
    wYMoveTo = wYLineTo;
    wXLineTo = wXLineTo ;
    wYLineTo = m_rcDraw.bottom()  ;
    pPainter->drawLine(QPoint(wXMoveTo , wYMoveTo ) , QPoint( wXLineTo , wYLineTo ) );

    pPainter->restore();
}

void CContactor::DrawHorizontal( QPainter *pPainter )
{
    if( !pPainter || m_rcDraw.isNull() )
    {
        Q_ASSERT( FALSE ) ;
        return  ;
    }

    if( m_rcDraw.isEmpty() )
        return  ;

    //draw Breaker
    DrawHorizontalTop( pPainter ) ;
    DrawHorizontalBottom( pPainter ) ;


}

void CContactor::DrawHorizontalTop( QPainter * pPainter )
{
    if( !pPainter || m_rcDraw.isNull() )
    {
        Q_ASSERT( FALSE ) ;
        return  ;
    }

    if( m_rcDraw.isEmpty() )
        return  ;

    pPainter->save();

    QPen pen ;
    pen.setStyle( Qt::SolidLine );
    if( m_byLeftTopPower == POWER_ON )
        pen.setColor( QColor( 255 , 0 , 0 , 255 ));
    else
        pen.setColor( QColor( 0 , 255 , 0 , 255 ) );

    pPainter->setPen( pen );

    QPainterPath BeginPath;

    BeginPath.moveTo(m_rcDraw.x(),m_rcDraw.y()+m_rcDraw.height()/2);
    BeginPath.lineTo(m_rcDraw.x()+m_rcDraw.width()/3,m_rcDraw.y()+m_rcDraw.height()/2);
    BeginPath.cubicTo(m_rcDraw.x()+m_rcDraw.width()/3,m_rcDraw.y()+m_rcDraw.height()/2,
                     m_rcDraw.x()+m_rcDraw.width()*5/18,m_rcDraw.y()+m_rcDraw.height()*2/3,
                     m_rcDraw.x()+m_rcDraw.width()*2/9,m_rcDraw.y()+m_rcDraw.height()/2);
    pPainter->drawPath(BeginPath);
    pPainter->restore();
}

void CContactor::DrawHorizontalBottom( QPainter * pPainter )
{
    if( !pPainter || m_rcDraw.isNull() )
    {
        Q_ASSERT( FALSE ) ;
        return  ;
    }

    if( m_rcDraw.isEmpty() )
        return  ;

    pPainter->save();

    QPen pen ;
    pen.setStyle( Qt::SolidLine );
    if( m_byRightBottomPower == POWER_ON )
        pen.setColor( QColor( 255 , 0 , 0 , 255 ));
    else
        pen.setColor( QColor( 0 , 255 , 0 , 255 ) );

    pPainter->setPen( pen );

    WORD wHight = m_rcDraw.height();
    WORD wWidth = m_rcDraw.width() ;
     WORD cy = m_rcDraw.left();
    WORD wXMoveTo = 0 ;
    WORD wYMoveTo = 0 ;
    WORD wXLineTo = 0 ;
    WORD wYLineTo = 0 ;

    if( BREAKER_OPEN == m_byOpen )
    {
       wXMoveTo = cy + wWidth / 3 ;
       wYMoveTo = m_rcDraw.top() + wHight * 5 / 8 ;
       wXLineTo = cy + wWidth * 2 / 3 ;
       wYLineTo = m_rcDraw.top() + wHight * 4 / 8;
   }
   else if( BREAKER_CLOSE == m_byOpen )
   {
       wXMoveTo = cy + wWidth / 3 ;
       wYMoveTo = m_rcDraw.center().y();
       wXLineTo = cy + wWidth * 2 / 3 ;
       wYLineTo = m_rcDraw.center().y() ;
   }
   else
       Q_ASSERT( FALSE ) ;

    pPainter->drawLine(QPoint(wXMoveTo , wYMoveTo ) , QPoint( wXLineTo , wYLineTo ) );

    wXMoveTo = wXLineTo ;
    wYMoveTo = wYLineTo ;
    wXLineTo = m_rcDraw.right() ;
    wYLineTo = wYLineTo ;
    pPainter->drawLine(QPoint(wXMoveTo , wYMoveTo ) , QPoint( wXLineTo , wYLineTo ) );

    pPainter->restore();
}

void CContactor::DrawIsChoice( QPainter * pPainter )
{
    if( m_byIsChoice )
    {
        pPainter->save();
        QPen pen = pPainter->pen();
        pen.setStyle( Qt::SolidLine );
        pen.setColor( QColor( 128 , 128 , 128 , 100 ) );
        pPainter->setPen( pen );

        QBrush brush = pPainter->brush();
        brush.setStyle(Qt::SolidPattern);
        brush.setColor( QColor( 128 , 128 , 128 , 100 ) );
        pPainter->setBrush(brush);
        pPainter->drawRect( m_rcDraw );
        pPainter->restore();
    }
}

BOOL CContactor::CompareAttributeData( )
{
    int AttributeDataSize = Yx_ElementAttributeArray.size();
    if( !AttributeDataSize )
        return FALSE;

    int AttributeDataNo = 0;
    for( AttributeDataNo = 0; AttributeDataNo < AttributeDataSize; AttributeDataNo++ )
    {
        PPIC_ATTRIBUTE_DATA_ELEMENT tmpData = Yx_ElementAttributeArray.at(AttributeDataNo);
        switch( tmpData->byDataType )
        {
        case PIC_ATTRIBUTE_DATA_ELEMENT::YX_DTYPE:
            if( tmpData->byAttributeDataByte == 1 )
            {
                BYTE byVal = 0;
                AfxGetYx(tmpData->wBusNo,tmpData->wAddr,tmpData->wPntNo,byVal);
                if( tmpData->dbVal != byVal )
                {
                    if( byVal == 1 )
                        m_byOpen = BREAKER_CLOSE;
                    else
                        m_byOpen = BREAKER_OPEN;
                    tmpData->dbVal = byVal;
                    return TRUE;
                }
            }
            break;
        default:
            continue;
        }
    }

    return FALSE;
}

BOOL CContactor::ProcessMouseDown( QMouseEvent * pEvent )
{
    m_byIsChoice = TRUE;
    AfxGetWidget()->update(m_rcDraw);
    m_Menu->exec(pEvent->globalPos());
    m_byIsChoice = FALSE;
    AfxGetWidget()->update(m_rcDraw);
    return TRUE;
}

void CContactor::slots_QMenuAction(QAction * action)
{
    if( action == NULL )
        return;
    if( action->text() == UTF8("遥控执行") )
    {
        BYTE bySetBreakStatus = -1;

        QString str;
        switch( m_byOpen )
        {
        case BREAKER_CLOSE:
            bySetBreakStatus = BREAKER_OPEN;
            str = QString("是否执行分闸操作!!!");
            break;
        case BREAKER_OPEN:
            bySetBreakStatus = BREAKER_CLOSE;
            str = QString("是否执行合闸操作!!!");
            break;
        default:
            break;
        }
        QMessageBox box(QMessageBox::Question,"询问",str);
        box.setStandardButtons (QMessageBox::Yes|QMessageBox::No);
        box.setButtonText (QMessageBox::Yes,QString(UTF8("确认")));
        box.setButtonText (QMessageBox::No,QString(UTF8("取消")));
        switch( box.exec() )
        {
            case QMessageBox::Yes:
                SetAction(bySetBreakStatus);
                break;
            case QMessageBox::No:
                // 不保存退出
                break;
            default:
                break;
        }
    }
    else
        TableShow(action);
}


void CContactor::SetAction(BYTE Action)
{
    //WORD wBusNoDst = -1;
    //WORD wDevAddrDst = -1;
    //WORD wPntDst = -1;
    BYTE byYkSetState = FALSE;
//    BYTE byStatus = -1;

//    int AttributeDataNo = 0;
//    int AttributeDataSize = Yk_ElementAttributeArray.size();

    if( AfxGetLoginState() == CPage::LOGOUT )
    {
        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("尚未登录，请登录后操作!!!"),QMessageBox::Yes );
        return;
    }

    switch( Action )
    {
    case BREAKER_OPEN:
        YkActionType = YKACTIONOPEN;
        if( YkData[YkActionType].wVal == YK_OPEN )
            byYkSetState = TRUE;
    break;
    case BREAKER_CLOSE:
        YkActionType = YKACTIONCLOSE;
        if( YkData[YKACTIONCLOSE].wVal == YK_CLOSE )
            byYkSetState = TRUE;
    break;
    default:
    break;
    }

    if( !byYkSetState )
    {
        QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("没有配置对应操作遥控点"),QMessageBox::Yes );
        return ;
    }
    else
    {
        AfxGetWidget()->YkDlg->tShowParameter(TWOPOS,YkData[YkActionType]);
    }
}

BOOL CContactor::IsChangeColor( )
{
    if( m_byOpen == BREAKER_CLOSE )
    {
        if( m_wObjectNodeNext == m_wObjectNodeL )
        {
            m_byLeftTopPower = m_byRightBottomPower;
            m_wObjectNodeNextState = m_byRightBottomPower;
        }
        else if( m_wObjectNodeNext == m_wObjectNodeR )
        {
            m_byRightBottomPower = m_byLeftTopPower;
            m_wObjectNodeNextState = m_byLeftTopPower;
        }
    }
    else if( m_byOpen == BREAKER_OPEN )
    {
        if( m_wObjectNodeNext == m_wObjectNodeL )
            m_byLeftTopPower = POWER_OFF;
        else if( m_wObjectNodeNext == m_wObjectNodeR )
            m_byRightBottomPower = POWER_OFF;
        m_wObjectNodeNextState = POWER_OFF;
    }
    return TRUE;
}

BOOL CContactor::SetChangeNodeColor(WORD ChangeNode, BYTE PowerState)
{
    m_byLeftTopPower = POWER_OFF;
    m_byRightBottomPower = POWER_OFF;
    if( ChangeNode == m_wObjectNodeL )
    {
        m_byLeftTopPower = PowerState;
        if( m_byOpen == BREAKER_CLOSE )
        {
            m_byRightBottomPower = m_byLeftTopPower;
            m_wObjectNodeNextState = m_byLeftTopPower;
        }
        else if( m_byOpen == BREAKER_OPEN )
            m_wObjectNodeNextState = POWER_OFF;

        m_wObjectNodeNext = m_wObjectNodeR;
    }
    else if( ChangeNode == m_wObjectNodeR )
    {
        m_byRightBottomPower = PowerState;
        if( m_byOpen == BREAKER_CLOSE )
        {
            m_byLeftTopPower = m_byRightBottomPower;
            m_wObjectNodeNextState = m_byLeftTopPower;
        }
        else if( m_byOpen == BREAKER_OPEN )
            m_wObjectNodeNextState = POWER_OFF;

        m_wObjectNodeNext = m_wObjectNodeL;
    }
    else
        return FALSE;
    return TRUE;
}

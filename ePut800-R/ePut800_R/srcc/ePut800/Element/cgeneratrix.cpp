#include "cgeneratrix.h"

CGeneratrix::CGeneratrix()
{
    m_wObjectType = GENERATRIX ;
    m_rcDraw = QRect( 0 , 0 , 0 , 0 );
    m_IsPowerOn = POWER_OFF ;

    CreateMenu();
}

BOOL CGeneratrix::DrawElemnt( QPainter *pPainter )
{
    if( !pPainter )
    {
        Q_ASSERT( FALSE );
        return FALSE ;
    }

    pPainter->save();

    //m_rcDraw = QRect( 10 ,10 , 100 , 10 );
    QBrush brush ;
    brush.setStyle(Qt::SolidPattern);
    if( m_IsPowerOn == POWER_ON )
        brush.setColor( QColor( 255 ,0 , 0 , 255 ) );
    else
        brush.setColor( QColor( 0 , 255 , 0 , 255 ) );

    QPen pen( Qt::NoPen );
    pPainter->setBackgroundMode( Qt::TransparentMode );
    pPainter->setBrush( brush );
    pPainter->setPen( pen );


    pPainter->drawRect( m_rcDraw );
    pPainter->restore();
    return TRUE ;
}

BOOL CGeneratrix::InitData( )
{
    InitMenu();
    return TRUE ;
}

BOOL CGeneratrix::Create( QRect rc )
{
    if( !rc.isEmpty() )
        m_rcDraw = rc ;
    else
        m_rcDraw = QRect( 1 ,1, 2 ,2 ) ;

    return TRUE ;
}

void CGeneratrix::CreateMenu()
{
    m_Menu = new QMenu(AfxGetWidget());
    QAction * action = m_Menu->addAction(UTF8("遥测数据"));

    m_YcTab = new QTableWidget();
    m_YcTab->setWindowFlags((m_YcTab->windowFlags()|Qt::WindowStaysOnTopHint| Qt::WindowCloseButtonHint) &~Qt::WindowMinMaxButtonsHint);
    m_YcTab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_YcTab->setFixedSize(400,300);
    m_YcTab->move(200,150);
    m_YcTab->setColumnCount(3);
    m_YcTab->setRowCount(Yc_ElementAttributeArray.size());
    m_YcTab->setColumnWidth(0,200);
    m_YcTab->setColumnWidth(1,100);
    m_YcTab->setColumnWidth(2,100);

    QStringList header;
    header<<UTF8("名称")<<UTF8("点号")<<UTF8("值");
    m_YcTab->setHorizontalHeaderLabels(header);


    connect(m_Menu, SIGNAL(triggered(QAction*)), this, SLOT(slots_QMenuAction(QAction*)));

}

void CGeneratrix::InitMenu( )
{
    char BusAddrPnt[15] = "";

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
}

BOOL CGeneratrix::ProcessMouseDown( QMouseEvent * pEvent )
{
    m_byIsChoice = TRUE;
    AfxGetWidget()->update(m_rcDraw);
    m_Menu->exec(pEvent->globalPos());
    m_byIsChoice = FALSE;
    AfxGetWidget()->update(m_rcDraw);
    return TRUE;
}

void CGeneratrix::slots_QMenuAction(QAction * action)
{
    TableShow(action);
}

BOOL CGeneratrix::SetChangeNodeColor(WORD ChangeNode, BYTE PowerState)
{
    if( ChangeNode == m_wObjectNodeL )
    {
        m_IsPowerOn = POWER_ON;
        return TRUE;
    }
    return FALSE;
}

#include "clinker.h"

CLinker::CLinker()
{
    m_wObjectType = LINKER;
    m_rcDraw = QRect( 0 , 0 , 0 , 0 );
    m_IsPowerOn = POWER_OFF ;
    m_IsDirection = VER_DRAW;
}

BOOL CLinker::DrawElemnt( QPainter *pPainter )
{
    if( !pPainter )
    {
        Q_ASSERT( FALSE );
        return FALSE ;
    }

    pPainter->save();

    QPen pen = pPainter->pen();
    pen.setStyle(Qt::NoPen );
    pPainter->setPen(pen);

    QBrush brush = pPainter->brush();
    brush.setStyle(Qt::SolidPattern);
    if( m_IsPowerOn == POWER_ON )
        brush.setColor( Qt::red );
    else
        brush.setColor( Qt::green );

    pPainter->setBrush(brush);
//    if( m_IsDirection == HORZ_DRAW )
//    {
//        pPainter->drawLine(m_rcDraw.x(),m_rcDraw.y()+m_rcDraw.height()/2,
//                           m_rcDraw.x()+m_rcDraw.width(),m_rcDraw.y()+m_rcDraw.height()/2);
//    }
//    else
//    {
//        pPainter->drawLine(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y(),
//                           m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height());
//    }


    pPainter->drawRect( m_rcDraw );
    pPainter->restore();
    return TRUE ;
}

BOOL CLinker::InitData( )
{
    return FALSE ;
}

BOOL CLinker::Create( QRect rc )
{
    m_rcDraw = rc ;
    return TRUE ;
}

BOOL CLinker::SetChangeNodeColor(WORD ChangeNode, BYTE PowerState)
{
    if( ChangeNode == m_wObjectNodeL )
    {
        m_IsPowerOn = PowerState;
        return TRUE;
    }
    return FALSE;
}

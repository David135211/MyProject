#include "cpower.h"

CPower::CPower()
{
    m_wObjectType = LINKER;
    m_rcDraw = QRect( 0 , 0 , 0 , 0 );
    m_IsPowerOn = POWER_OFF ;
    m_IsDirection = VER_DRAW;
}

BOOL CPower::DrawElemnt( QPainter *pPainter )
{
    if( !pPainter )
    {
        Q_ASSERT( FALSE );
        return FALSE ;
    }

    pPainter->save();

    if( m_IsPowerOn == POWER_ON )
        pPainter->setPen( Qt::red );
    else
        pPainter->setPen( Qt::green );

    QPainterPath path;
    path.addEllipse( m_rcDraw );
    if( m_IsDirection == HORZ_DRAW )
    {
        path.moveTo(m_rcDraw.x()+m_rcDraw.width()/4,m_rcDraw.y()+m_rcDraw.height()/2);
        path.cubicTo(m_rcDraw.x()+m_rcDraw.width()/4,m_rcDraw.y()+m_rcDraw.height()/2,
                     m_rcDraw.x()+m_rcDraw.width()*3/8,m_rcDraw.y()+m_rcDraw.height()*3/8,
                     m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()/2);
        path.cubicTo(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()/2,
                     m_rcDraw.x()+m_rcDraw.width()*5/8,m_rcDraw.y()+m_rcDraw.height()*5/8,
                     m_rcDraw.x()+m_rcDraw.width()*3/4,m_rcDraw.y()+m_rcDraw.height()/2);
    }
    else
    {
        path.moveTo(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()/4);
        path.cubicTo(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()/4,
                     m_rcDraw.x()+m_rcDraw.width()*5/8,m_rcDraw.y()+m_rcDraw.height()*3/8,
                     m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()/2);
        path.cubicTo(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()/2,
                     m_rcDraw.x()+m_rcDraw.width()*3/8,m_rcDraw.y()+m_rcDraw.height()*5/8,
                     m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()*3/4);
    }

    pPainter->drawPath(path);

    //pPainter->drawRect( m_rcDraw );
    pPainter->restore();
    return TRUE ;
}

BOOL CPower::InitData( )
{
    return FALSE ;
}

BOOL CPower::Create( QRect rc )
{
    m_rcDraw = rc ;
    return TRUE ;
}

BOOL CPower::IsChangeColor( )
{
    if( m_IsPowerOn == POWER_OFF )
    {
        m_IsPowerOn = POWER_ON;
        m_wObjectNodeNext = m_wObjectNodeL;
        m_wObjectNodeNextState = POWER_ON;
        return TRUE;
    }
    return TRUE;
}

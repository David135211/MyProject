#include "clightning_protector.h"

CLightning_protector::CLightning_protector()
{
    m_wObjectType = LINKER;
    m_rcDraw = QRect( 0 , 0 , 0 , 0 );
    m_IsPowerOn = POWER_OFF ;
    m_IsDirection = VER_DRAW;
}

BOOL CLightning_protector::DrawElemnt( QPainter *pPainter )
{
    if( !pPainter )
    {
        Q_ASSERT( FALSE );
        return FALSE ;
    }

    pPainter->save();

    if( m_IsPowerOn == POWER_ON )
    {
        pPainter->setPen( Qt::red );
    }
    else
    {
        pPainter->setPen( Qt::green );
    }

    QPainterPath path;

    if( m_IsDirection == HORZ_DRAW )
    {
        path.addRect( m_rcDraw.x()+m_rcDraw.width()/4,m_rcDraw.y(),m_rcDraw.width()/2,m_rcDraw.height());
        path.moveTo(m_rcDraw.x(),m_rcDraw.y()+m_rcDraw.height()/2);
        path.lineTo(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()/2);
        path.lineTo(m_rcDraw.x()+m_rcDraw.width()*3/8,m_rcDraw.y()+m_rcDraw.height()/4);
        path.moveTo(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()/2);
        path.lineTo(m_rcDraw.x()+m_rcDraw.width()*3/8,m_rcDraw.y()+m_rcDraw.height()*3/4);
        path.moveTo(m_rcDraw.x()+m_rcDraw.width()*3/4,m_rcDraw.y()+m_rcDraw.height()/2);
        path.lineTo(m_rcDraw.x()+m_rcDraw.width()*7/8,m_rcDraw.y()+m_rcDraw.height()/2);
        path.moveTo(m_rcDraw.x()+m_rcDraw.width()*7/8,m_rcDraw.y()+m_rcDraw.height()/8);
        path.lineTo(m_rcDraw.x()+m_rcDraw.width()*7/8,m_rcDraw.y()+m_rcDraw.height()*7/8);
        path.moveTo(m_rcDraw.x()+m_rcDraw.width()*15/16,m_rcDraw.y()+m_rcDraw.height()/4);
        path.lineTo(m_rcDraw.x()+m_rcDraw.width()*15/16,m_rcDraw.y()+m_rcDraw.height()*3/4);
        path.moveTo(m_rcDraw.x()+m_rcDraw.width(),m_rcDraw.y()+m_rcDraw.height()*3/8);
        path.lineTo(m_rcDraw.x()+m_rcDraw.width(),m_rcDraw.y()+m_rcDraw.height()*5/8);
    }
    else
    {
        path.addRect( m_rcDraw.x(),m_rcDraw.y()+m_rcDraw.height()/4,m_rcDraw.width(),m_rcDraw.height()/2 );
        path.moveTo(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y());
        path.lineTo(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()/2);
        path.lineTo(m_rcDraw.x()+m_rcDraw.width()/4,m_rcDraw.y()+m_rcDraw.height()*3/8);
        path.moveTo(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()/2);
        path.lineTo(m_rcDraw.x()+m_rcDraw.width()*3/4,m_rcDraw.y()+m_rcDraw.height()*3/8);
        path.moveTo(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()*3/4);
        path.lineTo(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height()*7/8);
        path.moveTo(m_rcDraw.x()+m_rcDraw.width()/8,m_rcDraw.y()+m_rcDraw.height()*7/8);
        path.lineTo(m_rcDraw.x()+m_rcDraw.width()*7/8,m_rcDraw.y()+m_rcDraw.height()*7/8);
        path.moveTo(m_rcDraw.x()+m_rcDraw.width()/4,m_rcDraw.y()+m_rcDraw.height()*15/16);
        path.lineTo(m_rcDraw.x()+m_rcDraw.width()*3/4,m_rcDraw.y()+m_rcDraw.height()*15/16);
        path.moveTo(m_rcDraw.x()+m_rcDraw.width()*3/8,m_rcDraw.y()+m_rcDraw.height());
        path.lineTo(m_rcDraw.x()+m_rcDraw.width()*5/8,m_rcDraw.y()+m_rcDraw.height());
    }

    pPainter->drawPath(path);


    pPainter->restore();
   // pPainter->drawRect( m_rcDraw );
    return TRUE ;
}

BOOL CLightning_protector::InitData( )
{
    return FALSE ;
}

BOOL CLightning_protector::Create( QRect rc )
{
    m_rcDraw = rc ;
    return TRUE ;
}

BOOL CLightning_protector::SetChangeNodeColor(WORD ChangeNode, BYTE PowerState)
{
    if( ChangeNode == m_wObjectNodeL )
    {
        m_IsPowerOn = PowerState;
        return TRUE;
    }
    return FALSE;
}

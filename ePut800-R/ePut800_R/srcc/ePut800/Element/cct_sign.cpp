#include "cct_sign.h"

CCt_sign::CCt_sign()
{
    m_wObjectType = GENERATRIX ;
    m_rcDraw = QRect( 0 , 0 , 0 , 0 );
    m_IsPowerOn = POWER_OFF ;
    m_IsDirection = VER_DRAW;
}

BOOL CCt_sign::DrawElemnt( QPainter *pPainter )
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
    if( m_IsDirection == HORZ_DRAW )
    {
        pPainter->drawEllipse(m_rcDraw.x()+m_rcDraw.width()/8,m_rcDraw.y(),
                              m_rcDraw.width()*3/4,m_rcDraw.height());
        pPainter->drawLine(m_rcDraw.x(),m_rcDraw.y()+m_rcDraw.height()/2,
                           m_rcDraw.x()+m_rcDraw.width(),m_rcDraw.y()+m_rcDraw.height()/2);
    }
    else
    {
        pPainter->drawEllipse(m_rcDraw.x(),m_rcDraw.y()+m_rcDraw.height()/8,
                              m_rcDraw.width(),m_rcDraw.height()*3/4);
        pPainter->drawLine(m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y(),
                           m_rcDraw.x()+m_rcDraw.width()/2,m_rcDraw.y()+m_rcDraw.height());
    }

    //pPainter->drawRect( m_rcDraw );
    pPainter->restore();
    return TRUE ;
}

BOOL CCt_sign::InitData( )
{
    return FALSE ;
}

BOOL CCt_sign::Create( QRect rc )
{
    m_rcDraw = rc ;
    return TRUE ;
}

BOOL CCt_sign::SetChangeNodeColor(WORD ChangeNode, BYTE PowerState)
{
    if( ChangeNode == m_wObjectNodeL )
    {
        m_IsPowerOn = PowerState;
        return TRUE;
    }
    return FALSE;
}

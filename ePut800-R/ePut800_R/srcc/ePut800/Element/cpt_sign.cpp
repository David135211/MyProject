#include "cpt_sign.h"
#include <math.h>

CPt_sign::CPt_sign()
{
    m_wObjectType = GENERATRIX ;
    m_rcDraw = QRect( 0 , 0 , 0 , 0 );
    m_IsPowerOn = POWER_OFF ;
    m_IsDirection = VER_DRAW;
}

BOOL CPt_sign::DrawElemnt( QPainter *pPainter )
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
        pPainter->drawEllipse(m_rcDraw.x()+m_rcDraw.width()*sqrt(2)/(2+sqrt(2))*4/5+m_rcDraw.width()/10,
                              m_rcDraw.y(),
                              m_rcDraw.width()*2/(2+sqrt(2))*4/5,m_rcDraw.height()*2/(2+sqrt(2)));
        pPainter->drawEllipse(m_rcDraw.x()+m_rcDraw.width()/10,
                              m_rcDraw.y()+m_rcDraw.height()*sqrt(2)/(2+sqrt(2)),
                              m_rcDraw.width()*2/(2+sqrt(2))*4/5,m_rcDraw.height()*2/(2+sqrt(2)));
        pPainter->drawEllipse(m_rcDraw.x()+m_rcDraw.width()*sqrt(2)/(2+sqrt(2)),
                              m_rcDraw.y()+m_rcDraw.height()*sqrt(2)/(2+sqrt(2)),
                              m_rcDraw.width()*2/(2+sqrt(2))*4/5,m_rcDraw.height()*2/(2+sqrt(2)));
        pPainter->drawLine(m_rcDraw.x(),m_rcDraw.y()+m_rcDraw.width()*(1-1/(2+sqrt(2))),
                           m_rcDraw.x()+m_rcDraw.width(),m_rcDraw.y()+m_rcDraw.height()*(1-1/(2+sqrt(2))));
    }
    else
    {
        pPainter->drawEllipse(m_rcDraw.x(),m_rcDraw.y()+m_rcDraw.height()/10,
                              m_rcDraw.width()*2/(2+sqrt(2)),m_rcDraw.height()*2/(2+sqrt(2))*4/5);
        pPainter->drawEllipse(m_rcDraw.x(),m_rcDraw.y()+m_rcDraw.height()*sqrt(2)/(2+sqrt(2)),
                              m_rcDraw.width()*2/(2+sqrt(2)),m_rcDraw.height()*2/(2+sqrt(2))*4/5);
        pPainter->drawEllipse(m_rcDraw.x()+m_rcDraw.width()*sqrt(2)/(2+sqrt(2)),
                              m_rcDraw.y()+m_rcDraw.height()*sqrt(2)/(2+sqrt(2)),
                              m_rcDraw.width()*2/(2+sqrt(2)),m_rcDraw.height()*2/(2+sqrt(2))*4/5);
        pPainter->drawLine(m_rcDraw.x()+m_rcDraw.width()/(2+sqrt(2)),m_rcDraw.y(),
                           m_rcDraw.x()+m_rcDraw.width()/(2+sqrt(2)),m_rcDraw.y()+m_rcDraw.height());
    }

    //pPainter->drawRect( m_rcDraw );
    pPainter->restore();
    return TRUE ;
}

BOOL CPt_sign::InitData( )
{
    return FALSE ;
}

BOOL CPt_sign::Create( QRect rc )
{
    m_rcDraw = rc ;
    return TRUE ;
}

BOOL CPt_sign::SetChangeNodeColor(WORD ChangeNode, BYTE PowerState)
{
    if( ChangeNode == m_wObjectNodeL )
    {
        m_IsPowerOn = PowerState;
        return TRUE;
    }
    return FALSE;
}

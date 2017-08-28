#include "ctransformer.h"

CTransformer::CTransformer()
{
    m_wObjectType = LINKER;
    m_rcDraw = QRect( 0 , 0 , 0 , 0 );
    m_IsPowerOn = POWER_OFF ;
    m_IsDirection = VER_DRAW;
}

BOOL CTransformer::DrawElemnt( QPainter *pPainter )
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
        pPainter->drawEllipse(m_rcDraw.x(),m_rcDraw.y(),
                              m_rcDraw.width()*5/8,m_rcDraw.height());
        pPainter->drawEllipse(m_rcDraw.x()+m_rcDraw.width()*3/8,m_rcDraw.y(),
                              m_rcDraw.width()*5/8,m_rcDraw.height());
    }
    else
    {
        pPainter->drawEllipse(m_rcDraw.x(),m_rcDraw.y(),
                              m_rcDraw.width(),m_rcDraw.height()*5/8);
        pPainter->drawEllipse(m_rcDraw.x(),m_rcDraw.y()+m_rcDraw.height()*3/8,
                              m_rcDraw.width(),m_rcDraw.height()*5/8);
    }

    //pPainter->drawRect( m_rcDraw );
    pPainter->restore();
    return TRUE ;
}

BOOL CTransformer::InitData( )
{
    return FALSE ;
}

BOOL CTransformer::Create( QRect rc )
{
    m_rcDraw = rc ;
    return TRUE ;
}

BOOL CTransformer::SetChangeNodeColor(WORD ChangeNode, BYTE PowerState)
{
    if( ChangeNode == m_wObjectNodeL )
    {
        m_IsPowerOn = PowerState;
        return TRUE;
    }
    return FALSE;
}

#include "ctext_sign.h"

CText_sign::CText_sign()
{
    m_wObjectType = LINKER;
    m_rcDraw = QRect( 0 , 0 , 0 , 0 );
    m_IsPowerOn = POWER_OFF ;
    m_IsDirection = VER_DRAW;
}

BOOL CText_sign::DrawElemnt( QPainter *pPainter )
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

    QFont font = pPainter->font();
    font.setPixelSize(12);
    pPainter->setFont(font);

    pPainter->drawText(m_rcDraw,Qt::AlignVCenter,m_text);

    //pPainter->drawRect( m_rcDraw );
    pPainter->restore();
    return TRUE ;
}

BOOL CText_sign::InitData( )
{
    return FALSE ;
}

BOOL CText_sign::Create( QRect rc )
{
    m_rcDraw = rc ;
    return TRUE ;
}

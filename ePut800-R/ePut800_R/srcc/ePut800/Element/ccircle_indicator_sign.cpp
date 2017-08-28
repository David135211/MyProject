#include "ccircle_indicator_sign.h"

CCircle_indicator_sign::CCircle_indicator_sign()
{
    m_wObjectType = LINKER;
    m_rcDraw = QRect( 0 , 0 , 0 , 0 );
    m_IsPowerOn = POWER_OFF ;
    m_IsDirection = VER_DRAW;
}

BOOL CCircle_indicator_sign::DrawElemnt( QPainter *pPainter )
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
        pPainter->setBrush( Qt::red );
    }
    else
    {
        pPainter->setPen( Qt::green );
        pPainter->setBrush( Qt::green );
    }


    pPainter->drawEllipse( m_rcDraw );

    //pPainter->setBrush( Qt::NoBrush );
    //pPainter->drawRect( m_rcDraw );
    pPainter->restore();
    return TRUE ;
}

BOOL CCircle_indicator_sign::InitData( )
{
    return FALSE ;
}

BOOL CCircle_indicator_sign::Create( QRect rc )
{
    m_rcDraw = rc ;
    return TRUE ;
}

BOOL CCircle_indicator_sign::CompareAttributeData( )
{
    int AttributeDataSize = Yx_ElementAttributeArray.size();
    if( AttributeDataSize != 1 )
        return FALSE;

    PPIC_ATTRIBUTE_DATA_ELEMENT tmpData = Yx_ElementAttributeArray.at(0);
    if( tmpData->byDataType == PIC_ATTRIBUTE_DATA_ELEMENT::YX_DTYPE )
    {
        BYTE byVal = 0;
        AfxGetYx(tmpData->wBusNo,tmpData->wAddr,tmpData->wPntNo,byVal);
        if( tmpData->dbVal != byVal )
        {
            if( byVal == 1 )
                m_IsPowerOn = POWER_ON;
            else
                m_IsPowerOn = POWER_OFF;
            tmpData->dbVal = byVal;
            return TRUE;
        }
    }
    return FALSE;
}

#ifndef CCIRCLE_INDICATOR_SIGN_H
#define CCIRCLE_INDICATOR_SIGN_H

#include "ceelment.h"

class CCircle_indicator_sign : public CElement
{
public:
    CCircle_indicator_sign();

    virtual BOOL DrawElemnt( QPainter *pPainter ) ;
    virtual BOOL InitData( ) ;
    virtual BOOL Create( QRect rc ) ;
    BOOL CompareAttributeData( );
    BYTE m_IsPowerOn ;
    BYTE m_IsDirection ;
};

#endif // CCIRCLE_INDICATOR_SIGN_H

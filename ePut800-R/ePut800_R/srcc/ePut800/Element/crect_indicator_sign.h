#ifndef CRECT_INDICATOR_SIGN_H
#define CRECT_INDICATOR_SIGN_H

#include "ceelment.h"

class CRect_indicator_sign : public CElement
{
public:
    CRect_indicator_sign();

    virtual BOOL DrawElemnt( QPainter *pPainter ) ;
    virtual BOOL InitData( ) ;
    virtual BOOL Create( QRect rc ) ;
    virtual BOOL CompareAttributeData( );
    BYTE m_IsPowerOn ;
    BYTE m_IsDirection ;
};

#endif // CRECT_INDICATOR_SIGN_H

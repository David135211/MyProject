#ifndef CTEXT_SIGN_H
#define CTEXT_SIGN_H

#include "ceelment.h"

class CText_sign : public CElement
{
public:
    CText_sign();

    virtual BOOL DrawElemnt( QPainter *pPainter ) ;
    virtual BOOL InitData( ) ;
    virtual BOOL Create( QRect rc ) ;
    BYTE m_IsPowerOn ;
    BYTE m_IsDirection ;
};

#endif // CTEXT_SIGN_H

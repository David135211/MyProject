#ifndef CPT_SIGN_H
#define CPT_SIGN_H

#include "ceelment.h"

class CPt_sign : public CElement
{
public:
    CPt_sign();

    virtual BOOL DrawElemnt( QPainter *pPainter ) ;
    virtual BOOL InitData( ) ;
    virtual BOOL Create( QRect rc ) ;
    virtual BOOL SetChangeNodeColor(WORD ChangeNode, BYTE PowerState);
    BYTE m_IsPowerOn ;
    BYTE m_IsDirection ;
};

#endif // CPT_SIGN_H

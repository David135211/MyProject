#ifndef CCT_SIGN_H
#define CCT_SIGN_H

#include "ceelment.h"

class CCt_sign : public CElement
{
public:
    CCt_sign();

    virtual BOOL DrawElemnt( QPainter *pPainter ) ;
    virtual BOOL InitData( ) ;
    virtual BOOL Create( QRect rc ) ;
    virtual BOOL SetChangeNodeColor(WORD ChangeNode, BYTE PowerState);
    BYTE m_IsPowerOn ;
    BYTE m_IsDirection ;
};

#endif // CCT_SIGN_H

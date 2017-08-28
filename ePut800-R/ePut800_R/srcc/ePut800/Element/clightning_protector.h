#ifndef CLIGHTNING_PROTECTOR_H
#define CLIGHTNING_PROTECTOR_H

#include "ceelment.h"

class CLightning_protector : public CElement
{
public:
    CLightning_protector();

    virtual BOOL DrawElemnt( QPainter *pPainter ) ;
    virtual BOOL InitData( ) ;
    virtual BOOL Create( QRect rc ) ;
    virtual BOOL SetChangeNodeColor(WORD ChangeNode, BYTE PowerState);
    BYTE m_IsPowerOn ;
    BYTE m_IsDirection ;
};

#endif // CLIGHTNING_PROTECTOR_H

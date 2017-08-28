#ifndef CTRANSFORMER_H
#define CTRANSFORMER_H

#include "ceelment.h"

class CTransformer : public CElement
{
public:
    CTransformer();

    virtual BOOL DrawElemnt( QPainter *pPainter ) ;
    virtual BOOL InitData( ) ;
    virtual BOOL Create( QRect rc ) ;
    virtual BOOL SetChangeNodeColor(WORD ChangeNode, BYTE PowerState);
    BYTE m_IsPowerOn ;
    BYTE m_IsDirection ;
};

#endif // CTRANSFORMER_H

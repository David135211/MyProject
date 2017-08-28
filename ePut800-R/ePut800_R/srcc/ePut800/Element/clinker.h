#ifndef CLINKER_H
#define CLINKER_H

#include "ceelment.h"

class CLinker : public CElement
{
public:
    CLinker();
    virtual BOOL DrawElemnt( QPainter *pPainter ) ;
    virtual BOOL InitData( ) ;
    virtual BOOL Create( QRect rc ) ;
    virtual BOOL SetChangeNodeColor(WORD ChangeNode, BYTE PowerState);
    BYTE m_IsPowerOn ;
    BYTE m_IsDirection ;
};

#endif // CLINKER_H

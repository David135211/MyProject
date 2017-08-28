#ifndef CPOWER_H
#define CPOWER_H

#include "ceelment.h"

class CPower : public CElement
{
public:
    CPower();

    virtual BOOL DrawElemnt( QPainter *pPainter ) ;
    virtual BOOL InitData( ) ;
    virtual BOOL Create( QRect rc ) ;
    virtual BOOL IsChangeColor( );
    BYTE m_IsPowerOn ;
    BYTE m_IsDirection ;
};

#endif // CPOWER_H

#ifndef CANALOG_DATA_SIGN_H
#define CANALOG_DATA_SIGN_H

#include "ceelment.h"

class CAnalog_data_sign : public CElement
{
public:
    CAnalog_data_sign();

    virtual BOOL DrawElemnt( QPainter *pPainter ) ;
    virtual BOOL InitData( ) ;
    virtual BOOL Create( QRect rc ) ;
    virtual BOOL CompareAttributeData( );
    BYTE m_IsPowerOn ;
    BYTE m_IsDirection ;
    QString QstrData;
};

#endif // CANALOG_DATA_SIGN_H

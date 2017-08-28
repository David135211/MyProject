#ifndef CGENERATRIX_H
#define CGENERATRIX_H

#include "ceelment.h"

class CGeneratrix : public CElement
{
    Q_OBJECT
public:
    CGeneratrix();
    virtual BOOL DrawElemnt( QPainter *pPainter ) ;
    virtual BOOL InitData( ) ;
    virtual BOOL Create( QRect rc ) ;
    virtual BOOL ProcessMouseDown( QMouseEvent * pEvent );
    virtual BOOL SetChangeNodeColor(WORD ChangeNode, BYTE PowerState);
    void CreateMenu();
    void InitMenu();
    BYTE m_IsPowerOn ;

public slots:
    void slots_QMenuAction(QAction * action);
};

#endif // CGENERATRIX_H

#ifndef CLOAD_BREAKER_3POS_H
#define CLOAD_BREAKER_3POS_H

#include "ceelment.h"

class CLoad_breaker_3pos : public CElement
{
    Q_OBJECT
public:
    CLoad_breaker_3pos();

    virtual BOOL DrawElemnt( QPainter *pPainter ) ;
    virtual BOOL InitData( ) ;
    virtual BOOL Create( QRect rc ) ;
    virtual BOOL CompareAttributeData( );
    virtual BOOL ProcessMouseDown( QMouseEvent * pEvent ) ;
    virtual BOOL IsChangeColor( );
    virtual BOOL SetChangeNodeColor(WORD ChangeNode, BYTE PowerState);
    void CreateMenu();
    void InitMenu();
    void DrawVertical( QPainter *pPainter );
    void DrawVerticalTop( QPainter * pPainter ) ;
    void DrawVerticalBottom( QPainter * pPainter );
    void DrawHorizontal( QPainter *pPainter );
    void DrawHorizontalTop( QPainter * pPainter ) ;
    void DrawHorizontalBottom( QPainter * pPainter );
    void DrawIsChoice( QPainter * pPainter );
    void SetAction( BYTE Action , BYTE CurStatus );

    BYTE m_IsDirection ;
    BYTE m_byLeftTopPower ; //whether left /top of object has power
    BYTE m_byRightBottomPower ; //whether right/bottom of object has power
    BYTE m_byOpen ; //开关量分合

public slots:
    void slots_QMenuAction(QAction * action);//执行点击Qmenu的Action的操作
};

#endif // CLOAD_BREAKER_3POS_H

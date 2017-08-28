#ifndef CPAGE_HOME_H
#define CPAGE_HOME_H

#include <./Pages/cpage.h>

class CPage_Home : public CPage
{
public:
    CPage_Home( BYTE byType );
    virtual BOOL AddData( LPVOID pVoid ) ;
    virtual BOOL DrawPage( QPainter * pPainter );
    virtual BOOL mousePressEvent(QMouseEvent *pEvent );

    BOOL DrawProjName( QPainter * pPainter ) ;
    void ChangePage( BYTE index ) ;
    BOOL DrawDate(QPainter *pPainter);

    QRect m_rcPage[4];
    QString m_strProjectName ;
    QRect m_rcDate ;
    enum { HOME_SYS , HOME_QUERY , HOME_DIAGNOSDIC , HOME_PARAM };
};

#endif // CPAGE_HOME_H

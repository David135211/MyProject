#ifndef CPAGE_SYSPIC_H
#define CPAGE_SYSPIC_H

#include <./Pages/cpage.h>
class CPage_SysPic : public CPage
{
public:
    CPage_SysPic( BYTE byType );
    ~CPage_SysPic();
    virtual BOOL AddData( LPVOID pVoid ) ;
    virtual BOOL DrawPage( QPainter * pPainter );
    virtual BOOL mousePressEvent(QMouseEvent *pEvent );
    virtual BOOL ProcessPageDown( );
    virtual BOOL ProcessPageUp( );
/*lel*/
#if 0
    BYTE GetCurPage( ) ;
#endif
/*endif*/
    PST_SYSPAGE GetSysPage_Strcut( BYTE byPageIndex );
protected:
    QSYS_PAGE_ARRAY m_sysPageArray ;
    const BYTE m_MAX_PAGE  ; // support total pages 20
/*lel*/
#if 0
    BYTE  m_CurPage ;
#endif
	QPixmap pixmap;
	BYTE byPrePage;
	BYTE bg_flag;
	char sPrimitive[50];
/*end*/
    BYTE m_TotalPage ;  // total page
protected:
    void AddTestData( );
    void AddTestDataAttribute();
    void AddTestData( QPAGE_FILEST_ARRAY * pArray );
    BOOL SetCurPage( BYTE byPageID );
    BYTE GetTotalPageNum( );
    void DrawCurPageNumber( QPainter * pPainter );
    void DrawPageTitle( QPainter *pPainter ) ;
    BOOL ProcessElementOpt( QMouseEvent *pEvent ) ;
/*lel*/
	void DrawText(CElement *pEle, QPainter *pPainter);
/*end*/
};

#endif // CPAGE_SYSPIC_H

#include "cpage_home.h"
#include "cglobal.h"

CPage_Home::CPage_Home( BYTE byType ):
        CPage( byType )
{
    m_strPic += "ePut800-Home.jpg";
   // m_strProjectName = "ePut800 intelligentize gallery project";

    m_rcDate = QRect( QPoint(180 , 487 ) , QPoint( 620 , 527 ) ) ;
    m_rcPage[HOME_SYS] = QRect(QPoint(34 ,138),QPoint(388,286)) ;//sys_pic
    m_rcPage[HOME_QUERY] = QRect(QPoint( 412 , 138 ) , QPoint(767,286));//query
    m_rcPage[HOME_DIAGNOSDIC] = QRect(QPoint( 34 , 306 ) , QPoint(388,452 ));//diagnostic
    m_rcPage[HOME_PARAM] = QRect(QPoint( 412 , 305 ) , QPoint(767,452 )) ;//sys param
}


BOOL CPage_Home::AddData( LPVOID pVoid )
{
    AfxGetProjectName(m_strProjectName);
    if( !pVoid )
        return FALSE ;

    return TRUE ;
}

BOOL CPage_Home::DrawPage( QPainter * pPainter )
{
    if( !pPainter )
    {
        Q_ASSERT( FALSE ) ;
        return FALSE ;
    }

    CPage::DrawPage( pPainter ) ;

    //draw Time
    DrawDate( pPainter ) ;

    //draw Projectname
    DrawProjName( pPainter ) ;

    return TRUE ;
}

BOOL CPage_Home::DrawDate(QPainter *pPainter)
{
    if( !pPainter )
        return FALSE ;

    QFont font ;
    font.setPixelSize( 40 );
    font.setBold( TRUE );
    pPainter->setFont(font);
    QPen pen( QColor( 255 , 0 , 0 , 255 ) );
    pPainter->setPen( pen );

    QString strTime = AfxGetGlobal()->GetRealTimeStr() ;
    pPainter->drawText( m_rcDate , Qt::AlignLeft , strTime );

    return TRUE ;
}

BOOL CPage_Home::DrawProjName( QPainter * pPainter )
{
    QFont font ;
    font.setPixelSize( 35 );
    font.setBold( TRUE );
    pPainter->setFont(font);
    QPen pen( QColor( 255 , 255 , 255 , 255 ) );
    pPainter->setPen( pen );

    QRect rcProj( QPoint( 6 , 15 ) , QPoint( 798 , 71 ) ) ;
    pPainter->drawText( rcProj , Qt::AlignLeft | Qt::AlignVCenter , m_strProjectName );

    return TRUE ;
}

BOOL CPage_Home::mousePressEvent(QMouseEvent * pEvent )
{
    if( CPage::mousePressEvent( pEvent ) )
    {
        AfxRefreshScreen();
        return TRUE ;
    }

    if( !pEvent )
    {
        Q_ASSERT( FALSE ) ;
        return FALSE;
    }

    QPoint pt = pEvent->pos() ;

    int size = sizeof( m_rcPage ) / sizeof( m_rcPage[ 0 ] );
    for( BYTE i = 0 ; i < size ; i++ )
    {
        if(m_rcPage[ i ].contains( pt ) )
            ChangePage( i );
    }

    return TRUE ;
}

void CPage_Home::ChangePage( BYTE index )
{
    if( index == HOME_SYS )
        AfxSetCurPageIndex( Widget::PAGE_SYS_PIC ) ;
    else if( index == HOME_QUERY )
        AfxSetCurPageIndex( Widget::PAGE_DATA_QUERY );
    else if( index == HOME_DIAGNOSDIC )
        AfxSetCurPageIndex( Widget::PAGE_DIOGNOSTIC );
    else if( index == HOME_PARAM )
        AfxSetCurPageIndex( Widget::PAGE_SYS_PARA );
    else
    {
        Q_ASSERT( FALSE ) ;
        return ;
    }

    AfxRefreshScreen();
}

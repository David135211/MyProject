#include "cpage.h"
#include "widget.h"
#include <string.h>

CPage::CPage( BYTE byType )
{
	m_byPageType = byType ;
	m_byPrePageType = 0;
	m_cxSize = Widget::HOR_SIZE ;
	m_cySize = Widget::VER_SIZE ;
	m_strPic = "/usr/share/ePut800/Pics/";
	m_strBackground = m_strPic;
	m_strLoginPath = m_strPic ;
	m_rcLoginPic = QRect( 646 ,558 , 25 ,26) ;
	m_rcLoginInfo = QRect( QPoint( 684 , 556 ) , QPoint( 794 , 589 )) ;

	//PageUp
	m_rcNavigation[ 0 ] = QRect(QPoint(533,21),QPoint(600 , 65)) ;

	//PageDown
	m_rcNavigation[ 1 ] = QRect(QPoint(616,21),QPoint(683 , 67)) ;

	//back
	m_rcNavigation[ 2 ] = QRect(QPoint(700,21),QPoint(767 , 67));

	//LOGINOUT
	QRect rc( m_rcLoginInfo ) ;
	m_rcNavigation[ 3 ] = rc.united(m_rcLoginPic ) ;
}

BOOL CPage::DrawPage( QPainter * pPainter )
{
	if( !pPainter )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	//paste localpage bmp
	PastePage( pPainter ) ;
	/*lel*/
#if 0
	if(m_byPageType == 18)
		PasteSysBackground(pPainter);
#endif
	/*end*/

	//paste login bmp
	/*lel*/
#if 1
	if(m_byPageType == Widget::PAGE_HOME || m_byPageType == Widget::PAGE_SYS_PARA)
		DrawLogin(pPainter);
#else
	DrawLogin(pPainter);
#endif
	/*end*/
	return TRUE ;
}

BOOL CPage::DrawLogin( QPainter * pPainter )
{
	if( !pPainter )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	BYTE bState = AfxGetLoginState( ) ;
	QString strPath( m_strLoginPath ) ;
	QPoint pt( 646 ,558 ) ;

	if( bState == LOGIN )
	{
		strPath += "Login.jpg" ;
		PasteJPGFromPath( pPainter , strPath , pt ) ;
		DrawTimeOutTime( pPainter ) ;
	}
	else if( bState == LOGOUT )
	{
		strPath += "Logout.jpg" ;
		PasteJPGFromPath( pPainter , strPath , pt ) ;
		DrawText( pPainter , "未登录" , m_rcLoginInfo );
	}
	else
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	return TRUE ;
}

void CPage::DrawTimeOutTime( QPainter * painter )
{
	if( !painter )
	{
		Q_ASSERT( FALSE );
		return ;
	}

	time_t bEnd = 0 ;
	time( &bEnd ) ;
	time_t bBegin = AfxGetGlobal()->GetCountdownTime() ;
	if( !bBegin )
	{
		//Q_ASSERT( FALSE );
		return ;
	}

	int internal = difftime( bEnd, bBegin );
	if( internal > CGlobal::ELAPSE_TIMEOUT + 10 )
	{
		AfxGetGlobal()->SetCountdownTime( bEnd );
		internal = 0 ;
	}

	int iElapseTime = CGlobal::ELAPSE_TIMEOUT - internal;
	if( iElapseTime <= 0 )
		AfxSetLoginState( CPage::LOGOUT );

	painter->save();

	QString strTime ;
	strTime = AfxFormatStr( "%02d:%02d" , iElapseTime / 60 , iElapseTime % 60 );
	painter->setPen( QColor( 255 , 0 , 0 ));
	WORD wDwFlag = Qt::AlignLeft ;
	QFont font ;
	font.setPixelSize( 30 );
	font.setBold( TRUE );
	painter->setFont( font );
	painter->drawText( m_rcLoginInfo , wDwFlag , strTime );

	painter->restore();
}

void CPage::DrawText( QPainter * pPainter , QString strText , QRect rc )
{
	if( strText.size() <= 0 || !pPainter || rc.isEmpty() || rc.isNull() )
		return ;

	pPainter->save();
	QFont font ;
	font.setPixelSize( 30 );
	font.setBold( TRUE );
	pPainter->setFont(font);
	QPen pen( QColor( 255 , 255 , 255 , 255 ) );
	pPainter->setPen( pen );

	QRect rcProj( rc ) ;
	pPainter->drawText( rcProj , Qt::AlignCenter , strText );
	pPainter->restore();
}

BOOL CPage::PasteJPGFromPath( QPainter * pPainter , QString strPath , QPoint pt )
{
	if( !pPainter)
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	int size = strPath.size() ;
	if( size <= 0 )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	QPixmap pix;
//	printf("%s %d\n", __func__, __LINE__);
//	qDebug(strPath.toAscii().data());
	bool bFlag = pix.load( strPath );
	if( bFlag ==  false )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	pPainter->drawPixmap( pt , pix );
	pix.detach();

	return TRUE ;
}

BOOL CPage::PastePage( QPainter * pPainter )
{
	if( !pPainter )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	int iWidth = m_cxSize ;
	int iHeight = m_cySize;

	/*lel*/
//	printf("%s %d m_byPageType = %d, m_byPrePageType = %d\n", __func__, __LINE__, m_byPageType, m_byPrePageType);
//	qDebug(m_strPic.toAscii().data());
#if 1
	bool bFlag;
	if(m_byPrePageType != m_byPageType){
	//	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
		m_byPrePageType = m_byPageType;
		bFlag = pix.load( m_strPic );
	}
#else
	QPixmap pix;
	bool bFlag = pix.load( m_strPic );
#endif
	/*end*/
	if( bFlag ==  false )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	pPainter->drawPixmap(0,0,iWidth,iHeight,pix );
	pix.detach();

	return TRUE ;
}
/*lel*/
#if 0
BOOL CPage::PasteSysBackground( QPainter * pPainter )
{
	if( !pPainter )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

//	int iWidth = m_cxSize ;
//  int iHeight = m_cySize;

	QPixmap pix;
#if 0
	BYTE cur_page = GetCurPage();
	QString str = m_strBackground.insert(28, cur_page + "0");
	printf("%s %d cur_page = %d\n", __func__, __LINE__, cur_page);
	qDebug(m_strBackground.toAscii().data());
	qDebug(str.toAscii().data());
	bool bFlag = pix.load(str);
#else
	bool bFlag = pix.load( m_strBackground );
#endif
	if( bFlag ==  false )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

//	pPainter->drawPixmap(0,0,iWidth,iHeight,pix );
	pPainter->drawPixmap(0, 67, 800, 490, pix);
	pix.detach();

	return TRUE ;
}
#endif
/*end*/

BYTE CPage::GetNavigationFlag( QPoint pt )
{
	BYTE bySize = sizeof( m_rcNavigation ) / sizeof( m_rcNavigation[ 0 ] ) ;
	for( BYTE i = 0 ; i < bySize ; i++ )
	{
		if( m_rcNavigation[ i ].contains( pt ) )
		{
			if( i == 0 )
				return PAGEUP ;
			else if( i == 1 )
				return PAGEDOWN ;
			else if( i ==  2 )
				return BACK ;
			else if( i == 3 )
				return PAGE_LOGINOUT ;
			else
			{
				Q_ASSERT( FALSE ) ;
				return PAGE_NULL;
			}
		}
	}

	return PAGE_NULL ;
}

BOOL CPage::mousePressEvent(QMouseEvent *pEvent )
{
	//update countdown
	AfxUpdateTimeoutTime( );

	if( !pEvent )
	{
		Q_ASSERT( FALSE );
		return FALSE;
	}

	QPoint pt( pEvent->pos() ) ;
	BYTE byVal = GetNavigationFlag( pt );
	if( CPage::PAGE_NULL != byVal )
	{
		if( processNavigationState( byVal ) )
			return TRUE ;
		else
			return FALSE ;
	}

	return FALSE ;
}

void CPage::ProcessLogin( )
{
	Widget * TempWidget = AfxGetWidget();
	TempWidget->LoginDlg->show();
}

BOOL CPage::processNavigationState( BYTE byVal )
{
	if( byVal != CPage::PAGEDOWN &&
			byVal != CPage::PAGEUP &&
			byVal != CPage::BACK &&
			byVal != CPage::PAGE_LOGINOUT )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}
	switch( byVal )
	{
		case CPage::PAGEDOWN:
			ProcessPageDown( );
			break;
		case CPage::PAGEUP:
			ProcessPageUp( ) ;
			break;
		case CPage::BACK:
			AfxSetCurPageIndex( Widget::PAGE_HOME );
			break;
		case CPage::PAGE_LOGINOUT:
			/*lel*/
#if 1
			if(m_byPageType == Widget::PAGE_HOME || m_byPageType == Widget::PAGE_SYS_PARA)
				ProcessLogin();
#else
			ProcessLogin();
#endif
			/*end*/
			break;
		default:
			{
				Q_ASSERT( FALSE ) ;
				return FALSE ;
			}
	}

	return TRUE ;
}


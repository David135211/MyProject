#ifndef CPAGE_H
#define CPAGE_H
#include <QPainter>
#include "../Typedef.h"
#include <QMouseEvent>
#include "cglobal.h"
#include <QObject>

class CPage : public QObject
{
	Q_OBJECT
	public:
		CPage( BYTE byType );
		virtual ~CPage(){ return ; }
		virtual BOOL AddData( LPVOID pVoid ) { return FALSE ;}
		virtual BOOL DrawPage( QPainter * pPainter );
		virtual BOOL mousePressEvent(QMouseEvent * pEvent);
		virtual BOOL processNavigationState( BYTE byVal ) ;
		virtual BOOL ProcessPageDown( ){ return FALSE ;}
		virtual BOOL ProcessPageUp( ){ return TRUE ;}
		/*lel*/
#if 1
		BYTE GetCurPage();
		enum{
			COVERS = 0,
			PTZ_CAMERA,
			INFRARED_RADIATION,
			LIQUID_LEVEL_SENSOR,
			DRAIN_PUMP,
			FIXED_CAMERA,
			TEMPERATURE_HUMIDITY_SENSOR,
			GAS_DETECTOR,
			PRIMITIVE_MAX,
		};
#endif
		/*end*/

		enum{ PAGEUP = 0x21 , PAGEDOWN , BACK , PAGE_LOGINOUT , PAGE_NULL };
		enum{ LOGIN = 0x31 , LOGOUT } ;

		void ProcessLogin( );
		void DrawText( QPainter * pPainter , QString strText , QRect rc );
		BYTE GetNavigationFlag( QPoint pt );
		BOOL PasteJPGFromPath( QPainter * pPainter , QString strPath , QPoint pt );
		BOOL DrawLogin(  QPainter * pPainter );
		BOOL PastePage( QPainter * pPainter ) ;
		/*lel*/
		BOOL PasteSysBackground( QPainter * pPainter ) ;
		BYTE  m_CurPage;
		QPixmap pix;
		/*end*/
		void DrawTimeOutTime( QPainter * pPainter );

		QRect m_rcLoginInfo ; //info about login
		QRect m_rcLoginPic ;  //info about login pic
		BYTE m_byPageType ;
		int m_cxSize ; // width of Screen
		int m_cySize ;  //height of Screen
		QString m_strPic ; //图片路径
		/*lel*/
		BYTE m_byPrePageType ;
		QString m_strBackground;      // 系统背景图路径
	//	QString m_strBackgroundpaste; // 系统背景图路径
		/*end*/
		QString m_strLoginPath; //path of login bmp
		QRect m_rcNavigation[ 4 ] ;
};

#endif // CPAGE_H

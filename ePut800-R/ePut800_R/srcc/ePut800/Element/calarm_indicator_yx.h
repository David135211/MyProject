#ifndef CALARM_INDICATOR_YX_H
#define CALARM_INDICATOR_YX_H

#include "ceelment.h"

class CAlarm_indicator_yx : public CElement
{
	public:
		CAlarm_indicator_yx();
		CAlarm_indicator_yx(QString);
		~CAlarm_indicator_yx();

		virtual BOOL DrawElemnt( QPainter *pPainter ) ;
		virtual BOOL InitData( ) ;
		virtual BOOL Create( QRect rc ) ;
		virtual BOOL CompareAttributeData( );
		virtual BOOL ProcessMouseDown(QMouseEvent *);
		virtual void SetAction(BYTE);
	private:
		BYTE byVal;
		BYTE byVal1;
		float valued;				//变换后的值!
		char str[64];
		QMessageBox *box;
		/*lel*/
#if 1
		BYTE blink_flag;
		QPixmap pixmap;
#endif
		/*end*/
		//		BYTE m_IsPowerOn ;
		//		BYTE m_IsDirection ;
};

#endif // CALARM_INDICATOR_YX_H

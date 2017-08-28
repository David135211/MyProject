#ifndef CIMAGE_YC_H
#define CIMAGE_YC_H

#include "ceelment.h"

class CImage_Yc : public CElement
{
	public:
		CImage_Yc();
		~CImage_Yc();
		CImage_Yc(QString);

		virtual BOOL DrawElemnt( QPainter *pPainter ) ;
		virtual BOOL InitData( ) ;
		virtual BOOL Create( QRect rc ) ;
		virtual BOOL CompareAttributeData();
		virtual BOOL ProcessMouseDown(QMouseEvent *);
		virtual void SetAction(BYTE);

	private:
		BYTE blink_flag;
		BYTE yxstatus;
		BYTE yxstatus1;
		float valued;
		/*lel*/				//为了整合温度湿度，不得已而为之。
#if 1
		float valued1;
//		BYTE flag_temper_humi;
		BYTE blink_flag_humi;
		QPixmap pixmap;
#endif
		/*end*/
		QMessageBox *box;
		char str[64];
};

#endif // CIMAGE_YC_H

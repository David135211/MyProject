#ifndef CEELMENT_H
#define CEELMENT_H
#include <Typedef.h>
#include <QPainter>
#include <QWidget>
#include <QObject>
#include "cglobal.h"
#include "Typedef.h"
#include "../Pages/cpage.h"
#include "../Proto/cmethod.h"

//start
#if 0
class Alarm{							//遥测阈值设置!
	public:
		BYTE toppole;							//上限阈值极	上上
		BYTE top;								//上限阈值		上
		BYTE toprange;							//上限范围		范围上
		BYTE bottomrange;						//下限范围		范围下
		BYTE bottom;							//下限阈值		下
		BYTE bottompole;						//下限阈值极	下下
		//		BYTE yc_id;								//关联的遥测
		BYTE busno;								//关联的遥控
		BYTE devaddr;
		WORD point;
		BYTE checkbox;							//和checkbox关联的编号
};
#endif
//end

class CElement : public QObject
{
	Q_OBJECT
	public:
		CElement();
		virtual BOOL DrawElemnt( QPainter *pPainter ) = 0 ;
		virtual BOOL Create( QRect rc ) = 0 ;
		virtual BOOL InitData( ) { return FALSE ; }
		virtual BOOL CompareAttributeData( ) { return FALSE ; }
		virtual BOOL IsChangeColor( ) { return FALSE ; }
		virtual BOOL SetChangeNodeColor( WORD ChangeNode , BYTE PowerState ) { return FALSE ; }
		virtual BOOL ProcessMouseDown( QMouseEvent * pEvent ) { return FALSE ;}
		WORD GetObjectType();
		void SetObjectType(WORD ElementType);
		QRect GetQRect();

		enum{ GENERATRIX = 0x1000, //母线
			LINKER,//链接线
			BREAKER/*断路器*/ ,
			POWER , //电源
			TRANSFORMER ,//变压器
			CT_SIGN ,//CT
			PT_SIGN ,//PT
			ANALOG_DATA_SIGN ,//遥测
			TEXT_SIGN ,//文本
			CIRCLE_INDICATOR_SIGN ,//圆形指示器
			RECT_INDICATOR_SIGN ,//矩形指示器
			LIGHTNING_PROTECTOR ,//避雷针
			CONTACTOR/*接触器*/ ,
			LOAD_BREAKER_2POS/*两工位负荷开关*/ ,
			LOAD_BREAKER_3POS/*三工位负荷开关*/ ,
			SWITCH_2POS/*两工位刀闸*/ ,
			SWITCH_3POS/*三工位刀闸*/,
			SWITCH_CONNECT_EARTH/*接地刀闸*/,
			//start
			IMAGE_FOR_AI,	/*	遥测身边的图片!	*/	//0x1012
			ALARM_FOR_AI,	/*	遥测告警图元	*/	//0x1013
			//end
			END_PIC
		} ;

		enum{ POWER_ON = 0x01 , POWER_OFF } ;
		enum{ HORZ_DRAW = 0x00 , VER_DRAW } ;
		enum{ PEN_WIDTH = 10 } ;
		enum{ BREAKER_OPEN , BREAKER_CLOSE , BREAKER_EARTH } ;
		enum{ TWOPOS , THREEPOS } ;
		enum{ YKACTIONOPEN , YKACTIONCLOSE , YKEARTHACTIONOPEN , YKEARTHACTIONCLOSE } ;
	protected:

		//对象类型
		QRect m_rcDraw ;     //绘制对象的矩形
		WORD m_wObjectType ; //对象类型
		BOOL m_byIsChoice ;
		BYTE m_byTransparency;

		CElement * m_pLeftNode ; //left of object
		CElement * m_prightNode  ; //right of object
		QPen m_Pen ;
		QMenu * m_Menu;
		QTableWidget * m_YcTab;
		QTableWidget * m_YxTab;
		QTableWidget * m_YmTab;
		QTableWidget * m_YkTab;
	public:
	//	static CElement * CreateElement( WORD wType) ;
		static CElement * CreateElement( PELE_FST pele_fst) ;
		WORD m_wObjectID ;   //PicID exclusive ID
		WORD m_wLinkObjectId ;   //linkobjectID exclusive ID
		WORD m_wPageNo ;     // page number which page own local pic element
		WORD m_wDirect ;     // PicDirect
		WORD m_wObjectNodeL ;     //if elemet have two Nodes(left or top Node),if one Node(itself)
		WORD m_wObjectNodeR ;     //if elemet have two Nodes(right or last Node)
		WORD m_wObjectNodeNext ;
		WORD m_wObjectNodeNextState ;
		char m_text[50];
		/*lel*/
		WORD w_top;
		WORD w_bottom;
		WORD w_left;
		WORD w_right;
		char m_picname[50];
		char m_picintroduce[50];
		float f_valued;
		float f_valued1;
		BYTE w_primitivetype;
		WORD w_elementtype;
		BYTE w_blink_flag;
		/*end*/
		QPIC_ATTRIBUTE_DATA_ELEMENT Yx_ElementAttributeArray ;
		QPIC_ATTRIBUTE_DATA_ELEMENT Yc_ElementAttributeArray ;
		QPIC_ATTRIBUTE_DATA_ELEMENT Ym_ElementAttributeArray ;
		QPIC_ATTRIBUTE_DATA_ELEMENT Yk_ElementAttributeArray ;

		void TableShow(QAction *action);

		BYTE YkActionType;
		YKST_MSG YkData[4];

		// BOOL CreateYkDiallog( BYTE TwoOrThree , QWidget * tmp);
		BOOL InitYkData();
		//start
//		Alarm RelaAla;				//
		BYTE relanum;				//表示图元的最后一个字段relanum
		//end
};


#endif // CEELMENT_H

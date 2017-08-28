#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "Typedef.h"
#include <vector>
#include "clogindlg.h"
//start
#include <qcheckbox.h>
//end
#include "loadlibrary.h"
#include "./Proto/cmethod.h"
#include "cmsg.h"
#include "profile.h"
#include "Dbase/dbasemanager.h"
#include "cykdlg.h"
#include "calarmdlg.h"
//start
#include "./Element/ceelment.h"
#include <QTimerEvent>
//end
//模块路径
#define SYSDATAPATH		"./config"
#define DEVNAME                 "Bus"
class CPage ;

namespace Ui {
	class Widget;
}

class Widget : public QWidget
{
	Q_OBJECT

	public:
		explicit Widget(QWidget *parent = 0);
		~Widget();
		BOOL OnInitialize() ;

	private:
		Ui::Widget *ui;

	public:
		enum{ HOR_SIZE = 800 , VER_SIZE = 600 } ;
		enum{ PAGE_HOME = 0x11, PAGE_SYS_PIC , PAGE_DATA_QUERY ,
			PAGE_DIOGNOSTIC , PAGE_SYS_PARA , PAGE_MAX } ;
		enum{ BOARD_LOGIN_DLG , BOARD_WIDGET };
	public:
		QPAGE_ARRAY m_PageMag ;
		QTHREAD_ARRAY m_ThreadArray ;
		QBUS_ARRAY m_busArray ;
		CLoadLibrary m_loadLibrary ;
		CMethod *m_pMethod ;
		QCAB_ARRAY m_cabArray ;
		QSCIRCUIT_ARRAY m_sCircuitArray ;
		QELEMENT_PICOBJ_MAP m_objAllElement ; //save all elements
		QPAGE_FILEST_ARRAY m_objPageElement ; //save each page elements and other property
		QPIC_ATTRIBUTE_FST_ARRAY m_objAllElementAttribute;//save all elements //save all elements`s Attribute
		QPIC_ATTRIBUTE_FST_MAP m_objAllElementAttribute_Map;
		//QPIC_NODE_TO_ID_MAP m_AllElementNodeToPicId_Map;//save all element Node to Id
		//QPIC_NODE_TO_ID_ARRAY m_PowerElementNodeToPicId_Array;//save Power element Node to Id
		CLoginDlg * LoginDlg;
		CYkDlg * YkDlg;
		CAlarmDlg * AlarmDlg;
		Keyboard * Numkeyboard;
		BYTE NumkeyboardFocusWindow;
		WORD m_wDevNum;
		char m_strProjectName[50] ;
		CDBaseManager *m_pDBase;	 //数据库指针 
		//start
		//	QMap<BYTE, QVector<Alarm *>*> alarmobj;
		QMap<BYTE, QVector<Alarm> > alarmobj;
		QVector<CElement *> yc_ykvec;				//为什么要在这里添加一个容器?	曰:如果通过原来的方式等待线程轮询调用cimage_yc对象则需要进入系统图页执行遥控的语句才会执行，主页下不执行，在这里添加一个容器用来实时调用遥控下发命令!下同，只是关联的事遥信和遥控!
		QVector<CElement *> yx_ykvec;
		//end
	private:
		// 初始化数据库
		BOOL InitDBase( void );
		// 析构数据库
		void DeleteDBase(void);
	protected:

		BOOL LoadPageData( );
		BOOL InitPics( );

		void clearData( ) ;
		CPage * CreatePage( BYTE byType );
		BOOL CreatePageFactory( );
		BOOL CreateThread( );
		BOOL CreateThread( THREAD_PROC pProc , CMsg::MSG_TYPE dwType );
		BOOL CreateThread(THREAD_PROC pProc, CMsg::MSG_TYPE dwType,
				PBUS_LINEST pBus );
		//temporary fuction
		BOOL LoadBusData();
		BOOL OpenPort( ) ;
		BOOL InitPicsDataFromMemory( );
		BOOL ReadPicData(PPAGE_FST pPageFst);
		BOOL InitPicsAttributeFromMemory( ) ;
		BOOL ReadBindData(PPIC_ATTRIBUT_FST  pPicAttFst);
		THREAD_PROC m_ThreadRealProc ;
		BOOL LoadAllKindsOfData( );
		//start
	//	BOOL ReadAlarmFile(PELE_FST, PPAGE_FST);			//无用武之地!
	//	BOOL ReadAlarmFile(QVector<Alarm*> *, BYTE);
		BOOL ReadAlarmFile(BYTE, QVector<Alarm> &);
		//end
	protected:
		void paintEvent( QPaintEvent * pEvent );
		void mousePressEvent(QMouseEvent *);
		//start
		void timerEvent(QTimerEvent *);
		//end
	protected:
		//test function
		BOOL AddDataToDev( CProfile &profile , PDEVST pDev , QDEV_DATA_ARRAY * pArray , BYTE byType );
		BOOL CreateBusProperty( PBUS_LINEST pBus );
		BOOL CreateBusProtocol( PBUS_LINEST pBus );
		BOOL AddProtoModule( PBUS_LINEST pBus );
		BOOL CreateCabinet( );
		BOOL ReadCircuit(PCABINET pCab);
		BOOL ReadLoop(PSINGALCIRCUIT pCircuit);
		BOOL CreateLogicalCircuit( );
		BOOL CreateComThread( );
	private:
		BOOL GetDevData( PBUS_LINEST pBus );
		BOOL ProcessFileData( CProfile &profile , PBUS_LINEST pBus );
		//void ReadConfig(LPCSTR lpszFile);
		BOOL GetBusProfileString( CProfile &Profile , char * sSect , char * sKey , char * sTemp , int &size );
		BOOL AddNetPara( PBUSDATA pBusData , CProfile &profile , BYTE byNo );
		BOOL AddPortOtherPara( PBUSDATA pBusData , CProfile &profile , char * busString , BYTE byNo);
		void SetYcParam(PDataType PData, char* szParam);
		void SetYxParam(PDataType PData, char* szParam);
		void SetYmParam(PDataType PData, char* szParam);
		void SetYkParam(PDataType PData, char* szParam);
		int GetBusProtoInterval( CProfile &Profile , char * sSect , char * sKey );
		//BOOL SetLocalNetPara( CProfile &profile );
		BOOL ReadPortPara( INITBUS & bus );
		//start
		int timerid;
		//end
};

#endif // WIDGET_H

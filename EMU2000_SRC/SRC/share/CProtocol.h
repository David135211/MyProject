#ifndef CPROTOCOL_H
#define CPROTOCOL_H

#include <string.h>
#include <vector>
#include "typedef.h"
#include "profile.h"
#include "semObj.h"

class CProtocol ;
class CMethod ;

typedef std::vector<CProtocol*> CPROTO_ARRAY;

//模块路径
#define SYSDATAPATH		"/mynand/config"
#define DEVNAME				"Bus"

//通讯状态
#define  COM_DEV_NORMAL	0  //装置通讯状态正常
#define  COM_NORMAL			0  //总线通讯状态正常
#define  COM_DEV_ABNORMAL  1 //装置通讯状态异常
#define  COM_ABNORMAL			1 //总线通讯状态异常

//协议类型
#define PROTOCO_TRANSPROT	0
#define PROTOCO_GATHER			1

typedef struct _MSGSTNINFO
{
	BYTE byBusNo ; //总线地址
	WORD wDevNo ;//装置地址
} MSGSTNINFO;

typedef struct BUS_MSG
{
	BUS_MSG( )
	{
		DataNum = 0;
		dwDataType = 0;
		DataLen = 0;
		byMsgType = 0 ;
		memset( &DstInfo, 0, sizeof( MSGSTNINFO ) );
		memset( &SrcInfo, 0, sizeof( MSGSTNINFO ) );
		pData = NULL ;
	}
	~BUS_MSG()
	{
		byMsgType = 0 ;
		if( pData != NULL )
		{
			//第二版将改成多态
			operator delete ( pData  ) ; //不调用构造函数 危险需要
			pData = NULL ;
		}
	}

	MSGSTNINFO DstInfo;//目的装置信息
	MSGSTNINFO SrcInfo;//源装置信息

	// BYTE byBusNo ; //总线地址
	// WORD wDevNo ;//装置地址
	// WORD wSerialNo ;//和共享内存共用的模块号
	int DataNum;
	DWORD dwDataType ;//设置的数据类型 遥控预置
	// WORD dwDataNo;//设置的数据序号 遥控第几路
	// DWORD dwDataVal ; //设置的数据值 遥控是分 是合

	BYTE byMsgType ;
	int DataLen;
	void * pData ; //扩展数据--目前不用
}BUSMSG , *PBUSMSG;

//消息
#define BROADCASET_PROTO		1
#define YK_PROTO						   2
#define THREAD_EXIT		3		/*退出线程命令  */
#define	DZ_PROTO		4		/*协议定值消息  */

//遥控动作处理
#define YK_SEL				 1
#define YK_EXCT				2
#define YK_CANCEL			 3
#define YK_SEL_RTN		   4
#define YK_EXCT_RTN				5
#define YK_CANCEL_RTN		 6
#define YK_ERROR			0xFF

//定值消息处理
#define	DZZONE_CALL					1			/* 召唤定值区 */
#define	DZZONE_CALL_RTN				2			/* 召唤定值区返回 */
#define	DZZONE_SWITCH_PRESET		3			/* 定值区切换预置 */
#define	DZZONE_SWITCH_PRESET_RTN	4			/* 定值区切换预置返回 */
#define DZZONE_SWITCH_EXCT			5			/* 定值区切换执行 */
#define DZZONE_SWITCH_EXCT_RTN		6			/* 定值区切换执行返回 */
#define DZZONE_SWITCH_CANCEL		7			/* 定值区切换取消 */
#define DZZONE_SWITCH_CANCEL_RTN	8			/* 定值区切换取消返回 */
#define	DZ_CALL						9			/* 召唤定值 */
#define	DZ_CALL_RTN					10			/* 召唤定值返回 */
#define	DZ_WRITE_PRESET				11			/* 定值写预置 */
#define	DZ_WRITE_PRESET_RTN			12			/* 定值写预置返回 */
#define DZ_WRITE_EXCT				13			/* 定值写执行 */
#define DZ_WRITE_EXCT_RTN			14			/* 定值写执行返回 */
#define DZ_WRITE_CANCEL				15			/* 定值写取消 */
#define DZ_WRITE_CANCEL_RTN			16			/* 定值写取消返回 */
#define	DZZONE_ERROR				0xFF		/* 定值区错 */
#define	DZ_ERROR					0xFE		/* 定值错 */

typedef struct tagSetData
{
	WORD wSerialNo ; //装置编号
	WORD wPnt ; //装置点号
	float fVal ; //遥测使用数值
	WORD wVal ;  //遥信使用数据
	tagSetData( )
	{
		wSerialNo = 0 ;
		wPnt = 0 ;
		fVal = 0.0f ;
		wVal = 0 ;
	}
}SETDATA , *PSETDATA;

//获取实际配置遥测，遥信，遥脉，遥控点数
#define YC_SUM		0 
#define YX_SUM		1 
#define YM_SUM     2 
#define YK_SUM		3 

class CProtocol
{
	public:
		CProtocol() 
		{
			CProfile Profile( (char *)"/mynand/config/BusLine.ini" );
			m_TransDelay = Profile.GetProfileInt( (char *)"PROJECT" ,(char *)"transdelay" , 30) ;

		}
		virtual ~CProtocol() {}

		virtual BOOL GetProtocolBuf( BYTE * buf , int &len , PBUSMSG pBusMsg = NULL ){  return FALSE ;}
		virtual BOOL ProcessProtocolBuf( BYTE * buf , int len ){  return FALSE ; }
		virtual BOOL Init( BYTE byLineNo ) { return FALSE ;}
		virtual BOOL BroadCast( BYTE * buf , int &len ){  return FALSE ;}
		virtual void TimerProc(){printf("CProtocol\n"); return;  }
		virtual BOOL GetUnprocessBuf ( const BYTE *pBuf, const int iLen, void *pVoid ){return FALSE;}

		virtual BOOL WriteCIVal( WORD wSerialNo , WORD wPnt, float fVal){ return FALSE ;}
		virtual BOOL WriteAIVal(WORD wSerialNo ,WORD wPnt, WORD wVal){ return FALSE ; }
		virtual BOOL WriteDIVal(WORD wSerialNo ,WORD wPnt, WORD wVal){ return FALSE ; }
		virtual BOOL WritePIVal(WORD wSerialNo ,WORD wPnt, DWORD dwVal){ return FALSE ;}
		virtual BOOL WriteSOEInfo( WORD wSerialNo ,WORD wPnt, WORD wVal, LONG lTime, WORD wMiSecond){ return FALSE ;}
		virtual void ReadAnalogData(float *pData){}
		virtual void ReadDigitalData(BYTE *pData ){}
		virtual void ReadPulseData(DWORD *pData){}
		virtual BOOL GetDevCommState( ) { return FALSE  ; }
		virtual void  SetDevCommState( ) { return ; }
		virtual BOOL InitDevState( ) { return FALSE ; }
	public:

		int GetModuleNo( WORD wDevAddr )
		{
			int size = m_module.size() ;
			for( int i = 0 ; i < size ; i++ )
			{
				CProtocol * pProtocol = m_module[ i ] ;
				if( pProtocol->m_wDevAddr == wDevAddr )
					return i ;
			}
			return -1 ;
		}

	public:
		BYTE m_byLineNo ; //总线号
		WORD m_wDevAddr ;//装置地址
		WORD m_SerialNo ; //和共享内存映射的序列号 转发协议不用该成员变量
		WORD m_wModuleType ; //模块类型
		char m_sDevPath[ 200 ]; //装置文件路径
		char m_sDevName[ 50 ]; //装置名字
		char m_sTemplatePath[ 200 ] ;//模板名字
		CMethod *m_pMethod ; //公共方法
		CPROTO_ARRAY m_module ; //模块管理
		BOOL m_ProtoType ; //协议类型是上层还是下层
		WORD m_TransDelay;

};



#endif // CPROTOCOL_H

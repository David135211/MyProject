/*
 * =====================================================================================
 *
 *       Filename:  CDataTrans.h
 *
 *    Description:  关于ESD自定义历史数据上传 
 *
 *        Version:  1.0
 *        Created:  2015年06月09日 18时28分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp
 *   Organization:  
 *
 *		  history:	Time								Author			version			Description
 *					2015年06月09日 18时29分13秒         mengqp			1.0				created
 *
 * =====================================================================================
 */

#ifndef  CDATATRANS_INC
#define  CDATATRANS_INC

/* #####   HEADER FILE INCLUDES   ################################################### */
#include "CProtocol_DataTrans.h"

/* #####   TYPE DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ######################### */
/* #####   DATA TYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */



/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ################################ */

/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */

/*
 * =====================================================================================
 *        Class:  CDataTrans
 *  Description:  数据传送协议
 * =====================================================================================
 */
class CDataTrans : public CProtocol_DataTrans
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		CDataTrans ();                             /* constructor      */
		virtual ~CDataTrans ();                            /* destructor       */

		/* ====================  METHODS       ======================================= */
		//时间处理函数
		virtual void    TimerProc( void );
		//初始化协议数据
		virtual BOOL Init( BYTE byLineNo );
		//获取协议数据缓存
		virtual BOOL GetProtocolBuf( BYTE * buf , int &len , PBUSMSG pBusMsg = NULL ) ;
		//处理收到的数据缓存 
		virtual BOOL ProcessProtocolBuf( BYTE * pBuf , int len ) ;

		virtual  int  GetRealVal(BYTE byType, WORD wPnt, void *v);
		virtual BOOL WriteAIVal(WORD wSerialNo ,WORD wPnt, float fVal) ;
		virtual BOOL WriteDIVal(WORD wSerialNo ,WORD wPnt, WORD wVal) ;
		virtual BOOL WritePIVal(WORD wSerialNo ,WORD wPnt, DWORD dwVal) ;
		virtual BOOL WriteSOEInfo( WORD wSerialNo ,WORD wPnt, WORD wVal, LONG lTime, WORD wMiSecond) ;		

	protected:
		/* ====================  DATA MEMBERS  ======================================= */

	private:
		//协议错误处理 
		void ProtocolErrorProc ( void );
		//处理接收报文 
		BOOL ProcessRecvBuf ( BYTE *pBuf, int len );
		//设置接收参数 
		void SetRecvParam ( void  );
		//获取发送报文 
		BOOL GetSendBuf ( BYTE *buf, int &len );
		//根椐条件得到协议状态
		BOOL GetProtocolState ( void );
		//是否需要重发 
		BOOL IsResend ( void ) const;
		//是否有变化YX
		BOOL IsHaveChangeYX ( void  ) const;
		//是否有全部数据上送 
		BOOL IsHaveAll ( void  ) const;
		//是否有心跳数据上送 
		BOOL IsHaveHeart ( void ) const;
		//协议时间处理
		void TimeToProtocol( void  );
		//是否到时间发送全部数据 
		BOOL TimeToAll ( void );
		//是否到心跳时间
		BOOL TimeToHeartbeat ( void );
		//获取相应类型的数据
		BOOL GetSendTypeBuf ( BYTE *buf, int &len);
		//获取变化YX数据 
		BOOL GetChangeYXBuf ( BYTE *buf, int &len );
		//组织YX报文 
		BOOL PackChangeYXBuf ( BYTE *buf, int &len );
		//获取全部数据
		BOOL GetAllDataBuf ( BYTE *buf, int &len );
		//获取yc数据包 
		BOOL GetYCDataBuf ( BYTE *buf, int &len );
		//组织YC报文 
		BOOL PackYCBuf ( BYTE *buf, int &len );
		//获取yx数据包 
		BOOL GetYXDataBuf ( BYTE *buf, int &len );
		//组织YX报文 
		BOOL PackYXBuf ( BYTE *buf, int &len );
		//获取yM数据包 
		BOOL GetYMDataBuf ( BYTE *buf, int &len );
		//组织YM报文 
		BOOL PackYMBuf ( BYTE *buf, int &len );
		//组织心跳报文
		BOOL GetHeartBuf ( BYTE *buf, int &len );
		//获取重发数据
		void GetResendBuf ( BYTE *buf, int &len );
		//保存重发数据
		void SaveResendBuf ( BYTE *buf, int len, BOOL byValid );
		//设置发送参数
		void SetSendParam ( BOOL bIsSendValid );
		//读取配置模板信息
		BOOL ReadCfgInfo ( void );
		//读取点表信息 
		void ReadCfgMapInfo ( char *szPath );
		//读配置模板的特殊配置信息
		BOOL ReadCfgOtherInfo ( char *szPath );
		//初始化协议状态
		void InitProtocol ( void );
		//初始化协议参数
		void InitProtocolState ( void );
		//初始化转发信息
		void InitProtocolTransTab ( void );
		//初始化协议数据
		void InitProtocolData ( void );

		/* ====================  DATA MEMBERS  ======================================= */
		float    m_fYcBuf[DATATRANS_MAX_YC_NUM];
		DWORD   m_dwYmBuf[DATATRANS_MAX_YM_NUM];
		BYTE	m_byYxBuf[DATATRANS_MAX_YX_NUM] ;

		WORD m_wAllDataInterval;               /* 所有数据上传间隔 s */
		
		//重发相关
		BYTE m_byResendCount;
		int m_iResendLen;
		BYTE m_byResendBuf[DATATRANS_MAX_BUF_LEN];

		//全数据数据位置
		WORD m_wAllDataPos;

		//状态相关
		BOOL m_bSending;
		BYTE m_bySendCount;
		
		//等待时间
		DWORD m_LocalAddTime;
		DWORD m_LocalSumTime;
		DWORD m_LocalHeartbeatTime;
		DWORD m_LocalHeartbeatAddTime;

		BYTE m_byTimerCount;

}; /* -----  end of class CDataTrans  ----- */

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ############################ */

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ##################### */


#endif   /* ----- #ifndef CDATATRANS_INC  ----- */

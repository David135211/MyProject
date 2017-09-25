/*
 * =====================================================================================
 *
 *       Filename:  IEC101S_2002.h
 *
 *    Description:  IEC101子站 2002 版
 *
 *        Version:  1.0
 *        Created:  2014年11月18日 13时30分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp (), 
 *   Organization:  
 *
 *		  history:
 *
 * =====================================================================================
 */


#ifndef  IEC101S_2002_INC
#define  IEC101S_2002_INC


#include "CProtocol_IEC101S.h"

#define		IEC101S_2002_MAX_YC_NUM      4096		/* 最大遥测数量 */
#define		IEC101S_2002_MAX_YX_NUM      8192		/* 最大遥信数量 */
#define		IEC101S_2002_MAX_YM_NUM      1024		/* 最大遥脉数量 */


#define		IEC101S_2002_TOTAL_TYPE		 1			/* 总召类型 */
#define		IEC101S_2002_CHANGE_TYPE	 2			/* 变化数据类型 */
#define		IEC101S_2002_SOE_TYPE		 3			/* SOE 类型 */
#define		IEC101S_2002_YKSINGLE_TYPE   4			/* 单点遥信类型 */
#define		IEC101S_2002_YKDOUBLE_TYPE   5			/* 双点遥信类型 */

#define		IEC101S_2002_MAX_SEND_COUNT	 3			/* 最大发送计数 */
#define		IEC101S_2002_MAX_RESEND_COUNT	 3		/* 最大重发计数 */

/*
 * =====================================================================================
 *        Class:  CIEC101S_2002
 *  Description： 2002版 101子站类 
 * =====================================================================================
 */
class CIEC101S_2002 : public CProtocol_IEC101S
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		CIEC101S_2002 ();                             /* constructor      */
		~CIEC101S_2002 ();                            /* destructor       */

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
		/* 处理协议报文 */
		//处理接收控制字功能
		virtual BOOL ProcessCtlBit( BYTE c );
		//设置接收参数
		void SetRecvParam( void );
		//处理头为0x10的报文
		BOOL ProcessHead10Buf( const BYTE *pBuf, int len );
		//设置遥控参数
		void SetYkParam( BYTE byType, BYTE byCot, WORD wStn, WORD wPnt, BYTE byStatus );
		//获取遥控参数
		BOOL IsYkParamTrue( BYTE byType, BYTE byCot, WORD wStn, WORD wPnt, BYTE byStatus ) const; 
		//处理遥控数据
		BOOL ProcessYkBuf( const BYTE *buf, int len );
		//处理总招数据
		BOOL ProcessTotalCallBuf( const BYTE *buf, int len );
		//处理遥脉数据
		BOOL ProcessYMCallBuf( const BYTE *buf, int len );
		//处理对时数据
		BOOL ProcessTimeSyncBuf( const BYTE *buf, int len );
		//处理头为0x68的报文
		BOOL ProcessHead68Buf( const BYTE *pBuf, int len );
		//处理接收报文
		BOOL ProcessRecvBuf( const BYTE *pBuf, int len );

		/* 获取协议报文 */
		//处理总线消息
		BOOL DealBusMsg( PBUSMSG pBusMsg );
		//添加68头尾帧
		int Add68HeadAndTail( const BYTE *byAsduBuf, int iAsduLen, BYTE *buf );
		//获取链路状态报文
		BOOL GetLinkStatusBuf( BYTE *buf, int &len );
		//获取链路复位确认报文
		BOOL GetReconitionBuf( BYTE *buf, int &len );
		//用户数据 确认
		BOOL GetUserDataBuf( BYTE *buf, int &len );
		//没有数据的回复报文
		BOOL GetNoneDataBuf( BYTE *buf, int &len );
		//获取总召唤确认报文
		BOOL GetTotalCallRecoBuf( BYTE *buf, int &len, BYTE byCot );
		//获取对时确认报文
		BOOL GetTimeSyncRecoBuf( BYTE *buf, int &len, BYTE byCot );
		//获取累计量确认报文
		BOOL GetCallYmRecoBuf( BYTE *buf, int &len, BYTE byCot );

		//获取总召测量值 归一化值
		BOOL Get_M_ME_NA_1_TotalFrame( BYTE *buf, int &len );
		//获取变化测量量 归一化值
		BOOL Get_M_ME_NA_1_ChangeFrame( BYTE *buf, int &len );
		//获取测量值 归一化值
		BOOL Get_M_ME_NA_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取带时标突发测量量 归一化值
		BOOL Get_M_ME_TA_1_SoeFrame( BYTE *buf, int &len );
		//获取带时标测量值 归一化值
		BOOL Get_M_ME_TA_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取总召测量值 标度化值
		BOOL Get_M_ME_NB_1_TotalFrame( BYTE *buf, int &len );
		//获取变化测量量 标度化值
		BOOL Get_M_ME_NB_1_ChangeFrame( BYTE *buf, int &len );
		//获取测量值 标度化值
		BOOL Get_M_ME_NB_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取带时标突发测量量 标度化值
		BOOL Get_M_ME_TB_1_SoeFrame( BYTE *buf, int &len );
		//获取带时标测量值 标度化值
		BOOL Get_M_ME_TB_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取总召测量值 短浮点数
		BOOL Get_M_ME_NC_1_TotalFrame( BYTE *buf, int &len );
		//获取变化测量量 短浮点数
		BOOL Get_M_ME_NC_1_ChangeFrame( BYTE *buf, int &len );
		//获取测量值 短浮点数
		BOOL Get_M_ME_NC_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取带时标突发测量量 短浮点数
		BOOL Get_M_ME_TC_1_SoeFrame( BYTE *buf, int &len );
		//获取带时标测量值 短浮点数
		BOOL Get_M_ME_TC_1_Frame( BYTE *buf, int &len, int iFlag );

		// 测量值， 不带品质描述词的归一化值总召
		BOOL Get_M_ME_ND_1_TotalFrame ( BYTE *buf, int &len );
		// 测量值， 不带品质描述词的归一化值变化 
		BOOL Get_M_ME_ND_1_ChangeFrame ( BYTE *buf, int &len );
		// 测量值， 不带品质描述词的归一化值 
		BOOL Get_M_ME_ND_1_Frame ( BYTE *buf, int &len, int iFlag );

		//获取带CP56Time2a突发测量量 归一化值
		BOOL Get_M_ME_TD_1_SoeFrame( BYTE *buf, int &len );
		//获取带CP56Time2a测量值 归一化值
		BOOL Get_M_ME_TD_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取带CP56Time2a突发测量量 标度化值
		BOOL Get_M_ME_TE_1_SoeFrame( BYTE *buf, int &len );
		//获取带CP56Time2a测量值 标度化值
		BOOL Get_M_ME_TE_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取带CP56Time2a突发测量量 短浮点数
		BOOL Get_M_ME_TF_1_SoeFrame( BYTE *buf, int &len );
		//获取带CP56Time2a测量值 短浮点数
		BOOL Get_M_ME_TF_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取不带时标的总召单点信息
		BOOL Get_M_SP_NA_1_TotalFrame( BYTE *buf, int &len );
		//获取不带时标的变化单点信息
		BOOL Get_M_SP_NA_1_ChangeFrame( BYTE *buf, int &len );
		//不带时标的单点信息
		BOOL Get_M_SP_NA_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取带时标的突变单点信息
		BOOL Get_M_SP_TA_1_SoeFrame( BYTE *buf, int &len );
		//获取带时标的单点信息
		BOOL Get_M_SP_TA_1_Frame( BYTE *buf, int &len, int iFlag );


		//获取不带时标的总召双点信息
		BOOL Get_M_DP_NA_1_TotalFrame( BYTE *buf, int &len );
		//获取不带时标的变化双点信息
		BOOL Get_M_DP_NA_1_ChangeFrame( BYTE *buf, int &len );
		//不带时标的双点信息
		BOOL Get_M_DP_NA_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取带时标的突变双点信息
		BOOL Get_M_DP_TA_1_SoeFrame( BYTE *buf, int &len );
		//获取带时标的双点信息
		BOOL Get_M_DP_TA_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取带时标CP56Time2a的突变单点信息
		BOOL Get_M_SP_TB_1_SoeFrame( BYTE *buf, int &len );
		//获取带时标CP56Time2a的单点信息
		BOOL Get_M_SP_TB_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取带时标CP56Time2a的突变双点信息
		BOOL Get_M_DP_TB_1_SoeFrame( BYTE *buf, int &len );
		//获取带时标CP56Time2a的双点信息
		BOOL Get_M_DP_TB_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取不带时标的总召累计量
		BOOL Get_M_IT_NA_1_TotalFrame( BYTE *buf, int &len );
		//获取不带时标的变化累积量
		BOOL Get_M_IT_NA_1_ChangeFrame( BYTE *buf, int &len );
		//不带时标的累积量
		BOOL Get_M_IT_NA_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取带时标的总召累计量
		BOOL Get_M_IT_TA_1_TotalFrame( BYTE *buf, int &len );
		//获取带时标的突发累积量
		BOOL Get_M_IT_TA_1_SoeFrame( BYTE *buf, int &len );
		//带时标的累积量
		BOOL Get_M_IT_TA_1_Frame( BYTE *buf, int &len, int iFlag );

		//获取带CP56Time2a时标的总召累计量
		BOOL Get_M_IT_TB_1_TotalFrame( BYTE *buf, int &len );
		//获取带CP56Time2a时标的突发累积量
		BOOL Get_M_IT_TB_1_SoeFrame( BYTE *buf, int &len );
		//带时标CP56Time2a的累积量
		BOOL Get_M_IT_TB_1_Frame( BYTE *buf, int &len, int iFlag );

		//遥控返回报文
		BOOL GetYkRtnDataFrame ( BYTE *buf, int &len, int byYkRtnType );

		//查看是否有变化遥测
		BOOL IsHaveChangeYcData( void );
		//获取遥测数据报文
		BOOL GetChangeYcData( BYTE *buf, int &len );
		//获取二级数据
		BOOL GetLevel2Data( BYTE *buf, int &len );
		//查看是否有1级数据
		BOOL IsHaveLevel1Data( void );
		//查看是否有特殊数据
		BOOL IsHaveSpecialData( void ) const;
		//总召遥信上送
		BOOL GetTotalYxData( BYTE *buf, int &len );
		//总召遥测上送
		BOOL GetTotalYcData( BYTE *buf, int &len );
		//总召遥脉上送
		BOOL GetTotalYmData( BYTE *buf, int &len );
		//获取特殊数据
		BOOL GetSpecialData( BYTE *buf, int &len );
		//查看是否有变化遥信
		BOOL IsHaveChangeYxData( void );
		//是否有遥信SOE
		BOOL IsHaveYxSoeData ( void ) const;
		//获取遥信数据报文
		BOOL GetChangeYxData( BYTE *buf, int &len );
		//获取遥信soe数据
		BOOL GetSoeYxData ( BYTE *buf, int &len );
		//获取一级数据
		BOOL GetLevel1Data( BYTE *buf, int &len );
		//遥控返回信息是否有效
		BOOL IsYkRtnBusMsgValid ( PBUSMSG pBusMsg, DWORD dwYkType );
		//获取遥控返回报文
		BOOL GetYkRtnData( BYTE *buf, int &len );
		//获取发送报文
		BOOL GetSendBuf( BYTE *buf, int &len );
		//设置发送参数
		void SetSendParam( void );
		/* 初始化过程 */
		//读取模板信息
		BOOL ReadCfgTemplate ( void );
		// 读取配置文件信息
		BOOL ReadCfgInfo ( void );
		// 读取点表信息
		BOOL ReadCfgMap ( void );
		// 初始化协议参数
		void InitProtocolState ( void );
		// 初始化点表信息
		void InitProtocolTransTab( void );
		// 初始化数据信息
		void InitProtocolData( void );
		// 初始化协议状态
		void InitProtocol( void );

	protected:
		/* ====================  DATA MEMBERS  ======================================= */

		//类型数据的起始地址
		WORD m_wYcStartAddr;
		WORD m_wYxStartAddr;
		WORD m_wYkStartAddr;
		WORD m_wYmStartAddr;
		WORD m_wComStateAddr;

		//可变化的数据长度
		BYTE m_byCotLen;
		BYTE m_byAddrLen;
		BYTE m_byInfoAddrLen;

		//总召数据类型
		BYTE m_byTotalCallYx;
		BYTE m_byTotalCallYc;
		BYTE m_byTotalCallYm;

		//变化数据类型
		BYTE m_byChangeYx;
		BYTE m_bySoeYx;
		BYTE m_byChangeYc;
		BYTE m_byYkType;

	private:
		/* ====================  DATA MEMBERS  ======================================= */

		//遥控参数相关
		BYTE m_byYKAsduType;
		BYTE m_byYkCot;
		WORD m_wYkStn;
		WORD m_wYkPnt;
		BYTE m_byYkStatus;

		WORD m_wDataIndex;		//数据参数

		BOOL m_bLinkStatus;		//链接状态
		BOOL m_bDataInit;		//数据是否初始化
		BOOL m_bSending;        //发送状态
		BOOL m_bReSending;      //重发状态

		BYTE m_bySendCount;     //发送计数
		BYTE m_byRecvCount;     //接收计数
		BYTE m_byResendCount;	//重发计数

		BYTE m_byResendBuf[256];
		BYTE m_byResendLen;

		float    m_fYcBuf[IEC101S_2002_MAX_YC_NUM];
		DWORD   m_dwYmBuf[IEC101S_2002_MAX_YM_NUM];
		BYTE	m_byYxBuf[IEC101S_2002_MAX_YX_NUM] ;

}; /* -----  end of class CIEC101S_2002  ----- */



#endif   /* ----- #ifndef IEC101S_2002_INC  ----- */

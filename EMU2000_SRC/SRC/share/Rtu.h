/******************************************************************************
 *  Rtu.h: interface for the CRtuBase class on Linux
 *  Copyright (C): 2013 by houpeng
 ******************************************************************************/
#ifndef _RTUBASE_H_
#define _RTUBASE_H_

#include <vector>
#include <deque>
#include <map>
#include <pthread.h>

#include "../librtdb/semObj.h"
#include "BasePort.h"
#include "CProtocol.h"

typedef std::deque< SETDATA> CSetDataQueue;

#define YC_TRANSTOSERIALNO	 1
#define YX_TRANSTOSERIALNO	 2
#define YK_TRANSTOSERIALNO	 3
#define DD_TRANSTOSERIALNO	4
/*****************************************************************************/
#define  RTUSTATUS_STANDBY   0x00
#define  RTUSTATUS_ONLINE    0x01
#define  RTUSTATUS_FAULT     0x02
#define  RTUSTATUS_STOPUSE   0x03

#define  MAPMAX_AI_LEN    4096
#define  MAPMAX_DI_LEN    8192
#define  MAPMAX_PI_LEN    4096
#define  MAPMAX_DO_LEN    512
#define  MAPMAX_AO_LEN    128

#define  MAX_RTUDATA_LEN  1024

typedef struct _tagRTUMSG
{
	WORD   wPortNo;
	WORD   wMsgLen;
	BYTE   * MsgData;
} RTUMSG;

typedef struct _tagMAPITEM
{
	WORD   wStn; //不是具体的装置地址号,改号顺序号
	BYTE   byUnit;
	WORD   wPntNum;
} MAPITEM;

typedef struct _tagPROTITEM
{
	WORD   wStn;
	BYTE   byPro;
	WORD   wPnt;
	WORD   wAddr;
	WORD   wCode;
} PROTITEM;

typedef struct _tagMAPPROT
{
	BYTE    byMode;
	WORD    wTransNo;
	WORD    wSignSum;
	WORD    wSignCount[3];
	WORD    wSignOffset[3];
} MAPPROT;

typedef struct _tagMAPTAB
{
	WORD  wAISum;
	WORD  wDISum;
	WORD  wPISum;
	WORD  wDOSum;
	WORD  wAOSum;
	MAPITEM* pAIMapTab;
	MAPITEM* pDIMapTab;
	MAPITEM* pPIMapTab;
	MAPITEM* pDOMapTab;
	MAPITEM* pAOMapTab;
}MAPTAB;

typedef struct _tagSOEINFO
{
	LONG lTime;
	WORD wMiSecond;
	WORD wPntNum;
	WORD wStatus;
	WORD wSerialNo ;
} SOEINFO;

typedef struct _tagCommObjProp
{
	BYTE byStyle;		/*0:间隔对象，1:RTU对象，2:五防系统*/
	BYTE byObjNum;		/*对象索引*/
	char szObjName[24]; /*对象名称*/
	char szChannel[24]; /*通道属性*/
	WORD wPortNum;		/*端口编号*/
	WORD wStatus;		/*对象状态*/
} COMMOBJ_PROP;

#define RTUMAX_AIE_LEN   4096
#define RTUMAX_DIE_LEN   96
#define RTUMAX_SOE_LEN   96

typedef union
{
	DWORD dwValue;
	struct
	{
		BYTE  byVal0;
		BYTE  byVal1;
		BYTE  byVal2;
		BYTE  byVal3;
	} pi;
} PIDBDEF;

#pragma pack(1)

#define MAX_DATA_LEN  512
typedef struct _tagRTDBDATA
{
	DWORD dwAddrID;         /*地址标识(4字节)*/
	WORD  wDataLen;         /*数据长度(2字节)*/
	BYTE  byTypeID;         /*数据类型(1字节)*/
	BYTE  byDevNum;         /*设备编号(1字节)*/
	BYTE  byDataBuf[MAX_DATA_LEN]; /*数据内容*/
} RTDBDATA;

class Seri_stat{
	public:
		Seri_stat(WORD m, WORD n):serialno(m), stat(n){};
		WORD serialno;
		WORD stat;
};

#pragma pack()

/******************************************************************************
 * CRtuBase Object
 */
class CRtuBase : public CProtocol
{
	public:
		CRtuBase();
		virtual ~CRtuBase();

		/* Attributes */
	public:
		BYTE    m_byProID;       /*规约ID*/
		BYTE    m_byEnable;      /*使用/备用*/
		WORD    m_wObjNum;       /*对象编号*/
		char    m_szObjName[24]; /*对象名称*/
		WORD    m_wPortNum;      /*通道端口*/
		char    m_ComCtrl1[24];  /*通道1设置*/
		char    m_ComCtrl2[24];  /*通道2设置*/
		WORD    m_wRtuAddr;      /*RTU地址*/
		WORD    m_wDeadVal;      /*遥测死区值*/
		WORD    m_wRipeVal;      /*遥测发送类型: 0=二次值  1=一次值*/
		WORD    m_wRecvClock;    /*接受对时标志*/

		/*遥控属性*/
		BYTE    m_byAction;
		BYTE    m_byActBay, m_byActMark;
		WORD    m_wActNum;
		WORD    m_wRelayNum;
		WORD    m_wSelectTimer;

		/*数据发送配置参数*/
		WORD  m_wAISum;
		WORD  m_wDISum;
		WORD  m_wPISum;
		WORD  m_wDOSum;
		WORD  m_wAOSum;
		WORD  m_wSignSum;
		WORD m_wDZSum ;

		BOOL  m_bTaskRun;  /*任务运行*/
	protected:
		WORD*    m_pwCITrans;
		MAPITEM* m_pAIMapTab; WORD* m_pwAITrans;
		MAPITEM* m_pDIMapTab; WORD* m_pwDITrans;
		MAPITEM* m_pPIMapTab; WORD* m_pwPITrans;
		MAPITEM* m_pDOMapTab; WORD* m_pwDOTrans;
		MAPITEM* m_pAOMapTab; WORD* m_pwAOTrans;
		MAPITEM* m_pDZMapTab; WORD * m_pwDZTrans ;

		CSetDataQueue	m_dwAIEQueue;			 /*变化遥测队列*/
		CSetDataQueue	m_dwDIEQueue;            /*变化遥信队列*/
		SOEINFO	m_soeBuffer[RTUMAX_SOE_LEN]; /*SOE事件记录*/
		int     m_iSOE_wr_p;                 /*SOE事件写指针*/
		int     m_iSOE_rd_p;                 /*SOE事件读指针*/

		SYSINFO* m_pDBInfo;
		int     m_iSOEPos;
		int     m_iAIEPos;

		CSemObj m_semAIEMutex;  /*变化遥测信号*/
		CSemObj m_semDIEMutex;  /*变化遥信信号*/
		CSemObj m_semSOEMutex;  /*SOE事件信号*/

		//	std::vector<BYTE> vec_stat;					//by cyz!
		std::map<WORD, BYTE> map_stat;					//by cyz!
		std::deque<Seri_stat> deque_stat;			//by cyz!

		/* Implementation */
	protected:
		BOOL IsPortValid();
		void ClosePort();
	public:
		void CreateTransTab(void);
		void ReadMapConfig(LPCSTR lpszFile);
		void SetObjParam(WORD wPnt, WORD wSignSum, WORD wPos1, WORD wPos2, WORD wPos3);
		int  GetProtUnitAttr(BYTE byType, WORD wAddr, BYTE& byPro, WORD& wStn, WORD& wPnt);
		void AddAnalogEvt( WORD wSerialNo , WORD wPnt, float fVal);
		void AddDigitalEvt( WORD wSerialNo , WORD wPnt, WORD wVal);
		void AddSOEInfo( WORD wSerialNo , WORD wPnt, WORD wVal, LONG lTime, WORD wMiSecond);
		BOOL GetAnalogEvt(WORD &wSerialNo , WORD& wPnt, float& fVal );
		BOOL GetDigitalEvt( WORD &wSerialNo , WORD& wPnt, WORD& wVal);
		BOOL GetSOEInfo( WORD &wSerialNo , WORD& wPnt, WORD& wVal, LONG& lTime, WORD& wMiSecond);
		BOOL GetSOEInfo(WORD &wSerialNo , WORD *wPnt, WORD *wVal, void *pTime, WORD *wMiSecond);
		// 获取单个遥脉值
		DWORD GetPulseData ( WORD wStn , WORD wPnt , BOOL *bIsGet = NULL  );
		// 获取一次遥测值
		float CalcAIRipeVal ( WORD wStn = 0, WORD wPnt = 0, float fYcVal = 0, BOOL *bIsGet = NULL );
		float GetAIRipeVal ( WORD wStn = 0, WORD wPnt = 0, BOOL *bIsGet = NULL );
		// 计算一次遥脉值
		DWORD CalcPulseRipeVal( WORD wStn, WORD wPnt, DWORD dwYmVal, BOOL *bIsGet = NULL );

		void RtuWriteCIVal(WORD wSerialNo,  WORD wPnt,  float fVal);
		void RtuWriteAIVal(WORD wSerialNo,  WORD wPnt,  float fVal );
		void RtuWritePIVal(WORD wSerialNo,  WORD wPnt,  DWORD dwVal);
		void RtuWriteDIVal(WORD wSerialNo,  WORD wPnt,  WORD wVal);
		void RtuWriteSOEInfo(WORD wSerialNo,  WORD wPnt,  WORD wVal,  LONG lTime,  WORD wMiSecond);

		void RelayProc(BYTE byCommand, WORD wStn, WORD wCtrlNum, BYTE byResult);
		void ReadChangData (  );
		WORD GetSerialNoFromTrans( BYTE byType , WORD wTranNum ) ;

		virtual char* ProName(){return (char *)"Base";}
		virtual BOOL  InitRtuBase();
		virtual RTUMSG* LoadRtuMessage(void);
		virtual void    RtuCommandProc( BYTE* pRecvBuf, int nLen );
		virtual BOOL GetUnprocessBuf ( const BYTE *pBuf, const int iLen, void *pVoid ){return FALSE;}
		virtual void    TimerProc(){printf("rtu\n");return ; }
		virtual void ReadAnalogData(float *pData);
		virtual void ReadDigitalData( BYTE *pData );
		virtual void ReadPulseData(DWORD *pData);

		virtual int  GetRealVal(BYTE byType, WORD wPnt, void *v);
		virtual int  GetDIValue(WORD wPnt, WORD wSrcNo, void *v);

		virtual BOOL WriteCIVal(WORD wSerialNo, WORD wPnt, float fVal){ return FALSE ;}
		virtual BOOL WriteAIVal(WORD wSerialNo, WORD wPnt, float fVal ){ return FALSE ;}
		virtual BOOL WriteDIVal(WORD wSerialNo, WORD wPnt, WORD wVal){ return FALSE ;}
		virtual BOOL WritePIVal(WORD wSerialNo, WORD wPnt, DWORD dwVal){ return FALSE ;}
		virtual BOOL WriteSOEInfo(WORD wSerialNo, WORD wPnt, WORD wVal, LONG lTime, WORD wMiSecond){ return FALSE ;}

		virtual void RelayEchoProc(BYTE byCommand, WORD wIndex, BYTE byResult);
		virtual void WriteProtMessage(BYTE byPro, WORD wParam, BYTE* pBuf, int nByte);
		virtual void WriteProtVal(WORD wPnt, BYTE byType,
				WORD wNum, WORD wVal, LONG lTime, WORD wMiSecond);
		virtual int  TransMessage(BYTE byType, BYTE* pBuf, int nLen);
		virtual int  GetCommObjProp(COMMOBJ_PROP* pObjProp);

		//virtual void ReadChangeStatus(std::vector<BYTE> &, std::deque<Seri_stat> &);			//by cyz!
		virtual void ReadChangeStatus(std::map<WORD, BYTE> &, std::deque<Seri_stat> &);			//by cyz!
};

#endif

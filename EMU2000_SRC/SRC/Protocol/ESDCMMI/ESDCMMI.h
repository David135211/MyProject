#if !defined(ESDCMMI_)
#define ESDCMMI_



//#include "../../share/CProtocol.h"
#include "CProtocol_ESDCMMI.h"

#define ESDCMMIMAX_AI_LEN (9216)
#define ESDCMMIMAX_PI_LEN (9216)
#define ESDCMMIMAX_DI_LEN (9216)

#define ESDCMMIMAX_MSG_LEN (1024)

typedef struct STRUCT_YK_DATA /*返校或执行命令信息*/	
{
	BYTE	m_byLineNo;
	BYTE	m_byAddress;
	BYTE	m_byPointNo;
	BYTE	m_byValid;		/*是否有效。0-无效，1-有效*/
	time_t	m_tm; 			/*写数据时的时间*/		
	BYTE	m_byYkCmd;		/*遥控命令*/
	BYTE	m_byYkAction;	/*分合，0x00-分，0xFF-合*/
	BYTE	m_byStatus;		//1.向底层发送，2.底层返回，3.向上返回
}ESDCMMI_YK_STRUCT;



class ESDCMMI  : public CProtocol_ESDCMMI
{
public:
	ESDCMMI();
	virtual ~ESDCMMI();

	BOOL FirstFlag;
	BYTE bySendType;
	
 	float m_fYCBuf[ESDCMMIMAX_AI_LEN];
    DWORD m_dwYMBuf[ESDCMMIMAX_PI_LEN];
	BYTE m_byYXbuf[ESDCMMIMAX_DI_LEN ] ;

	int m_wErrorTimer;
	int m_byPortStatus;

	BOOL m_bRetTime;
	BYTE curSendType_normal;
	BYTE curSendType_prior;
	WORD ESDCMMI_sendOrder;
	ESDCMMI_YK_STRUCT ESDCMMI_Yk_Data;
	
	virtual BOOL GetProtocolBuf( BYTE * buf , int &len , PBUSMSG pBusMsg = NULL ) ;
	virtual BOOL ProcessProtocolBuf( BYTE * buf , int len ) ;
	virtual BOOL Init( BYTE byLineNo );
	virtual BOOL InitRtuBase();
	virtual BOOL WriteAIVal(WORD wSerialNo ,WORD wPnt, float wVal) ;
	virtual BOOL WriteDIVal(WORD wSerialNo ,WORD wPnt, WORD wVal) ;
	virtual BOOL WritePIVal(WORD wSerialNo ,WORD wPnt, DWORD dwVal) ;
	virtual BOOL WriteSOEInfo( WORD wSerialNo ,WORD wPnt, WORD wVal, LONG lTime, WORD wMiSecond) ;


	//获得装置通讯状态 by	zhanghg 2014-9-23
	virtual BOOL GetDevCommState( ) ;
	virtual void TimerProc() ;

private:
	void ESDCMMI_addSyncWord( BYTE *pBuf );
	void ESDCMMI_addControlWord(BYTE *pBuf,BYTE frameKind,BYTE infoWords);
	BOOL ESDCMMI_assembleYxFrame( BYTE * pBuf , int &len ); 
	BOOL ESDCMMI_assembleYcFrame( BYTE * pBuf , int &len ); 
	BOOL ESDCMMI_assembleYmFrame( BYTE * pBuf , int &len ); 
	BOOL ESDCMMI_assembleSoeFrame( BYTE * pBuf , int &len ); 
	BOOL ESDCMMI_assembleFailFrame( BYTE * pBuf , int &len ); 
	WORD ESDCMMI_addPriorWord( BYTE *pBuf , WORD nByte );
	void ESDCMMI_InsertWord(BYTE *pBuf,BYTE func_high,BYTE func_low,BYTE module_actionNo,BYTE byLineNo,BYTE module_addr,BYTE module_ykNo);
	BYTE ESDCMMI_assembleYkRevise(BYTE* pBuf);
	void ESDCMMI_assemSubRetTime(BYTE* pBuf);
	BOOL ESDCMMI_GetYxWordFromYxChP(WORD YxOrder,BYTE* pBuf);
	BOOL ESDCMMI_generateYxDWord(WORD wSendOrder,DWORD * pYxDWord,BYTE YxNum);
	BOOL ESDCMMI_insertYxChp(WORD YxChp,BYTE* pBuf);
	BOOL ESDCMMI_InsertFail(BYTE byLineNo,BYTE ModuleNoAddOne,BYTE *pBuf);
	BOOL ESDCMMI_isSyncWord( BYTE * pWord );
	BOOL ESDCMMI_setSubstationTime(BYTE* pBuf);
	BOOL ESDCMMI_handleYkCmd(BYTE* pBuf,BYTE cmdType);

	BOOL DealBusMsgInfo( PBUSMSG pBusMsg ); 

};

#endif // !defined(AFX_PROTOCOL_ESDCMMI_H__DB4E4A83_510B_4232_A294_B1B4EE1AF4FD__INCLUDED_)


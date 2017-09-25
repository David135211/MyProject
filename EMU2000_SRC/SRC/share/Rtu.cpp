/**********************************************************************
  Rtu.cpp : implementation file for the CRtuBase class on Linux
  Copyright (C): 2011 by houpeng
 ***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "Rtu.h"
#include "../share/profile.h"
#include "../share/rdbFun.h"
#include "../BayLayer/main.h"
#include "../BayLayer/BusManger.h"

extern CBusManger m_busManager ;

#define	RTUKEY1  2011200
#define	RTUKEY2  2011210
#define	RTUKEY3  2011220

#define  YC_NUM		 1
#define  YX_NUM		 2
#define  YM_NUM	    3
#define  YK_NUM		 4
#define   DZ_NUM    5
#define YC_DEAD		 6      //遥测死区
#define YC_PROPTY		 7      //遥测属性
#define TIMING		 8      //对时允许

#define YC_PROPERTY		40 //遥测属性
#define YX_PROPERTY		50 //遥信属性
#define YM_PROPERTY		60 //遥脉属性
#define YK_PROPERTY		70 //遥控属性
#define DZ_PROPERTY		80 //遥调属性

char g_lpcszTypeDef[][20] =
{
	"@#$%^&",
	"YC_AMOUNT", //遥测数量
	"YX_AMOUNT", //遥信数量
	"YM_AMOUNT", //遥脉数量
	"YK_AMOUNT", //遥控数量
	"PARAM_AMOUNT", //定值数量
	"YC_DEAD" ,//遥测死区
	"YC_PROPERTY", //遥测属性
	"TIMING" //允许对时
};

/*****************************************************************************/
/*in profile.cpp*/
extern "C" void ltrim(char *s);
/*in main.cpp*/
extern "C" void OutPromptText(char *lpszText);
extern "C" void LogPromptText(const char *fmt, ...);

/*****************************************************************************/

int GetMapItem(char *strItem, WORD *pwNum)
{/*{{{*/
	char  strType[32];
	int   i, nLen, nType=-1;
	int size = sizeof( g_lpcszTypeDef ) / sizeof( g_lpcszTypeDef[ 0 ] ) ;

	for( int i = 0 ; i < size ; i++ )
	{
		if( strstr(strItem, g_lpcszTypeDef[ i ] ) )
			return i ;
	}

	i = 0;
	nLen = strlen(strItem);
	while( (!isdigit(strItem[i])) && i<32 )
	{
		strType[i] = toupper(strItem[i]);//isalpha()
		if( ++i >= nLen ) break;
	}
	strType[i] = '\0';
	if( i >= nLen ) *pwNum = 0;
	else *pwNum = (WORD)atoi(&strItem[i]);

	if( strcmp(strType, "YC") == 0 ) return YC_PROPERTY ;
	if( strcmp(strType, "YX") == 0 ) return YX_PROPERTY ;
	if( strcmp(strType, "DD") == 0 ) return YM_PROPERTY ;
	if( strcmp(strType, "YK") == 0 ) return YK_PROPERTY ;
	if( strcmp(strType, "DZ") == 0 ) return DZ_PROPERTY ;
	return nType;
}/*}}}*/
/*

*/
void GetMapParam(char* strParam,
		WORD *pwVal1, WORD *pwVal2, WORD *pwVal3, WORD *pwVal4, WORD *pwVal5)
{/*{{{*/
	int i, nLen;
	char *pValue;

	nLen = strlen(strParam);
	if( nLen <= 0 ) return;
	pValue = strtok(strParam, ", ");

	for(i=0; pValue != NULL; i++)
	{
		switch(i)
		{
		case 0:
			if( pwVal1 ) *pwVal1 = (WORD)atoi(pValue);
			break;
		case 1:
			if( pwVal2 ) *pwVal2 = (WORD)atoi(pValue);
			break;
		case 2:
			if( pwVal3 ) *pwVal3 = (WORD)atoi(pValue);
			break;
		case 3:
			if( pwVal4 ) *pwVal4 = (WORD)atoi(pValue);
			break;
		case 4:
			if( pwVal5 ) *pwVal5 = (WORD)atoi(pValue);
			break;
		}
		pValue = strtok(NULL, ", ");
	}
}/*}}}*/

/*
   返回值=
   -1：错误
   0：注释行
   1：遥测点数
   2：遥信点数
   3：电量点数
   4：遥控路数
   5：遥调路数
   6：保护信息
   10：遥测属性
   20：遥信属性
   30：电量属性
   40：遥控属性
   50：遥调属性
   60：保护属性
   */
int ParseMapLine(char* strLine, WORD *pwNum,
		WORD *pwVal1, WORD *pwVal2, WORD *pwVal3, WORD *pwVal4, WORD *pwVal5)
{/*{{{*/
	char *pItem, *pParam;
	int nType=-1;

	pItem = strtok(strLine, "=,\t ");

	if( pItem == NULL )
		return -1;

	pParam = strtok(NULL, "=");
	if( pParam == NULL )
		return -2;

	nType = GetMapItem(pItem, pwNum);

	GetMapParam(pParam, pwVal1, pwVal2, pwVal3, pwVal4, pwVal5);
	return nType;
}/*}}}*/

int GetTransNum(BYTE byType, WORD wStn, WORD wPnt)
{/*{{{*/
	int	iNum = -1;
	if( byType == 0 )
	{
		const ANALOGITEM *pItem = Get_RTDB_Analog(wStn, wPnt);
		if( pItem ) iNum = pItem->iTransNum;
	}
	else if( byType == 1 )
	{
		const DIGITALITEM *pItem = Get_RTDB_Digital(wStn, wPnt);
		if( pItem ) iNum = pItem->iTransNum;
	}
	else if( byType == 2 )
	{
		const PULSEITEM *pItem = Get_RTDB_Pulse(wStn, wPnt);
		if( pItem ) iNum = pItem->iTransNum;
	}
	else if( byType == 3 )
	{
		const RELAYITEM *pItem = Get_RTDB_Relay(wStn, wPnt);
		if( pItem ) iNum = pItem->iTransNum;
	}
	return iNum;
}/*}}}*/

/*****************************************************************************
 * CRtuBase
 */
CRtuBase::CRtuBase()
{/*{{{*/
	m_byProID    = 0;
	m_byEnable   = 0;
	m_wPortNum   = (WORD)-1;
	sprintf(m_ComCtrl1, "%s", "");
	sprintf(m_ComCtrl2, "%s", "");
	m_wObjNum    = (WORD)-1;
	m_wRtuAddr   = (WORD)-1;
	m_wDeadVal   = 4;
	m_wRipeVal   = 0;
	m_wRecvClock = 0;

	m_byAction  = 0;
	m_byActBay  = 0;
	m_byActMark = 0;
	m_wActNum   = 0;
	m_wRelayNum = 0;
	m_wSelectTimer = 0;

	m_wAISum = 0;
	m_wDISum = 0;
	m_wPISum = 0;
	m_wDOSum = 0;
	m_wAOSum = 0;
	m_wSignSum = 0;
	m_wDZSum = 0 ;

	m_pwCITrans = NULL;
	m_pAIMapTab = NULL; m_pwAITrans = NULL;
	m_pDIMapTab = NULL; m_pwDITrans = NULL;
	m_pPIMapTab = NULL; m_pwPITrans = NULL;
	m_pDOMapTab = NULL; m_pwDOTrans = NULL;
	m_pAOMapTab = NULL; m_pwAOTrans = NULL;
	m_pDZMapTab = NULL ;m_pwDZTrans = NULL ;

	m_iSOE_wr_p = 0;
	m_iSOE_rd_p = 0;

	/* 获得数据库参数*/
	m_pDBInfo = (SYSINFO*)Get_RTDB_SysInfo();
	m_iAIEPos = m_pDBInfo->nAIEWritePos;
	m_iSOEPos = m_pDBInfo->nSOEWritePos;

	m_bTaskRun = FALSE;
}/*}}}*/

CRtuBase::~CRtuBase()
{/*{{{*/
	if(m_wAISum > 0)
	{
		if(m_pAIMapTab) delete []m_pAIMapTab;
		if(m_pwAITrans) delete []m_pwAITrans;
	}
	if(m_wDISum > 0)
	{
		if(m_pDIMapTab) delete []m_pDIMapTab;
		if(m_pwDITrans) delete []m_pwDITrans;
	}
	if(m_wPISum > 0)
	{
		if(m_pPIMapTab) delete []m_pPIMapTab;
		if(m_pwPITrans) delete []m_pwPITrans;
	}
	if(m_wDOSum > 0)
	{
		if(m_pDOMapTab) delete []m_pDOMapTab;
		if(m_pwDOTrans) delete []m_pwDOTrans;
	}
	if(m_wAOSum > 0)
	{
		if(m_pAOMapTab) delete []m_pAOMapTab;
		if(m_pwAOTrans) delete []m_pwAOTrans;
	}
	if(m_pwCITrans != NULL)
	{
		if(m_pwCITrans) delete []m_pwCITrans;
	}

	if( m_wDZSum > 0 )
	{
		if( m_pwDZTrans )
			delete [ ] m_pwDZTrans ;

		if( m_pDZMapTab )
			delete [ ] m_pDZMapTab ;
	}

	ClosePort();
	m_dwAIEQueue.clear();
	m_dwDIEQueue.clear();
	m_iSOE_wr_p = 0;
	m_iSOE_rd_p = 0;
}/*}}}*/

void CRtuBase::ClosePort()
{/*{{{*/
}/*}}}*/

BOOL CRtuBase::IsPortValid()
{/*{{{*/
	return FALSE;
}/*}}}*/

void CRtuBase::CreateTransTab(void)
{/*{{{*/
	int  i, k;
	/*
	   m_pwCITrans = new WORD[MAPMAX_CI_LEN];
	   memset((void*)m_pwCITrans, 0, sizeof(WORD)*MAPMAX_CI_LEN);
	   */
	int nAnalogSum = GetPntSum(0);
	if(m_wAISum>0 && nAnalogSum>0)
	{
		m_pwAITrans = new WORD[nAnalogSum];
		memset((void*)m_pwAITrans, 0xFFFF, sizeof(WORD)*nAnalogSum);
		for(i=0; i<m_wAISum; i++)
		{
			if( m_pAIMapTab[i].wStn==0 || m_pAIMapTab[i].wPntNum==0 ) continue ;
			k = GetTransNum(0, m_pAIMapTab[i].wStn-1, m_pAIMapTab[i].wPntNum-1);
			if(k>=0 && k<nAnalogSum)
			{
				m_pwAITrans[k] = i ;
			}
		}
	}
	int nDigitalSum = GetPntSum(1);
	if(m_wDISum>0 && nDigitalSum>0)
	{
		m_pwDITrans = new WORD[nDigitalSum];
		memset((void*)m_pwDITrans, 0xFFFF , sizeof(WORD)*nDigitalSum);
		for(i=0; i<m_wDISum; i++)
		{
			if( m_pDIMapTab[i].wStn==0 || m_pDIMapTab[i].wPntNum==0 ) continue;
			k = GetTransNum(1, m_pDIMapTab[i].wStn-1, m_pDIMapTab[i].wPntNum-1);
			if(k>=0 && k<nDigitalSum)
				m_pwDITrans[k] = i ;
		}
	}
	int nPulseSum = GetPntSum(2);
	if(m_wPISum>0 && nPulseSum>0)
	{
		m_pwPITrans = new WORD[nPulseSum];
		memset((void*)m_pwPITrans, 0xFFFF, sizeof(WORD)*nPulseSum);
		for(i=0; i<m_wPISum; i++)
		{
			if( m_pPIMapTab[i].wStn==0 || m_pPIMapTab[i].wPntNum==0 ) continue;
			k = GetTransNum(2, m_pPIMapTab[i].wStn-1, m_pPIMapTab[i].wPntNum-1);
			if(k>=0 && k<nPulseSum)
				m_pwPITrans[k] = i ;
		}
	}
	int nRelaySum = GetPntSum(3);
	if(m_wDOSum>0 && nRelaySum>0)
	{
		m_pwDOTrans = new WORD[nRelaySum];
		memset((void*)m_pwDOTrans, 0xFFFF, sizeof(WORD)*nRelaySum);
		for(i=0; i<m_wDOSum; i++)
		{
			if( m_pDOMapTab[i].wStn==0 ) continue;
			k = GetTransNum(3, m_pDOMapTab[i].wStn-1, m_pDOMapTab[i].wPntNum - 1 );
			if(k>=0 && k<nRelaySum)
				m_pwDOTrans[k] = i ;
		}
	}

	/*
	   int nAdjustSum = GetPntSum(4);
	   if(m_wAOSum>0 && nAdjustSum>0)
	   {
	   m_pwAOTrans = new WORD[nAdjustSum];
	   memset((void*)m_pwAOTrans, 0, sizeof(WORD)*nAdjustSum);
	   for(i=0; i<m_wAOSum; i++)
	   {
	   if( m_pAOMapTab[i].wStn==0 || m_pAOMapTab[i].wPntNum==0 ) continue;
	   k = GetTransNum(4, m_pAOMapTab[i].wStn-1, m_pAOMapTab[i].wPntNum-1);
	   if(k>=0 && k<nAdjustSum)
	   m_pwAOTrans[k] = i + 1;
	   }
	   }
	   */
}/*}}}*/

void CRtuBase::ReadMapConfig(LPCSTR lpszFile)
{/*{{{*/
	FILE*   fd;
	char    strLine[256];
	int     nType;
	WORD wNum, wVal1, wVal2, wVal3, wVal4, wVal5;

	fd = fopen(lpszFile, "r");					//rtux.txt
	if(fd == NULL ) {LogPromptText("\n Open file %s failure!", lpszFile); return;}
	while( fgets(strLine, sizeof(strLine), fd) )
	{
		ltrim(strLine);
		if( strLine[0]==';' || strLine[0]=='#' ) continue;
		nType = ParseMapLine(strLine, &wNum, &wVal1, &wVal2, &wVal3, &wVal4, &wVal5);
		switch(nType)
		{
		case YC_NUM:
			if(m_wAISum>0) break;
			m_wAISum = wVal1;
			if(m_wAISum > MAPMAX_AI_LEN) m_wAISum = MAPMAX_AI_LEN;
			if(m_wAISum==0) break;
			m_pAIMapTab = new MAPITEM[m_wAISum];
			memset((void*)m_pAIMapTab, 0, sizeof(MAPITEM)*m_wAISum );
			break;
		case YX_NUM:
			if(m_wDISum>0) break;
			m_wDISum = wVal1;
			if(m_wDISum > MAPMAX_DI_LEN) m_wDISum = MAPMAX_DI_LEN;
			if(m_wDISum==0) break;
			m_pDIMapTab = new MAPITEM[m_wDISum];
			memset((void*)m_pDIMapTab, 0, sizeof(MAPITEM)*m_wDISum );
			break;
		case YM_NUM:
			if(m_wPISum>0) break;
			m_wPISum = wVal1;
			if(m_wPISum > MAPMAX_PI_LEN) m_wPISum = MAPMAX_PI_LEN;
			if(m_wPISum==0) break;
			m_pPIMapTab = new MAPITEM[m_wPISum];
			memset((void*)m_pPIMapTab, 0, sizeof(MAPITEM)*m_wPISum );
			break;
		case YK_NUM:
			if(m_wDOSum>0) break;
			m_wDOSum = wVal1;
			if(m_wDOSum > MAPMAX_DO_LEN) m_wDOSum = MAPMAX_DO_LEN;
			if(m_wDOSum==0) break;
			m_pDOMapTab = new MAPITEM[m_wDOSum];
			memset((void*)m_pDOMapTab, 0, sizeof(MAPITEM)*m_wDOSum );
			break;
		case DZ_NUM:
			if(m_wDZSum>0) break;
			m_wDZSum = wVal1;
			if(m_wDZSum > MAPMAX_AO_LEN) m_wDZSum = MAPMAX_AO_LEN;
			if(m_wDZSum==0) break;
			m_pDZMapTab = new MAPITEM[m_wDZSum];
			memset((void*)m_pDZMapTab, 0, sizeof(MAPITEM)*m_wDZSum );
			break;
		case YC_DEAD: //遥测死区
			m_wDeadVal = wVal1 ;
			break;
		case YC_PROPTY://遥测属性
			m_wRipeVal = wVal1 ;
			break;
		case TIMING://对时允许
			m_wRecvClock = wVal1 ;
			break;
		case YC_PROPERTY :
			if((wNum-1)<m_wAISum)
			{
				m_pAIMapTab[wNum-1].wStn   = wVal1;//byte->word
				m_pAIMapTab[wNum-1].wPntNum = wVal2;
			}
			break;
		case YX_PROPERTY:
			if((wNum-1)<m_wDISum)
			{
				m_pDIMapTab[wNum-1].wStn   = wVal1;
				m_pDIMapTab[wNum-1].wPntNum = wVal2;
			}
			break;
		case YM_PROPERTY:
			if((wNum-1)<m_wPISum)
			{
				m_pPIMapTab[wNum-1].wStn   = wVal1;
				m_pPIMapTab[wNum-1].wPntNum = wVal2;
			}
			break;
		case YK_PROPERTY:
			if((wNum-1)<m_wDOSum)
			{
				m_pDOMapTab[wNum-1].wStn   = wVal1;
				m_pDOMapTab[wNum-1].wPntNum = wVal2;
			}
			break;
		case DZ_PROPERTY:
			if((wNum-1)<m_wDZSum)
			{
				m_pDZMapTab[wNum-1].wStn   = wVal1;
				m_pDZMapTab[wNum-1].wPntNum = wVal2;
			}
			break;
		}
	}
	fclose(fd);
}/*}}}*/

void CRtuBase::SetObjParam(WORD wPnt, WORD wSignSum, WORD wPos1, WORD wPos2, WORD wPos3)
{/*{{{*/
}/*}}}*/

int CRtuBase::GetProtUnitAttr(BYTE byType, WORD wAddr, BYTE& byPro, WORD& wStn, WORD& wPnt)
{/*{{{*/
	return -1;
}/*}}}*/

BOOL CRtuBase::InitRtuBase()
{/*{{{*/
	OutPromptText((char *)"****CRtuBase.Init()****");
	m_semAIEMutex.Create(RTUKEY1+m_wObjNum);
	m_semDIEMutex.Create(RTUKEY2+m_wObjNum);
	m_semSOEMutex.Create(RTUKEY3+m_wObjNum);
	return TRUE;
}/*}}}*/

RTUMSG* CRtuBase::LoadRtuMessage(void)
{/*{{{*/
	return NULL;
}/*}}}*/

void CRtuBase::RtuCommandProc( BYTE* pRecvBuf, int nLen )
{/*{{{*/
}/*}}}*/

int CRtuBase::GetRealVal(BYTE byType, WORD wPnt, void *v)
{/*{{{*/
	return -1;
}/*}}}*/

int CRtuBase::GetDIValue(WORD wPnt, WORD wSrcNo, void *v)
{/*{{{*/
	return -1;
}/*}}}*/

void CRtuBase::RelayEchoProc(BYTE byCommand, WORD wIndex, BYTE byResult)
{/*{{{*/
}/*}}}*/

void CRtuBase::WriteProtMessage(BYTE byPro, WORD wParam, BYTE* pBuf, int nByte)
{/*{{{*/
}/*}}}*/

void CRtuBase::WriteProtVal(WORD wPnt, BYTE byType, WORD wNum, WORD wVal, LONG lTime, WORD wMiSecond)
{/*{{{*/
}/*}}}*/

int CRtuBase::TransMessage( BYTE byType, BYTE* pBuf, int nLen )
{/*{{{*/
	return -1;
}/*}}}*/

void CRtuBase::ReadAnalogData(float *pData)
{/*{{{*/
	if(m_pAIMapTab==NULL) return;
	for( WORD i=0; i<m_wAISum; i++ )
	{
		if( m_pAIMapTab[i].wStn == 0 ||
				m_pAIMapTab[i].wPntNum == 0 ) continue;
		const ANALOGITEM *pItem =
			Get_RTDB_Analog(m_pAIMapTab[i].wStn-1, m_pAIMapTab[i].wPntNum-1);
		if( !pItem ) break;
		pData[i] = (WORD)pItem->fRealVal;
	}
}/*}}}*/

void CRtuBase::ReadDigitalData(BYTE *pData )
{/*{{{*/
	if(m_pDIMapTab==NULL) return;
	for( WORD i=0; i<m_wDISum; i++ )
	{
		if( m_pDIMapTab[i].wStn == 0 ||
				m_pDIMapTab[i].wPntNum == 0 ) continue;
		const DIGITALITEM *pItem =
			Get_RTDB_Digital(m_pDIMapTab[i].wStn-1, m_pDIMapTab[i].wPntNum-1);
		if( !pItem ) break;

		if( (pItem->wStatus & DISTATUS_VALUE) != 0 )
			pData[ i ] = 1 ;
		else
			pData[ i ] = 0 ;
	}
}/*}}}*/

void CRtuBase::ReadPulseData(DWORD *pData)
{/*{{{*/
	if(m_pPIMapTab==NULL) return;
	for( WORD i=0; i<m_wPISum; i++ )
	{
		if( m_pPIMapTab[i].wStn == 0 ||
				m_pPIMapTab[i].wPntNum == 0 ) continue;
		const PULSEITEM *pItem =
			Get_RTDB_Pulse(m_pPIMapTab[i].wStn-1, m_pPIMapTab[i].wPntNum-1);
		if( !pItem ) break;
		pData[i] = pItem->dwRawVal;
	}
}/*}}}*/

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CRtuBase
 *      Method:  GetPulseData
 * Description:  获取单个遥脉值 
 *       Input:  站序号 点号（从1开始）
 *		Return:  DWORD 遥脉值 bIsGet = FALSE 失败
 *--------------------------------------------------------------------------------------
 */
DWORD CRtuBase::GetPulseData ( WORD wStn, WORD wPnt, BOOL *bIsGet  )
{/*{{{*/
	*bIsGet = FALSE;

	if ( 0 == wStn || 0 == wPnt )
	{
		return 0;
	}

	const PULSEITEM *pItem = Get_RTDB_Pulse( wStn-1, wPnt-1 );
	if( !pItem )
	{
		return 0;
	}

	*bIsGet = TRUE;
	return pItem->dwRawVal;
}		/* -----  end of method CRtuBase::GetPulseData  ----- *//*}}}*/

void CRtuBase::AddAnalogEvt( WORD wSerialNo ,WORD wPnt, float fVal)
{/*{{{*/
	if((wSerialNo > 511) || (wPnt > 4095)){
		printf("\nserialno or point overflow  line:%d\n", __LINE__);
		return;
	}
	m_semAIEMutex.semTake();

	SETDATA data ;
	data.wPnt = wPnt ;
	data.fVal = fVal ;
	data.wSerialNo = wSerialNo ;
	m_dwAIEQueue.push_back( data );
	//printf("^^^^^^^^	m_dwAIEQueue:%d	^^^^^^^^\n", m_dwAIEQueue.size());
	if(m_dwAIEQueue.size()>RTUMAX_AIE_LEN)
		m_dwAIEQueue.pop_front();

	m_semAIEMutex.semGive();
}/*}}}*/

BOOL CRtuBase::GetAnalogEvt( WORD &wSerialNo ,WORD& wPnt, float& fVal)
{/*{{{*/
	m_semAIEMutex.semTake();
	if(m_dwAIEQueue.empty()){
		m_semAIEMutex.semGive();
		return FALSE;
	}

	SETDATA data = m_dwAIEQueue.front() ;
	if((data.wSerialNo > 511) || (data.wPnt > 4095)){
		printf("\nserialno or point overflow  line:%d\n", __LINE__);
		m_dwAIEQueue.pop_front();
		m_semAIEMutex.semGive();
		return FALSE;
	}
	wSerialNo = data.wSerialNo ;
	wPnt = data.wPnt ;
	fVal = data.fVal ;

	m_dwAIEQueue.pop_front();
	m_semAIEMutex.semGive();

	return TRUE;
}/*}}}*/

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CRtuBase
 *      Method:  GetAIRipeVal
 * Description:  获取一次遥测值
 *       Input:  站序号 点号（从1开始）
 *		Return:  float 遥测值 bIsGet == FALSE 失败
 *--------------------------------------------------------------------------------------
 */
float CRtuBase::GetAIRipeVal ( WORD wStn, WORD wPnt, BOOL *bIsGet )
{/*{{{*/
	float fVal;
	if( NULL != bIsGet )
		*bIsGet = FALSE;
	if( 0 == wStn || 0 == wPnt )
	{
		return 0;
	}

	const ANALOGITEM *pItem = Get_RTDB_Analog(wStn-1, wPnt-1);
	if( pItem )
	{
		fVal = pItem->fRealVal * pItem->fRatio + pItem->fOffset;
	}
	else
	{
		return 0;
	}
	if( NULL != bIsGet )
		*bIsGet = TRUE;
	return fVal;
}		/* -----  end of method CRtuBase::GetAIRipeVal  ----- *//*}}}*/

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CRtuBase
 *      Method:  CalcAIRipeVal
 * Description:  获取一次遥测值
 *       Input:  站序号 点号（从1开始）
 *		Return:  float 遥测值 bIsGet == FALSE 失败
 *--------------------------------------------------------------------------------------
 */
float CRtuBase::CalcAIRipeVal ( WORD wStn, WORD wPnt, float fYcVal, BOOL *bIsGet )
{/*{{{*/
	float fVal;
	if( NULL != bIsGet )
		*bIsGet = FALSE;
	if( 0 == wStn || 0 == wPnt )
	{
		return 0;
	}

	const ANALOGITEM *pItem = Get_RTDB_Analog(wStn-1, wPnt-1);
	if( pItem )
	{
		fVal = fYcVal * pItem->fRatio + pItem->fOffset;
	}
	else
	{
		return 0;
	}
	if( NULL != bIsGet )
		*bIsGet = TRUE;
	return fVal;
}		/* -----  end of method CRtuBase::CalcAIRipeVal  ----- *//*}}}*/

/*******************************************************************************
 * 类:CRtuBase
 * 函数名:CalcPulseRipepeVal
 * 功能描述: 计算遥脉的一次侧值
 * 参数: WORD wStn, WORD wPnt , dwYmVal 原始值
 * 返回值:成功bIsGet = TRUE; 
 ******************************************************************************/
DWORD CRtuBase::CalcPulseRipeVal( WORD wStn, WORD wPnt, DWORD dwYmVal, BOOL *bIsGet )
{/*{{{*/
	DWORD dwVal = 0;

	if( NULL != bIsGet )
		*bIsGet = FALSE;

	if( 0 == wStn || 0 == wPnt )
	{
		return 0;
	}

	const PULSEITEM *pItem = Get_RTDB_Pulse(wStn-1, wPnt-1);
	if( pItem )
	{
		dwVal = ( DWORD)( dwYmVal * pItem->fRatio ); 
	}
	else
	{
		return 0;
	}

	if( NULL != bIsGet )
		*bIsGet = TRUE;

	return dwVal;

}   /*-------- end class CRtuBase method CalcPulseRipeVal -------- *//*}}}*/

void CRtuBase::AddDigitalEvt(WORD wSerialNo ,WORD wPnt, WORD wVal)
{/*{{{*/
	m_semDIEMutex.semTake();

	SETDATA data ;
	data.wPnt = wPnt ;
	data.wVal = wVal ;
	data.wSerialNo = wSerialNo ;

	m_dwDIEQueue.push_back(data);
	if(m_dwDIEQueue.size()>RTUMAX_DIE_LEN)
		m_dwDIEQueue.pop_front();
	m_semDIEMutex.semGive();

}/*}}}*/

BOOL CRtuBase::GetDigitalEvt(WORD &wSerialNo ,WORD& wPnt, WORD& wVal)
{/*{{{*/
	if(m_dwDIEQueue.empty()) return FALSE;
	m_semDIEMutex.semTake();

	SETDATA data = m_dwDIEQueue.front() ;
	wSerialNo = data.wSerialNo ;
	wVal = data.wVal ;
	wPnt = data.wPnt ;

	m_dwDIEQueue.pop_front();
	m_semDIEMutex.semGive();

	return TRUE;
}/*}}}*/

void CRtuBase::AddSOEInfo(WORD wSerialNo ,WORD wPnt, WORD wVal, LONG lTime, WORD wMiSecond)
{/*{{{*/
	m_semSOEMutex.semTake();
	m_soeBuffer[m_iSOE_wr_p].lTime = lTime;
	m_soeBuffer[m_iSOE_wr_p].wMiSecond = wMiSecond;
	m_soeBuffer[m_iSOE_wr_p].wPntNum   = wPnt;
	m_soeBuffer[m_iSOE_wr_p].wStatus   = wVal;
	m_soeBuffer[m_iSOE_wr_p].wSerialNo = wSerialNo ;
	m_iSOE_wr_p = (m_iSOE_wr_p+1) % RTUMAX_SOE_LEN;
	if( m_iSOE_wr_p == m_iSOE_rd_p )
		m_iSOE_rd_p = (m_iSOE_rd_p+1) % RTUMAX_SOE_LEN;
	m_semSOEMutex.semGive();
}/*}}}*/

BOOL CRtuBase::GetSOEInfo(WORD &wSerialNo ,WORD& wPnt, WORD& wVal, LONG& lTime, WORD& wMiSecond)
{/*{{{*/
	/*if(m_iSOE_rd_p==m_iSOE_wr_p) return FALSE;*/
	m_semSOEMutex.semTake();
	lTime     = m_soeBuffer[m_iSOE_rd_p].lTime;
	wMiSecond = m_soeBuffer[m_iSOE_rd_p].wMiSecond;
	wPnt      = m_soeBuffer[m_iSOE_rd_p].wPntNum;
	wVal      = m_soeBuffer[m_iSOE_rd_p].wStatus;
	wSerialNo = m_soeBuffer[m_iSOE_rd_p].wSerialNo ;
	m_iSOE_rd_p = (m_iSOE_rd_p+1) % RTUMAX_SOE_LEN;
	m_semSOEMutex.semGive();
	return TRUE;
}/*}}}*/

BOOL CRtuBase::GetSOEInfo(WORD &wSerialNo ,WORD *wPnt, WORD *wVal, void *pTime, WORD *wMiSecond)
{/*{{{*/
	if(m_iSOE_rd_p==m_iSOE_wr_p)
		return FALSE;

	m_semSOEMutex.semTake();
	GetOwnStructTm(  m_soeBuffer[m_iSOE_rd_p].lTime, (struct tm *)pTime);
	// localtime_r((time_t *)&m_soeBuffer[m_iSOE_rd_p].lTime, (struct tm *)pTime);
	// struct tm* p = ( struct tm * )pTime;
	// printf ( "month=%d year=%d lTime=%ld\n", p->tm_mon, p->tm_year , m_soeBuffer[m_iSOE_rd_p].lTime);
	*wMiSecond = m_soeBuffer[m_iSOE_rd_p].wMiSecond;
	*wPnt      = m_soeBuffer[m_iSOE_rd_p].wPntNum;
	*wVal      = m_soeBuffer[m_iSOE_rd_p].wStatus;
	wSerialNo = m_soeBuffer[m_iSOE_rd_p].wSerialNo ;
	m_iSOE_rd_p = (m_iSOE_rd_p+1) % RTUMAX_SOE_LEN;
	m_semSOEMutex.semGive();
	return TRUE;
}/*}}}*/

int CRtuBase::GetCommObjProp(COMMOBJ_PROP* pObjProp)
{/*{{{*/
	pObjProp->byStyle  = 1;
	pObjProp->byObjNum = LOBYTE(m_wObjNum);
	sprintf(pObjProp->szObjName, m_szObjName);
	sprintf(pObjProp->szChannel, "%s", "");
	pObjProp->wPortNum = 0;
	pObjProp->wStatus  = 0;
	return 0;
}/*}}}*/
/******************************************************************************/

int GetPntSum(BYTE byType)
{/*{{{*/
	const SYSINFO *pDBInfo = Get_RTDB_SysInfo();
	if( !pDBInfo ) return -1;
	switch( byType )
	{
	case 0: return pDBInfo->nAnalogSum;
	case 1: return pDBInfo->nDigitalSum;
	case 2: return pDBInfo->nPulseSum;
	case 3: return pDBInfo->nRelaySum;
	}
	return -2;
}/*}}}*/

void CRtuBase::RtuWriteCIVal(WORD wStn, WORD wPnt,  float fVal)
{/*{{{*/
	WriteCIVal( wStn , wPnt,  fVal);
}/*}}}*/

void CRtuBase::RtuWriteAIVal(WORD wStn, WORD wPnt,  float fVal)		//wStn:顺序号, wPnt:表点号 fVal:转发值
{/*{{{*/
	int k;

	k = GetTransNum(0,  wStn,  wPnt);
	if( k < 0 || k  >=  MAX_ANALOG_SUM  ) return;
	WORD wNum = (WORD)k;											//k:转发通道点序号
	WriteAIVal( wStn , wNum,  fVal );

}/*}}}*/

void CRtuBase::RtuWriteDIVal(WORD wStn,   WORD wPnt,  WORD wVal)
{/*{{{*/
	int k;
	k = GetTransNum(1,  wStn,  wPnt);
	if( k < 0 || k  >=  MAX_DIGITAL_SUM  ) return;
	WORD wNum = (WORD)k;
	WriteDIVal( wStn , wNum,  wVal);

}/*}}}*/

void CRtuBase::RtuWritePIVal(WORD wStn,  WORD wPnt,  DWORD dwVal)
{/*{{{*/
	int k;
	k = GetTransNum(2,  wStn,  wPnt);
	if( k < 0 || k  >=  MAX_PULSE_SUM  ) return;
	WORD wNum = (WORD)k;
	WritePIVal( wStn , wNum,  dwVal);
}/*}}}*/

void CRtuBase::RtuWriteSOEInfo(WORD wStn,  WORD wPnt,  WORD wVal,  LONG lTime,  WORD wMiSecond)
{/*{{{*/
	int  k;
	k = GetTransNum(1,  wStn,  wPnt);
	if( k < 0 || k  >=  MAX_DIGITAL_SUM  ) return;
	WORD wNum = (WORD)k;
	WriteSOEInfo( wStn , wNum,  wVal,  lTime,  wMiSecond);
}/*}}}*/

void CRtuBase::RelayProc(BYTE byCommand, WORD wStn, WORD wCtrlNum, BYTE byResult)
{/*{{{*/
	int     k = 0 ;

	k = GetTransNum(3, wStn, wCtrlNum);
	if( k < 0 || k >= GetPntSum(3) ) return;
	WORD wIndex = (WORD)k;
	RelayEchoProc(byCommand, wIndex, byResult);
}/*}}}*/
/*
 *--------------------------------------------------------------------------------------
 *       Class:  CRtuBase
 *      Method:  ReadChangData
 * Description:  读变化的遥测遥信等数据
 *		 Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CRtuBase::ReadChangData (  )
{/*{{{*/
	/* 查询变位遥信和SOE信息*/
	while( m_iSOEPos != m_pDBInfo->nSOEWritePos  )
	{
		const SOEITEM *p = Read_RTDB_SOE(m_iSOEPos);
		if(p->wAttrib == 0) RtuWriteDIVal(p->wStnID,  p->wPntNum,  p->byState);
		else
		{
			if(p->wAttrib >= 2)
				RtuWriteDIVal(p->wStnID,  p->wPntNum,  p->byState);

			RtuWriteSOEInfo(p->wStnID,  p->wPntNum,  p->byState,  p->lTime,  p->wMiSecond);
		}

		//printf( "ReadPos = %d\n" , m_iSOEPos ) ;
		m_iSOEPos = (m_iSOEPos+1)%SOE_QUEUE_SUM;
	}

	/* 查询变化遥测*/
	while( m_iAIEPos != m_pDBInfo->nAIEWritePos  )
	{
		const AIEITEM *p = Read_RTDB_AIE(m_iAIEPos);
		RtuWriteAIVal(p->wStnID,  p->wPntNum,  p->fValue );
		m_iAIEPos = (m_iAIEPos+1)%AIE_QUEUE_SUM;
	}

	return ;
}		/* -----  end of method CRtuBase::ReadChangData  ----- *//*}}}*/

/*
 * 遍历设备状态，将变化者写入deque!
 */
//void CRtuBase::ReadChangeStatus(std::vector<BYTE> &vec_stat, std::deque<Seri_stat> &deque_stat)
void CRtuBase::ReadChangeStatus(std::map<WORD, BYTE> &map_stat, std::deque<Seri_stat> &deque_stat)
{/*{{{*/
	WORD i,size = map_stat.size();
	BOOL status;

	for(i = 0; i < size; i++){
		status = m_pMethod->GetDevCommState(i);
		//if(status != vec_stat[i]){
			//vec_stat[i] = status;
		if(status != map_stat[i]){
			map_stat[i] = status;
			if(deque_stat.size() > 256)
				deque_stat.pop_front();
			deque_stat.push_back(Seri_stat(i, status));
		}
	}
}/*}}}*/

//通过转发序号获得编号
WORD CRtuBase::GetSerialNoFromTrans( BYTE byType , WORD wTranNum )
{/*{{{*/
	WORD wSerialNo = 0xFFFF ;
	switch( byType )
	{
	case YC_TRANSTOSERIALNO:
		{
			if( wTranNum > m_wAISum )
				return wSerialNo  ;

			wSerialNo = m_pAIMapTab[ wTranNum ].wStn -1 ;
		}
		break;
	case YX_TRANSTOSERIALNO:
		{
			if( wTranNum > m_wDISum )
				return wSerialNo  ;

			wSerialNo = m_pDIMapTab[ wTranNum ].wStn -1 ;
		}
		break;
	case YK_TRANSTOSERIALNO:
		{
			if( wTranNum > m_wDOSum )
				return wSerialNo  ;

			wSerialNo = m_pDOMapTab[ wTranNum ].wStn -1 ;
		}
		break;
	case DD_TRANSTOSERIALNO:
		{
			if( wTranNum > m_wPISum )
				return wSerialNo  ;

			wSerialNo = m_pPIMapTab[ wTranNum ].wStn -1 ;
		}
		break;
	}

	return wSerialNo ;
}/*}}}*/

/*
 * =====================================================================================
 *
 *       Filename:  IEC101S_2002.c
 *
 *    Description:  IEC101子站 2002 版
 *
 *        Version:  1.0
 *        Created:  2014年11月18日 13时32分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp (),
 *   Organization:
 *
 *		  history:
 * =====================================================================================
 */


#include "IEC101S_2002.h"
#include <assert.h>
#include "../../share/global.h"


extern "C" void GetCurrentTime( REALTIME *pRealTime );
extern "C" int  SetCurrentTime( REALTIME *pRealTime );

typedef struct _CP56TIME2A
{
	BYTE byLoMis;
	BYTE byHiMis;
	BYTE byMin;
	BYTE byHour;
	BYTE byDay;
	BYTE byMon;
	BYTE byYear;
}CP56TIME2A;
/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  CIEC101S_2002
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
CIEC101S_2002::CIEC101S_2002 ()
{
	//设置装置数据起始地址
	m_wYxStartAddr = 0x0001;
	m_wYcStartAddr = 0x4001;
	m_wYkStartAddr = 0x6001;
	m_wYmStartAddr = 0x6401;
	m_wComStateAddr = 50000;

	//可变化的传送原因 公共地址 和 信息地字节长度
	m_byCotLen = 1;
	m_byAddrLen = 1;
	m_byInfoAddrLen = 2;

	//总召数据类型
	m_byTotalCallYx = 1;//单点遥信
	m_byTotalCallYc = 11;//测量值 标度化值
	m_byTotalCallYm = 15;//累积量

	m_byChangeYx = 1;	//单点信息
	m_bySoeYx = 30;    //cp56time2a 单点信息
	m_byChangeYc = 11; //标度化值
	m_byYkType = IEC101S_2002_YKSINGLE_TYPE;//单点遥信

	//清空缓冲区
	memset( m_fYcBuf, 0, sizeof( m_fYcBuf ) );
	memset( m_byYxBuf, 0, sizeof( m_byYxBuf ) );
	memset( m_dwYmBuf, 0, sizeof( m_dwYmBuf ) );

	//初始化协议状态
	InitProtocolState(  );
}  /* -----  end of method CIEC101S_2002::CIEC101S_2002  (constructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  ~CIEC101S_2002
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
CIEC101S_2002::~CIEC101S_2002 ()
{
}  /* -----  end of method CIEC101S_2002::~CIEC101S_2002  (destructor)  ----- */

/* ====================  OtherBegin    ======================================= */
static void GetCp56Time2a( CP56TIME2A *pCp56Time )
{
	REALTIME curTime;
	WORD wMilliSec;

	GetCurrentTime( &curTime );
	wMilliSec = curTime.wSecond*1000 + curTime.wMilliSec;
	pCp56Time->byLoMis = LOBYTE( wMilliSec );
	pCp56Time->byHiMis = HIBYTE( wMilliSec );
	pCp56Time->byMin = (BYTE)curTime.wMinute;
	pCp56Time->byHour = (BYTE)curTime.wHour;
	pCp56Time->byDay = LOBYTE(curTime.wDay + ( curTime.wDayOfWeek << 5 ));
	pCp56Time->byMon = (BYTE)curTime.wMonth;
	pCp56Time->byYear = curTime.wYear-2000;
}

static CP56TIME2A GetTmToCp56Time2a( struct tm t, WORD wMiSec )
{
	WORD wMilliSec;
	CP56TIME2A tt;

	wMilliSec = t.tm_sec*1000 + wMiSec;
	tt.byLoMis = LOBYTE( wMilliSec );
	tt.byHiMis = HIBYTE( wMilliSec );
	tt.byMin = (BYTE)t.tm_min;
	tt.byHour = (BYTE)t.tm_hour;
	tt.byDay = LOBYTE(t.tm_mday + ( t.tm_wday << 5 ));
	tt.byMon = (BYTE)t.tm_mon;
	tt.byYear = t.tm_year-100;

	return tt;
}

// static float CalcAIRipeVal(BYTE wStn, WORD wPnt, float fYcVal)
// {
// float fVal = 0;
// const ANALOGITEM *pItem = Get_RTDB_Analog(byStn, wPnt);
// if( pItem  )
// {
// fVal = fYcVal * pItem->fRatio + pItem->fOffset;

// }
// return fVal;

// }

int CIEC101S_2002::GetRealVal(BYTE byType, WORD wPnt, void *v)
{
	WORD  wValue = 0 ;
	switch(byType)
	{
	case 0:
		if(wPnt>=IEC101S_2002_MAX_YC_NUM) return -2;
		memcpy(v, &m_fYcBuf[wPnt], sizeof(WORD));
		break;
	case 1:
		{
			if(wPnt>=IEC101S_2002_MAX_YX_NUM)
				return -2;

			if( m_byYxBuf[ wPnt ] ==0 )
				wValue = 0;
			else
				wValue = 1;

			memcpy(v, &wValue, sizeof(WORD));
		}
		break;
	case 2:
		if(wPnt>=IEC101S_2002_MAX_YM_NUM) return -2;
		memcpy(v, &m_dwYmBuf[wPnt], sizeof(DWORD));
		break;
	default:
		return -1;
	}
	return 0;
}

BOOL CIEC101S_2002::WriteAIVal(WORD wSerialNo ,WORD wPnt, float fVal)
{
	if(m_pwAITrans==NULL) return FALSE;
	WORD wNum = m_pwAITrans[wPnt];
	if(wNum>m_wAISum) return FALSE;
	if(wNum<IEC101S_2002_MAX_YC_NUM)//mengqp 将<=改为< 否则m_wAIBuf[4096]越界
	{
		float fDelt = fVal - m_fYcBuf[wNum];
		if(abs((int)fDelt)>=m_wDeadVal)
		{
			m_fYcBuf[wNum] = fVal;
			if(m_bDataInit)
			{
				AddAnalogEvt( wSerialNo , wNum, fVal);
			}
		}
	}
	return TRUE ;
}

BOOL CIEC101S_2002::WriteDIVal(WORD wSerialNo ,WORD wPnt, WORD wVal)
{
	if(m_pwDITrans==NULL) return FALSE;
	WORD wNum = m_pwDITrans[wPnt] & 0x7fff;
	if(wNum>m_wDISum) return FALSE;
	if( wNum<IEC101S_2002_MAX_YX_NUM)//mengqp 将<= 改为<
	{
		if( m_byYxBuf[ wNum ] != wVal )
		{
			m_byYxBuf[ wNum ] = wVal ;
			if(m_bDataInit)
			{
				AddDigitalEvt( wSerialNo , wNum, wVal);
			}
		}
	}
	return TRUE ;
}
BOOL CIEC101S_2002::WritePIVal(WORD wSerialNo ,WORD wPnt, DWORD dwVal)
{
	if(m_pwPITrans==NULL) return FALSE;
	WORD wNum = m_pwPITrans[wPnt];
	if(wNum>m_wPISum) return FALSE;
	if(wNum<IEC101S_2002_MAX_YM_NUM)//mengqp 将<= 改为<
	{
		m_dwYmBuf[wNum] = dwVal;
	}
	return TRUE ;
}

BOOL CIEC101S_2002::WriteSOEInfo( WORD wSerialNo ,WORD wPnt, WORD wVal, LONG lTime, WORD wMiSecond)
{
	if(m_pwDITrans==NULL) return FALSE;
	WORD wNum = m_pwDITrans[wPnt] & 0x7fff;
	if(wNum>=m_wDISum) return FALSE;
	if(wNum<IEC101S_2002_MAX_YX_NUM)
	{
		AddSOEInfo(wSerialNo , wNum, wVal, lTime, wMiSecond);
	}
	return TRUE ;
}
/* ====================  OtherEnd    ======================================= */

/* ====================  RecvBegin     ======================================= */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIE101S_2002
 *      Method:  SetRecvParam
 * Description:  设置接收参数
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CIEC101S_2002::SetRecvParam ( void )
{
	m_bSending = FALSE;
	m_bReSending = FALSE;

	m_bySendCount = 0;
	m_byRecvCount ++;
	m_byResendCount = 0;

	m_bLinkStatus = TRUE;
}		/* -----  end of method CIE101S_2002::SetRecvParam  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  ProcessCtlBit
 * Description:  处理控制字 0xf
 *       Input:  字符
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::ProcessCtlBit ( BYTE c )
{
	switch ( c & 0x0f )
	{
	case 0: //发送/确认帧 远方链路复位
		m_SendStatus = RECOGNITION;
		InitProtocolState(  );		 //初始化协议参数
		m_dwSendFlag |= IEC101S_SPECIAL_DATA;
		print( "发送/确认帧 远方链路复位" );
		break;

	case 1:	//发送/确认帧 用户进程复位
		print( "发送/确认帧 用户进程复位" );
		break;

	case 2:	//发送/确认帧 平衡传输保留
		print( "发送/确认帧 平衡传输保留" );
		break;

	case 3:	//发送/确认帧 用户数据
		print( "发送/确认帧 用户数据" );
		m_SendStatus = USER_DATA;
		break;

	case 4: //发送/无回答帧 用户数据
		print( "发送/无回答帧 用户数据" );
		m_SendStatus = NONE_USER_DATA;
		break;

	case 8:	//发送/响应 要求访问位响应
		print( "发送/响应 要求访问位响应" );
		break;

	case 9:	//发送/响应 请求链路状态
		print( "发送/响应 请求链路状态" );
		m_SendStatus = LINK_STATUS;
		m_dwSendFlag |= IEC101S_SPECIAL_DATA;
		break;

	case 10://发送/响应 请求一级数据
		print( "发送/响应 请求一级数据" );
		m_SendStatus = LEVEL1_DATA;
		break;

	case 11://发送/响应 请求二级数据
		print( "发送/响应 请求二级数据" );
		m_SendStatus = LEVEL2_DATA;
		break;

	default:
		print( "保留或为协商的特殊应用保留" );
		return FALSE;
		break;
	}				/* -----  end switch  ----- */

	return TRUE;
}		/* -----  end of method CIEC101S_2002::ProcessCtlBit  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  SetYkParam
 * Description:  设置遥控参数
 *       Input:  ASDU类型 原因 站好 点号 值
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CIEC101S_2002::SetYkParam ( BYTE byType, BYTE byCot, WORD wStn, WORD wPnt, BYTE byStatus )
{
	m_byYKAsduType = byType;
	m_byYkCot = byCot;
	m_wYkStn = wStn;
	m_wYkPnt = wPnt;
	m_byYkStatus = byStatus;
}		/* -----  end of method CIEC101S_2002::SetYkParam  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  IsYkParamTrue
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::IsYkParamTrue ( BYTE byType, BYTE byCot, WORD wStn, WORD wPnt, BYTE byStatus ) const
{
	BYTE byTmp;
	if( 0x2d == byType )
	{
		byTmp = IEC101S_2002_YKSINGLE_TYPE;
	}
	else if( 0x2e == byType )
	{
		byTmp = IEC101S_2002_YKDOUBLE_TYPE;
	}
	else
	{
		printf ( "IEC101S yk type=%x is err!!!\n", byType );
		return FALSE;
	}

	if( byTmp != m_byYKAsduType
			|| byCot != m_byYkCot
			|| wStn != m_wYkStn
			|| wPnt != m_wYkPnt
			|| byStatus != m_byYkStatus)
	{
		return TRUE;
	}
	else
	{
		printf ( "IEC101S YK type=%x %x cot=%x %x stn=%d %d pnt=%d %d status=%d %d\n" ,
				byType, m_byYkType, byCot, m_byYkCot, wStn, m_wYkStn, wPnt, m_wYkPnt , byStatus, m_byYkStatus);

		return FALSE;
	}
}		/* -----  end of method CIEC101S_2002::IsYkParamTrue  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  ProcessYkBuf
 * Description:  处理遥控报文
 *       Input:
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::ProcessYkBuf ( const BYTE *buf, int len )
{
	const BYTE *pointer = &buf[8];

	//传送原因
	BYTE byCot = *pointer++;
	if( 2 == m_byCotLen )
	{
		pointer++;
	}

	// 公共地址
	WORD wAddr = *pointer++;
	if ( 2 == m_byAddrLen )
	{
		pointer ++;
	}
	if( wAddr != m_wDevAddr )
	{
		printf ( "IEC101S 地址不正确\n" );
		return FALSE;
	}

	WORD wInfoAddr = MAKEWORD( *pointer, *(	pointer + 1 ) );
	pointer += 2;
	if( 3 == m_byInfoAddrLen )
	{
		pointer ++;
	}
	// if( wInfoAddr < 6001 || wInfoAddr > 6200 )
	// {
	// return FALSE;
	// }

	//点号
	WORD wNum = wInfoAddr - m_wYkStartAddr;
	if ( wNum > m_wDOSum )
	{
		printf ( "IEC101S 位置不正确\n" );
		return FALSE;
	}
	WORD byStn = m_pDOMapTab[wNum].wStn - 1;
	WORD wPnt = m_pDOMapTab[wNum].wPntNum - 1;

	//遥控值
	BYTE bySdco = *pointer;
	BYTE byStatus = 0xff;
	if( IEC101S_2002_YKSINGLE_TYPE == m_byYkType )
	{
		byStatus = ( bySdco & 0x01 );
	}
	else if( IEC101S_2002_YKDOUBLE_TYPE == m_byYkType )
	{
		byStatus = ( bySdco & 0x03 ) -1;
	}
	if(0 != byStatus && 1 != byStatus)
	{
		printf ( "IEC101S 状态不正确\n" );
		return FALSE;
	}

	//遥控类型
	if( 0x06 == byCot )
	{
		if( 0 != (0x80 & bySdco ) )
		{
			BYTE byBusNo;
			WORD wDevAddr;
			printf ( "IEC101S 遥控选择\n" );
			if( m_pMethod->GetBusLineAndAddr( byStn, byBusNo, wDevAddr ) )
			{
				m_dwSendFlag = IEC101S_YK_SEL;
				SetYkParam( buf[6], byCot, byStn, wPnt, byStatus  );
				m_pMethod->SetYkSel(this, byBusNo, wDevAddr, wPnt, byStatus);
			}
		}
		else
		{
			if ( IsYkParamTrue( buf[6], byCot, byStn, wPnt, byStatus ) )
			{
				BYTE byBusNo;
				WORD wDevAddr;
				printf ( "IEC101S 遥控执行\n" );
				if( m_pMethod->GetBusLineAndAddr( byStn, byBusNo, wDevAddr ) )
				{
					m_dwSendFlag = IEC101S_YK_EXE;
					m_pMethod->SetYkExe(this, byBusNo, wDevAddr, wPnt, byStatus);
				}
			}
		}
	}
	else if( 0x08 == byCot )
	{
		BYTE byBusNo;
		WORD wDevAddr;
		if( m_pMethod->GetBusLineAndAddr( byStn, byBusNo, wDevAddr ) )
		{
			m_dwSendFlag = IEC101S_YK_CANCEL;
			SetYkParam( buf[6], byCot, byStn, wPnt, byStatus  );
			m_pMethod->SetYkCancel(this, byBusNo, wDevAddr, wPnt, byStatus);
		}
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::ProcessYkBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  ProcessTotalCallBuf
 * Description:  处理总招数据
 *       Input:
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::ProcessTotalCallBuf ( const BYTE *buf, int len )
{
	const BYTE *pointer = &buf[6];
	pointer ++;
	//判断限定词
	BYTE byVsq = *pointer++;
	if( 1 != byVsq )
	{
		sprintf( m_szPrintBuf, "IEC101S totalcall VSQ=%d err", byVsq );
		print( m_szPrintBuf );
		return FALSE;
	}

	//传送原因
	BYTE byCot = *pointer++;
	if( 2 == m_byCotLen )
	{
		pointer++;
	}
	if( 6 != byCot )
	{
		sprintf( m_szPrintBuf, "IEC101S totalcall COT=%d err", byCot );
		print( m_szPrintBuf );
		return FALSE;
	}

	// 公共地址
	WORD wAddr = *pointer++;
	if ( 2 == m_byAddrLen )
	{
		pointer ++;
	}
	if( wAddr != m_wDevAddr )
	{
		sprintf( m_szPrintBuf, "IEC101S totalcall ADDR=%d err", wAddr );
		print( m_szPrintBuf );
		return FALSE;
	}

	//信息对象地址
	WORD wInfoAddr = MAKEWORD( *pointer, *(	pointer + 1 ) );
	pointer += 2;
	if( 3 == m_byInfoAddrLen )
	{
		pointer ++;
	}
	if( 0 != wInfoAddr )
	{
		sprintf( m_szPrintBuf, "IEC101S totalcall INFOADDR=%d err", wInfoAddr );
		print( m_szPrintBuf );
		return FALSE;
	}

	//召换限定词
	BYTE byQoi = *pointer++;
	if( 0x14 != byQoi )
	{
		sprintf( m_szPrintBuf, "IEC101S totalcall QOI=%d err", byQoi );
		print( m_szPrintBuf );
		return FALSE;
	}
	return TRUE;
}		/* -----  end of method CIEC101S_2002::ProcessTotalCallBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  ProcessYMCallBuf
 * Description:	 处理遥脉招唤数据j
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::ProcessYMCallBuf ( const BYTE *buf, int len )
{
	const BYTE *pointer = &buf[6];
	pointer ++;
	//判断限定词
	BYTE byVsq = *pointer++;
	if( 1 != byVsq )
	{
		sprintf( m_szPrintBuf, "IEC101S ymcall VSQ=%d err", byVsq );
		print( m_szPrintBuf );
		return FALSE;
	}

	//传送原因
	BYTE byCot = *pointer++;
	if( 2 == m_byCotLen )
	{
		pointer++;
	}
	if( 6 != byCot )
	{
		sprintf( m_szPrintBuf, "IEC101S ymcall COT=%d err", byCot );
		print( m_szPrintBuf );
		return FALSE;
	}

	// 公共地址
	WORD wAddr = *pointer++;
	if ( 2 == m_byAddrLen )
	{
		pointer ++;
	}
	if( wAddr != m_wDevAddr )
	{
		sprintf( m_szPrintBuf, "IEC101S ymcall ADDR=%d err", wAddr );
		print( m_szPrintBuf );
		return FALSE;
	}

	//信息对象地址
	WORD wInfoAddr = MAKEWORD( *pointer, *(	pointer + 1 ) );
	pointer += 2;
	if( 3 == m_byInfoAddrLen )
	{
		pointer ++;
	}
	if( 0 != wInfoAddr )
	{
		sprintf( m_szPrintBuf, "IEC101S ymcall INFOADDR=%d err", wInfoAddr );
		print( m_szPrintBuf );
		return FALSE;
	}

	//召换限定词
	BYTE byQoi = *pointer++;
	if( 0x14 != byQoi )
	{
		sprintf( m_szPrintBuf, "IEC101S ymcall QOI=%d err", byQoi );
		print( m_szPrintBuf );
		return FALSE;
	}
	return TRUE;
}		/* -----  end of method CIEC101S_2002::ProcessYMCallBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  ProcessTimeSyncBuf
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::ProcessTimeSyncBuf ( const BYTE *buf, int len )
{
	const BYTE *pointer = &buf[6];
	pointer ++;
	//判断限定词
	BYTE byVsq = *pointer++;
	if( 1 != byVsq )
	{
		sprintf( m_szPrintBuf, "IEC101S timesync VSQ=%d err", byVsq );
		print( m_szPrintBuf );
		return FALSE;
	}

	//传送原因
	BYTE byCot = *pointer++;
	if( 2 == m_byCotLen )
	{
		pointer++;
	}
	if( 6 != byCot )
	{
		sprintf( m_szPrintBuf, "IEC101S timesync COT=%d err", byCot );
		print( m_szPrintBuf );
		return FALSE;
	}

	// 公共地址
	WORD wAddr = *pointer++;
	if ( 2 == m_byAddrLen )
	{
		pointer ++;
	}
	if( wAddr != m_wDevAddr && wAddr != 0xff)
	{
		sprintf( m_szPrintBuf, "IEC101S timesync ADDR=%d err", wAddr );
		print( m_szPrintBuf );
		return FALSE;
	}

	//信息对象地址
	WORD wInfoAddr = MAKEWORD( *pointer, *(	pointer + 1 ) );
	pointer += 2;
	if( 3 == m_byInfoAddrLen )
	{
		pointer ++;
	}
	if( 0 != wInfoAddr )
	{
		sprintf( m_szPrintBuf, "IEC101S timesync INFOADDR=%d err", wInfoAddr );
		print( m_szPrintBuf );
		return FALSE;
	}

	//cp56time2a
	REALTIME curTime;
	WORD wMiSecond = MAKEWORD( *(pointer), *(pointer + 1) );
	pointer += 2;
	curTime.wMilliSec = wMiSecond % 1000;
	curTime.wSecond = wMiSecond / 1000;
	curTime.wMinute = (*pointer++) & 0x3f;
	curTime.wHour = (*pointer++) & 0x1f;
	curTime.wDay = (*pointer++) & 0x1f;
	curTime.wMonth = (*pointer++) & 0x0f;
	curTime.wYear = ( (*pointer++) + 2000 );

	if( curTime.wSecond < 60
			&& curTime.wMinute < 60
			&& curTime.wHour < 24
			&& curTime.wDay <= 31
			&& curTime.wMonth <= 12
			&& curTime.wYear < 2030)
	{
		SetCurrentTime( &curTime );
	}
	else
	{
		sprintf( m_szPrintBuf, "IEC101 timesync SetCurrentTime err!!!time=%d-%d-%d %d:%d:%d",
				curTime.wYear, curTime.wMonth, curTime.wDay, curTime.wHour, curTime.wMinute,curTime.wSecond);
		print( m_szPrintBuf );
	}
	return TRUE;
}		/* -----  end of method CIEC101S_2002::ProcessTimeSyncBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  ProcessHead68Buf
 * Description:  处理第一个缓冲区是0x68 的数据
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::ProcessHead68Buf ( const BYTE *pBuf, int len )
{
	//判断控制字的标示位是否有效
	if ( !ProcessJudgeFlag( pBuf[4] & 0xf0 ) )
	{
		print( "ProcessJudgeFlag" );
		return FALSE;
	}

	//处理控制字
	if ( !ProcessCtlBit( pBuf[4] & 0x0f ) )
	{
		print( "ProcessCtlBit" );
		return FALSE;
	}

	switch ( pBuf[6] )
	{
	case 45:
		print( "单点遥控" );
		m_byYkType = IEC101S_2002_YKSINGLE_TYPE;
		ProcessYkBuf( pBuf, len );
		break;

	case 46:
		print( "双点遥控" );
		m_byYkType = IEC101S_2002_YKDOUBLE_TYPE;
		ProcessYkBuf( pBuf, len );
		break;

	case 47:
		print( "步调节" );
		break;

	case 48:
		print( "设定值 归一化值" );
		break;

	case 49:
		print( "设定值 标度化值" );
		break;

	case 50:
		print( "设定值 短浮点数" );
		break;

	case 100:
		print( "总召唤命令" );
		if ( ProcessTotalCallBuf( pBuf, len ) )
		{
			m_dwSendFlag |= (IEC101S_TOTAL_CALL | IEC101S_TOTAL_YX | IEC101S_TOTAL_YC| IEC101S_TOTAL_CALL_END);
			m_dwSendFlag |= IEC101S_SPECIAL_DATA;
		}
		else
		{
			print( "总召唤命令错误" );
		}
		break;

	case 101:
		// m_SendStatus = TOTAL_CALL_YM_BEGIN;
		print( "计量数召唤命令" );
		if ( ProcessYMCallBuf( pBuf, len ) )
		{
			m_dwSendFlag |= IEC101S_CALL_YM | IEC101S_TOTAL_YM | IEC101S_CALL_YM_END;
			m_dwSendFlag |= IEC101S_SPECIAL_DATA;
		}
		else
		{
			print( "计量数召唤命令错误" );
		}
		break;

	case 102:
		print( "读命令" );
		break;

	case 103:
		print( "时钟同步命令" );
		if( ProcessTimeSyncBuf( pBuf, len ) )
		{
			m_SendStatus = TIME_SYNC;
			m_dwSendFlag |= IEC101S_TIME_SYNC ;
			m_dwSendFlag |= IEC101S_SPECIAL_DATA;
		}
		else
		{
			print( "对时错误" );
		}
		break;

	case 104:
		print( "测试命令" );
		break;

	case 105:
		print( "复位进程命令" );
		break;

	case 106:
		print( "延时获得命令" );
		break;

	default:
		break;
	}				/* -----  end switch  ----- */

	return TRUE;
}		/* -----  end of method CIEC101S_2002::ProcessHead68Buf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  ProcessHead10Buf
 * Description:  处理第一个缓冲区是0x10 的数据
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::ProcessHead10Buf ( const BYTE *pBuf, int len )
{
	//判断控制字的标示位是否有效
	if ( !ProcessJudgeFlag( pBuf[1] & 0xf0 ) )
	{
		print( "IEC101S:ProcessJudgeFlag err" );
		return FALSE;
	}

	//处理控制字
	if ( !ProcessCtlBit( pBuf[1] & 0x0f ) )
	{
		print( "IEC101S:ProcessCtlBit err" );
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::ProcessHead10Buf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  ProcessRecvBuf
 * Description:  处理接收数据
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::ProcessRecvBuf ( const BYTE *pBuf, int len )
{
	if ( pBuf[0] == 0x10 )  //固定帧数据处理
	{
		return ProcessHead10Buf( pBuf, len );
	}
	else if( pBuf[0] == 0x68 ) //变化帧数据处理
	{
		return ProcessHead68Buf( pBuf, len );
	}
	else//未找到类型帧
	{
		print( "ProcessRecvBuf err" );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::ProcessRecvBuf  ----- */

/* ====================  RecvEnd     ======================================= */
/* ====================  SendBegin     ======================================= */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  IsYkBusMsgValue
 * Description:  遥控返回信息是否有效
 *       Input:  消息指针
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::IsYkRtnBusMsgValid ( PBUSMSG pBusMsg, DWORD dwYkType )
{
	assert ( pBusMsg != NULL );

	//判断消息数量长度
	if( 1 != pBusMsg->DataNum
			|| pBusMsg->DataLen != sizeof(YK_DATA))
	{
		printf ( "IEC101S Yk Msg err\n" );
		return FALSE;
	}

	//判断本地是否属于遥控状态
	if( ( m_dwSendFlag & IEC101S_YK_SEL ) == 0
			&& (m_dwSendFlag & IEC101S_YK_EXE) == 0
			&& (m_dwSendFlag & IEC101S_YK_CANCEL) == 0)
	{
		printf ( "IEC101S None Yk Status\n" );
		SetYkParam( 0, 0, 0, 0, 0 );
		return FALSE;
	}

	//判断返回的遥控和本地正在控制的遥控是否是同一个
	YK_DATA *pData = (YK_DATA *)pBusMsg->pData;
	if( !IsYkParamTrue( m_byYKAsduType, m_byYkCot, pBusMsg->SrcInfo.wDevNo, pData->wPnt, pData->byVal ) )
	{
		printf ( "IEC101S MsgData is err\n" );
		return FALSE;
	}

	if( !( m_dwSendFlag & dwYkType ) )
	{
		printf ( "IEC101S None Yk Status\n" );
		SetYkParam( 0, 0, 0, 0, 0 );
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::IsYkBusMsgValue  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  DealBusMsg
 * Description:  处理总线消息
 *       Input:  消息指针
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::DealBusMsg ( PBUSMSG pBusMsg )
{
	switch ( pBusMsg->byMsgType )
	{
	case YK_PROTO:
		{
			switch ( pBusMsg->dwDataType  )
			{
			case YK_SEL_RTN:
				if( IsYkRtnBusMsgValid( pBusMsg, IEC101S_YK_SEL ) )
				{
					printf ( "IEC101S YK_SEL_RTN\n" );
					m_SendStatus = YK_RTN_DATA;
				}
				break;

			case YK_EXCT_RTN:
				if( IsYkRtnBusMsgValid( pBusMsg, IEC101S_YK_EXE ) )
				{
					printf ( "IEC101S YK_EXCT_RTN\n" );
					m_SendStatus = YK_RTN_DATA;
				}
				break;

			case YK_CANCEL_RTN:
				if( IsYkRtnBusMsgValid( pBusMsg, IEC101S_YK_CANCEL ) )
				{
					printf ( "IEC101S YK_CANCEL_RTN\n" );
					m_SendStatus = YK_RTN_DATA;
				}
				break;

			default:
				print( "IEC101S:DealBusMsg can't find the datatype" );
				return FALSE;
				break;
			}				/* -----  end switch  ----- */
		}
		break;

	default:
		print( "IEC101S:DealBusMsg can't find the msgtype" );
		return FALSE;
		break;
	}				/* -----  end switch  ----- */

	return TRUE;
}		/* -----  end of method CIEC101S_2002::DealBusMsg  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Add68HeadAndTail
 * Description:	 为组好的ASDU报文添加68的头尾
 *       Input:  组织好的ASDU报文 长度 要获取的报文 长度
 *		Return:  void
 *--------------------------------------------------------------------------------------
 */
int  CIEC101S_2002::Add68HeadAndTail ( const BYTE *byAsduBuf, int iAsduLen, BYTE *buf )
{
	//TYPE:1 + VSQ:1 + COT:(1-2) + ADDR:(1-2) + INFOADDR(2-3) >= 6
	if( iAsduLen < 6 )
	{
		return -1;
	}
	//68Head
	buf[0] = 0x68;
	buf[1] = iAsduLen + 2;
	buf[2] = iAsduLen + 2;
	buf[3] = 0x68;
	if ( IsHaveLevel1Data(  ) )
	{
		buf[4] = 0x28;
	}
	else
	{
		buf[4] = 0x08;
	}
	buf[5] = m_wDevAddr;

	//data
	memcpy( buf+6, byAsduBuf, iAsduLen );

	//68tail
	buf[iAsduLen + 6] = GetCs( buf+4, iAsduLen + 2 );
	buf[iAsduLen + 7] = 0x16;

	return iAsduLen + 8;
}		/* -----  end of method CIEC101S_2002::Add68HeadAndTail  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetLinkStatusBuf
 * Description:  获取通讯状态报文上送
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetLinkStatusBuf ( BYTE *buf, int &len )
{
	len = 0;
	buf[len++] = 0x10;
	buf[len++] = 0x0b;
	buf[len++] = m_wDevAddr;
	buf[len++] = GetCs( buf + 1, 2 );
	buf[len++] = 0x16;

	return TRUE;
}		/* -----  end of method CIEC101S_2002::GetLinkStatusBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetReconitionBuf
 * Description:  获取链路复位
 *       Input:  缓冲区 长度
 *		Return:  TRUE
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetReconitionBuf ( BYTE *buf, int &len )
{
	len = 0;
	buf[len++] = 0x10;
	if ( IsHaveLevel1Data(  ) )
	{
		buf[len++] = 0x20;
	}
	else
	{
		buf[len++] = 0x00;
	}
	buf[len++] = m_wDevAddr;
	buf[len++] = GetCs( buf + 1, 2 );
	buf[len++] = 0x16;

	return TRUE;
}		/* -----  end of method CIEC101S_2002::GetReconitionBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetUserDataBuf
 * Description:  用户数据确认
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetUserDataBuf ( BYTE *buf, int &len )
{
	return GetLevel1Data( buf, len );
}		/* -----  end of method CIEC101S_2002::GetUserDataBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetNoneDataBuf
 * Description:  获取无数据报文回复
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetNoneDataBuf ( BYTE *buf, int &len )
{
	len = 0;
	buf[len++] = 0x10;
	buf[len++] = 0x09;
	buf[len++] = m_wDevAddr;
	buf[len++] = GetCs( buf + 1, 2 );
	buf[len++] = 0x16;

	return TRUE;
}		/* -----  end of method CIEC101S_2002::GetNoneDataBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetToTalCallRecoBuf
 * Description:  获取总召唤确认报文  ASDU100
 *       Input:  缓冲区 长度 标志位 传输原因：7激活确认 10激活终止
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetTotalCallRecoBuf ( BYTE *buf, int &len, BYTE byCot )
{
	print( "总招确认帧" );
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x64;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = byCot;	//7激活确认 10激活终止
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//信息对象地址
	byAsduBuf[iAsduLen++] = 0x00;
	byAsduBuf[iAsduLen++] = 0x00;
	if ( 3 == m_byInfoAddrLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//召唤限定词 AOI
	byAsduBuf[iAsduLen++] = 0x14;	//站召唤

	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::GetToTalCallRecoBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetCallYmRecoBuf
 * Description:  获取累计量认可报文
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetCallYmRecoBuf ( BYTE *buf, int &len, BYTE byCot )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x65;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = byCot;	//7激活确认 10激活终止
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//信息对象地址
	byAsduBuf[iAsduLen++] = 0x00;
	byAsduBuf[iAsduLen++] = 0x00;
	if ( 3 == m_byInfoAddrLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//召唤限定词 QCC
	byAsduBuf[iAsduLen++] = 0x05;	//总的请求计数量

	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::GetCallYmRecoBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetTimeSyncRecoBuf
 * Description:  对时确认报文
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetTimeSyncRecoBuf ( BYTE *buf, int &len, BYTE byCot )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x67;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = byCot;	//7激活确认 10激活终止
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//信息对象地址
	byAsduBuf[iAsduLen++] = 0x00;
	byAsduBuf[iAsduLen++] = 0x00;
	if ( 3 == m_byInfoAddrLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//CP56Time2a
	CP56TIME2A t;
	GetCp56Time2a( &t );

	byAsduBuf[iAsduLen++] = t.byLoMis;
	byAsduBuf[iAsduLen++] = t.byHiMis;
	byAsduBuf[iAsduLen++] = t.byMin;
	byAsduBuf[iAsduLen++] = t.byHour;
	byAsduBuf[iAsduLen++] = t.byDay;
	byAsduBuf[iAsduLen++] = t.byMon;
	byAsduBuf[iAsduLen++] = t.byYear;

	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::GetTimeSyncRecoBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_NA_1_TotalFrame
 * Description:  获取总召测量量 归一化值
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_NA_1_TotalFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x09;			//TYPE
	byAsduBuf[iAsduLen++] = 0x81;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x14;			// 响应站召唤
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址
	//信息对象地址
	WORD wPnt = m_wDataIndex + m_wYcStartAddr;
	byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
	byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
	if ( 3 == m_byInfoAddrLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	// NVA归一化值 QDS品质描述词 未考虑溢出
	BYTE byCount = 0;
	for ( int i=m_wDataIndex; i<m_wAISum; i++)
	{
		BYTE byQds = 0;
		float fVal = m_fYcBuf[i];
		if( m_pAIMapTab[i].wStn>0 && m_pAIMapTab[i].wPntNum>0 )
			fVal = CalcAIRipeVal(m_pAIMapTab[i].wStn, m_pAIMapTab[i].wPntNum, m_fYcBuf[i]);
				//组织遥测值
				short sVal = ( short )fVal;
		byAsduBuf[iAsduLen++] = LOBYTE(sVal);
		byAsduBuf[iAsduLen++] = HIBYTE(sVal);

		//获取装置状态， 如果装置不通， 则设为无效数据非当前值
		WORD wSerialNo = GetSerialNoFromTrans( YC_TRANSTOSERIALNO , i ) ;
		BOOL bDevState = m_pMethod->GetDevCommState( wSerialNo ) ;
		if( bDevState == COM_DEV_ABNORMAL )
		{
			byQds |= 0xC0 ;
		}
		byAsduBuf[iAsduLen++] = byQds;

		byCount ++;
		//每次最多上传40个遥测
		if ( byCount > 40 )
		{
			break;
		}
	}
	byAsduBuf[1] = 0x80 | byCount;

	//如果遥测数据已发送完， 则取消总召遥测状态
	m_wDataIndex += byCount;
	if ( m_wDataIndex >= m_wAISum )
	{
		m_dwSendFlag &= ~IEC101S_TOTAL_YC;
		m_wDataIndex = 0;
	}

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_NA_1_TotalFrame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_NA_1_ChangeFrame
 * Description:  获取变化测量量 归一化值
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_NA_1_ChangeFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x09;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x03;			// 突发（自发）
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址 NVA QDS
	BYTE byCount = 0;
	int iSize = m_dwAIEQueue.size();
	for ( int i=0; i<iSize; i++)
	{
		//获取顺序号， 内部点号， 值
		WORD wSerialNo;
		WORD wNum;
		WORD wVal;
		float fVal;
		if( !GetAnalogEvt( wSerialNo, wNum, fVal ) )
		{
			break;
		}
		wVal = (WORD)fVal;

		//信息对象地址
		WORD wPnt = wNum + m_wYcStartAddr;
		byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
		byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
		if ( 3 == m_byInfoAddrLen )
		{
			byAsduBuf[iAsduLen++] = 0x00;
		}

		//组织 NVA
		byAsduBuf[iAsduLen++] = LOBYTE(wVal);
		byAsduBuf[iAsduLen++] = HIBYTE(wVal);

		//组织 QDS  未处理溢出
		BYTE byQds = 0;
		//获取装置状态， 如果装置不通， 则设为无效数据非当前值
		BOOL bDevState = m_pMethod->GetDevCommState( wSerialNo ) ;
		if( bDevState == COM_DEV_ABNORMAL )
		{
			byQds |= 0xC0 ;
		}
		byAsduBuf[iAsduLen++] = byQds;

		byCount ++;
		//每次最多上传40个遥测
		if ( byCount > 40 )
		{
			break;
		}
	}
	byAsduBuf[1] = byCount;

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_NA_1_ChangeFrame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_NA_1_Frame
 * Description:  //获取测量值 归一化值  ASDU9
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_NA_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if( IEC101S_2002_TOTAL_TYPE == iFlag )
	{
		return Get_M_ME_NA_1_TotalFrame( buf, len );
	}

	if( IEC101S_2002_CHANGE_TYPE == iFlag )
	{
		return Get_M_ME_NA_1_ChangeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_NA_1_Frame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_TA_1_SoeFrame
 * Description:  //获取带时标突发测量量 归一化值
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_TA_1_SoeFrame ( BYTE *buf, int &len )
{
	print( "未组织ASDU 10 M_ME_TA_1 突发报文" );
	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_TA_1_SoeFrame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_TA_1_Frame
 * Description:  //获取带时标测量值 归一化值 ASDU10
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_TA_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if( IEC101S_2002_SOE_TYPE == iFlag )
	{
		return Get_M_ME_TA_1_SoeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_TA_1_Frame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_NB_1_TotalFrame
 * Description:   //获取总召测量值 标度化值
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_NB_1_TotalFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x0b;			//TYPE
	byAsduBuf[iAsduLen++] = 0x81;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x14;			// 响应站召唤
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址
	//信息对象地址
	WORD wPnt = m_wDataIndex + m_wYcStartAddr;
	byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
	byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
	if ( 3 == m_byInfoAddrLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	// SVA标度化值 QDS品质描述词
	BYTE byCount = 0;
	for ( int i=m_wDataIndex; i<m_wAISum; i++)
	{
		BYTE byQds = 0;
		float fVal = m_fYcBuf[i];
		if( m_pAIMapTab[i].wStn>0 && m_pAIMapTab[i].wPntNum>0 )
			fVal = CalcAIRipeVal(m_pAIMapTab[i].wStn, m_pAIMapTab[i].wPntNum, m_fYcBuf[i]);
				//组织遥测值
				short sVal = (short)fVal;
		byAsduBuf[iAsduLen++] = LOBYTE(sVal);
		byAsduBuf[iAsduLen++] = HIBYTE(sVal);

		//获取装置状态， 如果装置不通， 则设为无效数据非当前值
		WORD wSerialNo = GetSerialNoFromTrans( YC_TRANSTOSERIALNO , i ) ;
		BOOL bDevState = m_pMethod->GetDevCommState( wSerialNo ) ;
		if( bDevState == COM_DEV_ABNORMAL )
		{
			byQds |= 0xC0 ;
		}
		byAsduBuf[iAsduLen++] = byQds;

		byCount ++;
		//每次最多上传40个遥测
		if ( byCount > 40 )
		{
			break;
		}
	}
	byAsduBuf[1] = 0x80 | byCount;

	//如果遥测数据已发送完， 则取消总召遥测状态
	m_wDataIndex += byCount;
	if ( m_wDataIndex >= m_wAISum )
	{
		m_dwSendFlag &= ~IEC101S_TOTAL_YC;
		m_wDataIndex = 0;
	}

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_NB_1_TotalFrame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_NB_1_ChangeFrame
 * Description:  //获取变化测量量 标度化值
 *       Input: 缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_NB_1_ChangeFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x0b;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x03;			// 突发（自发）
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址 SVA QDS
	BYTE byCount = 0;
	int iSize = m_dwAIEQueue.size();
	for ( int i=0; i<iSize; i++)
	{
		//获取顺序号， 内部点号， 值
		WORD wSerialNo;
		WORD wNum;
		WORD wVal;
		float fVal;
		if( !GetAnalogEvt( wSerialNo, wNum, fVal ) )
		{
			break;
		}
		wVal = (WORD)fVal;

		//信息对象地址
		WORD wPnt = wNum + m_wYcStartAddr;
		byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
		byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
		if ( 3 == m_byInfoAddrLen )
		{
			byAsduBuf[iAsduLen++] = 0x00;
		}

		//组织 SVA
		byAsduBuf[iAsduLen++] = LOBYTE(wVal);
		byAsduBuf[iAsduLen++] = HIBYTE(wVal);

		//组织 QDS  未处理溢出
		BYTE byQds = 0;
		//获取装置状态， 如果装置不通， 则设为无效数据非当前值
		BOOL bDevState = m_pMethod->GetDevCommState( wSerialNo ) ;
		if( bDevState == COM_DEV_ABNORMAL )
		{
			byQds |= 0xC0 ;
		}
		byAsduBuf[iAsduLen++] = byQds;

		byCount ++;
		//每次最多上传40个遥测
		if ( byCount > 40 )
		{
			break;
		}
	}
	byAsduBuf[1] = byCount;


	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_NB_1_ChangeFrame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_NB_1_Frame
 * Description:  //获取测量值 标度化值  ASDU11
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_NB_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if( IEC101S_2002_TOTAL_TYPE == iFlag )
	{
		return Get_M_ME_NB_1_TotalFrame( buf, len );
	}

	if( IEC101S_2002_CHANGE_TYPE == iFlag )
	{
		return Get_M_ME_NB_1_ChangeFrame(buf, len);
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_NB_1_Frame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_TB_1_SoeFrame
 * Description:  //获取带时标突发测量量 标度化值
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_TB_1_SoeFrame ( BYTE *buf, int &len )
{
	print( "not organize M_ME_TB_1 ASDU12" );
	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_TB_1_SoeFrame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_TB_1_Frame
 * Description: 获取带时标测量值 标度化值 ASDU12
 *       Input:  缓冲区  长度
 *		Return: BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_TB_1_Frame ( BYTE *buf, int &len , int iFlag )
{
	if( IEC101S_2002_SOE_TYPE == iFlag )
	{
		return Get_M_ME_TB_1_SoeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_TB_1_Frame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_NC_1_TotalFrame
 * Description:  //获取总召测量值 短浮点数
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_NC_1_TotalFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x0d;			//TYPE
	byAsduBuf[iAsduLen++] = 0x81;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x14;			// 响应站召唤
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址
	//信息对象地址
	WORD wPnt = m_wDataIndex + m_wYcStartAddr;
	byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
	byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
	if ( 3 == m_byInfoAddrLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	// STD754短浮点数 QDS品质描述词
	BYTE byCount = 0;
	for ( int i=m_wDataIndex; i<m_wAISum; i++)
	{
		//组织遥测值
		float fVal = m_fYcBuf[i];
		if( m_pAIMapTab[i].wStn>0 && m_pAIMapTab[i].wPntNum>0  )
		{
			fVal = CalcAIRipeVal(m_pAIMapTab[i].wStn, m_pAIMapTab[i].wPntNum, (int)m_fYcBuf[i]);
		}

		//处理fVal顺序 linux和window在传送浮点数时，高低字节不对
		BYTE byBuffer[ 4  ] ;
		BYTE byBuffer1[ 4  ] ;
		memcpy( byBuffer , &fVal , 4  ) ;
		GlobalCopyByEndian(byBuffer1,byBuffer,4);
		// byBuffer1[ 0  ] = byBuffer[ 3  ] ;
		// byBuffer1[ 1  ] = byBuffer[ 2  ] ;
		// byBuffer1[ 2  ] = byBuffer[ 1  ] ;
		// byBuffer1[ 3  ] = byBuffer[ 0  ] ;

		memcpy(&byAsduBuf[iAsduLen], byBuffer1, 4);
		iAsduLen += 4;

		// 组织QDS
		BYTE byQds = 0;
		//获取装置状态， 如果装置不通， 则设为无效数据非当前值
		WORD wSerialNo = GetSerialNoFromTrans( YC_TRANSTOSERIALNO , i ) ;
		BOOL bDevState = m_pMethod->GetDevCommState( wSerialNo ) ;
		if( bDevState == COM_DEV_ABNORMAL )
		{
			byQds |= 0xC0 ;
		}
		byAsduBuf[iAsduLen++] = byQds;

		byCount ++;
		//每次最多上传40个遥测
		if ( byCount > 40 )
		{
			break;
		}
	}
	byAsduBuf[1] = 0x80 | byCount;

	//如果遥测数据已发送完， 则取消总召遥测状态
	m_wDataIndex += byCount;
	if ( m_wDataIndex >= m_wAISum )
	{
		m_dwSendFlag &= ~IEC101S_TOTAL_YC;
		m_wDataIndex = 0;
	}

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_NC_1_TotalFrame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_NC_1_ChangeFrame
 * Description:  //获取变化测量量 短浮点数
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_NC_1_ChangeFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x0d;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x03;			// 突发（自发）
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址 STD QDS
	BYTE byCount = 0;
	int iSize = m_dwAIEQueue.size();
	for ( int i=0; i<iSize; i++)
	{
		//获取顺序号， 内部点号， 值
		WORD wSerialNo;
		WORD wNum;
		// WORD wVal;
		float fYcVal;
		if( !GetAnalogEvt( wSerialNo, wNum, fYcVal ) )
		{
			break;
		}

		//信息对象地址
		WORD wPnt = wNum + m_wYcStartAddr;
		byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
		byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
		if ( 3 == m_byInfoAddrLen )
		{
			byAsduBuf[iAsduLen++] = 0x00;
		}

		//组织 SVA
		float fVal = fYcVal;
		BYTE byBuffer1[4];
		BYTE byBuffer[4];
		if( m_pAIMapTab[wNum].wStn>0 &&
				m_pAIMapTab[wNum].wPntNum>0 )
		{
			fVal = CalcAIRipeVal(m_pAIMapTab[wNum].wStn-1,
					m_pAIMapTab[wNum].wPntNum-1,
					fVal);
			memcpy( byBuffer , &fVal , 4  ) ;
			GlobalCopyByEndian(byBuffer1,byBuffer,4);
			// byBuffer1[ 0  ] = byBuffer[ 3  ] ;
			// byBuffer1[ 1  ] = byBuffer[ 2  ] ;
			// byBuffer1[ 2  ] = byBuffer[ 1  ] ;
			// byBuffer1[ 3  ] = byBuffer[ 0  ] ;
		}
		memcpy(&byAsduBuf[iAsduLen], &byBuffer1, 4);
		iAsduLen += 4;

		//组织 QDS  未处理溢出
		BYTE byQds = 0;
		//获取装置状态， 如果装置不通， 则设为无效数据非当前值
		BOOL bDevState = m_pMethod->GetDevCommState( wSerialNo ) ;
		if( bDevState == COM_DEV_ABNORMAL )
		{
			byQds |= 0xC0 ;
		}
		byAsduBuf[iAsduLen++] = byQds;

		byCount ++;
		//每次最多上传40个遥测
		if ( byCount > 40 )
		{
			break;
		}
	}
	byAsduBuf[1] = byCount;


	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_NC_1_ChangeFrame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_NC_1_Frame
 * Description:  //获取测量值 短浮点数 ASDU13
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_NC_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if( IEC101S_2002_TOTAL_TYPE == iFlag )
	{
		return Get_M_ME_NC_1_TotalFrame( buf, len );
	}

	if( IEC101S_2002_CHANGE_TYPE == iFlag )
	{
		return Get_M_ME_NC_1_ChangeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_NC_1_Frame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_TC_1_SoeFrame
 * Description:  //获取带时标突发测量量 短浮点数
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_TC_1_SoeFrame ( BYTE *buf, int &len )
{
	print( "not organize M_ME_TC_1 ASDU14 soeFrame" );
	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_TC_1_SoeFrame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_TC_1_Frame
 * Description:  //获取带时标测量值 短浮点数 ASDU14
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_TC_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if ( IEC101S_2002_SOE_TYPE == iFlag )
	{
		return Get_M_ME_TC_1_SoeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_TC_1_Frame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_ND_1_TotalFrame
 * Description:  测量值， 不带品质描述词的归一化值总招
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_ND_1_TotalFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x15;			//TYPE
	byAsduBuf[iAsduLen++] = 0x81;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x14;			// 响应站召唤
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址
	//信息对象地址
	WORD wPnt = m_wDataIndex + m_wYcStartAddr;
	byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
	byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
	if ( 3 == m_byInfoAddrLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	// NVA归一化值
	BYTE byCount = 0;
	for ( int i=m_wDataIndex; i<m_wAISum; i++)
	{
		float fVal = m_fYcBuf[i];
		if( m_pAIMapTab[i].wStn>0 && m_pAIMapTab[i].wPntNum>0 )
			fVal = CalcAIRipeVal(m_pAIMapTab[i].wStn, m_pAIMapTab[i].wPntNum, m_fYcBuf[i]);
				//组织遥测值
				short sVal = ( short )fVal;
		byAsduBuf[iAsduLen++] = LOBYTE(sVal);
		byAsduBuf[iAsduLen++] = HIBYTE(sVal);

		byCount ++;
		//每次最多上传40个遥测
		if ( byCount > 40 )
		{
			break;
		}
	}
	byAsduBuf[1] = 0x80 | byCount;

	//如果遥测数据已发送完， 则取消总召遥测状态
	m_wDataIndex += byCount;
	if ( m_wDataIndex >= m_wAISum )
	{
		m_dwSendFlag &= ~IEC101S_TOTAL_YC;
		m_wDataIndex = 0;
	}

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_ND_1_TotalFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_ND_1_ChangeFrame
 * Description:   测量值， 不带品质描述词的归一化值变化
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_ND_1_ChangeFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x15;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x03;			// 突发（自发）
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址 NVA
	BYTE byCount = 0;
	int iSize = m_dwAIEQueue.size();
	for ( int i=0; i<iSize; i++)
	{
		//获取顺序号， 内部点号， 值
		WORD wSerialNo;
		WORD wNum;
		WORD wVal;
		float fVal;
		if( !GetAnalogEvt( wSerialNo, wNum, fVal ) )
		{
			break;
		}
		wVal = (WORD)fVal;

		//信息对象地址
		WORD wPnt = wNum + m_wYcStartAddr;
		byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
		byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
		if ( 3 == m_byInfoAddrLen )
		{
			byAsduBuf[iAsduLen++] = 0x00;
		}

		//组织 NVA
		byAsduBuf[iAsduLen++] = LOBYTE(wVal);
		byAsduBuf[iAsduLen++] = HIBYTE(wVal);

		byCount ++;
		//每次最多上传40个遥测
		if ( byCount > 40 )
		{
			break;
		}
	}
	byAsduBuf[1] = byCount;

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_ND_1_ChangeFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_ND_1_Frame
 * Description:	 测量值， 不带品质描述词的归一化值
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_ND_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if( IEC101S_2002_TOTAL_TYPE == iFlag )
	{
		return Get_M_ME_ND_1_TotalFrame( buf, len );
	}

	if( IEC101S_2002_CHANGE_TYPE == iFlag )
	{
		return Get_M_ME_ND_1_ChangeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_ND_1_Frame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_TD_1_SoeFrame
 * Description:  //获取带CP56Time2a突发测量量 归一化值
 *       Input:  缓冲区 长度
 *		Return: BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_TD_1_SoeFrame ( BYTE *buf, int &len )
{
	print( "not organize M_ME_TD_1 ASDU34 SoeFrame" );
	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_TD_1_SoeFrame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_TD_1_Frame
 * Description:  //获取带CP56Time2a测量值 归一化值  ASDU34
 *       Input:  缓冲区 长度 标识
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_TD_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if ( IEC101S_2002_SOE_TYPE == iFlag )
	{
		return Get_M_ME_TD_1_SoeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_TD_1_Frame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_TE_1_SoeFrame
 * Description:  //获取带CP56Time2a突发测量量 标度化值
 *       Input:  缓冲区 长度
 *		Return: BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_TE_1_SoeFrame ( BYTE *buf, int &len )
{
	print( "not organize M_ME_TE_1 ASDU35 SoeFrame" );

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_TE_1_SoeFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_TE_1_Frame
 * Description:  //获取带CP56Time2a测量值 标度化值 ASDU35
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_TE_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if ( IEC101S_2002_SOE_TYPE == iFlag )
	{
		return Get_M_ME_TE_1_SoeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_TE_1_Frame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_TF_1_SoeFrame
 * Description:  //获取带CP56Time2a突发测量量 短浮点数
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_TF_1_SoeFrame ( BYTE *buf, int &len )
{
	print( "not organize M_ME_TE_1 ASDU36 SoeFrame" );

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_TF_1_SoeFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_ME_TF_1_Frame
 * Description:  //获取带CP56Time2a测量值 短浮点数
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_ME_TF_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if ( IEC101S_2002_SOE_TYPE == iFlag )
	{
		return Get_M_ME_TF_1_SoeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_ME_TF_1_Frame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_SP_NA_1_TotalFrame
 * Description:  获取单点信息总召报文
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_SP_NA_1_TotalFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x01;			//TYPE
	byAsduBuf[iAsduLen++] = 0x81;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x14;			// 站召唤
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//信息对象地址
	WORD wPnt;
	wPnt = m_wDataIndex + m_wYxStartAddr;
	byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
	byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
	if ( 3 == m_byInfoAddrLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	// 组织bysiq报文
	BYTE byCount = 0;
	for(int i=m_wDataIndex; i<m_wDISum; i++)
	{
		WORD wVal;
		//获取遥信值
		GetRealVal(1, (WORD)i, &wVal);
		BYTE bySiq = 0 ;

		if( wVal )
			bySiq = 1 ;
		else
			bySiq = 0 ;

		//获取装置状态， 如果装置不通， 则设为无效数据非当前值
		WORD wSerialNo = GetSerialNoFromTrans( YX_TRANSTOSERIALNO , i ) ;
		BOOL bDevState = m_pMethod->GetDevCommState( wSerialNo ) ;
		if( bDevState == COM_DEV_ABNORMAL )
		{
			bySiq |= 0xC0 ;
		}

		//组包
		byAsduBuf[iAsduLen++] = bySiq ;

		//计数， 每次最多传送128个遥信
		byCount++;
		if ( byCount >= 127 )
		{
			break;
		}
	}
	//组织vsq
	byAsduBuf[1] = 0x80 | byCount;

	//如果遥信数据已发送完， 则取消总召遥信状态
	m_wDataIndex += byCount;
	// printf ( "m_wDataIndex=%d m_wDISum=%d\n", m_wDataIndex, m_wDISum );
	if ( m_wDataIndex >= m_wDISum )
	{
		m_dwSendFlag &= ~IEC101S_TOTAL_YX;
		m_wDataIndex = 0;
	}

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_SP_NA_1_TotalFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_SP_NA_1_ChangeFrame
 * Description:    //获取不带时标的变化单点信息
 *       Input:  缓冲区长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_SP_NA_1_ChangeFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x01;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x03;			// 突发（自发）
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址 变化值
	BYTE byCount = 0;
	int iSize = m_dwDIEQueue.size();
	for(int i=0; i<iSize; i++)
	{
		WORD wSerialNo;
		WORD wPnt;
		WORD wVal;
		if(!GetDigitalEvt( wSerialNo , wPnt, wVal ))
		{
			break;
		}

		//信息对象地址
		wPnt += m_wYxStartAddr;
		byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
		byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
		if ( 3 == m_byInfoAddrLen )
		{
			byAsduBuf[iAsduLen++] = 0x00;
		}

		//变化值
		BYTE bySiq = 0;
		if ( wVal & 0x01  )
		{
			bySiq = 0x01 ;
		}
		else
		{
			bySiq = 0 ;
		}
		BOOL bDevState = FALSE ;
		bDevState = m_pMethod->GetDevCommState( wSerialNo  ) ;
		if( bDevState == COM_DEV_ABNORMAL  )
		{
			bySiq |= 0xC0 ;
		}
		byAsduBuf[iAsduLen++] = bySiq;

		//最大数量
		byCount++;
		if ( byCount > 20 )
		{
			break;
		}
	}
	byAsduBuf[1] = byCount;			//可变结构限定词VSQ

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_SP_NA_1_ChangeFrame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_SP_NA_1_Frame
 * Description:  获取不带时标的单点信息帧  ASDU1
 *       Input:  缓存区 长度 标志位：0代表总召回复， 1代表变化 2代表突变回复
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_SP_NA_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	// 处理总召遥信
	if( IEC101S_2002_TOTAL_TYPE == iFlag )
	{
		return Get_M_SP_NA_1_TotalFrame( buf, len );
	}

	if( IEC101S_2002_CHANGE_TYPE == iFlag )
	{
		return Get_M_SP_NA_1_ChangeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_SP_NA_1_Frame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_SP_TA_1_SoeFrame
 * Description:  //获取带时标的突变单点信息
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_SP_TA_1_SoeFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x02;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x03;			// 突发（自发）
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址 siq cp24time2a
	BYTE byCount = 0;
	while(m_iSOE_rd_p != m_iSOE_wr_p)
	{
		WORD wSerialNo;
		WORD wPnt;
		WORD wVal;
		struct tm tmStruct;
		WORD wMiSecond;
		WORD wTime;

		GetSOEInfo( wSerialNo , &wPnt, &wVal, &tmStruct, &wMiSecond );

		//信息对象地址
		wPnt += m_wYxStartAddr;
		byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
		byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
		if ( 3 == m_byInfoAddrLen )
		{
			byAsduBuf[iAsduLen++] = 0x00;
		}

		//siq
		wVal  = wVal & 0x0001;
		byAsduBuf[iAsduLen++] = LOBYTE(wVal);

		//cp24time2a
		wTime = tmStruct.tm_sec*1000 + wMiSecond;
		byAsduBuf[iAsduLen++] = LOBYTE(wTime);
		byAsduBuf[iAsduLen++] = HIBYTE(wTime);
		byAsduBuf[iAsduLen++] = (BYTE)tmStruct.tm_min;

		//最大数量
		byCount++;
		if( byCount > 20  )
		{
			break;
		}
	}
	byAsduBuf[1] = byCount;			//可变结构限定词VSQ

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_SP_TA_1_SoeFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_SP_TA_1_Frame
 * Description:  //获取带时标的单点信息  ASDU2
 *       Input:  标志位：0代表总召回复， 1代表变化 2代表soe回复
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_SP_TA_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if( IEC101S_2002_SOE_TYPE ==  iFlag)
	{
		return Get_M_SP_TA_1_SoeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_SP_TA_1_Frame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_DP_TA_1_TotalFrame
 * Description:  //获取不带时标的总召双点信息
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_DP_NA_1_TotalFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;
	WORD wPnt;
	BYTE byCount = 0;

	byAsduBuf[iAsduLen++] = 0x03;			//TYPE
	byAsduBuf[iAsduLen++] = 0x81;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x14;			// 站召唤
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//信息对象地址
	wPnt = m_wDataIndex + m_wYxStartAddr;
	byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
	byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
	if ( 3 == m_byInfoAddrLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	// 组织bysiq报文
	for(int i=m_wDataIndex; i<m_wDISum; i++)
	{
		WORD wVal;
		//获取遥信值
		GetRealVal(1, (WORD)i, &wVal);
		BYTE bySiq = 0 ;

		if( wVal )
			bySiq = 2 ;
		else
			bySiq = 1 ;

		//获取装置状态， 如果装置不通， 则设为无效数据非当前值
		WORD wSerialNo = GetSerialNoFromTrans( YX_TRANSTOSERIALNO , i ) ;
		BOOL bDevState = m_pMethod->GetDevCommState( wSerialNo ) ;
		if( bDevState == COM_DEV_ABNORMAL )
		{
			bySiq |= 0xC0 ;
		}

		//组包
		byAsduBuf[iAsduLen++] = bySiq ;

		//计数， 每次最多传送128个遥信
		byCount++;
		if ( byCount >= 127 )
		{
			break;
		}
	}
	//组织vsq
	byAsduBuf[1] = 0x80 | byCount;

	//如果遥信数据已发送完， 则取消总召遥信状态
	m_wDataIndex += byCount;
	if ( m_wDataIndex >= m_wDISum )
	{
		m_dwSendFlag &= ~IEC101S_TOTAL_YX;
		m_wDataIndex = 0;
	}

	//如果遥信数据已发送完， 则取消总召遥信状态
	m_wDataIndex += byCount;
	if ( m_wDataIndex >= m_wDISum )
	{
		m_dwSendFlag &= ~IEC101S_TOTAL_YX;
		m_wDataIndex = 0;
	}

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}
	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_DP_TA_1_TotalFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_DP_TA_1_ChangeFrame
 * Description:  //获取不带时标的变化双点信息
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_DP_NA_1_ChangeFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x03;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x03;			// 突发（自发）
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址 DIQ
	BYTE byCount = 0;
	int iSize = m_dwDIEQueue.size();
	for(int i=0; i<iSize; i++)
	{
		WORD wSerialNo;
		WORD wPnt;
		WORD wVal;
		if(!GetDigitalEvt( wSerialNo , wPnt, wVal ))
		{
			break;
		}

		//信息对象地址
		wPnt += m_wYxStartAddr;
		byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
		byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
		if ( 3 == m_byInfoAddrLen )
		{
			byAsduBuf[iAsduLen++] = 0x00;
		}

		//变化值
		BYTE byDiq = 0;
		if ( wVal & 0x01  )
		{
			byDiq = 0x02 ;
		}
		else
		{
			byDiq = 0x01 ;
		}
		BOOL bDevState = FALSE ;
		bDevState = m_pMethod->GetDevCommState( wSerialNo  ) ;
		if( bDevState == COM_DEV_ABNORMAL  )
		{
			byDiq |= 0xC0 ;
		}
		byAsduBuf[iAsduLen++] = byDiq;

		//最大数量
		byCount++;
		if ( byCount > 20 )
		{
			break;
		}
	}
	byAsduBuf[1] = byCount;			//可变结构限定词VSQ

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_DP_TA_1_ChangeFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_DP_TA_1_Frame
 * Description:  //不带时标的双点信息  ASDU3
 *       Input:  标志位：0代表总召回复， 1代表变化 2代表soe回复
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_DP_NA_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if( IEC101S_2002_TOTAL_TYPE == iFlag )
	{
		return Get_M_DP_NA_1_TotalFrame( buf, len );
	}

	if( IEC101S_2002_CHANGE_TYPE == iFlag )
	{
		return Get_M_DP_NA_1_ChangeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_DP_TA_1_Frame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_DP_TA_1_SoeFrame
 * Description:  获取带时标的双点soe信息
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_DP_TA_1_SoeFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x04;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x03;			// 突发（自发）
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址 diq cp24time2a
	BYTE byCount = 0;
	while(m_iSOE_rd_p != m_iSOE_wr_p)
	{
		WORD wSerialNo;
		WORD wPnt;
		WORD wVal;
		struct tm tmStruct;
		WORD wMiSecond;
		WORD wTime;

		GetSOEInfo( wSerialNo , &wPnt, &wVal, &tmStruct, &wMiSecond );

		//信息对象地址
		wPnt += m_wYxStartAddr;
		byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
		byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
		if ( 3 == m_byInfoAddrLen )
		{
			byAsduBuf[iAsduLen++] = 0x00;
		}

		//diq
		wVal  = wVal & 0x0001;
		byAsduBuf[iAsduLen++] = LOBYTE(wVal) + 1;

		//cp24time2a
		wTime = tmStruct.tm_sec*1000 + wMiSecond;
		byAsduBuf[iAsduLen++] = LOBYTE(wTime);
		byAsduBuf[iAsduLen++] = HIBYTE(wTime);
		byAsduBuf[iAsduLen++] = (BYTE)tmStruct.tm_min;

		//最大数量
		byCount++;
		if( byCount > 20  )
		{
			break;
		}
	}
	byAsduBuf[1] = byCount;			//可变结构限定词VSQ


	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_DP_TA_1_SoeFrame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_DP_TA_1_Frame
 * Description:  //获取带时标的双点信息  ASDU4
 *       Input:  缓冲区 长度
 *		Return:	  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_DP_TA_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if( IEC101S_2002_SOE_TYPE == iFlag )
	{
		return Get_M_DP_TA_1_SoeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_DP_TA_1_Frame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_SP_TB_1_SoeFrame
 * Description:  //获取带时标CP56Time2a的突变单点信息
 *       Input:  缓冲区 长度
 *		Return:  BOOl
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_SP_TB_1_SoeFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x1e;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x03;			// 突发（自发）
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址 siq cp56time2a
	BYTE byCount = 0;
	while(m_iSOE_rd_p != m_iSOE_wr_p)
	{
		WORD wSerialNo;
		WORD wPnt;
		WORD wVal;
		struct tm tmStruct;
		WORD wMiSecond;

		GetSOEInfo( wSerialNo , &wPnt, &wVal, &tmStruct, &wMiSecond );

		//信息对象地址
		wPnt += m_wYxStartAddr;
		byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
		byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
		if ( 3 == m_byInfoAddrLen )
		{
			byAsduBuf[iAsduLen++] = 0x00;
		}

		//siq
		wVal  = wVal & 0x0001;
		byAsduBuf[iAsduLen++] = LOBYTE(wVal) ;

		//cp56time2a
		CP56TIME2A t = GetTmToCp56Time2a( tmStruct, wMiSecond );
		byAsduBuf[iAsduLen++] = t.byLoMis;
		byAsduBuf[iAsduLen++] = t.byHiMis;
		byAsduBuf[iAsduLen++] = t.byMin;
		byAsduBuf[iAsduLen++] = t.byHour;
		byAsduBuf[iAsduLen++] = t.byDay;
		byAsduBuf[iAsduLen++] = t.byMon;
		byAsduBuf[iAsduLen++] = t.byYear;

		//最大数量
		byCount++;
		if( byCount > 20  )
		{
			break;
		}
	}
	byAsduBuf[1] = byCount;			//可变结构限定词VSQ

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_SP_TB_1_SoeFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_SP_TB_1_Frame
 * Description:  //获取带时标CP56Time2a的单点信息  ASDU30
 *       Input:  缓冲区 长度
 *		Return: BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_SP_TB_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if( IEC101S_2002_SOE_TYPE == iFlag )
	{
		return Get_M_SP_TB_1_SoeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_SP_TB_1_Frame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_DP_TB_1_SoeFrame
 * Description:  //获取带时标CP56Time2a的突变双点信息
 *       Input:  缓冲区 长度
 *		Return:   BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_DP_TB_1_SoeFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x1f;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x03;			// 突发（自发）
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址 diq cp56time2a
	BYTE byCount = 0;
	while(m_iSOE_rd_p != m_iSOE_wr_p)
	{
		WORD wSerialNo;
		WORD wPnt;
		WORD wVal;
		struct tm tmStruct;
		WORD wMiSecond;

		GetSOEInfo( wSerialNo , &wPnt, &wVal, &tmStruct, &wMiSecond );

		//信息对象地址
		wPnt += m_wYxStartAddr;
		byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
		byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
		if ( 3 == m_byInfoAddrLen )
		{
			byAsduBuf[iAsduLen++] = 0x00;
		}

		//diq
		wVal  = wVal & 0x0001;
		byAsduBuf[iAsduLen++] = LOBYTE(wVal) + 1 ;

		//cp56time2a
		CP56TIME2A t = GetTmToCp56Time2a( tmStruct, wMiSecond );
		byAsduBuf[iAsduLen++] = t.byLoMis;
		byAsduBuf[iAsduLen++] = t.byHiMis;
		byAsduBuf[iAsduLen++] = t.byMin;
		byAsduBuf[iAsduLen++] = t.byHour;
		byAsduBuf[iAsduLen++] = t.byDay;
		byAsduBuf[iAsduLen++] = t.byMon;
		byAsduBuf[iAsduLen++] = t.byYear;

		//最大数量
		byCount++;
		if( byCount > 20  )
		{
			break;
		}
	}
	byAsduBuf[1] = byCount;			//可变结构限定词VSQ

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_DP_TB_1_SoeFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_DP_TB_1_Frame
 * Description:  //获取带时标CP56Time2a的双点信息 ASDU31
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_DP_TB_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if ( IEC101S_2002_SOE_TYPE == iFlag )
	{
		return Get_M_DP_TB_1_SoeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_DP_TB_1_Frame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_IT_NA_1_TotalFrame
 * Description:  //获取不带时标的总召累计量
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_IT_NA_1_TotalFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x0f;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x25;			// 响应计数量总召唤
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址
	WORD wPnt = m_wDataIndex + m_wYmStartAddr;
	byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
	byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
	if ( 3 == m_byInfoAddrLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	m_pMethod->ReadAllYmData(&m_dwYmBuf[0]);
	//组织数据包 BCR 二进制计数器读数
	BYTE byCount = 0;
	for(int i=m_wDataIndex; i<m_wDISum; i++)
	{
		DWORD dwVal = m_dwYmBuf[i];
		// BCR格式：24页 6.4.7 前四个字节
		byAsduBuf[iAsduLen++]  = LOBYTE(LOWORD(dwVal));
		byAsduBuf[iAsduLen++] = HIBYTE(LOWORD(dwVal));
		byAsduBuf[iAsduLen++] = LOBYTE(HIWORD(dwVal));
		byAsduBuf[iAsduLen++] = HIBYTE(HIWORD(dwVal));


		//BCR 第五个字节
		BYTE bySiq = 0 ;
		//获取装置状态， 如果装置不通， 则设为无效数据非当前值
		WORD wSerialNo = GetSerialNoFromTrans( DD_TRANSTOSERIALNO , i ) ;
		BOOL bDevState = m_pMethod->GetDevCommState( wSerialNo ) ;
		if( bDevState == COM_DEV_ABNORMAL )
		{
			bySiq |= 0xC0 ;
		}
		//组包
		byAsduBuf[iAsduLen++] = bySiq ;

		//计数， 每次最多传送128个遥信
		byCount++;
		if ( byCount >= 20 )
		{
			break;
		}
	}
	//组织vsq
	byAsduBuf[1] = 0x80 | byCount;

	//如果遥脉数据已发送完， 则取消总召遥脉状态
	m_wDataIndex += byCount;
	if ( m_wDataIndex >= m_wPISum )
	{
		m_dwSendFlag &= ~IEC101S_TOTAL_YM;
		m_wDataIndex = 0;
	}

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_IT_NA_1_TotalFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_IT_NA_1_ChangeFrame
 * Description:  //获取不带时标的变化累积量
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_IT_NA_1_ChangeFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x0f;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x03;			// 突发（自发）
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址 BCR


	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_IT_NA_1_ChangeFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_IT_NA_1_Frame
 * Description: //不带时标的累计量  ASDU15
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_IT_NA_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if ( IEC101S_2002_TOTAL_TYPE == iFlag )
	{
		return Get_M_IT_NA_1_TotalFrame(buf, len);
	}

	if ( IEC101S_2002_CHANGE_TYPE == iFlag )
	{
		return Get_M_IT_NA_1_ChangeFrame(buf, len);
	}
	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_IT_NA_1_Frame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_IT_TA_1_TotalFrame
 * Description:  //获取带时标的总召累计量
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_IT_TA_1_TotalFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x10;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x25;			// 响应计数量总召唤
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	m_pMethod->ReadAllYmData(&m_dwYmBuf[0]);
	//组织数据包 信息地址 BCR CP24time2a
	BYTE byCount = 0;
	for(int i=m_wDataIndex; i<m_wDISum; i++)
	{
		DWORD dwVal = m_dwYmBuf[i];
		// BCR格式：24页 6.4.7 前四个字节
		byAsduBuf[iAsduLen++]  = LOBYTE(LOWORD(dwVal));
		byAsduBuf[iAsduLen++] = HIBYTE(LOWORD(dwVal));
		byAsduBuf[iAsduLen++] = LOBYTE(HIWORD(dwVal));
		byAsduBuf[iAsduLen++] = HIBYTE(HIWORD(dwVal));


		//BCR 第五个字节
		BYTE bySiq = 0 ;
		//获取装置状态， 如果装置不通， 则设为无效数据非当前值
		WORD wSerialNo = GetSerialNoFromTrans( DD_TRANSTOSERIALNO , i ) ;
		BOOL bDevState = m_pMethod->GetDevCommState( wSerialNo ) ;
		if( bDevState == COM_DEV_ABNORMAL )
		{
			bySiq |= 0xC0 ;
		}
		//组包
		byAsduBuf[iAsduLen++] = bySiq ;

		//cp24Time2a
		CP56TIME2A t;
		GetCp56Time2a( &t );
		byAsduBuf[iAsduLen++] = t.byLoMis;
		byAsduBuf[iAsduLen++] = t.byHiMis;
		byAsduBuf[iAsduLen++] = t.byMin;

		//计数， 每次最多传送128个遥信
		byCount++;
		if ( byCount >= 20 )
		{
			break;
		}
	}
	//组织vsq
	byAsduBuf[1] = 0x80 | byCount;

	//如果遥脉数据已发送完， 则取消总召遥脉状态
	m_wDataIndex += byCount;
	if ( m_wDataIndex >= m_wPISum )
	{
		m_dwSendFlag &= ~IEC101S_TOTAL_YM;
		m_wDataIndex = 0;
	}


	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_IT_TA_1_TotalFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_IT_TA_1_SoeFrame
 * Description:  //获取带时标的突发累积量
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_IT_TA_1_SoeFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x10;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x03;			// 突发（自发）
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址 BCR CP24time2a


	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_IT_TA_1_SoeFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_IT_TA_1_Frame
 * Description:  //带时标的累计量  ASDU16
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_IT_TA_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if ( IEC101S_2002_TOTAL_TYPE == iFlag )
	{
		return Get_M_IT_TA_1_TotalFrame( buf, len );
	}

	if ( IEC101S_2002_SOE_TYPE == iFlag )
	{
		return Get_M_IT_TA_1_SoeFrame( buf, len );
	}

	return FALSE;
}		/* -----  end of method CIEC101S_2002::Get_M_IT_TA_1_Frame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_IT_TB_1_TotalFrame
 * Description:  //获取带CP56Time2a时标的总召累计量
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_IT_TB_1_TotalFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x25;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x25;			// 响应计数量总召唤
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	m_pMethod->ReadAllYmData(&m_dwYmBuf[0]);
	//组织数据包 信息地址 BCR CP56time2a
	BYTE byCount = 0;
	for(int i=m_wDataIndex; i<m_wDISum; i++)
	{
		DWORD dwVal = m_dwYmBuf[i];
		// BCR格式：24页 6.4.7 前四个字节
		byAsduBuf[iAsduLen++]  = LOBYTE(LOWORD(dwVal));
		byAsduBuf[iAsduLen++] = HIBYTE(LOWORD(dwVal));
		byAsduBuf[iAsduLen++] = LOBYTE(HIWORD(dwVal));
		byAsduBuf[iAsduLen++] = HIBYTE(HIWORD(dwVal));


		//BCR 第五个字节
		BYTE bySiq = 0 ;
		//获取装置状态， 如果装置不通， 则设为无效数据非当前值
		WORD wSerialNo = GetSerialNoFromTrans( DD_TRANSTOSERIALNO , i ) ;
		BOOL bDevState = m_pMethod->GetDevCommState( wSerialNo ) ;
		if( bDevState == COM_DEV_ABNORMAL )
		{
			bySiq |= 0xC0 ;
		}
		//组包
		byAsduBuf[iAsduLen++] = bySiq ;

		//cp24Time2a
		CP56TIME2A t;
		GetCp56Time2a( &t );
		byAsduBuf[iAsduLen++] = t.byLoMis;
		byAsduBuf[iAsduLen++] = t.byHiMis;
		byAsduBuf[iAsduLen++] = t.byMin;
		byAsduBuf[iAsduLen++] = t.byHour;
		byAsduBuf[iAsduLen++] = t.byDay;
		byAsduBuf[iAsduLen++] = t.byMon;
		byAsduBuf[iAsduLen++] = t.byYear;

		//计数， 每次最多传送128个遥信
		byCount++;
		if ( byCount >= 20 )
		{
			break;
		}
	}
	//组织vsq
	byAsduBuf[1] = 0x80 | byCount;

	//如果遥脉数据已发送完， 则取消总召遥脉状态
	m_wDataIndex += byCount;
	if ( m_wDataIndex >= m_wPISum )
	{
		m_dwSendFlag &= ~IEC101S_TOTAL_YM;
		m_wDataIndex = 0;
	}

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_IT_TB_1_TotalFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_IT_TB_1_SoeFrame
 * Description:  //获取带CP56Time2a时标的突发累积量
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_IT_TB_1_SoeFrame ( BYTE *buf, int &len )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	byAsduBuf[iAsduLen++] = 0x10;			//TYPE
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	byAsduBuf[iAsduLen++] = 0x03;			// 突发（自发）
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//组织数据包 信息地址 BCR CP56time2a


	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_IT_TB_1_SoeFrame  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Get_M_IT_TB_1_Frame
 * Description:  //带CP56Time2a时标的累计量  ASDU37
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Get_M_IT_TB_1_Frame ( BYTE *buf, int &len, int iFlag )
{
	if ( IEC101S_2002_TOTAL_TYPE == iFlag )
	{
		return Get_M_IT_TB_1_TotalFrame( buf, len );
	}

	if ( IEC101S_2002_SOE_TYPE == iFlag )
	{
		return Get_M_IT_TB_1_SoeFrame( buf, len );
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Get_M_IT_TB_1_Frame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIE101S_2002
 *      Method:  GetYkRtnDataFrame
 * Description:  遥控返回报文
 *       Input:  缓冲区 长度 类型
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetYkRtnDataFrame ( BYTE *buf, int &len, int byYkRtnType )
{
	BYTE byAsduBuf[256];
	int iAsduLen = 0;

	if( m_byYKAsduType == 45 || m_byYKAsduType == 46 )
	{
		byAsduBuf[iAsduLen++] = m_byYKAsduType;			//TYPE
	}
	else
	{
		return FALSE;
	}
	byAsduBuf[iAsduLen++] = 0x01;			//VSQ

	//传送原因
	if( m_dwSendFlag & IEC101S_YK_CANCEL )
	{
		byAsduBuf[iAsduLen++] = 0x09;			// 停止激活确认
		m_dwSendFlag &= ~IEC101S_YK_CANCEL;
	}
	else
	{
		byAsduBuf[iAsduLen++] = 0x07;			// 激活确认
		if( m_dwSendFlag & IEC101S_YK_SEL )
		{
			m_dwSendFlag &= ~IEC101S_YK_SEL;
		}
		else if( m_dwSendFlag & IEC101S_YK_EXE )
		{
			m_dwSendFlag &= ~IEC101S_YK_EXE;
		}
		else
		{
			return FALSE;
		}
	}
	if( 2 == m_byCotLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	//公共地址
	byAsduBuf[iAsduLen++] = LOBYTE(m_wDevAddr);	//低位地址
	if ( 2 == m_byAddrLen )
	{
		byAsduBuf[iAsduLen++] = HIBYTE(m_wDevAddr); //高位公共地址
	}

	//信息对象地址
	WORD wPnt = m_wYkPnt + m_wYkStartAddr;
	if( wPnt < m_wYkStartAddr || wPnt > ( m_wYkStartAddr + 0x400 ) )
	{
		printf ( "IEC101S Yk pnt=%d err\n", wPnt );
		return FALSE;
	}
	byAsduBuf[iAsduLen++] = LOBYTE( wPnt );
	byAsduBuf[iAsduLen++] = HIBYTE( wPnt );
	if( 3 == m_byInfoAddrLen )
	{
		byAsduBuf[iAsduLen++] = 0x00;
	}

	// SDCS
	BYTE byStatus = 0;
	switch ( byYkRtnType )
	{
	case IEC101S_YK_SEL:
		byStatus = m_byYkStatus | 0x80;
		break;

	case IEC101S_YK_EXE:
		byStatus = m_byYkStatus;
		break;

	case IEC101S_YK_CANCEL:
		byStatus = m_byYkStatus;
		break;

	default:
		printf ( "IEC101S SDCS err\n" );
		return FALSE;
		break;
	}				/* -----  end switch  ----- */
	if( m_byYKAsduType == 46 )
	{
		byAsduBuf[iAsduLen++] = byStatus + 1;
	}
	else
	{
		byAsduBuf[iAsduLen++] = byStatus;
	}

	//添加报文头尾
	len = Add68HeadAndTail( byAsduBuf, iAsduLen, buf );
	if(len <= 0)
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIE101S_2002::GetYkRtnDataFrame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetChangeYcData
 * Description:  获取变化遥测
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetChangeYcData ( BYTE *buf, int &len )
{
	switch ( m_byChangeYc )
	{
	case 9: //测量值 归一化值
		return Get_M_ME_NA_1_Frame( buf, len, IEC101S_2002_CHANGE_TYPE );
		break;

	case 11://测量值 标度化值
		return Get_M_ME_NB_1_Frame( buf, len, IEC101S_2002_CHANGE_TYPE );
		break;

	case 13://测量值 短浮点数
		return Get_M_ME_NC_1_Frame( buf, len, IEC101S_2002_CHANGE_TYPE );
		break;

	case 21://测量值 不带品质描述次的归一化值
		return Get_M_ME_ND_1_Frame( buf, len, IEC101S_2002_CHANGE_TYPE );
		break;

	default:
		print( "IEC101S:GetTotalYcData can't find the changyc type" );
		break;
	}				/* -----  end switch  ----- */

	return GetNoneDataBuf( buf, len );
}		/* -----  end of method CIEC101S_2002::GetChangeYcData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetLevel2Data
 * Description:  获取二级数据
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetLevel2Data ( BYTE *buf, int &len )
{
	//如果建立链路阶段 回应无数据
	if( IsHaveSpecialData(  ) )
	{
		return GetSpecialData( buf, len );
	}

	if ( IsHaveLevel1Data(  ) )
	{
		return GetReconitionBuf( buf, len );
	}

	//遥信soe数据
	if( IsHaveYxSoeData(  ) )
	{
		print( "变化SOE" );
		//获取soe数据
		return GetSoeYxData (buf, len);
	}

	//是否有变化遥测数据
	if( IsHaveChangeYcData(  ) )
	{
		print( "变化遥测" );
		return GetChangeYcData( buf, len );
	}

	return GetNoneDataBuf( buf, len );
}		/* -----  end of method CIEC101S_2002::GetLevel2Data  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  IsHaveLevel1Data
 * Description:  查看是否有1级数据
 *       Input:  void
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::IsHaveLevel1Data (  )
{
	// if ( (m_dwSendFlag & IEC101S_TOTAL_YX) || IsHaveYxSoeData(  ) || IsHaveChangeYxData(  ) )
	if ( (m_dwSendFlag & IEC101S_TOTAL_YX) || IsHaveChangeYxData(  ) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}		/* -----  end of method CIEC101S_2002::IsHaveLevel1Data  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  IsHaveSpecialData
 * Description:  查看是否有特殊数据 特指开启链路 总召 对时等阶段
 *       Input:  void
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::IsHaveSpecialData ( void ) const
{
	if ( m_dwSendFlag & IEC101S_SPECIAL_DATA )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}		/* -----  end of method CIEC101S_2002::IsHaveSpecialData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  IsHaveYxSoeData
 * Description:  是否有YXSOE数据
 *       Input:  void
 *		Return: BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::IsHaveYxSoeData ( void ) const
{
	if ( m_iSOE_rd_p != m_iSOE_wr_p )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}		/* -----  end of method CIEC101S_2002::IsHaveYxSoeData  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  IsHaveChangeYxData
 * Description:  是否有变化遥信
 *       Input:	 void
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::IsHaveChangeYxData ( void )
{
	if ( m_dwDIEQueue.size( ) > 0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}		/* -----  end of method CIEC101S_2002::IsHaveChangeYxData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  IsHaveChangeYcData
 * Description:  是否有变化遥测
 *       Input:  void
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::IsHaveChangeYcData ( void )
{
	if ( m_dwAIEQueue.size(  ) > 0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}		/* -----  end of method CIEC101S_2002::IsHaveChangeYcData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetTotalYmData
 * Description:	 总召遥脉上送
 *       Input:  缓冲区 长度
 *		Return: BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetTotalYmData ( BYTE *buf, int &len )
{
	switch ( m_byTotalCallYm )
	{
	case 15://累积量
		print( "累计量" );
		return Get_M_IT_NA_1_Frame( buf, len, IEC101S_2002_TOTAL_TYPE );
		break;

	case 16://带时标的累计量
		print( "带时标的累计量" );
		return Get_M_IT_TA_1_Frame( buf, len, IEC101S_2002_TOTAL_TYPE );
		break;

	case 37://带CP56Time2a时标的累积量
		print( "带CP56TIME2a时标的累计量" );
		return Get_M_IT_TB_1_Frame( buf, len, IEC101S_2002_TOTAL_TYPE );
		break;

	default:
		sprintf( m_szPrintBuf, "全遥脉上送设置错误 m_byTotalCallYm=%d", m_byTotalCallYm );
		print( m_szPrintBuf );
		break;
	}				/* -----  end switch  ----- */
	return GetNoneDataBuf( buf, len );
}		/* -----  end of method CIEC101S_2002::GetTotalYmData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetTotalYcData
 * Description:	 总召遥测上送
 *       Input:  缓冲区 长度
 *		Return: BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetTotalYcData ( BYTE *buf, int &len )
{
	switch ( m_byTotalCallYc )
	{
	case 9:	//归一化值
		print( "归一化值" );
		return Get_M_ME_NA_1_Frame( buf, len, IEC101S_2002_TOTAL_TYPE );
		break;

	case 10://带时标的归一化值
		print( "带时标的归一化值" );
		break;

	case 11://标度化值
		print( "标度化值" );
		return Get_M_ME_NB_1_Frame( buf, len, IEC101S_2002_TOTAL_TYPE );
		break;

	case 12://带时标的标度化值
		print( "带时标的标度化值" );
		break;

	case 13://短浮点数
		print( "短浮点数" );
		return Get_M_ME_NC_1_Frame( buf, len, IEC101S_2002_TOTAL_TYPE );
		break;

	case 14://带时标的段浮点数
		print( "带时标的短浮点数" );
		break;

	case 21://测量值 不带品质描述词的归一化值
		print( "测量值 不带品质描述词的归一化值" );
		return Get_M_ME_ND_1_Frame( buf, len, IEC101S_2002_TOTAL_TYPE );
		break;

	case 34://带CP55Time2a的归一化值
		print( "带CP55Time2a的归一化值" );
		break;

	case 35://带CP55Time2a的标度化值
		print( "带CP55Time2a的标度化值" );
		break;

	case 36://带CP55Time2a的短浮点数
		print( "带CP55Time2a的短浮点数" );
		break;

	default:
		sprintf( m_szPrintBuf, "全遥测上送设置错误 m_byTotalCallYc=%d", m_byTotalCallYc );
		print( m_szPrintBuf );
		break;
	}				/* -----  end switch  ----- */

	return GetNoneDataBuf( buf, len );
}		/* -----  end of method CIEC101S_2002::GetTotalYcData  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetTotalYxData
 * Description:  总召遥信上送
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetTotalYxData ( BYTE *buf, int &len )
{
	switch ( m_byTotalCallYx )
	{
	case 1://单点信息
		print( "全遥信单点信息" );
		return Get_M_SP_NA_1_Frame( buf, len, IEC101S_2002_TOTAL_TYPE );
		break;

	case 2://带时标的单点信息
		print( "全遥信带时标的单点信息" );
		break;

	case 3://双点信息
		print( "全遥信带时标的单点信息" );
		return Get_M_DP_NA_1_Frame( buf, len, IEC101S_2002_TOTAL_TYPE );
		break;

	case 4://带时标的双点信息
		print( "全遥信双点信息" );
		break;

	case 30://带CP56Time2a时标的单点信息
		print( "全遥信带CP56Time2a时标的单点信息" );
		break;

	case 31://带CP56Time2a时标的双点信息
		print( "全遥信带CP56Time2a时标的单点信息" );
		break;

	default:
		sprintf( m_szPrintBuf, "全遥信上送设置错误 m_byTotalCallYx=%d", m_byTotalCallYx );
		print( m_szPrintBuf );
		break;
	}				/* -----  end switch  ----- */

	return FALSE;
}		/* -----  end of method CIEC101S_2002::GetTotalYxData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetSpecialData
 * Description:  获取 总召 遥脉 对时 时的数据报文
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetSpecialData ( BYTE *buf, int &len )
{
	if( m_dwSendFlag & IEC101S_TOTAL_CALL )			//总召确认
	{
		if ( GetTotalCallRecoBuf( buf, len, 0x07 ) )
		{
			m_dwSendFlag &= ~IEC101S_TOTAL_CALL;
			return TRUE;
		}
	}
	else if( m_dwSendFlag & IEC101S_TOTAL_YX )		//总召遥信上送
	{
		return GetTotalYxData( buf, len );
	}
	else if( m_dwSendFlag & IEC101S_TOTAL_YC )		//总召遥测上送
	{
		return  GetTotalYcData( buf, len );
	}
	else if( m_dwSendFlag & IEC101S_TOTAL_CALL_END ) //总召终止
	{
		if( GetTotalCallRecoBuf( buf, len, 0x0a ) )
		{
			m_dwSendFlag &= ~IEC101S_TOTAL_CALL_END;
			m_dwSendFlag &= ~IEC101S_SPECIAL_DATA;
			return TRUE;
		}
	}
	else if( m_dwSendFlag & IEC101S_CALL_YM )		//召唤遥脉确认
	{
		if( GetCallYmRecoBuf( buf, len, 0x07 ) )
		{
			m_dwSendFlag &= ~IEC101S_CALL_YM;
			return TRUE;
		}
	}
	else if( m_dwSendFlag & IEC101S_TOTAL_YM )      //召唤遥脉上送
	{
		return GetTotalYmData( buf, len );
	}
	else if( m_dwSendFlag & IEC101S_CALL_YM_END )    //召唤遥脉终止
	{
		if( GetCallYmRecoBuf( buf, len, 0x0a ) )
		{
			m_dwSendFlag &= ~IEC101S_CALL_YM_END;
			m_dwSendFlag &= ~IEC101S_SPECIAL_DATA;
			return TRUE;
		}
	}
	else if( m_dwSendFlag & IEC101S_TIME_SYNC )		//对时确认
	{
		if( GetTimeSyncRecoBuf( buf, len, 0x07 ) )
		{
			m_dwSendFlag &= ~IEC101S_TIME_SYNC;
			return TRUE;
		}
	}
	else if( m_dwSendFlag & IEC101S_TIME_SYNC_END ) //对时终止
	{
		if( GetTimeSyncRecoBuf( buf,len,0x0a ) )
		{
			m_dwSendFlag &= ~IEC101S_TIME_SYNC_END;
			m_dwSendFlag &= ~IEC101S_SPECIAL_DATA;
			return TRUE;
		}
	}

	return GetNoneDataBuf( buf, len );
}		/* -----  end of method CIEC101S_2002::GetSpecialData  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetChangeYxData
 * Description:  获取变化遥信报文
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetChangeYxData ( BYTE *buf, int &len )
{
	switch ( m_byChangeYx )
	{
	case 1:	//单点信息
		return Get_M_SP_NA_1_Frame( buf, len, IEC101S_2002_CHANGE_TYPE );
		break;

	case 3:	//双点信息
		return Get_M_DP_NA_1_Frame( buf, len, IEC101S_2002_CHANGE_TYPE );
		break;

	default:
		print( "IEC101S:GetChangeYxData err" );
		break;
	}				/* -----  end switch  ----- */

	return GetNoneDataBuf( buf, len );
}		/* -----  end of method CIEC101S_2002::GetChangeYxData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetSoeYxData
 * Description:  获取soe信息
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetSoeYxData ( BYTE *buf, int &len )
{
	switch ( m_bySoeYx  )
	{
	case 2:	//带时标的单点信息
		return Get_M_SP_TA_1_Frame( buf, len, IEC101S_2002_SOE_TYPE );
		break;

	case 4:	//带时标的双点信息
		return Get_M_DP_TA_1_Frame( buf, len, IEC101S_2002_SOE_TYPE );
		break;

	case 30://带CP56Time2a时标的单点信息
		return Get_M_SP_TB_1_Frame( buf, len, IEC101S_2002_SOE_TYPE );
		break;

	case 31://带CP56Time2a时标的双点信息
		return Get_M_DP_TB_1_Frame( buf, len, IEC101S_2002_SOE_TYPE );
		break;

	default:
		print( "IEC101S GetSoeYxData err" );
		break;
	}				/* -----  end switch  ----- */

	return GetNoneDataBuf( buf, len );
}		/* -----  end of method CIEC101S_2002::GetSoeYxData  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetLevel1Data
 * Description:  获取二级数据
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetLevel1Data ( BYTE *buf, int &len )
{
	//特殊数据
	if( IsHaveSpecialData(  ) )
	{
		//能否获取数据
		return GetSpecialData( buf, len );
	}

	//变化遥信数据
	if( IsHaveChangeYxData(  ) )
	{
		print( "变化遥信" );
		//获取变化遥信数据
		return GetChangeYxData( buf, len );
	}


	//无上述数据 返回无数据报文
	return GetNoneDataBuf( buf, len );
}		/* -----  end of method CIEC101S_2002::GetLevel1Data  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIE101S_2002
 *      Method:  GetYkRtnData
 * Description:  获取遥控返回数据
 *       Input:  buf, len
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetYkRtnData ( BYTE *buf, int &len )
{
	if( m_dwSendFlag & IEC101S_YK_CANCEL )
	{
		return GetYkRtnDataFrame( buf, len, IEC101S_YK_CANCEL );
	}
	else if( m_dwSendFlag & IEC101S_YK_SEL )
	{
		return GetYkRtnDataFrame( buf, len, IEC101S_YK_SEL );
	}
	else if( m_dwSendFlag & IEC101S_YK_EXE )
	{
		return GetYkRtnDataFrame( buf, len, IEC101S_YK_EXE );
	}

	printf ( "IEC101S GetYkRtnData err\n" );

	return GetNoneDataBuf( buf, len );
}		/* -----  end of method CIE101S_2002::GetYkRtnData  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetSendBuf
 * Description:  获取发送报文
 *       Input:  缓冲区 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetSendBuf ( BYTE *buf, int &len )
{
	BOOL bRtn = FALSE;

	if( m_bReSending && m_bLinkStatus )
	{
		m_SendStatus = RESEND;
	}
	// sprintf( m_szPrintBuf, "m_SendStatus=%d m_dwSendFlag=%x",m_SendStatus, m_dwSendFlag );
	// print( m_szPrintBuf );

	switch ( m_SendStatus )
	{
	case LINK_STATUS:
		print( "响应帧 链路状态或要求访问" );
		bRtn = GetLinkStatusBuf( buf, len );
		break;

	case RECOGNITION:
		print( "确认帧 认可" );
		bRtn = GetReconitionBuf( buf, len );
		break;

	case USER_DATA:
		print( "用户确认帧 认可" );
		bRtn = GetUserDataBuf( buf, len );
		break;

	case LEVEL2_DATA:
		print( "二级数据" );
		bRtn = GetLevel2Data( buf, len );
		break;

	case LEVEL1_DATA:
		print( "一级数据" );
		bRtn = GetLevel1Data( buf, len );
		break;

	case YK_RTN_DATA:
		print( "遥控返回数据" );
		bRtn = GetYkRtnData( buf, len );
		break;

	case RESEND:
		print( "重发报文" );
		memcpy( buf, m_byResendBuf, m_byResendLen );
		len = m_byResendLen;
		m_byResendCount ++;
		bRtn = TRUE;
		break;

	default:
		sprintf( m_szPrintBuf, "can't find m_SendStatus=%d", m_SendStatus );
		if( m_bLinkStatus )
		{
			print( m_szPrintBuf );
		}
		break;
	}				/* -----  end switch  ----- */

	//设置重发
	if( bRtn )
	{
		m_byResendLen = (BYTE)len;
		memcpy( m_byResendBuf, buf, m_byResendLen );
	}

	return bRtn;
}		/* -----  end of method CIEC101S_2002::GetSendBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIE101S_2002
 *      Method:  SetSendParam
 * Description:  设置发送参数
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CIEC101S_2002::SetSendParam ( void )
{
	m_bSending = TRUE;
	m_bySendCount ++;

	if( m_bySendCount > 1 )
	{
		m_bReSending = TRUE;
	}

	m_SendStatus = NULL_STATUS;
}		/* -----  end of method CIE101S_2002::SetSendParam  ----- */

/* ====================  SendEnd     ======================================= */
/* ====================  InitBegin     ======================================= */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  ReadCfgTemplate
 * Description:  读取模板信息  暂时未用 留取接口
 *       Input:	 void
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::ReadCfgTemplate ( void )
{
	FILE *fp ;
	char szFileName[256] = "";
	sprintf( szFileName, "%s%s" , IEC101SPREFIXFILENAME, m_sTemplatePath);
	// print( szFileName );
	char szLineBuf[256];
	int iLineNum = 0;

	fp = fopen( szFileName, "r" );
	if( fp == NULL )
	{
		printf ( "open file %s err!!!\n", szFileName );
		return FALSE;
	}

	while ( fgets(szLineBuf, sizeof(szLineBuf), fp) != NULL && iLineNum < 30 )
	{
		iLineNum ++;
		if( strncmp( szLineBuf, "COTLEN=", 7 ) == 0 )
		{
			BYTE byCfgVal = atoi( &szLineBuf[7] );
			if( 2 == byCfgVal || 1 == byCfgVal )
			{
				//1或2
				m_byCotLen = byCfgVal;
			}
		}
		else if( strncmp( szLineBuf, "ADDRLEN=", 8 ) == 0 )
		{
			BYTE byCfgVal = atoi( &szLineBuf[8] );
			if( 2 == byCfgVal || 1 == byCfgVal )
			{
				//1或2
				m_byAddrLen = byCfgVal;
			}
		}
		else if( strncmp( szLineBuf, "INFOADDRLEN=", 12 ) == 0 )
		{
			BYTE byCfgVal = atoi( &szLineBuf[12] );
			if( 2 == byCfgVal || 3 == byCfgVal )
			{
				//3或2
				m_byInfoAddrLen = byCfgVal;
			}
		}
		else if( strncmp( szLineBuf, "TOTALYCTYPE=", 12 ) == 0 )
		{
			BYTE byCfgVal = atoi( &szLineBuf[12] );
			if( 1 == byCfgVal || 3 == byCfgVal )
			{
				//1或3
				m_byTotalCallYx = byCfgVal;
			}
		}
		else if( strncmp( szLineBuf, "TOTALYXTYPE=", 12 ) == 0 )
		{
			BYTE byCfgVal = atoi( &szLineBuf[12] );
			if( 9 == byCfgVal || 11 == byCfgVal || 13==byCfgVal || 21==byCfgVal )
			{
				//9 11 13 21
				m_byTotalCallYc = byCfgVal;
			}

		}
		else if( strncmp( szLineBuf, "TOTALYMTYPE=", 12 ) == 0 )
		{
			BYTE byCfgVal = atoi( &szLineBuf[12] );
			if( 15 == byCfgVal || 16 == byCfgVal || 17==byCfgVal )
			{
				//15 16 17
				m_byTotalCallYm = byCfgVal;
			}
		}
		else if( strncmp( szLineBuf, "CHANGEYX=", 9 ) == 0 )
		{
			BYTE byCfgVal = atoi( &szLineBuf[9] );
			if( 3 == byCfgVal || 1 == byCfgVal)
			{
				//15 16 17
				m_byChangeYx = byCfgVal;
			}
		}
		else if( strncmp( szLineBuf, "CHANGEYC=", 9 ) == 0 )
		{
			BYTE byCfgVal = atoi( &szLineBuf[9] );
			if( 11 == byCfgVal || 13 == byCfgVal || 21==byCfgVal || 9==byCfgVal )
			{
				//15 16 17
				m_byChangeYc = byCfgVal;
			}
		}
		else if( strncmp( szLineBuf, "SOEYX=", 6 ) == 0 )
		{
			BYTE byCfgVal = atoi( &szLineBuf[6] );
			if( 2 == byCfgVal || 4 == byCfgVal || 30==byCfgVal || 31==byCfgVal )
			{
				//15 16 17
				m_bySoeYx = byCfgVal;
			}
		}
	}

	fclose( fp );

	return TRUE;
}		/* -----  end of method CIEC101S_2002::ReadCfgTemplate  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  ReadCfgMap
 * Description:  读取点表信息
 *       Input:	 void
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::ReadCfgMap ( void )
{
	char szFileName[256] = "";
	sprintf( szFileName, "%s%s" , IEC101SPREFIXFILENAME, m_sTemplatePath);
	print( szFileName );

	//读取需要转发的数据到该模块
	ReadMapConfig( szFileName );

	return TRUE;
}		/* -----  end of method CIEC101S_2002::ReadCfgMap  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  ReadCfgInfo
 * Description:  读取配置信息
 *       Input:	 void
 *		Return:	 BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::ReadCfgInfo ( void  )
{
	if( !ReadCfgTemplate(  ) )
	{
		return FALSE;
	}

	if( !ReadCfgMap(  ) )
	{
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CIEC101S_2002::ReadCfgInfo  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  InitProtocolState
 * Description:  初始化协议参数
 *       Input:  void
 *		Return:  void
 *--------------------------------------------------------------------------------------
 */
void CIEC101S_2002::InitProtocolState ( void )
{
	m_bLinkStatus = FALSE;
	m_dwSendFlag |= IEC101S_SPECIAL_DATA;

	//遥控参数
	m_byYKAsduType = 0;
	m_byYkCot = 0;
	m_wYkStn = 0;
	m_wYkPnt = 0;
	m_byYkStatus = 0;

	m_wDataIndex = 0; //数据编号  用于总召
	m_bDataInit = 1; //始终为1

	//置发送状态
	m_bSending = FALSE;
	m_bReSending = FALSE;

	//置计数状态
	m_bySendCount = 0;
	m_byRecvCount = 0;
	m_byResendCount = 0;

	//清空重发缓存
	memset( m_byResendBuf, 0, 256 );
	m_byResendLen = 0;

	//置FCB
	m_bFcb = FALSE;
}		/* -----  end of method CIEC101S_2002::InitProtocolState  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  InitProtocolTransTab
 * Description:  初始化转发信息
 *       Input:  void
 *		Return:  void
 *--------------------------------------------------------------------------------------
 */
void CIEC101S_2002::InitProtocolTransTab ( void )
{
	//获取转发序号
	CreateTransTab();
}		/* -----  end of method CIEC101S_2002::InitProtocolTransTab  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  InitProtocolData
 * Description:  初始化协议数据
 *       Input:  void
 *		Return:  void
 *--------------------------------------------------------------------------------------
 */
void CIEC101S_2002::InitProtocolData ( void )
{
	//从内存数据库中--获取转发表默认数据
	m_pMethod->ReadAllYcData(&m_fYcBuf[0]);
	m_pMethod->ReadAllYmData(&m_dwYmBuf[0]);
	m_pMethod->ReadAllYxData( &m_byYxBuf[ 0 ] ) ;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  InitProtocol
 * Description:  初始化协议状态
 *       Input:	 void
 *		Return:  void
 *--------------------------------------------------------------------------------------
 */
void CIEC101S_2002::InitProtocol ( void )
{
	//初始化基本状态参数
	InitProtocolState(  );

	//初始化点表信息
	InitProtocolTransTab(  );

	//初始化协议数据
	InitProtocolData(  );
}		/* -----  end of method CIEC101S_2002::InitProtocol  ----- */

/* ====================  InitEnd     ======================================= */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIE101S_2002
 *      Method:  TimerProc
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CIEC101S_2002::TimerProc ( void )
{
	ReadChangData();
	// sprintf ( m_szPrintBuf, "sendcount=%d recvcout=%d m_bLinkStatus=%d yc=%d yx=%d" , m_bySendCount, m_byRecvCount,m_bLinkStatus,m_dwAIEQueue.size(  ) ,m_dwDIEQueue.size(  ) );
	// print( m_szPrintBuf );
	if( m_bySendCount > IEC101S_2002_MAX_SEND_COUNT )
	{
		print( "IEC101S m_bySendCount > 3" );
		InitProtocolState(  );
	}

	if( m_byResendCount > IEC101S_2002_MAX_RESEND_COUNT )
	{
		print( "IEC101S m_RCount > 3" );
		InitProtocolState(  );
	}
}		/* -----  end of method CIE101S_2002::TimerProc  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  ProcessProtocolBuf
 * Description:  处理协议报文
 *       Input:  缓冲区指针 长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::ProcessProtocolBuf ( BYTE *pBuf, int len )
{
	//判断报文 并找出一帧合适报文
	if( !WhetherBufValid( pBuf, len ) )
	{
		print( "WhetherBufValid err" );
		m_bReSending = TRUE;
		return FALSE;
	}

	//处理报文
	if( !ProcessRecvBuf( pBuf, len ) )
	{
		print( "IEC101S:ProcessRecvBuf err!!!" );
		return FALSE;
	}

	//设置接收参数
	SetRecvParam(  );

	return TRUE;
}		/* -----  end of method CIEC101S_2002::ProcessProtocolBuf  ----- */



/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  GetProtocolBuf
 * Description:  获取协议报文
 *       Input:  缓冲区 长度 消息指针
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::GetProtocolBuf ( BYTE *buf, int &len, PBUSMSG pBusMsg )
{
	assert( buf != NULL );
	BOOL bRtn = TRUE;

	if( pBusMsg )
	{
		//处理消息
		if ( !DealBusMsg( pBusMsg ) )
		{
			printf( "IEC101S:消息处理错误\n" );
			// return FALSE;
		}
	}

	//获取发送报文
	if( !GetSendBuf( buf, len ) )
	{
		bRtn = FALSE;
	}
	// sprintf (m_szPrintBuf,  "len=%d\n", len );
	// print( m_szPrintBuf );

	//设置发送参数
	if( m_bLinkStatus )
	{
		// print( "set" );
		SetSendParam(  );
		if( bRtn )
		{
			m_byRecvCount = 0;
		}
	}

	return bRtn;
}		/* -----  end of method CIEC101S_2002::GetProtocolBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CIEC101S_2002
 *      Method:  Init
 * Description:  初始化协议
 *       Input:	 总线号
 *		Return:	 BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CIEC101S_2002::Init ( BYTE byLineNo )
{
	//读配置文件
	if ( !ReadCfgInfo(  ) )
	{
		printf( "IEC101S_2002:ReadCfgInfo err!!!\n" );
		return FALSE;
	}

	//初始化协议
	InitProtocol(  );

	return TRUE;
}		/* -----  end of method CIEC101S_2002::Init  ----- */

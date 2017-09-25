/*
 * =====================================================================================
 *
 *       Filename:  Cjt188_2004.cpp
 *
 *    Description:  Cjt188_2004 版本协议
 *
 *        Version:  1.0
 *        Created:  2015年03月12日 10时31分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp (), 
 *   Organization:  
 *
 *		  history:
 * =====================================================================================
 */
#include <stdio.h>
#include <assert.h>
#include "Cjt188_2004.h"

#define	CJT188_2004_SYNC_INTERVAL	60*20			/* 对时间隔  单位s*/

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  CCjt188_2004
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
CCjt188_2004::CCjt188_2004 ()
{
	InitProtocolStatus(  );
}  /* -----  end of method CCjt188_2004::CCjt188_2004  (constructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  ~CCjt188_2004
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
CCjt188_2004::~CCjt188_2004 ()
{
}  /* -----  end of method CCjt188_2004::~CCjt188_2004  (destructor)  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  ProcessDataT1
 * Description:  处理水表数据 10H-19H
 *       Input:  buf:68开头的报文缓冲区
 *				 len：缓冲区的长度
 *		Return:  TRUE：正确处理 FALSE：未正确处理
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::ProcessDataT1 ( const BYTE *buf, int len  )
{
	BYTE byYxVal;
	WORD wYcPnt = 0;
	WORD wYxPnt = 0;
	int i ;
	const BYTE *pointer = buf;
	pointer += 10;

	//判断数据长度是否正确
	if( 0x16 != ( *pointer ) )
	{
		print( "数据长度异常" );	
		return FALSE;
	}

	pointer += 4;  //切换到数据区
	
	/* yc 处理 */
	// 当前累积流量  结算日当前累积流量
	//5byte 
	for( i=0; i<2; i++ )
	{
		DWORD dwYcVal;
		float fYcVal;
		dwYcVal = HexToBcd( *pointer ) 
			+ HexToBcd( *(pointer + 1) ) * 100	
			+ HexToBcd( *(pointer + 2) ) * 10000 
			+ HexToBcd( *(pointer + 3) ) * 1000000;

		fYcVal = (float)(dwYcVal);
		sprintf( m_szPrintBuf, "%d yc%d update%f",m_SerialNo, wYcPnt, fYcVal );
		print( m_szPrintBuf );

		m_pMethod->SetYcData( m_SerialNo, wYcPnt, fYcVal );

		wYcPnt ++;
		pointer += 5;
	}

	//实时时间 不处理
	//7byte
	pointer += 7;

	/* yx 处理 */
	//阀门状态 两位 00：开 01：关 11：异常
	//此处当两个yx处理 阀门状态开关 
	if(  0 == ( *pointer & 0x03 ) )	
	{
		byYxVal = 0;
		m_pMethod->SetYxData( m_SerialNo, wYxPnt, byYxVal );		
		sprintf( m_szPrintBuf, "%d yx%d update%d",m_SerialNo, wYxPnt, byYxVal );
		print( m_szPrintBuf );
	}
	else if( 1 == (*pointer & 0x03) )
	{
		byYxVal = 1;
		m_pMethod->SetYxData( m_SerialNo, wYxPnt, byYxVal );		
		sprintf( m_szPrintBuf, "%d yx%d update%d",m_SerialNo, wYxPnt, byYxVal );
		print( m_szPrintBuf );
	}
	wYxPnt ++;

	if(  3 == ( *pointer & 0x03 ) )	
	{
		byYxVal = 0;
	}
	else 
	{
		byYxVal = 1;
	}
	m_pMethod->SetYxData( m_SerialNo, wYxPnt, byYxVal );		
	sprintf( m_szPrintBuf, "%d yx%d update%d",m_SerialNo, wYxPnt, byYxVal );
	print( m_szPrintBuf );
	wYxPnt ++;

	//电池电压及其它
	for( i=0; i<6; i++ )	
	{
		byYxVal = ( *pointer >> (2 + i) ) & 0x01;
		m_pMethod->SetYxData( m_SerialNo, wYxPnt, byYxVal );		
		sprintf( m_szPrintBuf, "%d yx%d update%d",m_SerialNo, wYxPnt, byYxVal );
		print( m_szPrintBuf );
		wYxPnt ++;
	}
	pointer++;
	for( i=0; i<8; i++ )	
	{
		byYxVal = ( *pointer >> i ) & 0x01;
		m_pMethod->SetYxData( m_SerialNo, wYxPnt, byYxVal );		
		sprintf( m_szPrintBuf, "%d yx%d update%d",m_SerialNo ,wYxPnt, byYxVal );
		print( m_szPrintBuf );
		wYxPnt ++;
	}
	
	return TRUE;
}		/* -----  end of method CCjt188_2004::ProcessDataT1  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  ProcessDataT2
 * Description:  处理热量表数据 20H-29H
 *               buf:68开头的报文缓冲区
 *       Input:  len：缓冲区的长度
 *		Return:  TRUE：正确处理 FALSE：未正确处理
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::ProcessDataT2 ( const BYTE *buf, int len  )
{
	DWORD dwYcVal;
	float fYcVal;
	BYTE byYxVal;
	WORD wYcPnt = 0;
	WORD wYxPnt = 0;
	int i ;
	const BYTE *pointer = buf;
	pointer += 10;

	//判断数据长度是否正确
	if( 0x2e != ( *pointer ) )
	{
		print( "数据长度异常" );	
		return FALSE;
	}

	pointer += 4;  //切换到数据区
	/* yc 处理 */
	// 结算日热量 当前热量 热功率 流量 当前累积流量
	//5byte 
	for( i=0; i<5; i++ )
	{
		dwYcVal = HexToBcd( *pointer ) 
			+ HexToBcd( *(pointer + 1) ) * 100	
			+ HexToBcd( *(pointer + 2) ) * 10000 
			+ HexToBcd( *(pointer + 3) ) * 1000000;
		fYcVal = (float)(dwYcVal) ;
		m_pMethod->SetYcData( m_SerialNo, wYcPnt, fYcVal );
		sprintf( m_szPrintBuf, "%d yc%d update%f",m_SerialNo, wYcPnt, fYcVal );
		print( m_szPrintBuf );

		wYcPnt ++;
		pointer += 5;
	}

	//供水温度 回水温度
	for ( i=0; i<2; i++ )
	{
		dwYcVal = HexToBcd( *pointer ) 
			+ HexToBcd( *(pointer + 1) ) * 100	
			+ HexToBcd( *(pointer + 2) ) * 10000 ;

		fYcVal = (float)(dwYcVal) / 100;
		m_pMethod->SetYcData( m_SerialNo, wYcPnt, fYcVal );
		sprintf( m_szPrintBuf, "%d yc%d update%f",m_SerialNo, wYcPnt, fYcVal );
		print( m_szPrintBuf );
		wYcPnt ++;
		pointer += 3;
	}

	//积累工作时间
	//3byte
	dwYcVal = HexToBcd( *pointer ) 
		+ HexToBcd( *(pointer + 1) ) * 100	
		+ HexToBcd( *(pointer + 2) ) * 10000 ;

	fYcVal = (float)(dwYcVal);
	m_pMethod->SetYcData( m_SerialNo, wYcPnt, fYcVal );
	sprintf( m_szPrintBuf, "%d yc%d update%f",m_SerialNo, wYcPnt, fYcVal );
	print( m_szPrintBuf );
	wYcPnt ++;
	pointer += 3;

	//实时时间 不处理
	//7byte
	pointer += 7;

	/* yx 处理 */
	//阀门状态 两位 00：开 01：关 11：异常
	//此处当两个yx处理 阀门状态开关 
	if(  0 == ( *pointer & 0x03 ) )	
	{
		byYxVal = 0;
		m_pMethod->SetYxData( m_SerialNo, wYxPnt, byYxVal );		
		sprintf( m_szPrintBuf, "%d yx%d update%d",m_SerialNo, wYxPnt, byYxVal );
		print( m_szPrintBuf );
	}
	else if( 1 == (*pointer & 0x03) )
	{
		byYxVal = 1;
		m_pMethod->SetYxData( m_SerialNo, wYxPnt, byYxVal );		
		sprintf( m_szPrintBuf, "%d yx%d update%d",m_SerialNo, wYxPnt, byYxVal );
		print( m_szPrintBuf );
	}
	wYxPnt ++;

	if(  3 == ( *pointer & 0x03 ) )	
	{
		byYxVal = 0;
	}
	else 
	{
		byYxVal = 1;
	}
	m_pMethod->SetYxData( m_SerialNo, wYxPnt, byYxVal );		
	sprintf( m_szPrintBuf, "%d yx%d update%d",m_SerialNo, wYxPnt, byYxVal );
	print( m_szPrintBuf );
	wYxPnt ++;

	//电池电压及其它
	for( i=0; i<6; i++ )	
	{
		byYxVal = ( *pointer >> (2 + i) ) & 0x01;
		m_pMethod->SetYxData( m_SerialNo, wYxPnt, byYxVal );		
		sprintf( m_szPrintBuf, "%d yx%d update%d",m_SerialNo,wYxPnt, byYxVal );
		print( m_szPrintBuf );
		wYxPnt ++;
	}
	pointer++;
	for( i=0; i<8; i++ )	
	{
		byYxVal = ( *pointer >> i ) & 0x01;
		m_pMethod->SetYxData( m_SerialNo, wYxPnt, byYxVal );		
		sprintf( m_szPrintBuf, "%d yx%d update%d",m_SerialNo, wYxPnt, byYxVal );
		print( m_szPrintBuf );
		wYxPnt ++;
	}
	
	return TRUE;
}		/* -----  end of method CCjt188_2004::ProcessDataT2  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  ProcessDataT3
 * Description:  处理燃气表数据 30H-39H
 *               buf:68开头的报文缓冲区
 *       Input:  len：缓冲区的长度
 *		Return:  TRUE：正确处理 FALSE：未正确处理
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::ProcessDataT3 ( const BYTE *buf, int len  )
{
	ProcessDataT1( buf, len );
	return TRUE;
}		/* -----  end of method CCjt188_2004::ProcessDataT3  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  ProcessReadData
 * Description:  处理格式正确的数据， 经过两步， 判断和处理
 *       Input: buf:68开头的报文缓冲区
 *              len：缓冲区的长度
 *		Return: TRUE：正确处理 FALSE：未正确处理
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::ProcessReadData ( const BYTE *buf, int len )
{
	const BYTE *pointer = buf;
	pointer += 9;
	/* 判断数据 */
	//判断数据有效性 D6 表示通讯是否异常
	if( 0x40 == ( 0x40 & *pointer )  )	
	{
		print( "通讯异常" );	
		return FALSE;
	}

	//判断数据长度是否正确
	pointer += 2;
	
	//判断d0 d1 是否正确
	if( m_CfgInfo[m_bySendPos].byDI0 != *(pointer) 
		|| m_CfgInfo[m_bySendPos].byDI1 != *(pointer+1) )
	{
		sprintf( m_szPrintBuf,
				"数据d0 或 d1不正确recv d0=%x d1=%x local d0=%x d1=%x",
				*pointer, *(pointer+1), m_CfgInfo[m_bySendPos].byDI0,m_CfgInfo[m_bySendPos].byDI1 );
		print( m_szPrintBuf );	
		return FALSE;
	}
	pointer += 2;

	//判断ser 的正确性
	if( m_bySer != *pointer )
	{
		sprintf( m_szPrintBuf,
				"数据序号不正确recv ser=%x local ser=%x",
				*pointer, m_bySer );
		print( m_szPrintBuf );	
		return FALSE;
	}

	/* 处理数据 */
	if( m_byMeterType >= 0x10 && m_byMeterType <= 0x19 )
	{
		print( "水表数据 " );
		return ProcessDataT1( buf, len );   //水表数据
	}
	else if( m_byMeterType >= 0x20 && m_byMeterType <= 0x29 )
	{
		print( "热量表数据 " );
		return ProcessDataT2( buf, len );   //热量表数据
	}
	else if( m_byMeterType >= 0x30 && m_byMeterType <= 0x39 )
	{
		print( "燃气表数据 " );
		return ProcessDataT3( buf, len );   //燃气表数据
	}

	return FALSE;
}		/* -----  end of method CCjt188_2004::ProcessReadData  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  ProcessBuf
 * Description:  处理接收报文
 *       Input:	 接收缓存区 长度
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::ProcessBuf ( const BYTE *buf, int len )
{
	switch ( m_byDataType )
	{
		case CJT188_READDATA_DATATYPE:	
			print( "读数据处理" );
			return ProcessReadData( buf, len );
			break;

		case CJT188_TIME_DATATYPE:	
			print( "对时数据" );
			break;

		default:	
			sprintf( m_szPrintBuf, "未找到该数据类型%d", m_byDataType );
			print( m_szPrintBuf );
			return FALSE;
			break;
	}				/* -----  end switch  ----- */

	return TRUE;
}		/* -----  end of method CCjt188_2004::ProcessBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  IsTimeToSync
 * Description:  是否对时  开机对时一次， 以后按CJT188_2004_SYNC_INTERVAL间隔对时
 *       Input:  void
 *		Return:  TRUE:对时 FALSE:不对时
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::IsTimeToSync ( void )
{
	static time_t oldTime;
	time_t curTime;

	//开机是对时一次
	if( m_bLinkStatus && m_bLinkTimeSyn )
	{
		oldTime = time( NULL );
		m_bLinkTimeSyn = FALSE;
		return TRUE;
	}

	//获取当前时间 比较时间差 对时	
	curTime = time(NULL);
	if( difftime( curTime, oldTime ) > CJT188_2004_SYNC_INTERVAL )
	{
		oldTime =  curTime;
		return TRUE;
	}

	return FALSE;
}		/* -----  end of method CCjt188_2004::IsTimeToSync  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  RequestReadData
 * Description:  请求数据
 *       Input:  buf:组织报文的缓冲区 
 *				 len:报文的长度
 *		Return:  TRUE
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::RequestReadData ( BYTE *buf, int &len )
{
	len = 0;
	for ( int i=0; i<m_byFENum; i++)
	{
		buf[len++] = 0xfe;
	}
	buf[len++] = 0x68;
	buf[len++] = m_byMeterType;
	//地址位
	for ( int i=0; i<7; i++)
	{
		buf[len++] = m_bySlaveAddr[i];
	}
	buf[len++] = 0x01;  //控制码 读数据
	buf[len++] = 0x03;	//数据长度
	//2004为2个标识符
	buf[len++] = m_CfgInfo[m_bySendPos].byDI0;
	buf[len++] = m_CfgInfo[m_bySendPos].byDI1;
	buf[len++] = ( ++m_bySer ) % 0xff;	//顺序号
	buf[len++] = GetCs( buf + m_byFENum, len - m_byFENum );
	buf[len++] = 0x16;

	return TRUE;
}		/* -----  end of method CCjt188_2004::RequestReadData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  TimeSync
 * Description:  对时报文
 *       Input:	 buf:组织报文的缓冲区 
 *               len:报文的长度
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::TimeSync ( BYTE *buf, int &len )
{
	REALTIME curTime;
	len = 0;
	for ( int i=0; i<m_byFENum; i++)
	{
		buf[len++] = 0xfe;
	}
	buf[len++] = 0x68;
	buf[len++] = m_byMeterType;
	//地址位
	for ( int i=0; i<7; i++)
	{
		buf[len++] = m_bySlaveAddr[i];
	}
	buf[len++] = 0x04;	//写数据
	buf[len++] = 0x0a;	//数据长度

	//2004为2个标识符
	buf[len++] = m_CfgInfo[m_bySendPos].byDI0;
	buf[len++] = m_CfgInfo[m_bySendPos].byDI1;

	buf[len++] = ( ++m_bySer ) % 0xff;	//顺序号
	//时间
	GetCurrentTime( &curTime );
	buf[len++] = BcdToHex( (BYTE)curTime.wSecond );
	buf[len++] = BcdToHex( (BYTE)curTime.wMinute );
	buf[len++] = BcdToHex( (BYTE)curTime.wHour);
	buf[len++] = BcdToHex( (BYTE)curTime.wDay);
	buf[len++] = BcdToHex( (BYTE)curTime.wMonth ) ;
	buf[len++] = BcdToHex( (BYTE)(curTime.wYear%100));
	buf[len++] = BcdToHex( (BYTE)(curTime.wYear/100));

	buf[len++] = GetCs( buf + m_byFENum, len - m_byFENum );
	buf[len++] = 0x16;

	return TRUE;
}		/* -----  end of method CCjt188_2004::TimeSync  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  GetSendBuf
 * Description:	 获取发送报文和长度	  
 *       Input:	 发送缓冲区 长度
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::GetSendBuf ( BYTE *buf, int &len )
{
	switch ( m_byDataType )
	{
		case CJT188_READDATA_DATATYPE:	
			print( "请求数据" );
			return RequestReadData( buf, len );
			break;

		case CJT188_TIME_DATATYPE:	
			print( "对时" );
			return TimeSync( buf, len );
			break;

		default:	
			sprintf( m_szPrintBuf, "Cjt188_2004 第%d条命令数据类型配置错误", m_bySendPos );
			print( m_szPrintBuf );
			return FALSE;
			break;
	}				/* -----  end switch  ----- */
	return TRUE;
}		/* -----  end of method CCjt188_2004::GetSendBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  InitProtocolStatus
 * Description:  初始化协议状态数据 函数开始或通讯发生异常时调用此函数
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::InitProtocolStatus ( void )
{
	m_bLinkStatus = FALSE;		//链接状态为断
	m_bySendPos = 0;			//发送位置置0
	m_byDataType = 0;			//发送类型为空
	m_byRecvErrorCount = 0;     //接收错误计数0
	m_bIsReSend = FALSE;		//重发标识位0
	m_byResendCount = 0;		//重发次数清零
	m_bIsSending = FALSE;		//发送后置1 接收后值0
	m_bIsNeedResend = TRUE;		//是否需要重发
	m_bTimeSynFlag = FALSE;		//对时标识
	m_bLinkTimeSyn = TRUE;		//装置连通后对时一次
	m_bySer = 0;				//续号清0

	//重发缓存区清零
	m_byReSendLen = 0;
	memset( m_byReSendBuf, 0, CJT188_MAX_BUF_LEN );

	return TRUE;
}		/* -----  end of method CCjt188_2004::InitProtocolStatus  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  InitSendCfgInfo
 * Description:  初始化发送信息，方便发送信息管理
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CCjt188_2004::InitSendCfgInfo ( void )
{
	Cjt188CfgInfo tCfgInfo;

	//读计量信息
	tCfgInfo.byDataType = CJT188_READDATA_DATATYPE;
	tCfgInfo.byDI0 = 0x1f;
	tCfgInfo.byDI1 = 0x90;
	tCfgInfo.byCycle = 1;
	m_CfgInfo.push_back( tCfgInfo );

	//写标准时间
	tCfgInfo.byDataType = CJT188_TIME_DATATYPE;
	tCfgInfo.byDI0 = 0x15;
	tCfgInfo.byDI1 = 0xA0;
	tCfgInfo.byCycle = 0;
	m_CfgInfo.push_back( tCfgInfo );
}		/* -----  end of method CCjt188_2004::InitSendCfgInfo  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Cjt188
 *      Method:  GetDevNameToAddr
 * Description:  通过装置的名字读取通讯地址
 *       Input:  void
 *		Return:  
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::GetDevNameToAddr ( void )
{
	int len = strlen( m_sDevName );
	if( len < 14)
	{
		return FALSE;
	}

	m_bySlaveAddr[6] = atoh( m_sDevName + len - 14, 2, 1 );
	m_bySlaveAddr[5] = atoh( m_sDevName + len - 12, 2, 1 );
	m_bySlaveAddr[4] = atoh( m_sDevName + len - 10, 2, 1 );
	m_bySlaveAddr[3] = atoh( m_sDevName + len - 8, 2, 1 );
	m_bySlaveAddr[2] = atoh( m_sDevName + len - 6, 2, 1 );
	m_bySlaveAddr[1] = atoh( m_sDevName + len - 4, 2, 1 );
	m_bySlaveAddr[0] = atoh( m_sDevName + len - 2, 2, 1 );

	return TRUE;

}		/* -----  end of method CProtocol_Cjt188::GetDevNameToAddr  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  TimerProc
 * Description:  时间处理函数 主要处理一些超时
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CCjt188_2004::TimerProc ( void )
{
	if( ! m_bLinkStatus )
	{
		return;
	}
	//时间判断
	//
	//重发次数处理
	if ( m_bIsReSend && m_byResendCount >= CJT188_MAX_RESEND_COUNT )
	{
		sprintf( m_szPrintBuf, "resend count %d >= %d InitProtocolStatus", m_byResendCount, CJT188_MAX_RESEND_COUNT );
		print( m_szPrintBuf );
		InitProtocolStatus(  );
	}

	//接收错误次数处理
	if ( m_byRecvErrorCount >= CJT188_MAX_RECV_ERR_COUNT )
	{
		sprintf( m_szPrintBuf, "recv err count %d >= %d InitProtocolStatus", m_byRecvErrorCount, CJT188_MAX_RECV_ERR_COUNT );
		print( m_szPrintBuf );
		InitProtocolStatus(  );
	}
}		/* -----  end of method CCjt188_2004::TimerProc  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  ProcessProtocolBuf
 * Description:	 处理收到的数据缓存 
 *       Input:  接收到的数据缓存 缓存长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::ProcessProtocolBuf ( BYTE *buf, int len )
{
	int pos = 0;
	BOOL bRtn = TRUE;

	print( "ProcessProtocolBuf" );
	//判断报文合理性
	if( !WhetherBufValue( buf, len , pos ) )
	{
		//报文错误处理
		print ( "Cjt1886 WhetherBufValue buf Recv err!!!\n" );
		m_byRecvErrorCount ++;
		m_bIsReSend = TRUE;
		return FALSE;
	}

	//处理报文
	bRtn = ProcessBuf( buf+m_byFENum, len );
	if( !bRtn )
	{
		print( "处理报文发生错误或未处理" );
	}

	//更新状态
	m_byRecvErrorCount = 0;
	m_bLinkStatus = TRUE;
	m_bIsReSend = FALSE;
	m_byResendCount = 0;
	m_bIsSending = FALSE;

	//处理正确报文
	return TRUE;
}		/* -----  end of method CCjt188_2004::ProcessProtocolBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  GetProtocolBuf
 * Description:  获取协议数据缓存
 *       Input:  缓存区 缓存区数据长度 总线消息
 *		Return:	 BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::GetProtocolBuf ( BYTE *buf, int &len, PBUSMSG pBusMsg )
{
	BOOL bRtn = TRUE;
	//重发机制
	if (  m_bIsReSend && m_bIsSending )
	{
		len = m_byReSendLen; 
		memcpy( buf, m_byReSendBuf, len );
		m_byResendCount ++;
	}
	else if( m_bLinkStatus && pBusMsg != NULL )
	{
		print( "总线消息" );
		//cjt188目前未做任何消息命令 直接返回
		return FALSE;
	}
	else
	{
		print( "GetSendBuf" );		
		//发送位置移动
		ChangeSendPos(  );
		m_byDataType = m_CfgInfo[m_bySendPos].byDataType;

		//判判对时 如果是对时 对时优先
		if( IsTimeToSync() )
		{
			int i;
			print( "time to sync" );
			m_byDataType = CJT188_TIME_DATATYPE;	
			for( i=0; i<(int)m_CfgInfo.size(); i++ )
			{
				if( CJT188_TIME_DATATYPE == m_CfgInfo[i].byDataType )	
				{
					m_bySendPos = i;
					break;
				}
			}

			m_bIsNeedResend = FALSE;
		}

		//组织发送报文
		bRtn = GetSendBuf( buf, len );

		if ( bRtn && len > 0)
		{
			//组织重发报文
			m_byReSendLen = len;	
			memcpy( m_byReSendBuf, buf, m_byReSendLen );
			m_bIsReSend = TRUE;

			m_bIsSending = TRUE;
			//针对不需要重发的信息设置此标识位
			if( !m_bIsNeedResend )
			{
				m_bIsSending = FALSE;
				m_bIsNeedResend = TRUE;
			}
		}
	}

	return bRtn;
}		/* -----  end of method CCjt188_2004::GetProtocolBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  Init
 * Description:	 初始化协议数据  
 *       Input:  总线号
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::Init ( BYTE byLineNo )
{
	assert( byLineNo <= 22 );

	if( !GetDevNameToAddr(  ) )
	{
		print ( "CCjt188_2004:Addr Err!!!\n" );	
		return FALSE;
	}

	//读取配置文件
	if( !ReadCfgInfo() )
	{
		print ( "CCjt188_2004:ReadCfgInfo Err!!!\n" );	
		return FALSE;
	}

	//初始化协议状态
	if( !InitProtocolStatus() ) 
	{
		print ( "CCjt188_2004:InitProtocolStatus Err\n" );
		return FALSE;
	}

	//初始化发送命令
	InitSendCfgInfo(  );
	if( m_CfgInfo.empty() )
	{
		printf ( "CCjt188_2004:can't find the InitSendCfgInfo\n" );
		return FALSE;
	}

	print( "Cjt188 Init OK" );
	return TRUE;
}		/* -----  end of method CCjt188_2004::Init  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CCjt188_2004
 *      Method:  GetDevCommState
 * Description:	 设置装置链接状态  
 *       Input:
 *		Return:	 BOOL 
 *--------------------------------------------------------------------------------------
 */
BOOL CCjt188_2004::GetDevCommState ( void )
{
	if ( m_bLinkStatus )
		return COM_NORMAL;
	else
		return COM_DEV_ABNORMAL;
}		/* -----  end of method CCjt188_2004::GetDevCommState  ----- */


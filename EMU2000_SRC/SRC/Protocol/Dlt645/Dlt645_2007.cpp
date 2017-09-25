/*
 * =====================================================================================
 *
 *       Filename:  Dlt645_2007.cpp
 *
 *    Description:  dlt645 2007版本协议
 *
 *        Version:  1.0
 *        Created:  2014年11月10日 14时12分10秒
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
#include "Dlt645_2007.h"


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  CDlt645_2007
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
CDlt645_2007::CDlt645_2007 ()
{/*{{{*/
	InitProtocolStatus(  );
}  /* -----  end of method CDlt645_2007::CDlt645_2007  (constructor)  ----- *//*}}}*/

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  ~CDlt645_2007
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
CDlt645_2007::~CDlt645_2007 ()
{
}  /* -----  end of method CDlt645_2007::~CDlt645_2007  (destructor)  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  ProcessYcData
 * Description:  遥测处理
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDlt645_2007::ProcessYcData ( const BYTE *buf, int len )
{/*{{{*/
	BYTE byDataNum = 0;
	BYTE wPnt = 0;
	BYTE byDataFormat = 0;
	BYTE byDataLen = 0;
	DWORD dwYcVal = 0;
	const BYTE *pointer;
	if ( len < 16 )
		return FALSE;

	if ( buf[8] != 0x91 )
		return FALSE;

	byDataNum = m_CfgInfo[m_bySendPos].byDataNum;
	wPnt = (WORD)m_CfgInfo[m_bySendPos].byStartIndex;
	byDataFormat = m_CfgInfo[m_bySendPos].byDataFormat;
	byDataLen = m_CfgInfo[m_bySendPos].byDataLen;

	pointer = buf + 14;
	while( byDataNum > 0 )
	{
		float fYcVal;

		CalFormatData( pointer, byDataFormat, byDataLen, dwYcVal);
		fYcVal = (float)dwYcVal;
		m_pMethod->SetYcData( m_SerialNo, wPnt, fYcVal );

		pointer += byDataLen;
		wPnt++;
		byDataNum--;
		sprintf( m_szPrintBuf, "yc pnt:%d value:%f", wPnt, fYcVal );
		print( m_szPrintBuf );
	}

	return TRUE;
}		/* -----  end of method CDlt645_2007::ProcessYcData  ----- *//*}}}*/

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  ProcessYmData
 * Description:  遥脉处理
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDlt645_2007::ProcessYmData ( const BYTE *buf, int len )
{/*{{{*/
	BYTE byDataNum = 0;
	BYTE wPnt = 0;
	DWORD dwYmVal = 0;
	BYTE byDataFormat = 0;
	BYTE byDataLen = 0;
	const BYTE *pointer;
	if ( len < 16 )
  {
      print((char *)"len < 16");
      return FALSE;
  }

	if ( buf[8] != 0x91 )
  {
      sprintf( m_szPrintBuf, "buf[8]=%.2x", buf[8] );
      print(m_szPrintBuf);
      return FALSE;
  }

	byDataNum = m_CfgInfo[m_bySendPos].byDataNum;
	wPnt = (WORD)m_CfgInfo[m_bySendPos].byStartIndex;
	byDataFormat = m_CfgInfo[m_bySendPos].byDataFormat;
	byDataLen = m_CfgInfo[m_bySendPos].byDataLen;

	pointer = buf + 14;
	while( byDataNum > 0 )
	{
		CalFormatData( pointer, byDataFormat, byDataLen, dwYmVal );
		m_pMethod->SetYmData( m_SerialNo, wPnt, dwYmVal );

		pointer += byDataLen;
		wPnt++;
		byDataNum--;

		sprintf( m_szPrintBuf, "ym pnt:%d value:%lu", wPnt, dwYmVal );
		print( m_szPrintBuf );
	}

	return TRUE;
}		/* -----  end of method CDlt645_2007::ProcessYmData  ----- *//*}}}*/


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  ProcessBuf
 * Description:  处理接收报文
 *       Input:	 接收缓存区长度
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDlt645_2007::ProcessBuf ( const BYTE *buf, int len )
{/*{{{*/
	switch ( m_byDataType )
	{
		case DLT645_YC_DATATYPE:
			print( "遥测数据" );
			ProcessYcData( buf, len );
			break;

		case DLT645_YM_DATATYPE:
			print( "遥脉数据" );
			ProcessYmData( buf, len );
			break;

		default:
			sprintf( m_szPrintBuf, "未找到该数据类型%d", m_byDataType );
			print( m_szPrintBuf );
			return FALSE;
			break;
	}				/* -----  end switch  ----- */
	return TRUE;
}		/* -----  end of method CDlt645_2007::ProcessBuf  ----- *//*}}}*/

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  IsTimeToSync
 * Description:  是否对时
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDlt645_2007::IsTimeToSync ( void )
{/*{{{*/
	if( m_bLinkStatus && m_bLinkTimeSyn )
	{
		m_bLinkTimeSyn = FALSE;
		return TRUE;
	}

	REALTIME curTime;
	GetCurrentTime( &curTime );

	if( 12 == curTime.wHour )
	{
		if( 1 > curTime.wMinute && 10 > curTime.wSecond )
		{
			if( m_bTimeSynFlag )
				return FALSE;
			else
				return TRUE;
		}
		else
		{
			m_bTimeSynFlag = FALSE;
		}
	}

	return FALSE;
}		/* -----  end of method CDlt645_2007::IsTimeToSync  ----- *//*}}}*/


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  RequestReadData
 * Description:  请求数据
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDlt645_2007::RequestReadData ( BYTE *buf, int &len )
{/*{{{*/
	len = 0;
	for ( int i=0; i<m_CfgInfo[m_bySendPos].byFENum; i++)
	{
		buf[len++] = 0xfe;
	}
	buf[len++] = 0x68;
	//地址位
	for ( int i=0; i<6; i++)
	{
		buf[len++] = m_bySlaveAddr[i];
	}
	buf[len++] = 0x68;
	buf[len++] = 0x11;	//读数据
	buf[len++] = 0x04;	//数据长度
	//2007为4个标识符
	buf[len++] = m_CfgInfo[m_bySendPos].byDI0 + 0x33;
	buf[len++] = m_CfgInfo[m_bySendPos].byDI1 + 0x33;
	buf[len++] = m_CfgInfo[m_bySendPos].byDI2 + 0x33;
	buf[len++] = m_CfgInfo[m_bySendPos].byDI3 + 0x33;
	buf[len++] = GetCs( buf + m_CfgInfo[m_bySendPos].byFENum, 14 );				//by cyz!
	buf[len++] = 0x16;

	return TRUE;
}		/* -----  end of method CDlt645_2007::RequestReadData  ----- *//*}}}*/

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  TimeSync
 * Description:  对时报文
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDlt645_2007::TimeSync ( BYTE *buf, int &len )
{/*{{{*/
	REALTIME curTime;
	len = 0;
	for ( int i=0; i<m_CfgInfo[m_bySendPos].byFENum; i++)
	{
		buf[len++] = 0xfe;
	}
	buf[len++] = 0x68;
	//地址位
	for ( int i=0; i<6; i++)
	{
		buf[len++] = 0x99;
	}
	buf[len++] = 0x68;
	buf[len++] = 0x08;	//读数据
	buf[len++] = 0x06;	//数据长度

	GetCurrentTime( &curTime );
	//2007为4个标识符
	buf[len++] = (BYTE)(curTime.wSecond + 0x33);
	buf[len++] = (BYTE)curTime.wMinute + 0x33;
	buf[len++] = (BYTE)curTime.wHour + 0x33;
	buf[len++] = (BYTE)curTime.wDay + 0x33;
	buf[len++] = (BYTE)curTime.wMonth + 0x33;
	buf[len++] = (BYTE)(curTime.wYear-2000)+ 0x33;
	buf[len++] = GetCs( buf, 16 );
	buf[len++] = 0x16;

	return TRUE;
}		/* -----  end of method CDlt645_2007::TimeSync  ----- *//*}}}*/


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  GetSendBuf
 * Description:	 获取发送报文和长度
 *       Input:	 发送缓冲区 长度
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDlt645_2007::GetSendBuf ( BYTE *buf, int &len )
{/*{{{*/
	switch ( m_byDataType )
	{
		case DLT645_YC_DATATYPE:

		case DLT645_YM_DATATYPE:
			print( "请求数据" );
			RequestReadData( buf, len );
			break;

		case DLT645_TIME_DATATYPE:
			print( "对时" );
			TimeSync( buf, len );
			break;

		default:
			sprintf( m_szPrintBuf, "Dlt645_2007 第%d条命令数据类型配置错误", m_bySendPos );
			print( m_szPrintBuf );
			return FALSE;
			break;
	}				/* -----  end switch  ----- */
	return TRUE;
}		/* -----  end of method CDlt645_2007::GetSendBuf  ----- *//*}}}*/

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  InitProtocolStatus
 * Description:  初始化协议状态数据
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDlt645_2007::InitProtocolStatus ( void )
{/*{{{*/
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

	//重发缓存区清零
	m_byReSendLen = 0;
	memset( m_byReSendBuf, 0, DLT645_MAX_BUF_LEN );

	return TRUE;
}		/* -----  end of method CDlt645_2007::InitProtocolStatus  ----- *//*}}}*/


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  TimerProc
 * Description:  时间处理函数 主要处理一些超时
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDlt645_2007::TimerProc ( void )
{/*{{{*/
	//时间判断
	//
	//重发次数处理
	if ( m_bIsReSend && m_byResendCount > DLT645_MAX_RESEND_COUNT )
	{
		sprintf( m_szPrintBuf, "resend count %d > %d InitProtocolStatus", m_byResendCount, DLT645_MAX_RESEND_COUNT );
		print( m_szPrintBuf );
		InitProtocolStatus(  );
	}

	//接收错误次数处理
	if ( m_byRecvErrorCount > DLT645_MAX_RECV_ERR_COUNT )
	{
		sprintf( m_szPrintBuf, "recv err count %d > %d InitProtocolStatus", m_byRecvErrorCount, DLT645_MAX_RECV_ERR_COUNT );
		print( m_szPrintBuf );
		InitProtocolStatus(  );
	}
}		/* -----  end of method CDlt645_2007::TimerProc  ----- *//*}}}*/


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  ProcessProtocolBuf
 * Description:	 处理收到的数据缓存
 *       Input:  接收到的数据缓存 缓存长度
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDlt645_2007::ProcessProtocolBuf ( BYTE *buf, int len )
{/*{{{*/
	print( "ProcessProtocolBuf" );
	int pos = 0;
	BOOL bRtn = TRUE;
	if( !WhetherBufValue( buf, len , pos ) )
	{
		//报文错误处理
		print ( "Dlt6456 WhetherBufValue buf Recv err!!!\n" );
		m_byRecvErrorCount ++;
		m_bIsReSend = TRUE;
		return FALSE;
	}

	bRtn = ProcessBuf( buf+pos, len );
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
}		/* -----  end of method CDlt645_2007::ProcessProtocolBuf  ----- *//*}}}*/


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  GetProtocolBuf
 * Description:  获取协议数据缓存
 *       Input:  缓存区 缓存区数据长度 总线消息
 *		Return:	 BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDlt645_2007::GetProtocolBuf ( BYTE *buf, int &len, PBUSMSG pBusMsg )
{/*{{{*/
	BOOL bRtn = TRUE;
	//重发机制
	if ( m_bLinkStatus && m_bIsReSend )
	{
		len = m_byReSendLen;
		memcpy( buf, m_byReSendBuf, len );
		m_byResendCount ++;
	}
	else if( m_bLinkStatus && pBusMsg != NULL )
	{
		print( "总线消息" );
		//dlt645目前未做任何消息命令 直接返回
		return FALSE;
	}
	else
	{
		print( "GetSendBuf" );
		ChangeSendPos(  );
		m_byDataType = m_CfgInfo[m_bySendPos].byDataType;
		if( IsTimeToSync() )
		{
			m_byDataType = DLT645_TIME_DATATYPE;
			m_bIsNeedResend = FALSE;
		}

		bRtn = GetSendBuf( buf, len );

		if ( bRtn && len > 0)
		{
			m_byReSendLen = len;
			memcpy( m_byReSendBuf, buf, m_byReSendLen );
			m_bIsSending = TRUE;
			if( !m_bIsNeedResend )
			{
				m_bIsSending = FALSE;
				m_bIsNeedResend = TRUE;
			}
		}
	}

	return bRtn;
}		/* -----  end of method CDlt645_2007::GetProtocolBuf  ----- *//*}}}*/

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Cjt188
 *      Method:  GetDevNameToAddr
 * Description:  通过装置的名字读取通讯地址
 *       Input:  void
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDlt645_2007::GetDevNameToAddr ( void )
{/*{{{*/
	int len = strlen( m_sDevName );
	if( len < 12)
	{
		return FALSE;
	}

	m_bySlaveAddr[0] = atoh( m_sDevName + len - 2, 2, 1 );
	m_bySlaveAddr[1] = atoh( m_sDevName + len - 4, 2, 1 );
	m_bySlaveAddr[2] = atoh( m_sDevName + len - 6, 2, 1 );
	m_bySlaveAddr[3] = atoh( m_sDevName + len - 8, 2, 1 );
	m_bySlaveAddr[4] = atoh( m_sDevName + len - 10, 2, 1 );
	m_bySlaveAddr[5] = atoh( m_sDevName + len - 12, 2, 1 );

	return TRUE;

}		/* -----  end of method CProtocol_Cjt188::GetDevNameToAddr  ----- *//*}}}*/

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  Init
 * Description:	 初始化协议数据
 *       Input:  总线号
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDlt645_2007::Init ( BYTE byLineNo )
{/*{{{*/
	if( byLineNo > 22 )
		return FALSE;

	// if( !GetDevNameToAddr(  ) )
	// {
	// 	print ( "CDlt645_2007:Addr Err!!!\n" );
	// 	return FALSE;
	// }

	if( !ReadCfgInfo() )
	{
		print ( "CDlt645_2007:ReadCfgInfo Err!!!\n" );
		return FALSE;
	}

	if( !InitProtocolStatus() )
	{
		print ( "CDlt645_2007:InitProtocolStatus Err\n" );
		return FALSE;
	}

	print( "Dlt645 Init OK" );
	return TRUE;
}		/* -----  end of method CDlt645_2007::Init  ----- *//*}}}*/


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDlt645_2007
 *      Method:  GetDevCommState
 * Description:	 设置装置链接状态
 *       Input:
 *		Return:	 BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDlt645_2007::GetDevCommState ( void )
{/*{{{*/
	if ( m_bLinkStatus )
		return COM_NORMAL;
	else
		return COM_DEV_ABNORMAL;
}		/* -----  end of method CDlt645_2007::GetDevCommState  ----- *//*}}}*/

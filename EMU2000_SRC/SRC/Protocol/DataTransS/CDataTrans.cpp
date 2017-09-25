/*
 * =====================================================================================
 *
 *       Filename:  CDataTrans.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年06月09日 18时28分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp 
 *   Organization:  
 *
 *		  history:	Time								Author			version			Description
 *					2015年06月09日 18时29分13秒         mengqp			1.0				created
 * =====================================================================================
 */

#include "CDataTrans.h"
#include <stdio.h>
/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  CDataTrans
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
CDataTrans::CDataTrans ()
{
	memset( m_fYcBuf, 0, sizeof( m_fYcBuf ) );
	memset( m_byYxBuf, 0, sizeof( m_byYxBuf ) );
	memset( m_dwYmBuf, 0, sizeof( m_dwYmBuf ) );

	//默认是15s
	m_wAllDataInterval = 15;
	m_LocalHeartbeatTime = 60 * 1000;

	//初始化基本状态参数
	InitProtocolState(  );
	printf ( "CDataTrans construtor\n" );


}  /* -----  end of method CDataTrans::CDataTrans  (constructor)  ----- */



/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  ~CDataTrans
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
CDataTrans::~CDataTrans ()
{
	printf ( "CDataTrans destrutor\n" );
	/* 析构变量 */	
}  /* -----  end of method CDataTrans::~CDataTrans  (destructor)  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  TimerProc
 * Description:  时间处理
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDataTrans::TimerProc ( void )
{
	/* 	更新变化数据 */
	ReadChangData();	
	
	/* 时间协议处理 */
	TimeToProtocol(  );

	/* 	协议超时或超次数处理 */
	ProtocolErrorProc(  );
}		/* -----  end of method CDataTrans::TimerProc  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  ProcessProtocolBuf
 * Description:  处理协议报文
 *       Input:	 pBuf缓冲区指针
 *				 len:长度
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::ProcessProtocolBuf ( BYTE *pBuf, int len )
{
	int pos =0;
	/* 判断报文合理性  并找出一帧合理报文 */	
	if( !WhetherBufValid( pBuf, len, pos ) )
	{
		print( ( char * )"CDataTrans can't find right recv buf" );
		SetState( DATATRANS_RESEND_STATE );
		return FALSE;
	}

	/* 处理报文 */
	if( !ProcessRecvBuf( &pBuf[pos], len ) )
	{
		return FALSE;
	}
	
	/* 接收状态切换 */
	SetRecvParam(  );
	return TRUE;
}		/* -----  end of method CDataTrans::ProcessProtocolBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  GetProtocolBuf
 * Description:  获取协议报文 
 *       Input:  buf:组织报文缓冲区
 *				 len:缓冲区长度 
 *				 pBusMsg:消息指针  在此协议中无用
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::GetProtocolBuf ( BYTE *buf,
		int &len,
		PBUSMSG pBusMsg)
{
	BOOL bRtn = FALSE;
/* 	判断输入参数有效性 */
	//判断buf
	if( NULL == buf  )
	{
		print( ( char * )" CDataTrans GetProtocolBuf buf = NULL" );
		return FALSE;	
	}
	memset( buf, 0, 256 );
	//不判断pBusMsg

/* 	获取发送报文 */
	bRtn = GetSendBuf( buf, len );

/* 	发送状态参数切换 */
	SetSendParam( bRtn );

	return bRtn;
}		/* -----  end of method CDataTrans::GetProtocolBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  Init
 * Description:  初始化协议  
 *       Input:  byLineNo:总线号
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::Init ( BYTE byLineNo )
{
	//读配置文件
	if( !ReadCfgInfo(  ) )
	{
		return FALSE;
	}
	
	//初始化数据
	InitProtocol(  );
	
	// CloseLink(  );
	UnsetState( DATATRANS_LINK_STATE );
	return TRUE;
}		/* -----  end of method CDataTrans::Init  ----- */


/* #####   time 时间部分   ################################################### */
/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  TimeToProtocol
 * Description:  时间协议处理
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDataTrans::TimeToProtocol ( void )
{
	//是否到时间发送全数据 若数据没有发完 不经过此处
	if( TimeToAll(  ) )
	{
		print( (char *)"CDataTrans timetoall" );
		DWORD dwAll = DATATRANS_YC_STATE | DATATRANS_YX_STATE | DATATRANS_YM_STATE ;
		SetState( dwAll );
		// OpenLink(  );
		SetState( DATATRANS_LINK_STATE );
	}

	if( TimeToHeartbeat(  ) )
	{
		print( (char *)"CDataTrans timetoheart" );
		SetState( DATATRANS_HEARTBEAT_STATE );
		// OpenLink(  );
		SetState( DATATRANS_LINK_STATE );
	}

}		/* -----  end of method CDataTrans::TimeToProtocol  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  TimeToAll
 * Description:  是否到时间发送全部数据 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::TimeToAll ( void )
{
	m_LocalAddTime += 200;
	if( m_LocalAddTime  >= m_LocalSumTime )
	{
		//如果还有其它操作 
		if( m_ProtocolState )
		{
			return FALSE;
		}

		//重置心跳
		m_LocalHeartbeatAddTime = 0;
		//重置时间
		m_LocalAddTime = 0;
		return TRUE;
	}

	return FALSE;
}		/* -----  end of method CDataTrans::TimeToAll  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  TimeToHeartbeat
 * Description:  
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::TimeToHeartbeat ( void )
{
	m_LocalHeartbeatAddTime += 200;
	if( m_LocalHeartbeatAddTime  >= m_LocalHeartbeatTime )
	{
		//如果还有其它操作 
		if( m_ProtocolState )
		{
			return FALSE;
		}

		//重置心跳
		m_LocalHeartbeatAddTime = 0;
		return TRUE;
	}

	return FALSE;
}		/* -----  end of method CDataTrans::TimeToHeartbeat  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  ProtocolErrorProc
 * Description:  协议错误处理 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDataTrans::ProtocolErrorProc ( void )
{
	m_byTimerCount ++;

	if( m_byTimerCount > 20 )
	{
		m_byTimerCount = 0;
		if( ! IsHaveState(DATATRANS_LINK_STATE) )
		print( "CDataTrans is runing please wait" );
	}

	if( m_bySendCount > DATATRANS_MAX_SEND_COUNT )
	{
		sprintf( m_szPrintBuf, "sendcount=%d > %d init protocol",m_bySendCount,  DATATRANS_MAX_SEND_COUNT );
		print(m_szPrintBuf  );
		InitProtocolState(  );
	}
}		/* -----  end of method CDataTrans::ProtocolErrorProc  ----- */

/* #####   recv 接收部分   ################################################### */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  ProcessRecvBuf
 * Description:  处理接收报文 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::ProcessRecvBuf ( BYTE *pBuf, int len )
{
	if( !m_bSending )
	{
		return FALSE;
	}

	switch ( pBuf[1] )
	{
		case 0xF1:	
			if( !IsHaveState( DATATRANS_YC_STATE ) )
			{
				return FALSE;	
			}
			if( IsHaveState( DATATRANS_YC_OVER_STATE ) )
			{
				UnsetState( DATATRANS_YC_OVER_STATE );
				UnsetState( DATATRANS_YC_STATE );
			}
			print( (char *)"CDataTrans pocess recv yc" );
			break;

		case 0xF3:	
			if( !IsHaveState( DATATRANS_YX_STATE ) 
					&& !IsHaveState( DATATRANS_CHANGE_YX_STATE ) )
			{
				return FALSE;	
			}

			if( IsHaveState( DATATRANS_CHANGE_YX_STATE ) )
			{
				print( (char *)"CDataTrans pocess recv changeyx" );
				UnsetState( DATATRANS_CHANGE_YX_STATE );
			}

			if( IsHaveState( DATATRANS_YX_OVER_STATE ) )
			{
				UnsetState( DATATRANS_YX_OVER_STATE );
				UnsetState( DATATRANS_YX_STATE );
			}
			print( (char *)"CDataTrans pocess recv yx" );
			break;

		case 0xF5:	
			if( !IsHaveState( DATATRANS_YM_STATE ) )
			{
				return FALSE;	
			}
			if( IsHaveState( DATATRANS_YM_OVER_STATE ) )
			{
				UnsetState( DATATRANS_YM_OVER_STATE );
				UnsetState( DATATRANS_YM_STATE );
			}
			print( (char *)"CDataTrans pocess recv ym" );
			break;

		case 0xF7:	
			if( !IsHaveHeart(  ) )
			{
				return FALSE;
			}

			UnsetState( DATATRANS_HEARTBEAT_STATE );
			print( (char *)"CDataTrans pocess recv heart" );

			break;


		default:	
			return FALSE;
			break;
	}				/* -----  end switch  ----- */

	return TRUE;
}		/* -----  end of method CDataTrans::ProcessRecvBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  SetRecvParam
 * Description:  设置接收参数 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDataTrans::SetRecvParam ( void  )
{
	m_bSending = FALSE;
	m_bySendCount = 0;
	UnsetState( DATATRANS_RESEND_STATE );
}		/* -----  end of method CDataTrans::SetRecvParam  ----- */


/* #####   send 发送部分   ################################################### */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  GetSendBuf
 * Description:  获取发送报文 
 *       Input:  buf 缓冲区
 *				 len 长度
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::GetSendBuf ( BYTE *buf, int &len )
{
	BOOL bRtn = TRUE;
	//获取发送状态
	if( !GetProtocolState(  ) )	
	{
		return FALSE;
	}
	
	//根据格式组织相应发送报文
	bRtn = GetSendTypeBuf( buf, len );
	
	//储存重发报文
	SaveResendBuf( buf, len, bRtn );

	return bRtn;
}		/* -----  end of method CDataTrans::GetSendBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  GetProtocolState
 * Description:  根椐条件得到协议状态
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::GetProtocolState ( void )
{
	//重发格式判断
	if( IsResend(  ) )
	{
		// print( (char *)"CDataTrans resend" );
		SetState( DATATRANS_RESEND_STATE );	
		// OpenLink(  );
		SetState( DATATRANS_LINK_STATE );
		return TRUE;
	}

	//变化Yx数据
	if( IsHaveChangeYX(  ) )
	{
		print( (char *)"CDataTrans changeyx" );
		SetState( DATATRANS_CHANGE_YX_STATE );
		// OpenLink(  );
		SetState( DATATRANS_LINK_STATE );
		return TRUE;
	}
	
	//全数据
	if( IsHaveAll(  ) )
	{
		print( (char *)"CDataTrans alldata" );
		// OpenLink(  );
		SetState( DATATRANS_LINK_STATE );
		return TRUE;
	}

	//心跳
	if( IsHaveHeart(  ) )
	{
		print( (char *)"CDataTrans heart" );
		// OpenLink(  );
		SetState( DATATRANS_LINK_STATE );
		return TRUE;
	}

	// CloseLink(  );
	UnsetState( DATATRANS_LINK_STATE );
	return FALSE;
}		/* -----  end of method CDataTrans::GetProtocolState  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  IsResend
 * Description:  是否需要重发 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::IsResend ( void ) const
{
	return IsHaveState( DATATRANS_RESEND_STATE ); 
}		/* -----  end of method CDataTrans::IsResend  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  IsHaveChangeYX
 * Description:  是否有变化YX 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::IsHaveChangeYX ( void  ) const
{
	if ( m_dwDIEQueue.size( ) > 0 )
	{
		return TRUE;
	}

	return FALSE;
}		/*  -----  end of method CDataTrans::IsHaveChangeYXData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  IsHaveAll
 * Description:  是否有全部数据上送 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::IsHaveAll ( void  ) const
{
	DWORD dwAll = DATATRANS_YC_STATE | DATATRANS_YX_STATE | DATATRANS_YM_STATE 
		| DATATRANS_YC_OVER_STATE | DATATRANS_YX_OVER_STATE | DATATRANS_YM_OVER_STATE;
	if( dwAll & m_ProtocolState )
	{
		return TRUE;
	}

	return FALSE;
}		/*  -----  end of method CDataTrans::IsHaveAll  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  IsHaveHeart
 * Description:  
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::IsHaveHeart ( void ) const
{
	return IsHaveState( DATATRANS_HEARTBEAT_STATE );
}		/* -----  end of method CDataTrans::IsHaveHeart  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  GetSendTypeBuf
 * Description:  获取相应类型的数据
 *       Input:  buf 缓冲区
 *				 len 长度
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::GetSendTypeBuf ( BYTE *buf, int &len)
{
	/* 获取重发数据 */
	if( IsHaveState( DATATRANS_RESEND_STATE ) )
	{
		GetResendBuf( buf, len );	
		return TRUE;
	}

	/* 获取变化遥信数据 */
	if( IsHaveState( DATATRANS_CHANGE_YX_STATE ) )
	{
		return GetChangeYXBuf( buf, len  );
	}

	/* 获取全部数据 */
	if( GetAllDataBuf( buf, len ) )
	{
		return TRUE;
	}

	/*  获取心跳数据*/
	if( IsHaveHeart(  ) )
	{
		return GetHeartBuf( buf, len );
	}

	return FALSE;
}		/* -----  end of method CDataTrans::GetSendTypeBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  GetChangeYXBuf
 * Description:  获取变化YX数据 
 *       Input:  buf 缓冲区
 *				 len 长度
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::GetChangeYXBuf ( BYTE *buf, int &len )
{
	/* 组织变化YX报文 */
	BOOL bRtn =  PackChangeYXBuf( buf, len );
	//状态设置
	// UnsetState( DATATRANS_CHANGE_YX_STATE );
	print( (char *)"CDataTrans get changeyx" );

	return bRtn;
}		/* -----  end of method CDataTrans::GetChangeYXBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  PackChangeYXBuf
 * Description:  组织YX报文 
 *       Input:  buf 缓冲区
 *				 len 长度
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::PackChangeYXBuf ( BYTE *buf, int &len )
{
	WORD wSerialNo;
	WORD wPnt;
	WORD wVal;
	BOOL bDevState;
	BYTE byVal;

	//获取变化遥信的序号、点号和值
	if( !GetDigitalEvt( wSerialNo, wPnt, wVal ) )
	{
		return FALSE;
	}
	//获取装置状态
	bDevState = m_pMethod->GetDevCommState( wSerialNo );	
	//bDevState:0正常 1不正常 wVal:0分 1合 10 11无效 00 01有效
	byVal = ( bDevState << 1 ) | ( wVal & 0x01 );

    len = 0;	
	//报文头
	buf[len++] = 0x68;
	//地址
	buf[len++] = HIBYTE( m_wDevAddr );
	buf[len++] = LOBYTE( m_wDevAddr );
	//功能码
	buf[len++] = 0xF2;
	//起始点号
	buf[len++] = HIBYTE( wPnt );
	buf[len++] = LOBYTE( wPnt );
	//数量
	buf[len++] = 0x01;
	//值
	buf[len++] = byVal;

	return TRUE;
}		/* -----  end of method CDataTrans::PackChangeYXBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  GetAllDataBuf
 * Description:  获取全部数据
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::GetAllDataBuf ( BYTE *buf, int &len )
{
	/* YC数据 */
	if( IsHaveState( DATATRANS_YC_STATE ) )
	{
		print( (char *)"CDataTrans get yc" );
		return GetYCDataBuf( buf, len );
	}

	/* YX数据 */
	if( IsHaveState( DATATRANS_YX_STATE ) )
	{
		print( (char *)"CDataTrans get yx" );
		return GetYXDataBuf( buf, len );
	}

	/* YM数据 */
	if ( IsHaveState( DATATRANS_YM_STATE ) )
	{
		print( (char *)"CDataTrans get ym" );
		return GetYMDataBuf( buf, len );
	}

	return FALSE;
}		/* -----  end of method CDataTrans::GetAllDataBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  GetYcDataBuf
 * Description:  获取yc数据包 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::GetYCDataBuf ( BYTE *buf, int &len )
{
	/* 组织YC报文 */
	BOOL bRtn = PackYCBuf( buf, len );
	//状态设置
	if( m_wAllDataPos >= m_wAISum )
	{
		// UnsetState( DATATRANS_YC_STATE );
		SetState ( DATATRANS_YC_OVER_STATE );
		m_wAllDataPos = 0;
	}
	return bRtn;
}		/* -----  end of method CDataTrans::GetYcDataBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  PackYCBuf
 * Description:  组织YX报文 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::PackYCBuf ( BYTE *buf, int &len )
{
	BYTE byCount = 0;
    len = 0;	
	//报文头
	buf[len++] = 0x68;
	//地址
	buf[len++] = HIBYTE( m_wDevAddr );
	buf[len++] = LOBYTE( m_wDevAddr );
	//功能码
	buf[len++] = 0xF0;
	//起始点号
	buf[len++] = HIBYTE( m_wAllDataPos );
	buf[len++] = LOBYTE( m_wAllDataPos );
	//数量
	buf[len++] = 0x00;

	for( int i=m_wAllDataPos; i<m_wAISum; i++ )
	{
		float fVal = m_fYcBuf[i];
		BYTE szTmp[4];
		//获取装置状态， 如果装置不通， 则设为无效数据非当前值
		WORD wSerialNo = GetSerialNoFromTrans( YC_TRANSTOSERIALNO , i ) ;
		BOOL bDevState = m_pMethod->GetDevCommState( wSerialNo ) ;
		//有效性
		buf[len++] = (BYTE)bDevState;

		//值
		memcpy( szTmp, &fVal, 4 );
		buf[len++] = szTmp[3];
		buf[len++] = szTmp[2];
		buf[len++] = szTmp[1];
		buf[len++] = szTmp[0];

		// char szBuf[256];
		// sprintf( szBuf, "yc wSerialNo:%d wPnt:%d, State:%d, val=%f", wSerialNo, i, bDevState, fVal );
		// print( szBuf );
		
		//最大范围50
		byCount ++;
		if( byCount >= 45 )
		{
			break;
		}
	}

	//数量
	buf[6] = byCount;
	m_wAllDataPos += byCount;

	return TRUE;
}		/* -----  end of method CDataTrans::PackYCBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  GetYcDataBuf
 * Description:  获取yc数据包 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::GetYXDataBuf ( BYTE *buf, int &len )
{
	/* 组织YX报文 */
	BOOL bRtn = PackYXBuf( buf, len );
	//状态设置
	if( m_wAllDataPos >= m_wDISum )
	{
		// UnsetState( DATATRANS_YX_STATE );
		SetState ( DATATRANS_YX_OVER_STATE );
		m_wAllDataPos = 0;
	}
	return bRtn;
}		/* -----  end of method CDataTrans::GetYcDataBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  PackYXBuf
 * Description:  组织YX报文 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::PackYXBuf ( BYTE *buf, int &len )
{
	BYTE byCount = 0;
    len = 0;	
	//报文头
	buf[len++] = 0x68;
	//地址
	buf[len++] = HIBYTE( m_wDevAddr );
	buf[len++] = LOBYTE( m_wDevAddr );
	//功能码
	buf[len++] = 0xF2;
	//起始点号
	buf[len++] = HIBYTE( m_wAllDataPos );
	buf[len++] = LOBYTE( m_wAllDataPos );
	//数量
	buf[len++] = 0x00;

	for( int i=m_wAllDataPos; i<m_wDISum; i += 4 )
	{

		for ( int j=0; j<4; j++)
		{
			if( i + j >= m_wDISum )
			{
				break;
			}
			WORD wVal = m_byYxBuf[i+j];
			//获取装置状态， 如果装置不通， 则设为无效数据非当前值
			WORD wSerialNo = GetSerialNoFromTrans( YX_TRANSTOSERIALNO , i+j ) ;
			BOOL bDevState = m_pMethod->GetDevCommState( wSerialNo ) ;

			//值
			BYTE byVal = ( bDevState << 1 ) | ( wVal & 0x01 );
			buf[len]  = ( buf[len] | ( byVal << ( 2 * j ) ) ) ;

			// char szBuf[256];
			// sprintf( szBuf, "yx wSerialNo:%d wPnt:%d, State:%d, val=%d buflen=%x len=%d", wSerialNo, i +j, bDevState, byVal, buf[len], len );
			// print( szBuf );
			
		}

		len ++;	
		//最大范围240
		byCount += 4;
		if( byCount >= 240 )
		{
			break;
		}
	}

	//数量
	buf[6] = byCount;
	m_wAllDataPos += byCount;
	return TRUE;
}		/* -----  end of method CDataTrans::PackYXBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  GetYMDataBuf
 * Description:  获取yc数据包 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::GetYMDataBuf ( BYTE *buf, int &len )
{
	/* 组织YM报文 */
	BOOL bRtn = PackYMBuf( buf, len );
	//状态设置
	if( m_wAllDataPos >= m_wPISum )
	{
		// UnsetState( DATATRANS_YM_STATE );
		SetState ( DATATRANS_YM_OVER_STATE );
		m_wAllDataPos = 0;
	}
	return bRtn;
}		/* -----  end of method CDataTrans::GetYMDataBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  PackYMBuf
 * Description:  组织YX报文 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::PackYMBuf ( BYTE *buf, int &len )
{
	BYTE byCount = 0;
    len = 0;	

	m_pMethod->ReadAllYmData(&m_dwYmBuf[0]);
	//报文头
	buf[len++] = 0x68;
	//地址
	buf[len++] = HIBYTE( m_wDevAddr );
	buf[len++] = LOBYTE( m_wDevAddr );
	//功能码
	buf[len++] = 0xF4;
	//起始点号
	buf[len++] = HIBYTE( m_wAllDataPos );
	buf[len++] = LOBYTE( m_wAllDataPos );
	//数量
	buf[len++] = 0x00;

	for( int i=m_wAllDataPos; i<m_wPISum; i++ )
	{
		char szTmp[4];
		DWORD dwVal = m_dwYmBuf[i];
		//获取装置状态， 如果装置不通， 则设为无效数据非当前值
		WORD wSerialNo = GetSerialNoFromTrans( DD_TRANSTOSERIALNO , i ) ;
		BOOL bDevState = m_pMethod->GetDevCommState( wSerialNo ) ;
		//有效性
		buf[len++] = (BYTE)bDevState;
		float fVal = (float)dwVal;

		memcpy( szTmp, &fVal, 4 );
		buf[len++] = szTmp[3];
		buf[len++] = szTmp[2];
		buf[len++] = szTmp[1];
		buf[len++] = szTmp[0];

		//最大范围50
		byCount ++;
		if( byCount >= 45 )
		{
			break;
		}
	}

	//数量
	buf[6] = byCount;
	m_wAllDataPos += byCount;
	return TRUE;
}		/* -----  end of method CDataTrans::PackYMBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  GetHeartBuf
 * Description:  
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::GetHeartBuf ( BYTE *buf, int &len )
{
	len = 0;
	buf[len++] = 0x68;
	buf[len++] = 0xF6;

	return TRUE;
}		/* -----  end of method CDataTrans::GetHeartBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  GetResendBuf
 * Description:  获取重发数据 
 *       Input:  buf:缓冲区
 *				 len:长度
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDataTrans::GetResendBuf ( BYTE *buf, int &len )
{
	len = m_iResendLen;	
	memcpy( buf, m_byResendBuf, len );

	m_byResendCount ++;
}		/* -----  end of method CDataTrans::GetResendBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  SaveResendBuf
 * Description:  保存重发数据 
 *       Input:  buf:缓冲区
 *				 len:长度
 *				 byValid:是否保存
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDataTrans::SaveResendBuf ( BYTE *buf, int len, BOOL byValid )
{
	if( byValid )
	{
		m_iResendLen = len;	
		memcpy( m_byResendBuf, buf, m_iResendLen );
	}
}		/* -----  end of method CDataTrans::SaveResendBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  SetSendParam
 * Description:  设置发送参数
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDataTrans::SetSendParam ( BOOL bIsSendValid )
{
	if( bIsSendValid  )
	{
		m_bSending = TRUE;
		m_bySendCount ++;
		SetState( DATATRANS_RESEND_STATE );
	}
}		/* -----  end of method CDataTrans::SetSendParam  ----- */

/* #####   Init 初始化部分   ################################################### */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  ReadCfgInfo
 * Description:  读取配置模板信息
 *       Input:  void
 *		Return:  
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::ReadCfgInfo ( void )
{
	char szPath[256] = "";
	sprintf( szPath, "%s%s" ,DATATRANSSPREFIXFILENAME, m_sTemplatePath );
	print( szPath );
	
	//读配置模板的点表信息
    ReadCfgMapInfo ( szPath ); 

	//读配置模板的特殊配置信息
	//sprintf( szPath, "%sBus%.2dOtherCfg.txt",DATATRANSSPREFIXFILENAME,  m_byLineNo+1  );
	if( !ReadCfgOtherInfo( szPath ) )
	{
		//return FALSE;
	}
	
	print( (char *)"CDataTrans ReadCfgInfo OK" );
	return TRUE;
}		/* -----  end of method CDataTrans::ReadCfgInfo  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  ReadCfgMapInfo
 * Description:  读取点表信息 
 *       Input:  szPath 模板路径
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDataTrans::ReadCfgMapInfo ( char *szPath )
{
	//调用Rtu.cpp读取
	ReadMapConfig( szPath );
}		/* -----  end of method CDataTrans::ReadCfgMapInfo  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  ReadCfgOtherInfo
 * Description:  读配置模板的特殊配置信息
 *       Input:  szPath 模板路径
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDataTrans::ReadCfgOtherInfo ( char *szPath )
{
	FILE *fp = NULL;
	char szLineBuf[256];
	int iLineNum = 0;
	int iOtherInfoNum = 0;
	int iOtherInfoAllNum = 2;

	fp = fopen( szPath , "r");
	if ( NULL == fp )
	{
		printf ( "open file %s err!!!\n", szPath );
		return FALSE;
	}

	while ( NULL != fgets(szLineBuf, sizeof(szLineBuf), fp) 
			&& 30 > iLineNum )
	{
		iLineNum ++; 
		if( 0 == strncmp ( szLineBuf, "SENDINTERVAL=", 13 ) )
		{
			WORD wCfgVal = (WORD)( atoi( &szLineBuf[13] ) );
			if( wCfgVal >= 1 )
			{
				m_wAllDataInterval = wCfgVal;		
			}
			else
			{
				printf ( "CDataTrans SENDINTERVAL=%d error!!! default is used\n", wCfgVal );
				m_wAllDataInterval = 15;
			}

			m_LocalSumTime = m_wAllDataInterval *1000 ;
			iOtherInfoNum ++;
		}
		if( 0 ==strncmp( szLineBuf, "HEARTTIME=", 10 ) )
		{
			WORD wCfgVal = (WORD)( atoi( &szLineBuf[10] ) );
			if( wCfgVal >= 1 )
			{
				m_LocalHeartbeatTime = wCfgVal * 1000;		
			}
			else
			{
				printf ( "CDataTrans HEARTTIME=%d error!!! default is used\n", wCfgVal );
				m_LocalHeartbeatTime = 60 * 1000;
			}

			iOtherInfoNum ++;
		}

		if( iOtherInfoAllNum <= iOtherInfoNum )
		{
			printf ( "CDataTrans alldata interval=%lums, heat interval=%lums\n", m_LocalSumTime, m_LocalHeartbeatTime );
			break;
		}
	}

	fclose( fp );
	return TRUE;
}		/* -----  end of method CDataTrans::ReadCfgOtherInfo  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  InitProtocol
 * Description:  初始化协议状态
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDataTrans::InitProtocol ( void )
{
	//初始化基本状态参数
	InitProtocolState(  );

	//初始化点表信息
	InitProtocolTransTab(  ); 

	//初始化协议数据
	InitProtocolData(  ); 

	print( (char *)"CDataTrans InitProtocol OK" );
}		/* -----  end of method CDataTrans::InitProtocol  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  InitProtocolState
 * Description:  初始化协议参数
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDataTrans::InitProtocolState ( void )
{
	/* 初始化信息 */
	//初始化协议状态
	if( IsHaveState( DATATRANS_LINK_STATE ) )
	{
		// CloseLink(  );
		UnsetState( DATATRANS_LINK_STATE );
	}
	m_ProtocolState = 0;
	//初始化重发状态
	m_byResendCount = 0;
	m_iResendLen = 0;
	memset( m_byResendBuf, 0, sizeof( m_byResendBuf ) );
	//全数据位置
	m_wAllDataPos = 0;
	//状态相关
	m_bSending=FALSE;;
	m_bySendCount = 0;

	//时间相关
	m_LocalAddTime = 0;
	m_LocalHeartbeatAddTime = 0;
	m_byTimerCount = 0;



	print( (char *)"CDataTrans InitProtocolState" );

}		/* -----  end of method CDataTrans::InitProtocolState  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  InitProtocolTransTab
 * Description:  初始化转发信息
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDataTrans::InitProtocolTransTab ( void )
{
	/* 获取点表信息的转发序号 */
    CreateTransTab();
}		/* -----  end of method CDataTrans::InitProtocolTransTab  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDataTrans
 *      Method:  InitProtocolData
 * Description:  初始化协议数据
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDataTrans::InitProtocolData ( void )
{
	/* 从内存数据库中--获取转发表默认数据 */
	m_pMethod->ReadAllYcData(&m_fYcBuf[0]);
	m_pMethod->ReadAllYmData(&m_dwYmBuf[0]);
	m_pMethod->ReadAllYxData( &m_byYxBuf[ 0 ] ) ;
}		/* -----  end of method CDataTrans::InitProtocolData  ----- */

/* #####   other 其它部分   ################################################### */
int CDataTrans::GetRealVal(BYTE byType, WORD wPnt, void *v)
{
    WORD  wValue = 0 ;
    switch(byType)
    {
    case 0:
        if(wPnt>=DATATRANS_MAX_YC_NUM) return -2;
        memcpy(v, &m_fYcBuf[wPnt], sizeof(WORD));
        break;
    case 1:
        {
			if(wPnt>=DATATRANS_MAX_YX_NUM)
				return -2;

			if( m_byYxBuf[ wPnt ] ==0 )
				wValue = 0;
			else
				wValue = 1;

			memcpy(v, &wValue, sizeof(WORD));
		}
        break;
    case 2:
        if(wPnt>=DATATRANS_MAX_YM_NUM) return -2;
        memcpy(v, &m_dwYmBuf[wPnt], sizeof(DWORD));
        break;
    default:
        return -1;
    }
    return 0;
}

BOOL CDataTrans::WriteAIVal(WORD wSerialNo ,WORD wPnt, float fVal)
{
    if(m_pwAITrans==NULL) return FALSE;
    WORD wNum = m_pwAITrans[wPnt];
    if(wNum>m_wAISum) return FALSE;
    if(wNum<DATATRANS_MAX_YC_NUM)//mengqp 将<=改为< 否则m_wAIBuf[4096]越界
    {
        float fDelt = fVal - m_fYcBuf[wNum];
        if(abs((int)fDelt)>=m_wDeadVal)
        {
            m_fYcBuf[wNum] = fVal;
			// if(m_bDataInit)
			// {
                AddAnalogEvt( wSerialNo , wNum, fVal);
			// }
        }
    }
    return TRUE ;
}

BOOL CDataTrans::WriteDIVal(WORD wSerialNo ,WORD wPnt, WORD wVal)
{
    if(m_pwDITrans==NULL) return FALSE;
    WORD wNum = m_pwDITrans[wPnt] & 0x7fff;
    if(wNum>m_wDISum) return FALSE;
    if( wNum<DATATRANS_MAX_YX_NUM)//mengqp 将<= 改为<
    {
        if( m_byYxBuf[ wNum ] != wVal )
        {
            m_byYxBuf[ wNum ] = wVal ;
            // if(m_bDataInit)
			// {
                AddDigitalEvt( wSerialNo , wNum, wVal);
			// }
        }
    }
    return TRUE ;
}
BOOL CDataTrans::WritePIVal(WORD wSerialNo ,WORD wPnt, DWORD dwVal)
{
    if(m_pwPITrans==NULL) return FALSE;
    WORD wNum = m_pwPITrans[wPnt];
    if(wNum>m_wPISum) return FALSE;
    if(wNum<DATATRANS_MAX_YM_NUM)//mengqp 将<= 改为<
    {
        m_dwYmBuf[wNum] = dwVal;
    }
    return TRUE ;
}

BOOL CDataTrans::WriteSOEInfo( WORD wSerialNo ,WORD wPnt, WORD wVal, LONG lTime, WORD wMiSecond)
{
    if(m_pwDITrans==NULL) return FALSE;
    WORD wNum = m_pwDITrans[wPnt] & 0x7fff;
    if(wNum>=m_wDISum) return FALSE;
    if(wNum<DATATRANS_MAX_YX_NUM)
    {
        AddSOEInfo(wSerialNo , wNum, wVal, lTime, wMiSecond);
    }
    return TRUE ;
}
/* ====================  OtherEnd    ======================================= */


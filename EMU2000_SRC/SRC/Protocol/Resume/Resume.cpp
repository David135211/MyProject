#include "Resume.h"


const int ERROR_CONST = 5;
const int COMSTATUS_ONLINE = 1;
const int COMSTATUS_FAULT = 0;

const int ERROR_FUN = 1;
const int ERROR_REGISTER = 2;
const int ERROR_DATA = 3;
const int ERROR_CONFIG = 4;

Resume::Resume()
{/*{{{*/
	SendFlag = 0;
	MsgFlag = 0;
	FunNum = 0;
	m_wErrorTimer = 0;
	m_byPortStatus = 0;
	ErrorFlag = 0;
	yk_recv_flag = FALSE;
	time_flag = 0;
	Yk_FunNum = 0x05;
	sendtimeflag = 0;
	recvtimeflag = 0;
	memset(yk_bufSerial,0,sizeof(yk_bufSerial));
	memset(mt_sMasterAddr, 0, sizeof(mt_sMasterAddr));
}/*}}}*/

Resume::~Resume()
{/*{{{*/
	
}/*}}}*/

BOOL Resume::GetProtocolBuf( BYTE * buf , int &len , PBUSMSG pBusMsg ) 
{/*{{{*/
	
	if( buf == NULL )
		return FALSE ;
	
	m_wErrorTimer++;
	if( m_wErrorTimer > ERROR_CONST )
	{
		m_wErrorTimer = ERROR_CONST + 1 ;
		m_byPortStatus = COMSTATUS_FAULT;
	}
	else
	{
		m_byPortStatus = COMSTATUS_ONLINE;
	}
#if 0	
	if( pBusMsg )
	{
		DealBusMsgInfo( pBusMsg);
			//return FALSE;
	}
	
	if( MsgFlag == 2 )
	{
		return FALSE;
	}
	MsgFlag = 2;
	memcpy( buf , MsgBuf , MsgLen );
	len = MsgLen;
	memset( MsgBuf , 0 , MsgLen);
	MsgLen = 0;
	yk_recv_flag = FALSE;
#endif
	switch(type){
		case 0:
			YXPacket(buf, len);
			break;
		case 1:
			YcYmPacket(buf, len);
	}
	sendtimeflag = time(NULL);
	return TRUE;

}/*}}}*/

BOOL Resume::ProcessProtocolBuf( BYTE * buf , int len ) 
{/*{{{*/
#if 0
//printf("0\n");
	BOOL Rtn = FALSE;
	if( len == 0 || buf == NULL )
		return FALSE ;

	if(yk_recv_flag){
		time_t t;
		t = time(NULL);
		if((t - time_flag) < 5)
			return FALSE;
	}
	
	if(buf[7] == 0x05){
		yk_recv_flag = TRUE;
		time_t t;
		t = time(NULL);
		time_flag = t;
	}
//	printf("m_wRtuAddr== %d\n",m_wRtuAddr);
	if( buf[2]==0 && buf[3]==0 && buf[4]==0 
		&& (buf[5]+6)==len && buf[6]== m_wRtuAddr )//&& len == 12 )
	{//printf("1\n");
		FunNum = buf[7];
		MsgFlag = 0;
		switch( FunNum )
		{
			case 2:
			Rtn = YXPacket( buf , len );
			break;
			case 3:
			Rtn = YcYmPacket( buf , len );
			break;
			case 4:
			Rtn = YcYmPacket( buf , len );
			break;
			case 5:
			Rtn = YKMsg( buf , len );
			break;
			default:
			ErrorPacket( buf , ERROR_FUN ); 
			break;
		}
		if( Rtn == TRUE )
		{
			m_wErrorTimer = 0;
		}
		return TRUE ;
	}
#endif
	if((buf[0] == 0x68) && (buf[1] == 0xF1) && (len == 2)){
		recvtimeflag = time(NULL);
		return TRUE;
	}
	return FALSE ;
}/*}}}*/

BOOL Resume::Init( BYTE byLineNo )
{/*{{{*/
	m_byLineNo = byLineNo ;
	m_byProID    = 4;
    m_byEnable   = 1;
	m_wObjNum  = 1;
	sprintf( m_szObjName, "%s", m_sDevName );
	sprintf( m_ComCtrl1, "%s", mt_sMasterAddr );
	m_wRtuAddr = m_wDevAddr ;
	char szFileName[256] = "";

	sprintf( szFileName, "%s%s", MODBUSTCPPREFIXFILENAME, m_sTemplatePath );
	//读取需要转发的数据到该模块
    ReadMapConfig( szFileName );

	//初始化该模块
	InitRtuBase() ;

	//初始化上传装置状态时间
	//time( &m_DevStateTime ) ;
	return TRUE ;
}/*}}}*/

BOOL Resume::InitRtuBase( )
{/*{{{*/
    UINT uPort;
    BOOL bOk = FALSE;
    char szCtrl[32];

    //OutPromptText("****CRtu104.Init()****");

    CBasePort::GetCommAttrib(m_ComCtrl1, szCtrl, uPort);

    m_wPortNum = (WORD)uPort ;

	//获取转发序号
    CreateTransTab();

	//从内存数据库中--获取转发表默认数据
	m_pMethod->ReadAllYcData(&m_wYCBuf[0]);
	m_pMethod->ReadAllYmData(&m_dwYMBuf[0]);
	m_pMethod->ReadAllYxData( &m_byYXbuf[ 0 ] ) ;

	m_bTaskRun = TRUE;
    return bOk;
}/*}}}*/

BOOL Resume::DealBusMsgInfo( PBUSMSG pBusMsg )
{/*{{{*/
	int DeviceNo = 0;
	YK_DATA *pData = (YK_DATA *)(pBusMsg->pData);
//	WORD wSerialNo;
	switch ( pBusMsg->byMsgType )
	{
		case YK_PROTO:
			switch ( pBusMsg->dwDataType )
			{
				//case YK_SEL_RTN:
				case YK_EXCT_RTN:
				//case YK_CANCEL_RTN:
					if( pBusMsg->DataNum != 1
					|| pBusMsg->DataLen != sizeof(YK_DATA) )
					{
						printf("Resume Yk DataNum err\n");
						return -1;
					}

					DeviceNo = m_pMethod->GetSerialNo(pBusMsg->SrcInfo.byBusNo, pBusMsg->SrcInfo.wDevNo);
					if( DeviceNo == -1 )
						return -1 ;
					
						
					YKPacket( m_wRelayNum , pData->byVal );
					MsgFlag = 0;
					if( pData->byVal == YK_ERROR && DeviceNo > 10000 )
					{
						if( DeviceNo > 10000 )
							ErrorPacket( yk_bufSerial, ERROR_REGISTER ); 
						if( pData->byVal == YK_ERROR )
							ErrorPacket( yk_bufSerial, ERROR_DATA ); 
					}

					break;
				default:
					printf("Resume can't find the YK_DATATYPE %d\n", (int)pBusMsg->dwDataType);
					break;
			}				/* -----  end switch  ----- */
			break;

		default:
			break;
	}				/* -----  end switch  ----- */
	return TRUE;
}/*}}}*/

void Resume::RelayEchoProc(BYTE byCommand, WORD wIndex, BYTE byResult)
{/*{{{*/
	
}/*}}}*/

BOOL Resume::YXPacket( BYTE * buf , int len )
{/*{{{*/
	WORD index = 0;
	WORD mt_register = 256 * buf[8] + buf[9];
	WORD registernum = 256 * buf[10] + buf[11];
	
	WORD ModBusByteLen = ((registernum-1)/8)+1;
	WORD ResumeByteLen = 3+ModBusByteLen;
//printf("2\n");
	if( ( mt_register > 9999 || mt_register == 0 ) || ( registernum > 9999 || registernum == 0 ) )
	{
		if( mt_register > 9999 || mt_register == 0 ) 
			ErrorPacket( buf , ERROR_REGISTER ); 
		return FALSE;
	}
	MsgBuf[index++] = buf[0];
	MsgBuf[index++] = buf[1];
	MsgBuf[index++] = 0;
	MsgBuf[index++] = 0;
	MsgBuf[index++] = ResumeByteLen >> 8;
	MsgBuf[index++] = ResumeByteLen;		//Resume后续字节
	MsgBuf[index++] = m_wRtuAddr;
	MsgBuf[index++] = FunNum;
	MsgBuf[index++] = ModBusByteLen;		//ModBus后续字节
	int j = 0;
	for( j=0; j<ModBusByteLen; j++ )
	{//printf("3\n");
		//从共享内存中取数据
		BYTE i = 0;
		BYTE ByYXVal = 0;
		for( i=0; i<8; i++ )
		{//printf("4\n");
			if( j==ModBusByteLen-1 && i == registernum%8 && 0 != registernum%8)
				break;
				//printf("m_pDIMapTab[%d].wStn =%d\n",mt_register-1+(8*j+i),m_pDIMapTab[mt_register-1+(8*j+i)].wStn);
			if( m_pDIMapTab[mt_register-1+(8*j+i)].wStn > 0 && m_pDIMapTab[mt_register-1+(8*j+i)].wPntNum > 0 )
				ByYXVal |= m_byYXbuf[mt_register-1+(8*j+i)]<<i;
			else
			{
				ErrorPacket( buf , ERROR_REGISTER ); 
				return FALSE;
			}
		}
		MsgBuf[index++] = ByYXVal;
	}
	//printf("6\n");
	MsgLen = index;
	return TRUE;
}/*}}}*/

BOOL Resume::YcYmPacket( BYTE * buf , int len )
{/*{{{*/
	BOOL flag ;
	DWORD ymval ;
	WORD index = 0,wVal = 0;;
	float fVal = 0.0f ;
	WORD mt_register = 256 * buf[8] + buf[9];
	WORD registernum = 256 * buf[10] + buf[11];
	
	WORD ModBusByteLen = 2*registernum;
	WORD ResumeByteLen = 3+ModBusByteLen;
	//printf("mt_register = %d registernum = %d\n",mt_register,registernum);
	if( ( mt_register > 9999 || mt_register == 0 ) || ( registernum > 9999 || registernum == 0 )
		|| ( mt_register <= 6800 && mt_register+registernum > 6800 ) )
	{
		ErrorPacket( buf , ERROR_REGISTER ); 
		return FALSE;
	}
	MsgBuf[index++] = buf[0];
	MsgBuf[index++] = buf[1];
	MsgBuf[index++] = 0;
	MsgBuf[index++] = 0;
	MsgBuf[index++] = ResumeByteLen >> 8;
	MsgBuf[index++] = ResumeByteLen;		//Resume后续字节
	MsgBuf[index++] = m_wRtuAddr;
	MsgBuf[index++] = FunNum;
	MsgBuf[index++] = ModBusByteLen;		//ModBus后续字节
	int j = 0;
	for( j=0; j<registernum; j++ )
	{
		//从共享内存中取数据
		if( mt_register >= 1 && mt_register <= 6800 )
		{
			if( m_pAIMapTab[mt_register-1+j].wStn > 0 && m_pAIMapTab[mt_register-1+j].wPntNum > 0 )
			{
		        fVal = CalcAIRipeVal(m_pAIMapTab[mt_register-1+j].wStn, m_pAIMapTab[mt_register-1+j].wPntNum, m_wYCBuf[mt_register-1+j]);
				wVal = ( WORD )fVal ;
				MsgBuf[index++] = HIBYTE(wVal);
				MsgBuf[index++] = LOBYTE(wVal);
			}
			else
			{
				ErrorPacket( buf , ERROR_REGISTER ); 
				return FALSE;
			}
		}
		else if( mt_register >= 6801 && mt_register <= 9999 )
		{
			if( ( ( mt_register-6801 )%2 == 1 ) || ( registernum%2 == 1 ) )
			{
				ErrorPacket( buf , ERROR_REGISTER ); 
				return FALSE;
			}
			else
			{
				if( j%2 == 0 )
				{
					if( m_pPIMapTab[(mt_register-6801)/2+j/2].wStn > 0 && m_pPIMapTab[(mt_register-6801)/2+j/2].wPntNum > 0 )
					{
						ymval = GetPulseData ( m_pPIMapTab[(mt_register-6801)/2+j/2].wStn,
									m_pPIMapTab[(mt_register-6801)/2+j/2].wPntNum, &flag  );
						if( !flag )
						{
							ErrorPacket( buf , ERROR_DATA );
							return FALSE;
						}
						MsgBuf[index++] = HIBYTE(HIWORD(ymval));
						MsgBuf[index++] = LOBYTE(HIWORD(ymval));
						MsgBuf[index++] = HIBYTE(LOWORD(ymval));
						MsgBuf[index++] = LOBYTE(LOWORD(ymval));
					}
					else
					{
						ErrorPacket( buf , ERROR_REGISTER ); 
						return FALSE;
					}
				}
			}
		}
	}
	MsgLen = index;
	return TRUE;
}/*}}}*/

BOOL Resume::YKMsg( BYTE * buf , int len )
{/*{{{*/
	WORD mt_register = 256 * buf[8] + buf[9];
	
	if( ( mt_register > 10000 || mt_register == 0 ) || 
		!( ( buf[10] == 0xff || buf[10] == 0x00 ) && buf[11] == 0x00 ) )
	{
		if( mt_register > 10000 || mt_register == 0 )
			ErrorPacket( buf , ERROR_REGISTER ); 
		if( !( ( buf[10] == 0xff || buf[10] == 0x00 ) && buf[11] == 0x00 ) )
			ErrorPacket( buf , ERROR_DATA ); 
		return FALSE;
	}
	BYTE byBusNo;
	WORD byStn;
	BYTE byStatus;
	WORD wDevAddr;
	WORD wPnt;

	switch( buf[10] )
	{
		case 0xff:
		byStatus = 1;
		break;
		case 0x00:
		byStatus = 0;
		break;
	}

	byStn = m_pDOMapTab[mt_register-1].wStn - 1 ;
    wPnt  = m_pDOMapTab[mt_register-1].wPntNum - 1 ;
	if( m_pDOMapTab[mt_register-1].wStn > 0 && m_pDOMapTab[mt_register-1].wPntNum > 0 )
	{
		yk_bufSerial[0]=buf[0];
		yk_bufSerial[1]=buf[1];
		Yk_FunNum = buf[7];
		if(m_pMethod->GetBusLineAndAddr(byStn, byBusNo, wDevAddr))
		{	
			m_pMethod->SetYkExe(this, byBusNo, wDevAddr, wPnt, byStatus);
		}
		m_wRelayNum = mt_register;
		return TRUE;
	}
	else
	{
		ErrorPacket( buf , ERROR_REGISTER ); 
		return FALSE;
	}
}/*}}}*/

BOOL Resume::YKPacket( int yk_register , int val )
{/*{{{*/
	
	WORD index = 0;
	WORD ResumeByteLen = 6;

	if( yk_register > 10000 || yk_register == 0 )
	{
		ErrorPacket( yk_bufSerial , ERROR_REGISTER ); 
		return FALSE;
	}
	MsgBuf[index++] = yk_bufSerial[0];
	MsgBuf[index++] = yk_bufSerial[1];
	MsgBuf[index++] = 0;
	MsgBuf[index++] = 0;
	MsgBuf[index++] = ResumeByteLen >> 8;
	MsgBuf[index++] = ResumeByteLen;		//Resume后续字节
	MsgBuf[index++] = m_wRtuAddr;
//	MsgBuf[index++] = FunNum;
	MsgBuf[index++] = Yk_FunNum;
	
	MsgBuf[index++] = yk_register>>8;
	MsgBuf[index++] = yk_register;		//寄存器地址

	if( val==1 )
		MsgBuf[index++] = 0xff;		//	分
	else if( val==0 )
		MsgBuf[index++] = 0x00;
	else
	{
		ErrorPacket( yk_bufSerial , ERROR_DATA );
		return TRUE;
	}

	MsgBuf[index++] = 0x00;

	MsgLen = index;
	return TRUE;
}/*}}}*/

BOOL Resume::ErrorPacket(BYTE * buf,BYTE errorflag)
{/*{{{*/
	MsgFlag = 1;

	BYTE index = 0;

	MsgBuf[index++] = buf[0];
	MsgBuf[index++] = buf[1];
	MsgBuf[index++] = 0;
	MsgBuf[index++] = 0;
	MsgBuf[index++] = 0;
	MsgBuf[index++] = 3;		//Resume后续字节
	MsgBuf[index++] = m_wRtuAddr;
	MsgBuf[index++] = FunNum | 0x80;;
	MsgBuf[index++] = errorflag;
	
	MsgLen = index;
	return TRUE;
}/*}}}*/

BOOL Resume::WriteAIVal(WORD wSerialNo ,WORD wPnt, float wVal)
{/*{{{*/
    if(m_pwAITrans==NULL) return FALSE;
    WORD wNum = m_pwAITrans[wPnt];
    if(wNum>m_wAISum) return FALSE;
    if(wNum<MODBUSTCPMAX_AI_LEN)
    {
        float nDelt = wVal - m_wYCBuf[wNum];
        if(abs(nDelt)>=m_wDeadVal)
        {
            m_wYCBuf[wNum] = wVal;
            AddAnalogEvt( wSerialNo , wNum , wVal );
        }
    }
    return TRUE ;
}/*}}}*/

BOOL Resume::WriteDIVal(WORD wSerialNo ,WORD wPnt, WORD wVal)
{/*{{{*/
    if(m_pwDITrans==NULL) return FALSE;
    WORD wNum = m_pwDITrans[wPnt] & 0x7fff;
    if(wNum>m_wDISum) return FALSE;
    if( wNum<MODBUSTCPMAX_DI_LEN)
    {
        if( m_byYXbuf[ wNum ] != wVal )
        {
            m_byYXbuf[ wNum ] = wVal ;
            
            AddDigitalEvt( wSerialNo , wNum , wVal );
        }
    }
    return TRUE ;
}/*}}}*/

BOOL Resume::WritePIVal(WORD wSerialNo ,WORD wPnt, DWORD dwVal)
{/*{{{*/
    if(m_pwPITrans==NULL) return FALSE;
    WORD wNum = m_pwPITrans[wPnt];
    if(wNum>m_wPISum) return FALSE;
    if(wNum<MODBUSTCPMAX_PI_LEN)
    {
        m_dwYMBuf[wNum] = dwVal;
    }
    return TRUE ;
}/*}}}*/

BOOL Resume::WriteSOEInfo( WORD wSerialNo ,WORD wPnt, WORD wVal, LONG lTime, WORD wMiSecond)
{/*{{{*/
    if(m_pwDITrans==NULL) return FALSE;
    WORD wNum = m_pwDITrans[wPnt] & 0x7fff;
    if(wNum>=m_wDISum) return FALSE;
    if(wNum<MODBUSTCPMAX_DI_LEN)
    {
        AddSOEInfo(wSerialNo , wNum, wVal, lTime, wMiSecond);
    }
    return TRUE ;
}/*}}}*/

void Resume::TimerProc()
{/*{{{*/
	//从内存中读取变化遥信和遥测数据
	ReadChangData();

}/*}}}*/

BOOL Resume::GetDevCommState( )
{/*{{{*/
	if(m_byPortStatus == COMSTATUS_ONLINE)
	{
		return COM_DEV_NORMAL ;
    }
	else
	{
		return COM_DEV_ABNORMAL ;
	}	
}/*}}}*/


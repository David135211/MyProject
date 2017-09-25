#include "CProtocol_Cjt188.h"
#include "Cjt188_2004.h"

#define MODULE_CJT188_2004	1			/* 2004版 */

extern "C" void OutBusDebug(BYTE byBusNo, BYTE *buf, int len, int flag);


CProtocol_Cjt188::CProtocol_Cjt188()
{
    //ctor
	memset( m_sTemplatePath , 0 , sizeof( m_sTemplatePath ) ) ;
	memset( m_szPrintBuf, 0, sizeof( m_szPrintBuf ) );
	memset( m_bySlaveAddr, 0xAA, sizeof( m_bySlaveAddr ) );
	m_bySendPos = 0;
	m_byDataType = 0;
	m_byMeterType = CJT188_READDATA_DATATYPE;
	m_byFENum = 4;
	m_bySer = 0;
}

CProtocol_Cjt188::~CProtocol_Cjt188()
{
    //dtor
	int size = m_module.size() ;
	for(  int i = 0 ; i < size ; i++ )
	{
		delete m_module[ i ] ;
	}
	m_module.clear() ;
	m_CfgInfo.clear();
	printf( "Delete All CProtocol_Cjt188 OK . \n" );
}

BOOL CProtocol_Cjt188::GetProtocolBuf( BYTE * buf , int &len , PBUSMSG pBusMsg )
{
	return FALSE ;
}

BOOL CProtocol_Cjt188::ProcessProtocolBuf( BYTE * buf , int len )
{
	return FALSE ;
}

BOOL CProtocol_Cjt188::Init( BYTE byLineNo )
{
	//增加ModBus 采集模块数据
	//通过总线号获取读取的装置文件路径
	m_byLineNo = byLineNo ;
	//读取模板文件
	m_ProtoType = PROTOCO_GATHER ;

	return GetDevData( ) ;
}

BOOL CProtocol_Cjt188::GetDevData( )
{
	memset( m_sDevPath , 0 , sizeof( m_sDevPath ) ) ;
	sprintf( m_sDevPath , "%s/Cjt188/%s%02d.ini" , SYSDATAPATH , DEVNAME , m_byLineNo + 1 );
	CProfile profile( m_sDevPath ) ;

	return ProcessFileData( profile ) ;
}

BOOL CProtocol_Cjt188::ProcessFileData( CProfile &profile )
{
	if( !profile.IsValid() )
	{
		printf( "Open file %s Failed ! \n " , profile.m_szFileName );
		return FALSE ;
	}

	char sSect[ 200 ] = "DEVNUM" ;
	char sKey[ 20 ][ 100 ]={ "module" , "serialno" , "addr" , "name" , "template" } ;

	char sName[ 50 ] = { 0 };
	char stemplate[ 200 ] = { 0 };
	int iNum = 0 ;

	iNum = profile.GetProfileInt( sSect , (char *)"NUM"  , 0 ) ;
	if( iNum == 0 )
	{
		printf( "Get DEVNUM Failed ! \n " );
		return FALSE ;
	}

	for( int i = 0 ; i < iNum ; i++ )
	{
		WORD wModule = 0 ;
		int  serialno = 0 ;
		WORD addr = 0 ;
		BOOL bRtn = FALSE;

		sprintf( sSect , "%s%03d" ,  "DEV" , i + 1 );

		wModule = profile.GetProfileInt( sSect , sKey[ 0 ] , 0 ) ;
		serialno = profile.GetProfileInt( sSect , sKey[ 1 ] , 0 ) ;
		addr = profile.GetProfileInt( sSect , sKey[ 2 ] , 0 ) ;
		profile.GetProfileString( sSect , sKey[ 3 ] , (char *)"NULL"  , sName , sizeof( sName ) ) ;
		profile.GetProfileString( sSect , sKey[ 4 ] , (char *)"NULL" , stemplate , sizeof( stemplate ) ) ;

		//创建相应模块子类
		bRtn = CreateModule( wModule , serialno , addr , sName , stemplate ) ;
		if ( !bRtn )
		{
			printf ( "Create ModBus Module=%d serialno=%d addr=%d sName=%s stemplate=%s \
					Error \n", wModule, serialno, addr, sName, stemplate );
			return FALSE;
		}
	}

	return TRUE ;
}

BOOL CProtocol_Cjt188::CreateModule( int iModule , int iSerialNo , WORD iAddr , char * sName , char * stplatePath )
{
	CProtocol_Cjt188 * pProtocol = NULL ;
	// BOOL bRtn;

	switch ( iModule )
	{
	case MODULE_CJT188_2004:
		{
			pProtocol = new CCjt188_2004 ;
			pProtocol->m_byLineNo = m_byLineNo ;
			pProtocol->m_wModuleType = iModule ;
			pProtocol->m_wDevAddr = iAddr ;
			pProtocol->m_SerialNo = iSerialNo ;
			strcpy( pProtocol->m_sDevName , sName ) ;
			strcpy( pProtocol->m_sTemplatePath , stplatePath ) ;
			m_pMethod->m_pRtuObj = pProtocol;
			pProtocol->m_pMethod = m_pMethod ;
			pProtocol->m_ProtoType = PROTOCO_GATHER ;
			//初始化模板数据
			if( !pProtocol->Init( m_byLineNo ) ) 
			{
				printf( " Add bus = %d Addr = %d serialno = %d error!!!\n" , m_byLineNo , iAddr, iSerialNo ) ;
				return FALSE;
				
			}
			printf( " Add bus = %d Addr = %d serialno = %d\n" , m_byLineNo , iAddr, iSerialNo ) ;
		}
		break;

	default:
		{
			printf( "Cjt188 don't contain this module Failed .\n" );
			return FALSE ;
		}
	}
		m_module.push_back( pProtocol ) ;

	return TRUE ;
}

BYTE CProtocol_Cjt188::GetCs( const BYTE * pBuf , int len )
{
	BYTE byRtn = 0x00;
	int i;

	if( pBuf == NULL || len <= 0 )
		return byRtn;

	for ( i=0; i<len ; i++ )
	{
		byRtn += pBuf[i];
	}

	return byRtn;
}

BOOL CProtocol_Cjt188::BroadCast( BYTE * buf , int &len )
{
	//组织该模块的广播报文
	int index = 0 ;
	buf[ index++ ] = 0xFF ;
	buf[ index++ ] = 0x02 ;
	buf[ index++ ] = 0x03 ;
	buf[ index++ ] = 0x04 ;

	WORD wCRC = GetCs( buf, index );
    buf[ index++ ] = HIBYTE(wCRC);
    buf[ index++ ] = LOBYTE(wCRC);

	len = index ;

	printf( "\n CProtocol_Cjt188  TestBroadCast \n " ) ;
	return TRUE ;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Cjt188
 *      Method:  print
 * Description:  打印到终端 或者 总线方便调试 调试时使用
 *       Input:	 缓冲区 长度
 *		Return:	
 *--------------------------------------------------------------------------------------
 */
void CProtocol_Cjt188::print ( const char *szBuf, int len )
{
#ifdef  CJT188PRINT
	printf( "%s\n", szBuf );	
#endif     /* -----  not CJT188PRINT  ----- */

#ifdef  CJT188DEBUG
	OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
#endif     /* -----  not CJT188DEBUG  ----- */
}		/* -----  end of method CProtocol_Cjt188::print  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Cjt188
 *      Method:  WhetherBufValue
 * Description:  判断Cjt188报文的有效性 基本判断 并从缓存中获取一帧正确报文（如果有) 
 *       Input:  收到的缓存区buf 收到的数据长度len 缓存区有效位置pos 
 *		Return:  BOOL   
 *--------------------------------------------------------------------------------------
 */
BOOL CProtocol_Cjt188::WhetherBufValue (const BYTE *buf, int &len, int &pos )
{
	const BYTE *pointer = buf;
	int datalen;
	BYTE byCrc;
	pos = 0;

	if( buf == NULL || len <= 0 )
	{
		print( "buf==NULL or len <=0" );
		return FALSE;
	}
	
	while ( len > 0 )
	{
		switch(*pointer)
		{
			case 0x68:  //判断可变帧
				{
					//判断议表类型
					if( *(pointer+1) != m_byMeterType )
					{
						sprintf(m_szPrintBuf, 
								"Cjt188 recv metertype error datameter=%x meter=%x",
								*(pointer+1), m_byMeterType );
						OutBusDebug( m_byLineNo, (BYTE *)m_szPrintBuf, strlen(m_szPrintBuf), 2 );
						goto DEFAULT;
					}

					//判断控制位
					if( ( *( pointer + 9 ) & 0x80 ) == 0)
					{
						print( "Cjt188 recv 控制字最高位 不为1" );
						goto DEFAULT;
					}

					//判断数据长度合理性
					datalen=*(pointer+10);
					if( datalen+12>len || datalen > 0x64 )
					{
						sprintf(m_szPrintBuf, 
								"Cjt188 recv len error datalen=%d len=%d",
								datalen, len );
						OutBusDebug( m_byLineNo, (BYTE *)m_szPrintBuf, strlen(m_szPrintBuf), 2 );
						goto DEFAULT;
					}

					//判断校验位 及最后一个字节0x16
					byCrc=GetCs(pointer,datalen + 11);
					if(*(pointer+datalen+11)!=byCrc
							|| *(pointer+datalen+12)!=0x16)
					{
						sprintf(m_szPrintBuf, 
								"Cjt188 recv cs error GetCrc=%d crc=%d or last byte != 0x16 =%x",
								byCrc, *(pointer+datalen+10), *(pointer+datalen+11));
						OutBusDebug( m_byLineNo, (BYTE *)m_szPrintBuf, strlen(m_szPrintBuf), 2 );
						goto DEFAULT;
					}

					//判断地址
					for ( int k=0; k<7; k++)
					{
						if( 0xaa == m_bySlaveAddr[k] )
						{
							continue;
						}

						if ( *(pointer + 2 + k) != m_bySlaveAddr[k] )
						{
							sprintf(m_szPrintBuf, 
									"Cjt188 recv addr error the %d byte error Getaddr=%d addr=%d ",k, 
									*(pointer + 1 + k), m_bySlaveAddr[k] );
							print( m_szPrintBuf );
							goto DEFAULT;
						}
					}


					//获取正确的报文长度
					len = datalen + 12;
					buf = buf + pos;
					return TRUE;
				}
				break;
			default:
				break;
		}
DEFAULT:
	len--;
	pointer++;
	pos ++;
	}

	print( "Cjt188 can't find the correct recvbuf" );
	return FALSE;
}		/* -----  end of method CProtocol_Cjt188::WhetherBufValue  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Cjt188
 *      Method:  ReadCfg
 * Description:	 读取配置文件 
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CProtocol_Cjt188::ReadCfgInfo ( void )
{
	FILE *fp = NULL;	
	char szLineBuf[256];
	char szFileName[256] = "";

	printf ( "slaveaddr=%.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
			m_bySlaveAddr[0], m_bySlaveAddr[1],   m_bySlaveAddr[2],
			m_bySlaveAddr[3],   m_bySlaveAddr[4],   m_bySlaveAddr[5],
			m_bySlaveAddr[6]);

	sprintf( szFileName, "%s%s", CJT188PREFIXFILENAME, m_sTemplatePath);
	fp = fopen( szFileName, "r" );
	if( fp == NULL )
	{
		sprintf(m_szPrintBuf,  "Cjt188:ReadCfgInfo fopen %s err!!!\n", szFileName );
		printf ( "%s", m_szPrintBuf );
		return FALSE;
	}
	else
	{
		sprintf(m_szPrintBuf,  "Cjt188:ReadCfgInfo fopen %s Ok!!!\n", szFileName );
		printf ( "%s", m_szPrintBuf );
	}

	while( fgets( szLineBuf, sizeof(szLineBuf), fp ) != NULL )
	{
		rtrim( szLineBuf );
		if( szLineBuf[0] == '#' || szLineBuf[0] == ';' )
		{
			continue;
		}	

		if ( (szLineBuf[0]-'0') < 0 || (szLineBuf[0] - '0') > 9 )
		{
			// if ( strncmp( szLineBuf, "slaveaddr=", 10 ) == 0 )
			// {
				// int num[7];
				// sscanf( szLineBuf + 10, "%2x,%2x,%2x,%2x,%2x,%2x,%2x", 
						// (int *)&num[6],(int *)&num[5], (int *)&num[4],
						// (int *)&num[3],(int *)&num[2], (int *)&num[1],
						// (int *)&num[0]);
				// for( int i=0; i<7; i++ )
				// {
					// m_bySlaveAddr[i] = (BYTE)num[i];	
				// }
				
			// } 
			// else if( strncmp( szLineBuf, "metertype=", 10 ) == 0 )
			if( strncmp( szLineBuf, "metertype=", 10 ) == 0 )
			{
				int num;
				sscanf( ( szLineBuf+10 ), "%2x", &num );
				m_byMeterType = (BYTE)num;
				printf ( "metertype=%.2x\n", m_byMeterType );
			}
			else if( strncmp( szLineBuf, "fenum=", 6 ) == 0 )
			{
				int num;
				sscanf( szLineBuf+6, "%2d", (int *)&num );
				m_byFENum = (BYTE)num;
				printf ( "fenum=%.2d\n", m_byFENum );
			}

			continue;
		}

	}

	fclose( fp );

	return TRUE;
}		/* -----  end of method CProtocol_Cjt188::ReadCfg  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Cjt188
 *      Method:  ChangeSendPos
 * Description:  每发送一次报文移动一次位置
 *       Input:	 void
 *		Return:	 现在的位置
 *--------------------------------------------------------------------------------------
 */
BYTE CProtocol_Cjt188::ChangeSendPos ( void )
{
	int InfoNum = m_CfgInfo.size(  );
	while( InfoNum > 0 )
	{
		m_bySendPos = ( m_bySendPos + 1 )% ( m_CfgInfo.size() );
		//查找移动需要循环发送的信息返回
		if ( m_CfgInfo[m_bySendPos].byCycle > 0 )
		{
			return m_bySendPos;
		}
		else
		{
			//非循环信息
			InfoNum --;	
		}
	}

	return 0;
}		/* -----  end of method CProtocol_Cjt188::ChangeSendPos  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Cjt188
 *      Method:  HexToBcd
 * Description:	 16进制转BCD码 
 *       Input:  BYTE c:要转化的16进制数据;
 *		Return:  转化完成的BCD码
 *--------------------------------------------------------------------------------------
 */
BYTE CProtocol_Cjt188::HexToBcd ( BYTE c )
{
	return (BYTE)( ( c>>4 ) * 10 + ( c & 0x0f ));
}		/*  -----  end of method CProtocol_Cjt188::HexToBcd  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Cjt188
 *      Method:  BcdToHex
 * Description:  
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BYTE CProtocol_Cjt188::BcdToHex ( BYTE c )
{
	return (  (c%10) + (c/10)*16);
}		/* -----  end of method CProtocol_Cjt188::BcdToHex  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Cjt188
 *      Method:  Atoh
 * Description:  字符串转16进制
 *       Input:  char *szBuf 要转化的字符串指针
 *				 BYTE len	 只转化前len个字节， len <= 8	
 *				 BYTE byFlag byFlag=1:正序 byFlag=0 倒序
 *		Return:  转化的数值
 *--------------------------------------------------------------------------------------
 */
DWORD CProtocol_Cjt188::atoh ( char *szBuf , BYTE len, BYTE byFlag)
{
	BYTE i = 0, j;
	DWORD tempvalue = 0;
	DWORD value = 0;

	//判断长度
	if( 8 <= len )
	{
		len = 8;
	}

	for(i=0;i<len;i++)
	{
		if( 1 == byFlag ) //正序
		{
			j = i;
		}
		else if( 0 == byFlag )//倒序
		{
			j = len - i;	
		}

		//转化过程
		if( (szBuf[j]>='A') && (szBuf[j]<='F') )
		{
			tempvalue = szBuf[j]-'A'+10;
		}
		if( (szBuf[j]>='a') && (szBuf[j]<='f') )
		{
			tempvalue = szBuf[j]-'a'+10;
		}
		if( (szBuf[j]>='0') && (szBuf[j]<='9') )
		{
			tempvalue = szBuf[j]-'0';
		}

		value = value*16+tempvalue;
	}

	return value;
}		/* -----  end of method CProtocol_Cjt188::Atoh  ----- */


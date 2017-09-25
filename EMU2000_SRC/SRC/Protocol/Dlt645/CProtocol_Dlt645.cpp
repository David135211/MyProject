#include "CProtocol_Dlt645.h"
#include "Dlt645_2007.h"
#include "Dlt645_1997.h"

// #define  DLT645PRINT  1
#define  DLT645DEBUG 1

#define MODULE_DLT645_2007	1			/* 2007�� */
#define MODULE_DLT645_1997	2			/* 1997�� */

extern "C" void OutBusDebug(BYTE byBusNo, BYTE *buf, int len, int flag);

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Dlt645
 *      Method:  HexToBcd
 * Description:	 16����תBCD��
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
static BYTE HexToBcd ( BYTE c )
{
	c -= 0x33;
	return (BYTE)( ( c>>4 ) * 10 + ( c & 0x0f ));
}		/*  -----  end of method CProtocol_Dlt645::HexToBcd  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Dlt645
 *      Method:  Atoh
 * Description:  �ַ���ת16����
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
DWORD CProtocol_Dlt645::atoh ( char *szBuf )
{
	BYTE i = 0;
	DWORD tempvalue = 0;
	DWORD value = 0;
	BYTE len = strlen(szBuf);

	for(i=0;i<len;i++)
	{
		if( (szBuf[i]>='A') && (szBuf[i]<='F') )
		{
			tempvalue = szBuf[i]-'A'+10;
		}
		if( (szBuf[i]>='a') && (szBuf[i]<='f') )
		{
			tempvalue = szBuf[i]-'a'+10;
		}
		if( (szBuf[i]>='0') && (szBuf[i]<='9') )
		{
			tempvalue = szBuf[i]-'0';
		}
		value = value*16+tempvalue;
	}
	return value;
}		/* -----  end of method CProtocol_Dlt645::Atoh  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Cjt188
 *      Method:  Atoh
 * Description:  �ַ���ת16����
 *       Input:  char *szBuf Ҫת�����ַ���ָ��
 *				 BYTE len	 ֻת��ǰlen���ֽڣ� len <= 8
 *				 BYTE byFlag byFlag=1:���� byFlag=0 ����
 *		Return:  ת������ֵ
 *--------------------------------------------------------------------------------------
 */
DWORD CProtocol_Dlt645::atoh ( char *szBuf , BYTE len, BYTE byFlag)
{
	BYTE i = 0, j = 0;
	DWORD tempvalue = 0;
	DWORD value = 0;

	//�жϳ���
	if( 8 <= len )
	{
		len = 8;
	}

	for(i=0;i<len;i++)
	{
		if( 1 == byFlag ) //����
		{
			j = i;
		}
		else if( 0 == byFlag )//����
		{
			j = len - i;
		}

		//ת������
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


CProtocol_Dlt645::CProtocol_Dlt645()
{
    //ctor
	memset( m_sTemplatePath , 0 , sizeof( m_sTemplatePath ) ) ;
	memset( m_szPrintBuf, 0, sizeof( m_szPrintBuf ) );
	memset( m_bySlaveAddr, 0, sizeof( m_bySlaveAddr ) );
	m_bySendPos = 0;
	m_byDataType = 0;
}

CProtocol_Dlt645::~CProtocol_Dlt645()
{
    //dtor
	int size = m_module.size() ;
	for(  int i = 0 ; i < size ; i++ )
	{
		delete m_module[ i ] ;
	}
	m_module.clear() ;
	m_CfgInfo.clear();
	printf( "Delete All CProtocol_Dlt645 OK . \n" );
}

BOOL CProtocol_Dlt645::GetProtocolBuf( BYTE * buf , int &len , PBUSMSG pBusMsg )
{
	return FALSE ;
}

BOOL CProtocol_Dlt645::ProcessProtocolBuf( BYTE * buf , int len )
{
	return FALSE ;
}

BOOL CProtocol_Dlt645::Init( BYTE byLineNo )
{
	//����ModBus �ɼ�ģ������
	//ͨ�����ߺŻ�ȡ��ȡ��װ���ļ�·��
	m_byLineNo = byLineNo ;
	//��ȡģ���ļ�
	m_ProtoType = PROTOCO_GATHER ;

	return GetDevData( ) ;
}

BOOL CProtocol_Dlt645::GetDevData( )
{
	memset( m_sDevPath , 0 , sizeof( m_sDevPath ) ) ;
	sprintf( m_sDevPath , "%s/Dlt645/%s%02d.ini" , SYSDATAPATH , DEVNAME , m_byLineNo + 1 );
	CProfile profile( m_sDevPath ) ;

	return ProcessFileData( profile ) ;
}

BOOL CProtocol_Dlt645::ProcessFileData( CProfile &profile )
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

		//������Ӧģ������
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

BOOL CProtocol_Dlt645::CreateModule( int iModule , int iSerialNo , WORD iAddr , char * sName , char * stplatePath )
{
	CProtocol_Dlt645 * pProtocol = NULL ;
	BOOL bRtn = FALSE;
	switch ( iModule )
	{
	case MODULE_DLT645_2007:
		{
			pProtocol = new CDlt645_2007 ;
			pProtocol->m_byLineNo = m_byLineNo ;
			pProtocol->m_wModuleType = iModule ;
			pProtocol->m_wDevAddr = iAddr ;
			pProtocol->m_SerialNo = iSerialNo ;
			strcpy( pProtocol->m_sDevName , sName ) ;
			strcpy( pProtocol->m_sTemplatePath , stplatePath ) ;
			m_pMethod->m_pRtuObj = pProtocol;
			pProtocol->m_pMethod = m_pMethod ;
			pProtocol->m_ProtoType = PROTOCO_GATHER ;
			//��ʼ��ģ������
			bRtn = pProtocol->Init( m_byLineNo ) ;
			if ( !bRtn )
			{
				printf ( "Init Error \n");
				return FALSE;
			}
			printf( " Add bus = %d Addr = %d serialno = %d\n" , m_byLineNo , iAddr, iSerialNo ) ;
		}
		break;
	case MODULE_DLT645_1997:
		{
			pProtocol = new CDlt645_1997 ;
			pProtocol->m_byLineNo = m_byLineNo ;
			pProtocol->m_wModuleType = iModule ;
			pProtocol->m_wDevAddr = iAddr ;
			pProtocol->m_SerialNo = iSerialNo ;
			strcpy( pProtocol->m_sTemplatePath , stplatePath ) ;
			m_pMethod->m_pRtuObj = pProtocol;
			pProtocol->m_pMethod = m_pMethod ;
			pProtocol->m_ProtoType = PROTOCO_GATHER ;
			//��ʼ��ģ������
			bRtn = pProtocol->Init( m_byLineNo ) ;
			if ( !bRtn )
			{
				printf ( "Init Error \n");
				return FALSE;
			}
			printf( " Add bus = %d Addr = %d serialno = %d\n" , m_byLineNo , iAddr, iSerialNo ) ;
		}
		break;

	default:
		{
			printf( "ModBus don't contain this module Failed .\n" );
			return FALSE ;
		}
	}
		m_module.push_back( pProtocol ) ;

	return TRUE ;
}

BYTE CProtocol_Dlt645::GetCs( const BYTE * pBuf , int len )
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

BOOL CProtocol_Dlt645::BroadCast( BYTE * buf , int &len )
{
	//��֯��ģ��Ĺ㲥����
	int index = 0 ;
	buf[ index++ ] = 0xFF ;
	buf[ index++ ] = 0x02 ;
	buf[ index++ ] = 0x03 ;
	buf[ index++ ] = 0x04 ;

	WORD wCRC = GetCs( buf, index );
    buf[ index++ ] = HIBYTE(wCRC);
    buf[ index++ ] = LOBYTE(wCRC);

	len = index ;

	printf( "\n CProtocol_Dlt645  TestBroadCast \n " ) ;
	return TRUE ;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Dlt645
 *      Method:  print
 * Description:  ��ӡ���ն� ���� ���߷������ ����ʱʹ��
 *       Input:	 ������ ����
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CProtocol_Dlt645::print ( const char *szBuf, int len )
{
#ifdef  DLT645PRINT
	printf( "%s\n", szBuf );
#endif     /* -----  not DLT645PRINT  ----- */

#ifdef  DLT645DEBUG
	OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
#endif     /* -----  not DLT645DEBUG  ----- */
}		/* -----  end of method CProtocol_Dlt645::print  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Dlt645
 *      Method:  WhetherBufValue
 * Description:  �ж�Dlt645���ĵ���Ч�� �����ж� ���ӻ����л�ȡһ֡��ȷ���ģ������)
 *       Input:  �յ��Ļ�����buf �յ������ݳ���len ��������Чλ��pos
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CProtocol_Dlt645::WhetherBufValue (const BYTE *buf, int &len, int &pos )
{
	const BYTE *pointer = buf;
	int datalen;
	BYTE byCrc;
	pos = 0;

	if( buf == NULL || len <= 0 )
		return FALSE;

	while ( len > 0 )
	{
		switch(*pointer)
		{
			case 0x68:  //�жϿɱ�֡
				{
					//�ж�68 x x 68������
					if( *(pointer+7) != *pointer )
					{
						goto DEFAULT;
					}

					//�жϿ���λ
					if( ( *( pointer + 8 ) & 0x80 ) == 0)
					{
						print( "Dlt645 recv ���������λ ��Ϊ1" );
						goto DEFAULT;
					}

					//�ж����ݳ��Ⱥ�����
					datalen=*(pointer+9);
					if( datalen+11>len || datalen > 200 )
					{
						sprintf(m_szPrintBuf, "Dlt645 recv len error datalen=%d len=%d", datalen, len );
						OutBusDebug( m_byLineNo, (BYTE *)m_szPrintBuf, strlen(m_szPrintBuf), 2 );
						goto DEFAULT;
					}

					//�ж�У��λ �����һ���ֽ�0x16
					byCrc=GetCs(pointer,datalen + 10);
					if(*(pointer+datalen+10)!=byCrc
							|| *(pointer+datalen+11)!=0x16)
					{
						sprintf(m_szPrintBuf, "Dlt645 recv cs error GetCrc=%d crc=%d or last byte != 0x16 =%x", byCrc, *(pointer+datalen+10), *(pointer+datalen+11));
						OutBusDebug( m_byLineNo, (BYTE *)m_szPrintBuf, strlen(m_szPrintBuf), 2 );
						goto DEFAULT;
					}

					//�жϵ�ַ
					for ( int k=0; k<6; k++)
					{
						if( m_bySlaveAddr[k] != 0 && m_bySlaveAddr[k] != 0xAA )
						{
							if ( *(pointer + 1 + k) != m_bySlaveAddr[k] )
							{
								sprintf(m_szPrintBuf, "Dlt645 recv addr error the %d byte error Getaddr=%d addr=%d ",k, *(pointer + 1 + k), m_bySlaveAddr[k] );
								print( m_szPrintBuf );
								goto DEFAULT;
							}
						}
					}


					//��ȡ��ȷ�ı��ĳ���
					len = datalen + 12;
					buf = buf + pos;
          printf("pos=%d buf0=%.2x\n", pos, buf[0]);
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


	return FALSE;
}		/* -----  end of method CProtocol_Dlt645::WhetherBufValue  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Dlt645
 *      Method:  ReadCfgSlaveAddr
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CProtocol_Dlt645::ReadCfgSlaveAddr ( char *szLineBuf )
{
	char *p = szLineBuf;
  // printf("szLineBuf=%s\n",szLineBuf);
	if ( strncmp( szLineBuf, "slaveaddr=", 10 ) == 0 )
	{
		p += 10;
		p = strtok( p, "," );
		if( p == NULL )
		{
			return FALSE;
		}
		else
		{
			m_bySlaveAddr[0] = atoh(p);
		}
		for ( int k=1; k<6; k++)
		{
			if ( ( p = strtok( NULL, "," ) ) != NULL )
			{
				int iNum = 0;
				iNum = atoh(p);
				if( iNum > 255 || iNum < 0 )
				{
					sprintf( m_szPrintBuf, "Dlt645:ReadCfgInfo file: %s slaveaddr= byte:%d is err!!! \n", m_sTemplatePath, k);
					print ( m_szPrintBuf );
					continue;
				}
				m_bySlaveAddr[k] = atoh(p);
			}
		}

		printf ( "slaveaddr=%x %x %x %x %x %x\n", m_bySlaveAddr[0], m_bySlaveAddr[1],   m_bySlaveAddr[2],   m_bySlaveAddr[3],   m_bySlaveAddr[4],   m_bySlaveAddr[5] );
		return TRUE;
	}

	return FALSE;
}		/* -----  end of method CProtocol_Dlt645::ReadCfgSlaveAddr  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Dlt645
 *      Method:  ReadCfgVal
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CProtocol_Dlt645::ReadCfgVal ( char *szLineBuf )
{
	char *p = NULL;
	int iNum  = 0;
	int i = 0;
	Dlt645CfgInfo tCfgInfo;

	p = strtok( szLineBuf, "," );
	if( p == NULL )
	{
		return FALSE;
	}
	else
	{
		iNum = atoi(p);
		if( iNum != DLT645_YC_DATATYPE && iNum != DLT645_YM_DATATYPE )
		{
			sprintf( m_szPrintBuf, "Dlt645:ReadCfgInfo file: %s line:%zu byte:%d is err!!! \n", m_sTemplatePath, m_CfgInfo.size(), i);
			print ( m_szPrintBuf );
			return FALSE;
		}
		tCfgInfo.byDataType = atoi( p );
	}

	while( ( p = strtok( NULL, "," ) ) )
	{
		++i;
		iNum = atoi(p);
		if( iNum > 255 || iNum < 0 )
		{
			sprintf( m_szPrintBuf, "Dlt645:ReadCfgInfo file: %s line:%zu byte:%d is err!!! \n", m_sTemplatePath, m_CfgInfo.size(), i);
			print ( m_szPrintBuf );
			continue;
		}
		switch ( i  )
		{
			case 1:
				tCfgInfo.byDI0 = atoh( p );
				break;

			case 2:
				tCfgInfo.byDI1 = atoh( p );
				break;

			case 3:
				tCfgInfo.byDI2 = atoh( p );
				break;

			case 4:
				tCfgInfo.byDI3 = atoh( p );
				break;

			case 5:
				tCfgInfo.byDataNum=atoi( p );
				break;

			case 6:
				tCfgInfo.byStartIndex = atoi( p );
				break;

			case 7:
				tCfgInfo.byDataFormat = atoi( p );
				break;

			case 8:
				tCfgInfo.byDataLen = atoi( p);
				break;

			case 9:
				tCfgInfo.byFENum = atoi( p );
				break;

			case 10:
				tCfgInfo.byCycle = atoi(p);
				break;

			default:
				break;
		}				/* -----  end switch  ----- */
	}

	m_CfgInfo.push_back( tCfgInfo );

	return TRUE;
}		/* -----  end of method CProtocol_Dlt645::ReadCfgVal  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Dlt645
 *      Method:  ReadCfg
 * Description:	 ��ȡ�����ļ�
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CProtocol_Dlt645::ReadCfgInfo ( void )
{
	FILE *fp = NULL;
	char szLineBuf[256];
	char szFileName[256] = "";

	sprintf( szFileName, "%s%s", DLT645PREFIXFILENAME, m_sTemplatePath);
	fp = fopen( szFileName, "r" );
	if( fp == NULL )
	{
		sprintf(m_szPrintBuf,  "Dlt645:ReadCfgInfo fopen %s err!!!\n", szFileName );
		printf ( "%s", m_szPrintBuf );
		return FALSE;
	}
	else
	{
		sprintf(m_szPrintBuf,  "Dlt645:ReadCfgInfo fopen %s Ok!!!\n", szFileName );
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
			if( ReadCfgSlaveAddr( szLineBuf ) )
			{
			}
			continue;
		}

		if( ReadCfgVal( szLineBuf ) )
		{
		}
	}

	fclose( fp );

	if(m_CfgInfo.empty())
	{
		printf ( "dlt645:can't read Cfginfo from file %s\n", szFileName );
		return FALSE;
	}

	return TRUE;
}		/* -----  end of method CProtocol_Dlt645::ReadCfg  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Dlt645
 *      Method:  CalFormatData
 * Description:	 MMI �ϴ�����������
 *       Input:	 ������ ��ʽ ����
 *		Return:  DWORD ����ֵ
 *--------------------------------------------------------------------------------------
 */
BOOL CProtocol_Dlt645::CalFormatData ( const BYTE *buf, BYTE byDataFormat, BYTE byDataLen, DWORD &dwData )
{
	int i;
	BYTE byData,byTemp;

	if ( byDataFormat > 5 )
	{
		sprintf( m_szPrintBuf, "Dlt645 CalFormatData byDataFormat err = %d\n", byDataFormat );
		print( m_szPrintBuf );
		return FALSE;
	}

	switch(byDataFormat)
	{
		case 0:
			for(i=0;i<byDataLen;i++)
			{
				byData=*(buf+i)-0x33;
				byTemp=(byData>>4)*10+(byData&0x0F);
				dwData=dwData*100+byTemp;
			}
			break;
		case 1:
			for(i=byDataLen-1;i>=0;i--)
			{
				byData=*(buf+i)-0x33;
				byTemp=(byData>>4)*10+(byData&0x0F);
				dwData=dwData*100+byTemp;
			}
			break;

		case 2:
			if( 4 != byDataLen )
				break;
//			dwData = MAKELONG( (MAKEWORD( HexToBcd( buf[3] ), HexToBcd( buf[2] ))),
//							   (MAKEWORD( HexToBcd( buf[1] ), HexToBcd( buf[0] ))) );
			dwData = HexToBcd(buf[3]) * 1000000 + HexToBcd(buf[2]) * 10000 + HexToBcd(buf[1]) * 100 + HexToBcd(buf[0]);
			break;

		case 3:
			if( 4 != byDataLen )
				break;
//			dwData = MAKELONG( (MAKEWORD( HexToBcd( buf[2] ), HexToBcd( buf[3] ))),
//							   (MAKEWORD( HexToBcd( buf[0] ), HexToBcd( buf[1] ))) );
			dwData = HexToBcd(buf[2]) * 1000000 + HexToBcd(buf[3]) * 10000 + HexToBcd(buf[0]) * 100 + HexToBcd(buf[1]);
			break;

		case 4:
			if( 4 != byDataLen )
				break;
//			dwData = MAKELONG( (MAKEWORD( HexToBcd( buf[1] ), HexToBcd( buf[0] ))),
//							   (MAKEWORD( HexToBcd( buf[3] ), HexToBcd( buf[2] ))) );
			dwData = HexToBcd(buf[1]) * 1000000 + HexToBcd(buf[0]) * 10000 + HexToBcd(buf[3]) * 100 + HexToBcd(buf[2]);
			break;

		case 5:
			if( 4 != byDataLen )
				break;
//			dwData = MAKELONG( (MAKEWORD( HexToBcd( buf[0] ), HexToBcd( buf[1] ))),
//							   (MAKEWORD( HexToBcd( buf[2] ), HexToBcd( buf[3] ))) );
			dwData = HexToBcd(buf[0]) * 1000000 + HexToBcd(buf[1]) * 10000 + HexToBcd(buf[2]) * 100 + HexToBcd(buf[3]);
			break;

		default:
			break;

	}

	return TRUE;
}		/* -----  end of method CProtocol_Dlt645::CalFormatData  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CProtocol_Dlt645
 *      Method:  ChangeSendPos
 * Description:  û����һ�α����ƶ�һ��λ��
 *       Input:	 void
 *		Return:	 ���ڵ�λ��
 *--------------------------------------------------------------------------------------
 */
BYTE CProtocol_Dlt645::ChangeSendPos ( void )
{
	int InfoNum = m_CfgInfo.size(  );
	while( InfoNum > 0 )
	{
		m_bySendPos = ( m_bySendPos + 1 )% ( m_CfgInfo.size() );
		if ( m_CfgInfo[m_bySendPos].byCycle > 0 )
		{
			return m_bySendPos;
		}
		else
		{
			InfoNum --;
		}
	}

	return 0;
}		/* -----  end of method CProtocol_Dlt645::ChangeSendPos  ----- */

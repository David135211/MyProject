/*
 * =====================================================================================
 *
 *       Filename:  ModBusXiaoHuMCU.cpp
 *
 *    Description:  ���ϻ����ֳ�������������������  �������¼�����
					�ġ����ĸ�ʽ
						���б��ĸ�ʽ�������װ���·����ģ�

						0EBH				ͬ��ͷ
						90H	
						0EBH	
						90H	
						Ŀ�ĵ�ַ			װ�õ�ַ
						Դ��ַ				��λ����ַ
						״̬����00H��		����������
						LEN=02H				���ĳ���
						CODE=55H			������
						57H					У����
						55H					������
						AAH	

						���б��ĸ�ʽ��װ�������Ϸ����ģ�
						��1���޹��ϱ���
						0EBH				ͬ��ͷ
						90H	
						0EBH	
						90H	
						Ŀ�ĵ�ַ			��λ����ַ
						Դ��ַ				װ�õ�ַ
						״̬��				���뿪����״̬
						���ѹ				A��
						���ѹ				B��
						���ѹ				C��
						�ߵ�ѹ				Uab
						�ߵ�ѹ				Uac
						�ߵ�ѹ				Ubc
						LEN=02H				���ĳ���
						CODE=50H			������
						52H					У����
						55H					������
						AAH	



						��2�����ϱ���
						0EBH				ͬ��ͷ
						90H	
						0EBH	
						90H	
						Ŀ�ĵ�ַ			��λ����ַ
						Դ��ַ				װ�õ�ַ
						״̬��				����������
						������״̬			���뿪����״̬
						��					��Ϣ��
						��	
						��	
						ʱ	
						��	
						��	
						LEN=09H				���ĳ���
						CODE=50H			������
						��λ				�����
						��λ	
						55H					������
						AAH	
				   ע����1�����ĳ���ָ���������롢����ĸ�߶κš����ĳ��Ⱥͱ������ݳ���֮�͡�
						��2�������ָ���������롢����ĸ�߶κš����ĳ��ȡ��������ݳ��Ⱥ�������֮�͵ĵ�16λֵ��
						��3���������ͣ� 

						11H��ʾг��
						22H��ʾ��ѹ
						33H��ʾ��A��˿��
						44H��ʾ��B��˿��
						55H��ʾ��C��˿��

						66H��ʾ��A  PT��
						77H��ʾ��B  PT��
						88H��ʾ��C  PT��

						99H��ʾ��A  �����ӵ�
						AAH��ʾ��B  �����ӵ�
						BBH��ʾ��C  ������

						CCH��ʾ��A  ����ӵ�
						DDH��ʾ��B  ����ӵ�
						EEH��ʾ��C  ����ӵ�

						(4)���뿪����״̬


						0	1	C��˿ ״̬	B��˿ ״̬	A��˿ ״̬	C�Ӵ���״̬	B�Ӵ���״̬	A�Ӵ��� ״̬
						0	0						

						�����λ��01Ͷ��
						00 ͣ��
							
					 ��5��У������ָ���������롢���ĳ��Ⱥ�������֮�͵ĵ�8λ��
 *
 *        Version:  1.0
 *        Created:  2014��11��19�� 13ʱ15��10��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp (), 
 *   Organization:  
 *
 *		  history:
 * =====================================================================================
 */


#include "ModBusXiaoHuMCU.h"
extern "C" void OutBusDebug(BYTE byBusNo, BYTE *buf, int len, int flag);


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  CModBusXHMCU
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
CModBusXHMCU::CModBusXHMCU ()
{
	m_bLinkStatus = FALSE;
	m_bySendCount = 0;
	m_byRecvCount = 0;
	m_bySrcAddr = 1;
	m_byDataType = 0;
}  /* -----  end of method CModBusXHMCU::CModBusXHMCU  (constructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  ~CModBusXHMCU
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
CModBusXHMCU::~CModBusXHMCU ()
{
}  /* -----  end of method CModBusXHMCU::~CModBusXHMCU  (destructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  print
 * Description:  
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CModBusXHMCU::print ( char *buf ) const
{

#ifdef  XHMCU_PRINT
	OutBusDebug( m_byLineNo, (BYTE *)buf, strlen( buf ), 2 );
#endif     /* -----  not XHMCU_PRINT  ----- */
}		/* -----  end of method CModBusXHMCU::print  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  GetWordSumCheck
 * Description:	 ����У���  
 *       Input:  pBuf  ������ ע��pBuf�Ļ�������С �� > len
 *				 len   ��ҪУ��ĳ���
 *		Return:  У���
 *--------------------------------------------------------------------------------------
 */
WORD CModBusXHMCU::GetWordSumCheck ( BYTE *pBuf, int len  )
{
	WORD sum = 0;

	for ( int i=0; i<len; i++ )
	{
		sum += pBuf[i];
	}

	return sum;
}		/* -----  end of method CModBusXHMCU::GetWordSumCheck  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  GetSumCheck
 * Description:	 ����У���  
 *       Input:  pBuf  ������ ע��pBuf�Ļ�������С �� > len
 *				 len   ��ҪУ��ĳ���
 *		Return:  У���
 *--------------------------------------------------------------------------------------
 */
BYTE CModBusXHMCU::GetSumCheck ( BYTE *pBuf, int len )
{
	return LOBYTE( GetWordSumCheck( pBuf, len ) );
}		/* -----  end of method CModBusXHMCU::GetSumCheck  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  WhetherBufValue
 * Description:  �鿴���ձ�����Ч�� 
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusXHMCU::WhetherBufValue ( BYTE *buf, int &len, int &pos )
{
	BYTE *pointer = buf;
	pos = 0;

	while( len >= 18 )
	{
		//�ж�ͬ����
		if( 0xEB != *pointer
				|| 0x90 != *( pointer + 1 )
				|| 0xEB != *( pointer + 2)
				|| 0x90 != *( pointer + 3 ) )
		{
			print( (char *)"ͬ���ִ���" );
			goto DEFAULT;
		}
		//�жϵ�ַ
		if ( *( pointer + 4 ) != m_bySrcAddr
				|| *( pointer + 5 ) != m_wDevAddr)
		{
			print( (char *)"��ַ����" );
			goto DEFAULT;
		}

		//�жϽ�����
		if( 0x55 == *( pointer + 16 ) && 0xAA == *( pointer + 17 ))
		{
			//�жϱ��ĳ���
			if( 2 != *( pointer + 13 ) )
			{
				print( (char *)"���ĳ��ȴ���" );
				goto DEFAULT;
			}
			//�ж�������
			if( 0x50 != *( pointer + 14 ) )
			{
				print( (char *)"���������" );
				goto DEFAULT;
			}

			//�ж�У����
			if( GetSumCheck( pointer + 13, 2 ) != *( pointer + 15 ) )
			{
				print( (char *)"У�������" );
				goto DEFAULT;
			}

			buf = buf + pos;
			len = 18;
			m_byDataType = XHMCU_NONE_TROUBLE_DATATYPE;
			return TRUE;
			
		}
		else if(  0x55 == *( pointer + 18 ) && 0xAA == *( pointer + 19 ) )
		{
			WORD crc;
			//�жϱ��ĳ���
			if( 9 != *( pointer + 14 ) )
			{
				print( (char *)"���ĳ��ȴ���" );
				goto DEFAULT;
			}
			//�ж�������
			if( 0x50 != *( pointer + 15 ) )
			{
				print( (char *)"���������" );
				goto DEFAULT;
			}

			crc = GetWordSumCheck( pointer + 6, 10 );
			if( *( pointer + 16) != LOBYTE( crc ) 
					|| *( pointer + 17) != HIBYTE( crc ))
			{
				print( (char *)"����ʹ���" );
				goto DEFAULT;
			}
			buf = buf + pos;
			len = 20;
			m_byDataType = XHMCU_TROUBLE_DATATYPE;
			return TRUE;
			
		}
		else
		{
			m_byDataType = 0;
			print( (char *)"���������" );
		}

DEFAULT:
		pointer ++;
		len --;
		pos ++;
	}

	print( (char *) "CModBusXHMCU WhetherBufValue not find right buf ");

	return FALSE ;
}		/* -----  end of method CModBusXHMCU::WhetherBufValue  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  ProcessNoneTroubleData
 * Description:  
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusXHMCU::ProcessNoneTroubleData ( BYTE *buf, int len )
{
	BYTE byYxByte;
	BYTE byYxValue;
	float fYcValue;
	int i;
	char szBuf[256];
	if( len != 18 )
	{
		return FALSE;
	}

	//����yx
	byYxByte = buf[6];

	//����yx
	for( i=0; i<6; i++ )
	{
		byYxValue = ( byYxByte >> ( i ) ) & 0x01;
		m_pMethod->SetYxData( m_SerialNo, ( WORD )i, byYxValue );
		sprintf( szBuf, "yx pnt=%d val=%d", (WORD)i, byYxValue );
		print( szBuf );
	}

	//�Ƿ�Ͷ��
	if( 0x01 == ( ( byYxByte >> 6 ) & 0x03 ) )
	{
		byYxValue = 1;
		m_pMethod->SetYxData( m_SerialNo, (WORD)i, byYxValue );
	}
	else if ( 0x00 == ( ( byYxByte >> 6 ) & 0x03 ) )
	{
		byYxValue = 0;
		m_pMethod->SetYxData( m_SerialNo, (WORD)i, byYxValue );
	}
	else 
	{
		print( (char *)"Ͷ�˱������" );
		return FALSE;
	}

	sprintf( szBuf, "yx pnt=%d val=%d", i, byYxValue );
	print( szBuf );

	for( i=7; i<21; i++ )
	{
		byYxValue = 0;
		m_pMethod->SetYxData( m_SerialNo, (WORD)i, byYxValue );
		sprintf( szBuf, "yx pnt=%d val=%d", (WORD)i, byYxValue );
		print( szBuf );
	}


	//����yc
	for ( i=0; i<6; i++ )
	{
		fYcValue = ( float )buf[7 + i];
		m_pMethod->SetYcData( m_SerialNo, ( WORD )i, fYcValue );
		sprintf( szBuf, "yc pnt=%d val=%f", ( WORD )i, fYcValue );
		print( szBuf );
	}

	return TRUE;
}		/* -----  end of method CModBusXHMCU::ProcessNoneTroubleData  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  ProcessTroubleData
 * Description:  
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusXHMCU::ProcessTroubleData ( BYTE *buf, int len )
{
	BYTE byYxByte;
	BYTE byYxValue;
	WORD wPnt = 0;
	char szBuf[256];
	TIMEDATA ptmData;
	int i;
	if( len != 20 )
	{
		return FALSE;
	}

	//����yx
	byYxByte = buf[7];

	//����yx
	for( i=0; i<6; i++ )
	{
		byYxValue = ( byYxByte >> ( i ) ) & 0x01;
		m_pMethod->SetYxData( m_SerialNo, ( WORD )i, byYxValue );
		sprintf( szBuf, "yx pnt=%d val=%d", i, byYxValue );
		print( szBuf );
	}

	//�Ƿ�Ͷ��
	if( 0x01 == ( ( byYxByte>>6 ) & 0x03 ) )
	{
		byYxValue = 1;
		m_pMethod->SetYxData( m_SerialNo, ( WORD )i, byYxValue );
	}
	else if ( 0x00 == ( ( byYxByte >> 6 ) & 0x03 ) )
	{
		byYxValue = 0;
		m_pMethod->SetYxData( m_SerialNo, (WORD)i, byYxValue );
	}
	else 
	{
		print( (char *)"Ͷ�˱������" );
		return FALSE;
	}

	sprintf( szBuf, "yx pnt=%d val=%d", i, byYxValue );
	print( szBuf );

	//������Ϣ
	switch ( buf[6] )
	{
		case 0x11:	
			wPnt = 7;
			break;

		case 0x22:	
			wPnt = 8;
			break;

		case 0x33:	
			wPnt = 9;
			break;

		case 0x44:	
			wPnt = 10;
			break;

		case 0x55:	
			wPnt = 11;
			break;

		case 0x66:	
			wPnt = 12;
			break;

		case 0x77:	
			wPnt = 13;
			break;

		case 0x88:	
			wPnt = 14;
			break;

		case 0x99:	
			wPnt = 15;
			break;

		case 0xAA:	
			wPnt = 16;
			break;

		case 0xBB:	
			wPnt = 17;
			break;

		case 0xCC:	
			wPnt = 18;
			break;

		case 0xDD:	
			wPnt = 19;
			break;

		case 0xEE:	
			wPnt = 20;
			break;

		default:	
			print ( (char *)"�������������" );
			return FALSE;
			break;
	}				/* -----  end switch  ----- */

	if( !m_pMethod->IsSoeTime( 0,
				buf[13],
				buf[12],
				buf[11],
				buf[10],
				buf[9],
				buf[8]+2000) )
	{
		sprintf( szBuf, "ERROR:soe time err!!!%d-%d-%d %d:%d:%d", 
			    buf[8]+2000, buf[9], buf[10] ,
				buf[11], buf[12], buf[13]);
		print( szBuf );
		return FALSE;
	}
	ptmData.Year = buf[8] + 100;
	ptmData.Month  = buf[9];
	ptmData.Day  = buf[10];
	ptmData.Hour = buf[11];
	ptmData.Minute = buf[12];
	ptmData.Second = buf[13];


	if( 0 != wPnt )
	{
		m_pMethod->SetYxDataWithTime ( m_SerialNo,  wPnt, (BYTE)1, &ptmData );
		sprintf( szBuf, "yx soe  pnt=%d val=%d time = %d-%d-%d %d:%d:%d", 
				wPnt, 1, ptmData.Year + 1900, ptmData.Month,ptmData.Day ,
				ptmData.Hour, ptmData.Minute, ptmData.Second);
		print( szBuf );
	}

	return TRUE;
}		/* -----  end of method CModBusXHMCU::ProcessTroubleData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  ProcessRecvBuf
 * Description:  ������  ��������
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusXHMCU::ProcessRecvBuf ( BYTE *buf, int len )
{
	BOOL bRtn = FALSE;

	switch ( m_byDataType )
	{
		case XHMCU_NONE_TROUBLE_DATATYPE:	
			bRtn = ProcessNoneTroubleData( buf, len );
			break;

		case XHMCU_TROUBLE_DATATYPE:	
			bRtn = ProcessTroubleData( buf, len );
			break;

		default:	
			break;
	}				/* -----  end switch  ----- */

	m_byDataType = 0;

	return bRtn;
}		/* -----  end of method CModBusXHMCU::ProcessRecvBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  GetProtocolBuf
 * Description:  ��ȡ���ͱ���  
 *       Input:  ������ ���� ��Ϣ����ң�ص���Ϣ���� ʼ��ΪNULL��
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusXHMCU::GetProtocolBuf ( BYTE *buf, int &len, PBUSMSG pBusMsg )
{
	if( pBusMsg != NULL )
	{
		return FALSE;	
	}

	len = 0;

	//��֯ͬ��ͷ
	buf[len++] = 0xEB;
	buf[len++] = 0x90;
	buf[len++] = 0xEB;
	buf[len++] = 0x90;

	buf[len++] = m_wDevAddr; // Ŀ�ĵ�ַ 
	buf[len++] = m_bySrcAddr; // Դ��ַ	��дΪ1

	buf[len++] = 0x00; // ״̬��(00) 
	buf[len++] = 0x02; // ���ĳ��� 
	buf[len++] = 0x55; // ������ 
	buf[len++] = GetSumCheck( &buf[6], 3 ); // У���� 
	
	//������
    buf[ len++ ] = 0x55;
    buf[ len++ ] = 0xAA;

	m_bySendCount ++;

	return TRUE;
}		/* -----  end of method CModBusXHMCU::GetProtocolBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  ProcessProtocolBuf
 * Description:  ������ձ���
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusXHMCU::ProcessProtocolBuf ( BYTE *buf, int len )
{
	int pos = 0;
	if ( !WhetherBufValue( buf, len, pos ) )
	{
		char szBuf[256] = "";
		sprintf( szBuf, "%s",  "ModBusXiaoHuMCU recv buf err !!!\n" );
		print( szBuf );
		m_byRecvCount ++;
		return FALSE;	
	}
	
	ProcessRecvBuf( buf + pos, len );

	m_bLinkStatus = TRUE;
	m_bySendCount = 0;
	m_byRecvCount = 0;

	return TRUE;
}		/* -----  end of method CModBusXHMCU::ProcessProtocolBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  Init
 * Description:  ��ʼ��Э��
 *       Input:  ���ߺ�
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusXHMCU::Init ( BYTE byLineNo )
{
	return TRUE;
}		/* -----  end of method CModBusXHMCU::Init  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  TimerProc
 * Description:  ʱ�Ӵ���
 *       Input:  void
 *		Return:  void
 *--------------------------------------------------------------------------------------
 */
void CModBusXHMCU::TimerProc ( void )
{
	if( m_bySendCount > 3 || m_byRecvCount > 3)
	{
		m_bySendCount = 0;
		m_byRecvCount = 0;
		if( m_bLinkStatus  )
		{
			m_bLinkStatus = FALSE;
			print( (char *) "CModBusXHMCU:unlink\n");
		}
	}
}		/* -----  end of method CModBusXHMCU::TimerProc  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  GetDevCommState
 * Description:  ��ȡװ��״̬
 *       Input:  void
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusXHMCU::GetDevCommState ( void )
{
	if ( m_bLinkStatus )
	{
		return COM_DEV_NORMAL;
	}
	else
	{
		return COM_DEV_ABNORMAL;
	}
}		/* -----  end of method CModBusXHMCU::GetDevCommState  ----- */


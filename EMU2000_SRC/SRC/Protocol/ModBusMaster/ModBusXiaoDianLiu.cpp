/*
 * =====================================================================================
 *
 *       Filename:  ModBusXiaoDianLiu.cpp
 *
 *    Description:  ���ϻ����ֳ���������С�����ӵ�ѡ�߱���װ��  �������¼�����
					�������ݽ�������
						1��RTU֡��ʽ
							��ַ	������	����	Ч��
							8-BITS	8-BITS	N*8-BITS	16-BITS
						2��CRC-16У��   X16+X15+X2+1
						3����������֡�����У�
							��ң�������ͽӵ�����
							װ �� �� ַ	 1�ֽ�	������ 1�ֽ�	������ ʼ��ַ ����λ��	������ ʼ��ַ ����λ��	������ ������ ����λ��	������ ������ ����λ��	CRC ����λ��	CRC ����λ��
							1~0FEH					04				00					00						00						36							CRCУ�鷶Χ

							ע�ͣ�����������ʼ��ַ��������������ʾȡ�õ���������Ϊ��λ�������ֽ�Ϊ��λ��
						4��������Ӧ֡�����У�
						װ�õ�ַ��1�ֽ�	������ 1�ֽ�	�����ֽ�����1�ֽ�	����0��.	����N	CRC ����λ��	CRC ����λ��
						1~0FEH			04				DataLen				Data					CRCУ�鷶Χ

						������Լ��ʹ��
						1������Լֻ�ṩ�����¼��Ĳ�ѯ����ѯ�����¼�֡������Ϊ04��
							����Լ�ṩһ���¼������������£�
							���	�¼��б�
							1	ĸ��1�µĹ����¼����������ӵع��ϡ�ĸ�߹��ϡ���ѹ�澯��
							2	ĸ��2�µĹ����¼���ͬ��
							3	ĸ��3�µĹ����¼���ͬ��
							4	ĸ��4�µĹ����¼���ͬ��
							5	��ʷ�����¼�1
							6	��ʷ�����¼�2
							����	����
							36	��ʷ�����¼�32
							1-4�¼�Ĭ��Ϊ4��ĸ���µĵ�ǰʵʱ�����¼���5-36Ϊ32����ʷ�����¼�����ÿ���¼�֡ռ�ڴ� 18���ֽڣ���9���֣�
						�¼�֡���ڴ��ʽ˵�������£���ÿ���¼�֡ռ�ڴ� 18���ֽڣ���9���֣�
						ID		1	2	3	4	5	6	7	8
						�ֽ���	2	2	1	1	1	1	1	1
						λ����	������·��	����ʱ��ĸ�ߵ�ѹֵ	����ĸ�ߺ�	��������	SSec	SMin	SHour	SDay

						9	10	11	12	13	14	15	16
						1	1	1	1	1	1	1	1
						Smon	SYear	ESec	EMin	EHour	EDay	EMon	Eyear

						�¼�֡��ϸ˵����
						*������·�ţ�4-51ֻ����·��ţ�ռ�ڴ�2���ֽڣ�����λ��ǰ����λ�ں�
						*����ʱ��ĸ�ߵ�ѹֵ�� ��ռ�ڴ�2���ֽڣ���λ��ǰ����λ�ں�
						*����ĸ�ߺţ�0-3����ĸ�ߵ����
						*�������ͣ�0-�޹���,1-ĸ�߹��ϣ�2-�����ѹ���߱�����3-�ӵع���
						*���д�S��ʱ��Ϊ������ʼʱ�䣬E��ʱ��Ϊ����ʱ�䡣1-4��ĸ���µĵ�ǰ����û�н���ʱ�䡣
						2�� �����ֽ�����������CRC�ֽڡ�
						3�� CRC�Գ���������������ֽڴӡ�װ�õ�ַ����У�顣
						4�� ����ԼΪ�ʴ�ʽ������װ�����޽ӵ��¼�����⵽��ֻҪ�յ��������󣬾��������ݡ�
 *
 *        Version:  1.0
 *        Created:  2015��04��22�� 11ʱ13��29�� 
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp (), 
 *   Organization:  
 *
 *		  history:
 * =====================================================================================
 */


#include "ModBusXiaoDianLiu.h"
extern "C" void OutBusDebug(BYTE byBusNo, BYTE *buf, int len, int flag);


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXDL
 *      Method:  CModBusXDL
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
CModBusXDL::CModBusXDL ()
{
	m_bLinkStatus = FALSE;
	m_bySendCount = 0;
	m_byRecvCount = 0;
}  /* -----  end of method CModBusXDL::CModBusXDL  (constructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXDL
 *      Method:  ~CModBusXDL
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
CModBusXDL::~CModBusXDL ()
{
}  /* -----  end of method CModBusXDL::~CModBusXDL  (destructor)  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXDL
 *      Method:  print
 * Description:  
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CModBusXDL::print ( char *buf )
{
#ifdef  XDL_PRINT
	OutBusDebug( m_byLineNo, (BYTE *)buf, strlen( buf ), 2 );
#endif     /* -----  not XDL_PRINT  ----- */
}		/* -----  end of method CModBusXDL::print  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXDL
 *      Method:  WhetherBufValue
 * Description:  �鿴���ձ�����Ч�� 
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusXDL::WhetherBufValue ( BYTE *buf, int &len ,int &pos)
{
	BYTE *pointer = buf;
	WORD wCrc;
	pos = 0;

	while( len >= 77 )
	{
		//�жϵ�ַ
		if ( *pointer != m_wDevAddr )
		{
			print( (char *)"CModBusXDL WhetherBufValue addr error" );
			goto DEFAULT;
		}

		//�жϹ�����
		if ( *( pointer + 1 ) != 0x04 )
		{
			print( (char *)"CModBusXDL WhetherBufValue code error" );
			goto DEFAULT;
		}

		//�ж�У��
		wCrc = GetCrc( pointer, ( *( pointer + 2 ) + 3 ) );
		if( *( pointer + ( *( pointer + 2 ) + 3 ) ) !=  HIBYTE(wCrc)
		 || *( pointer + ( *( pointer + 2 ) + 4 ) ) !=  LOBYTE(wCrc))
		{
			print( (char *)"CModBusXDL WhetherBufValue crc error" );
			goto DEFAULT;
		}
			
		buf = buf + pos;
		len = *(pointer + 2) + 5;
		return TRUE;
DEFAULT:
		pointer ++;
		len --;
		pos ++;
	}

	print( (char *) "CModBusXDL WhetherBufValue not find right buf ");

	return FALSE ;
}		/* -----  end of method CModBusXDL::WhetherBufValue  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXDL
 *      Method:  ProcessRecvBuf
 * Description:  ������  ��������
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusXDL::ProcessRecvBuf ( BYTE *buf, int len )
{
	char szBuf[256];
	if( len < 77 )
	{
		return FALSE;
	}

	for ( int i=0; i<4; i++)
	{
		BYTE byYxValue = 0;
		BYTE byYxByte = 0;
		BYTE byYxPnt;

		byYxPnt = i*3;
		byYxByte = buf[2 + i * 18 + 6];

		if( 0 == byYxByte )
		{
			byYxValue = 0;
			m_pMethod->SetYxData( m_SerialNo, byYxPnt, byYxValue );
			sprintf( szBuf,"yx %d value=%d", byYxPnt , byYxValue   );
			print( szBuf );
			m_pMethod->SetYxData( m_SerialNo, byYxPnt + 1, byYxValue );
			sprintf( szBuf,"yx %d value=%d", byYxPnt + 1, byYxValue   );
			print( szBuf );
			m_pMethod->SetYxData( m_SerialNo, byYxPnt + 2, byYxValue );
			sprintf( szBuf,"yx %d value=%d", byYxPnt + 2, byYxValue   );
			print( szBuf );
		}
		else if( byYxByte > 0 && byYxByte < 4 )
		{
			byYxPnt = i * 3 + byYxByte - 1 ;
			byYxValue = 1;

			m_pMethod->SetYxData( m_SerialNo,  byYxPnt, byYxValue );
			sprintf( szBuf,"yx %d value=%d", byYxPnt, byYxValue   );
			print( szBuf );
		}
		else
		{
			char szBuf[256] = "";
			sprintf( szBuf, "ModBusXiaoDianLiu ProcessRecvBuf err !!! errtype > %d \n", byYxByte );
			print( szBuf );
			return FALSE;
		}

		// printf ( "yx pnt=%d val=%d\n", i, byYxValue );
	}

	return TRUE;
}		/* -----  end of method CModBusXDL::ProcessRecvBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXDL
 *      Method:  GetProtocolBuf
 * Description:  ��ȡ���ͱ���  
 *       Input:  ������ ���� ��Ϣ����ң�ص���Ϣ���� ʼ��ΪNULL��
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusXDL::GetProtocolBuf ( BYTE *buf, int &len, PBUSMSG pBusMsg )
{
	if( pBusMsg != NULL )
	{
		return FALSE;	
	}

	len = 0;

	buf[len++] = m_wDevAddr;
	buf[len++] = 0x04;

	buf[len++] = 0x00;
	buf[len++] = 0x00;
	buf[len++] = 0x00;
	buf[len++] = 0x36;
	
	WORD wCRC = GetCrc( buf, len );
    buf[ len++ ] = HIBYTE(wCRC);
    buf[ len++ ] = LOBYTE(wCRC);

	m_bySendCount ++;

	return TRUE;
}		/* -----  end of method CModBusXDL::GetProtocolBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXDL
 *      Method:  ProcessProtocolBuf
 * Description:  ������ձ���
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusXDL::ProcessProtocolBuf ( BYTE *buf, int len )
{
	int pos = 0;
	if ( !WhetherBufValue( buf, len, pos ) )
	{
		char szBuf[256] = "";
		sprintf( szBuf, "%s",  "ModBusXiaoDianLiu recv buf err !!!\n" );
		print( szBuf );

		m_byRecvCount ++;
		return FALSE;	
	}
	
	ProcessRecvBuf( buf+pos, len );

	m_bLinkStatus = TRUE;
	m_bySendCount = 0;
	m_byRecvCount = 0;

	return TRUE;
}		/* -----  end of method CModBusXDL::ProcessProtocolBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXDL
 *      Method:  Init
 * Description:  ��ʼ��Э��
 *       Input:  ���ߺ�
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusXDL::Init ( BYTE byLineNo )
{
	return TRUE;
}		/* -----  end of method CModBusXDL::Init  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXDL
 *      Method:  TimerProc
 * Description:  ʱ�Ӵ���
 *       Input:  void
 *		Return:  void
 *--------------------------------------------------------------------------------------
 */
void CModBusXDL::TimerProc ( void )
{
	if( m_bySendCount > 3 || m_byRecvCount > 3)
	{
		m_bySendCount = 0;
		m_byRecvCount = 0;
		if( m_bLinkStatus  )
		{
			m_bLinkStatus = FALSE;
			print( ( char * ) "CModBusXDL:unlink\n");
		}
	}
}		/* -----  end of method CModBusXDL::TimerProc  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXDL
 *      Method:  GetDevCommState
 * Description:  ��ȡװ��״̬
 *       Input:  void
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModBusXDL::GetDevCommState ( void )
{
	if ( m_bLinkStatus )
	{
		return COM_DEV_NORMAL;
	}
	else
	{
		return COM_DEV_ABNORMAL;
	}
}		/* -----  end of method CModBusXDL::GetDevCommState  ----- */


/*
 * =====================================================================================
 *
 *       Filename:  ModBusWenKongyi.cpp
 *
 *    Description:  ���������ֳ��������ĸ�ʽ�¿���Э��  ��ɫ��Ŀ
					��1����λ�����ݸ�ʽ��
					��ַ�ֽڡ������ֽڡ���ʼ��ַ���ֽڡ���ʼ��ַ���ֽڡ������ݸ������ֽڡ������ݸ������ֽڡ�У����ֽڡ�У����ֽ�
					0x01(��ַ�ɱ�)��0X03(��֧�ֶ�����������,����Ϊ�Ƿ�����)��0X00��0X00��0X00��0X05��
					CRCLO--CRCHI
					����������Ǵ���ŵ�ַ�ֿ�ʼ����������֡�
					֧�ַ�ʽ�����ǣ�
					��ʼ��ַ�֣����ڵ���0��С�ڵ���4�������ݸ����֣����ڵ���1��С�ڵ���5����
					��ʼ��ַ�ֵ�����Ϊ��Ҫ��������ʼ����������λ���Ĵ洢��ַ�������ݸ����ֵ�����Ϊ������ʼ�����ֿ�ʼ���������������ֵĸ���������Ķ�Ӧ��ϵ�μ�����ı�񣬿ɸ�����Ҫ��ȷ����
					���ݸ�λ�ֽڡ�����λ���ֽڣ��������ֵķ��룬������ʮ�����ơ�
					������=��λ�ֽڡ�256����λ�ֽڣ�����ݾ���Ҫ������ȷ�ķ�Χ���롣

					��2����λ�����ص����ݸ�ʽ��
					������ȷ���ص����ݸ�ʽΪ
					��ַ�ֽڡ������ֽڡ������ֽ������ֳ�2�������ݸ��ֽڡ����ݵ��ֽڡ��ߡ��͡�.���ߡ��͡�У����ֽڡ�У����ֽ�
					��������ص����ݸ�ʽ
					1���������0X03	�������ַ�ֽڡ������ֽڡ�0X01��У����ֽڡ�У����ֽ�
					2�����ܴ�0X03�����ʼ��ַ��Ҫ������ݳ��ȴ�,CRCУ�������ַ�ֽڡ������ֽڡ�0X02��У����ֽڡ�У����ֽ�

					3��������λ���Ĵ洢��ַ
					��ַ���֣�			����					��ע
					0	״̬��		���������ֽڣ�
					1	A���¶���	����λΪ0.1���϶ȣ���ͬ��
					2	B���¶���	
					3	C���¶���	
					4	��ʷ����¶�ֵ��	
					״̬��   ����λ�� XX  XX  XX  XX����λ��
					TZ  GZ  CW  FJ
					TZ��	����λ��11������ʾ��λ���Ѿ���բ��
					GZ��	����λ��11������ʾ��λ���������ϡ�
					CW��	����λ��11������ʾ��λ���Ѿ����¡�
					FJ��	����λ��11������ʾ��λ���Ѿ����������
					4 ����˵��
					��λ����Ҫ��ȡ���Ǳ��������Ϣ����������Ϊ��
					0X01��0X03��0X00��0X00��0X00��0X05��0X85��0XC9
					��ʱ��λ����������Ϊ��
					0X01��0X03��0X0A��0X00��0X03��0X00��0XFD��0X01��0X02��0X01��0X00��0X01��
					0XC2��0X F1��0X95
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


#include "ModBusWenKongyi.h"
extern "C" void OutBusDebug(BYTE byBusNo, BYTE *buf, int len, int flag);


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusWenKongYi
 *      Method:  CModbusWenKongYi
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
CModbusWenKongYi::CModbusWenKongYi ()
{
	m_bLinkStatus = FALSE;
	m_bySendCount = 0;
	m_byRecvCount = 0;
}  /* -----  end of method CModbusWenKongYi::CModbusWenKongYi  (constructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusWenKongYi
 *      Method:  ~CModbusWenKongYi
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
CModbusWenKongYi::~CModbusWenKongYi ()
{
}  /* -----  end of method CModbusWenKongYi::~CModbusWenKongYi  (destructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusWenKongYi
 *      Method:  WhetherBufValue
 * Description:  �鿴���ձ�����Ч�� 
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModbusWenKongYi::WhetherBufValue ( BYTE *buf, int &len )
{
	BYTE *pointer = buf;
	WORD wCrc;
	int pos = 0;

	while( len >= 4 )
	{
		//�жϵ�ַ
		if ( *pointer != m_wDevAddr )
		{
			goto DEFAULT;
		}

		//�жϹ�����
		if ( *( pointer + 1 ) != 0x03 )
		{
			goto DEFAULT;
		}

		//�ж�У��
		wCrc = GetCrc( pointer, ( *( pointer + 2 ) + 3 ) );
		if( *( pointer + ( *( pointer + 2 ) + 3 ) ) !=  HIBYTE(wCrc)
		 || *( pointer + ( *( pointer + 2 ) + 4 ) ) !=  LOBYTE(wCrc))
		{
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
	return FALSE ;
}		/* -----  end of method CModbusWenKongYi::WhetherBufValue  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusWenKongYi
 *      Method:  ProcessRecvBuf
 * Description:  ������  ��������
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModbusWenKongYi::ProcessRecvBuf ( BYTE *buf, int len )
{
	if( len < 15 )
	{
		return FALSE;
	}

	for ( int i=0; i<4; i++)
	{
		BYTE byYxValue = 0;
		BYTE byYxBit = 0;
		byYxBit = ( buf[4] >> ( 2 * i ) ) & 0x03 ;
		if ( byYxBit == 0x03 )
		{
			byYxValue = 1;
		}
		else
		{
			byYxValue = 0;
		}

		m_pMethod->SetYxData( m_SerialNo, i, byYxValue );
		// printf ( "yx pnt=%d val=%d\n", i, byYxValue );
	}

	for ( int i=0; i<4; i++)
	{
		float fYcValue;	
		fYcValue = (float)( MAKEWORD( buf[6 + 2 * i], buf[ 5 + 2 * i ] ) );

		m_pMethod->SetYcData( m_SerialNo, i, fYcValue );
		// printf ( "yc pnt=%d val=%f\n", i, fYcValue );
	}

	return TRUE;
}		/* -----  end of method CModbusWenKongYi::ProcessRecvBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusWenKongYi
 *      Method:  GetProtocolBuf
 * Description:  ��ȡ���ͱ���  
 *       Input:  ������ ���� ��Ϣ����ң�ص���Ϣ���� ʼ��ΪNULL��
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModbusWenKongYi::GetProtocolBuf ( BYTE *buf, int &len, PBUSMSG pBusMsg )
{
	if( pBusMsg != NULL )
	{
		return FALSE;	
	}

	len = 0;

	buf[len++] = m_wDevAddr;
	buf[len++] = 0x03;

	buf[len++] = 0x00;
	buf[len++] = 0x00;
	buf[len++] = 0x00;
	buf[len++] = 0x05;
	
	WORD wCRC = GetCrc( buf, len );
    buf[ len++ ] = HIBYTE(wCRC);
    buf[ len++ ] = LOBYTE(wCRC);

	m_bySendCount ++;

	return TRUE;
}		/* -----  end of method CModbusWenKongYi::GetProtocolBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusWenKongYi
 *      Method:  ProcessProtocolBuf
 * Description:  ������ձ���
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModbusWenKongYi::ProcessProtocolBuf ( BYTE *buf, int len )
{
	if ( !WhetherBufValue( buf, len ) )
	{
		char szBuf[256] = "";
		sprintf( szBuf, "%s",  "ModBusWenKongyi recv buf err !!!\n" );
		OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen( szBuf ), 2 );

		m_byRecvCount ++;
		return FALSE;	
	}
	
	ProcessRecvBuf( buf, len );

	m_bLinkStatus = TRUE;
	m_bySendCount = 0;
	m_byRecvCount = 0;

	return TRUE;
}		/* -----  end of method CModbusWenKongYi::ProcessProtocolBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusWenKongYi
 *      Method:  Init
 * Description:  ��ʼ��Э��
 *       Input:  ���ߺ�
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModbusWenKongYi::Init ( BYTE byLineNo )
{
	return TRUE;
}		/* -----  end of method CModbusWenKongYi::Init  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusWenKongYi
 *      Method:  TimerProc
 * Description:  ʱ�Ӵ���
 *       Input:  void
 *		Return:  void
 *--------------------------------------------------------------------------------------
 */
void CModbusWenKongYi::TimerProc ( void )
{
	if( m_bySendCount > 3 || m_byRecvCount > 3)
	{
		m_bySendCount = 0;
		m_byRecvCount = 0;
		if( m_bLinkStatus  )
		{
			m_bLinkStatus = FALSE;
			OutBusDebug( m_byLineNo, (BYTE *)"CModbusWenKongYi:unlink\n", 30, 2 );
		}
	}
}		/* -----  end of method CModbusWenKongYi::TimerProc  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusWenKongYi
 *      Method:  GetDevCommState
 * Description:  ��ȡװ��״̬
 *       Input:  void
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModbusWenKongYi::GetDevCommState ( void )
{
	if ( m_bLinkStatus )
	{
		return COM_DEV_NORMAL;
	}
	else
	{
		return COM_DEV_ABNORMAL;
	}
}		/* -----  end of method CModbusWenKongYi::GetDevCommState  ----- */


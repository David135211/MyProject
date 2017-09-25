/*
 * =====================================================================================
 *
 *       Filename:  ModBusLekuThermometer.cpp
 *
 *    Description:  Ф���ưݶ�ҽҩ�ֳ����Ϻ��ֿ��ѹ���õ����¶ȼ�
				
				1.  ͨѶ����˵��
				    ��Э����� MODBUS ͨѶЭ��� RTU ��ʽ��
				    ��  ���� RS485 �첽ͨѶ�����ݴ��͸�ʽ��
				    ��  ���ò����� 9600bps��4800bps��2400bps������żУ�飬8λ����λ��1λֹͣλ��
				    ��  ֡У����� CRC16 У�鷽ʽ��
				    ��  ͨѶ���룼1200m/˫���ߣ����¿���������ͨѶ��·�йأ���

				2.  ���ݸ�ʽ
					a.  ��λ���´����ݸ�ʽ
						���ʹ���	˵��	��ֵ	��ע
							1	������ַ	01H��C7H	
							2	���ܴ���	03H	Ϊ�̶���ʽ
							3	��ʼ��ַ��  ��8λ	00H	
							4	��ʼ��ַ��  ��8λ	00H	
							5	�����ݵĸ�����  ��8λ	00H	
							6	�����ݵĸ�����  ��8λ	06H	
							7	CRC У��  ��8λ	����H	
							8	CRC У��  ��8λ	����H	

					b.  ��λ���ϴ����ݸ�ʽ
						���ʹ���	˵��	��ֵ	��ע
							1	������ַ	01H��C7H	
							2	���ܴ���	03H	
							3	�������ݵ��ֽ���	0CH	�������ݵ��ֽڡ�2
							4	����״̬��  ��8λ	00H	
							5	����״̬��  ��8λ	����H	״̬�ֶ�����±�
							6	A���¶�����  ��8λ	00H	��λ1�棬�������ݼ�23H��Ϊʵ���¶�
							7	A���¶�����  ��8λ	����H	
							8	B���¶�����  ��8λ	00H	
							9	B���¶�����  ��8λ	����H	
							10	C���¶�����  ��8λ	00H	
							11	C���¶�����  ��8λ	����H	
							12	�����ʱ����ʱ�� ��8λ	00H	Ϊ��0������ʾ��ȡ�������ʱ����ʱ��
							13	�����ʱ����ʱ�� ��8λ	����H	
							14	D���¶�����  ��8λ	00H	�޸����¶ȼ�⹦��ʱ����������Ч
							15	D���¶�����  ��8λ	����H	
							16	CRC У��  ��8λ	����H	
							17	CRC У��  ��8λ	����H	

					c.����״̬�ֵ�8λ����
					��0λ����0������ʾA�ഫ����������������0λ����1������ʾA�ഫ�����й���
					��1λ����0������ʾB�ഫ����������������1λ����1������ʾB�ഫ�����й���
					��2λ����0������ʾC�ഫ����������������2λ����1������ʾC�ഫ�����й���
					��3λ����0������ʾ���δ����        ����3λ����1������ʾ���������
					��4λ����0������ʾ��ѹ������δ����  ����4λ����1������ʾ��ѹ�������ѳ���
					��5λ����0������ʾδ�����բ�ź�    ����5λ����1������ʾ�������բ�ź�
					��6λ����0������ʾD�ഫ����������������6λ����1������ʾD�ഫ�����й���
					��7λ����0������ʹ��

					d.���߾�ֹʱ��Ҫ��
						��������ǰҪ���������߾�ֹʱ�伴�����ݷ���ʱ����ڣ�5ms��������9600�����ʷ���5���ֽ����ݵ�ʱ�䡣

					e.����˵��
						��ǰ��һ̨��λ���������ַΪ��1���� A���¶�Ϊ64�棬B���¶�Ϊ54�棬C���¶�Ϊ68�棬�����ʱ����ʱ��Ϊ��24��Сʱ��D���¶�Ϊ65�棬Pt100�������޹��ϣ����δ������û��������±�������բ�źš�
						��λ�����͵�����Ϊ��
							01H��03H��00H��00H��00H��06H��C5H��C8H
				         	��ʱ��λ���ظ�������Ϊ��
				          		01��03H��0CH��00H��00H��00H��63H��00H��59H��00H��67H��00H��18H��00H��64H��52H��A3H

				3.  ͨѶ��ַ������
				      ÿ̨����������ͨѶ��ַ������û���һ̨PC��ͬʱ��ض�̨������ÿ̨���������ò�ͬ��ͨѶ��ַ��
 *        Version:  1.0
 *        Created:  2016��04��21�� 09ʱ45��00��
 *       Revision:  none
 *       Compiler:  
 *
 *         Author:  ykk
 *   Organization:  
 *
 *		  history:
 * =====================================================================================
 */


#include "ModBusLekuThermometer.h"
extern "C" void OutBusDebug(BYTE byBusNo, BYTE *buf, int len, int flag);


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  CModBusXHMCU
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
ModBusLekuThermometer::ModBusLekuThermometer ()
{
	m_bLinkStatus = FALSE;
	m_bySendCount = 0;
	m_byRecvCount = 0;
}  /* -----  end of method CModBusXHMCU::CModBusXHMCU  (constructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModBusXHMCU
 *      Method:  ~CModBusXHMCU
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
ModBusLekuThermometer::~ModBusLekuThermometer ()
{
}  /* -----  end of method CModBusXHMCU::~CModBusXHMCU  (destructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLekuThermometer
 *      Method:  print
 * Description:  
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void ModBusLekuThermometer::print ( char *buf ) const
{

#ifdef  XHMCU_PRINT
	OutBusDebug( m_byLineNo, (BYTE *)buf, strlen( buf ), 2 );
#endif     /* -----  not XHMCU_PRINT  ----- */
}		/* -----  end of method CModBusXHMCU::print  ----- */



/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLekuThermometer
 *      Method:  WhetherBufValue
 * Description:  �鿴���ձ�����Ч�� 
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL ModBusLekuThermometer::WhetherBufValue ( BYTE *buf, int &len, int &pos )
{
	BYTE *pointer = buf;
	pos = 0;

	while( len >= 17 )
	{
		//�ж�ͬ����
		if( m_wDevAddr != *pointer
				|| 0x03 != *( pointer + 1 )
				|| 0x0c != *( pointer + 2 ) )
		{
			print( (char *)"����ͷ" );
			pointer ++;
			len --;
			pos ++;
			continue;
		}
		//�жϵ�ַ
		WORD wCRC = GetCrc( buf, buf[2] + 3 );
		if( ( HIBYTE(wCRC) == buf[ buf[2] + 3 ] ) && ( LOBYTE(wCRC) == buf[ buf[2] + 4 ] ) )
		{
			return TRUE;
		}
		else
		{
			print( (char *)"У��δͨ��" );
			pointer ++;
			len --;
			pos ++;
			continue;
		}
	}

	print( (char *) "CModBusXHMCU WhetherBufValue not find right buf ");

	return FALSE ;
}		/* -----  end of method ModBusLekuThermometer::WhetherBufValue  ----- */

	
/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLekuThermometer
 *      Method:  ProcessRecvBuf
 * Description:  ������  ��������
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL ModBusLekuThermometer::ProcessRecvBuf ( BYTE *buf, int len )
{
	BYTE i;
	BYTE Temp_Yx = buf[4];
	WORD wVal;
	for( i=0;i<8;i++ )
	{
		wVal = Temp_Yx%2;
		m_pMethod->SetYxData ( m_SerialNo , i , wVal );
		Temp_Yx /= 2;
	}
	for( i=0;i<5;i++ )
	{
		wVal = (buf[5+(i*2)]<<8 | buf[6+(i*2)]);
		m_pMethod->SetYcData( m_SerialNo , i , (float)wVal );
	}
	
	return TRUE;
}		/* -----  end of method ModBusLekuThermometer::ProcessRecvBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLekuThermometer
 *      Method:  GetProtocolBuf
 * Description:  ��ȡ���ͱ���  
 *       Input:  ������ ���� ��Ϣ����ң�ص���Ϣ���� ʼ��ΪNULL��
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL ModBusLekuThermometer::GetProtocolBuf ( BYTE *buf, int &len, PBUSMSG pBusMsg )
{
	if( pBusMsg != NULL )
	{
		return FALSE;	
	}

	len = 0;

	//��֯ͬ��ͷ
	buf[len++] = m_wDevAddr;
	buf[len++] = 0x03;
	buf[len++] = 0x00;
	buf[len++] = 0x00;// �Ĵ�����ַ 
	buf[len++] = 0x00; 
	buf[len++] = 0x06; // �Ĵ�������

	WORD wCRC = GetCrc( buf, len );
    buf[ len++ ] = HIBYTE(wCRC);
    buf[ len++ ] = LOBYTE(wCRC);
	m_bySendCount ++;

	return TRUE;
}		/* -----  end of method ModBusLekuThermometer::GetProtocolBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLekuThermometer
 *      Method:  ProcessProtocolBuf
 * Description:  ������ձ���
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL ModBusLekuThermometer::ProcessProtocolBuf ( BYTE *buf, int len )
{
	int pos = 0;
	if ( !WhetherBufValue( buf, len, pos ) )
	{
		char szBuf[256] = "";
		sprintf( szBuf, "%s",  "ModBusLekuThermometer recv buf err !!!\n" );
		print( szBuf );
		m_byRecvCount ++;
		return FALSE;	
	}
	
	ProcessRecvBuf( buf + pos, len );

	m_bLinkStatus = TRUE;
	m_bySendCount = 0;
	m_byRecvCount = 0;

	return TRUE;
}		/* -----  end of method ModBusLekuThermometer::ProcessProtocolBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLekuThermometer
 *      Method:  Init
 * Description:  ��ʼ��Э��
 *       Input:  ���ߺ�
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL ModBusLekuThermometer::Init ( BYTE byLineNo )
{
	return TRUE;
}		/* -----  end of method ModBusLekuThermometer::Init  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLekuThermometer
 *      Method:  TimerProc
 * Description:  ʱ�Ӵ���
 *       Input:  void
 *		Return:  void
 *--------------------------------------------------------------------------------------
 */
void ModBusLekuThermometer::TimerProc ( void )
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
}		/* -----  end of method ModBusLekuThermometer::TimerProc  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  ModBusLekuThermometer
 *      Method:  GetDevCommState
 * Description:  ��ȡװ��״̬
 *       Input:  void
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL ModBusLekuThermometer::GetDevCommState ( void )
{
	if ( m_bLinkStatus )
	{
		return COM_DEV_NORMAL;
	}
	else
	{
		return COM_DEV_ABNORMAL;
	}
}		/* -----  end of method ModBusLekuThermometer::GetDevCommState  ----- */


/*
 * =====================================================================================
 *
 *       Filename:  ModBusEPS.cpp
 *
 *    Description:��ɫ�ֳ��� �ӳ���Ҫ������Э�飬 ��������
 ///////////////////////////////////////////////////////////////////////////////////////
// ��λ����12864������Э��
 //��λ����ѯ�豸״̬���������
// ��λ���������������ѯ״̬��
// AAN 03 AAH AAL 00 04 CRCL CRCH //AAH��AAL������Ҫ��ѯ�����ݣ��������¡������ʽ�����

// AAN���豸�ţ���������豸ͬʱ���ӵ�һ��������ʱ��ʹ�á�
// ���磺���豸5���в�������AAN=0x05��
// �豸���ǳ���ʱ�Բ�Ʒ�����ã���Э������ָ����Ըı䡣


// AAH AAL�������ַ�룬���±�
  // 0	//ϵͳ״̬    //=0��ʱ��Ƚ��ر𣬻ظ���������ÿһλ���ض����壬���忴��һС��
  // 1	//�е��ѹA 
  // 2	//�е��ѹB 
  // 3	//�е��ѹC 
  // 4	//�����ѹA  
  // 5	//�����ѹB 
  // 6	//�����ѹC  
  // 7	//�������A  
  // 8	//�������B 
  // 9	//�������C 
  // 10	//��ص�ѹ  
  // 11	//���1��ѹ
  // 12	//���2��ѹ
  // 13	//���3��ѹ
  // 14	//���4��ѹ
  // 15	//���5��ѹ
  // 16	//���6��ѹ
  // 17	//���7��ѹ
  // 18	//���8��ѹ
  // 19	//���9��ѹ
  // 20	//���10��ѹ
  // 21	//���11��ѹ
  // 22	//���12��ѹ
  // 23	//���13��ѹ
  // 24	//���14��ѹ
  // 25	//���15��ѹ
  // 26	//���16��ѹ
  // 27	//������
  // 28	//�������
  // 29	//���Ƶ��	
        // 30 //����֡���� ������������λ��Ϊ�豸���ò������õ���		 
	// 31 //�е��ѹAϵ����1.000  
	// 32 //�е��ѹAƫ�0.00
	// 33 //�е��ѹBϵ����1.000
	// 34 //�е��ѹBƫ�0.00
	// 35 //�е��ѹCϵ����1.000
	// 36 //�е��ѹCƫ�0.00

	// 37 //�����ѹAϵ����1.000  
	// 38 //�����ѹAƫ�0.00
	// 39 //�����ѹBϵ����1.000
	// 40 //�����ѹBƫ�0.00
	// 41 //�����ѹCϵ����1.000
	// 42 //�����ѹCƫ�0.00

	// 43 //�������Aϵ����1.000  
	// 44 //�������Aƫ�0.00
	// 45 //�������Bϵ����1.000
	// 46 //�������Bƫ�0.00
	// 47 //�������Cϵ����1.000
	// 48 //�������Cƫ�0.00
	
	// 49 //�ܵ�ص�ѹϵ����1.000
	// 50 //�ܵ�ص�ѹƫ�0.00

	// 51 //1��ϵ����1.000
	// 52 //1��ƫ�0.00
	// 53 //2��ϵ����1.000
	// 54 //2��ƫ�0.00
	// 55 //3��ϵ����1.000
	// 56 //3��ƫ�0.00
	// 57 //4��ϵ����1.000
	// 58 //4��ƫ�0.00


	// 59 //��ذ�ʱ��100A
	// 60 //������ϵ����1.000
	// 61 //������ƫ�0.00

	// 62 //�������4
	// 63 //ϵͳ���ʣ�1.00
	// 64 //���ػ�ָ�1.00

	// 67 //����1  �ػ�0   �ֶ�1 �Զ�0    ������0 ����1  ��ǿ��0 ǿ��1


// *************************����Ϊ��ѯ�ظ��������**************************************

// �豸�ظ�����������ر���ѯ״̬��
// AAN 03 04 DA3 DA4 DA1 DA2 CRCL CRCH //���������״̬���ݣ�ע�⣡���������˳����3412

// DA1 DA2 DA3 DA4:�ĸ��ֽ�Ϊ���ص�����
// CRCH CRCL��16λCRC����

// ���ڷ�������DA1~DA4�������Ϊ�����ֽ���˵����
/////////////////////////AAH AALΪ0ʱ�Ľ���///////////////////////////////////////////
// ��AAH\AALΪ0ʱ������ѯϵͳ״̬���������ݸ�ʽ���£�

// DA1    
// bit0��bit3    ϵͳ״̬  
              // 0 �ػ� 
              // 1	�е�
              // 2	���
              // 3	Ӧ��
              // 4	�ֶ�
              // 5	����
              // 6	����
              // 7	����
              // 8	����
              // 9	ǿ��
              // 10	����
              // 11	�¼�
              // 12	���
              // 13	���
// Bit4��bit5   	�е�״̬
              // 0	����
              // 1	Ƿѹ
              // 2	��ѹ
              // 3	��·
// Bit6��bit7   ���״̬
              // 0	����
              // 1	Ƿѹ
              // 2	��ѹ
              // 3	��·
            
// DA2       
// bit0��bit1   ���״̬
            // 0	����
            // 1	����
            // 2	��·
            // 3	����
// Bit2��bit3    Ѳ��״̬
              // 0	����
              // 1	Ƿѹ
              // 2	��ѹ
              // 3	��·
// Bit4��bit5   ���״̬
            // 0	����
            // 1	��·
// Bit6---bit7   �¼�״̬
               // 0 ����
               // 1 ʧ��
               // 2 δ���
// DA3
// bit0---bit1   ���״̬
              // 0 ����
              // 1 ʧ��
              // 2 δ���
// Bit2---bit3   ������ 
              // 0	�ޱ�����
              // 1	������
// DA4
             // Bit4          ����     0  �ر� ��1 ��LED3
             // Bit5          �е��     0  �ر�   1 ��LED1
             // Bit6          ����     0  �ر�   1 ��LED2
             // Bit7          ���ϵ�     0  �ر�   1 ��LED4
/////////////////////////AAH AALΪ��0ʱ�Ľ���///////////////////////////////////////////

// ��0ʱ���ѯ���ǵ�ѹ����������ֵ����˼��㹫ʽ���£�
// ���Ƚ���λ���ݰ��� DA1 DA2 DA3 DA4 ���һ��U32����ֵ����ΪX

// ʵ�ʵ�ѹֵ=X/1000

// �����ʽ������������ֵ��ѯ�������ʹ�õ�ʱ��ע���յ�������˳��
// ���磬�յ�����Ϊ01 03 04 0x11 0x22 0x33 0x44 CRCL CRCH
// ��ʵ�ʵ�ѹ=0x22331122/1000 ��


///////////�� �� ����λ�� ��ѯ �豸״̬��״̬�ظ���˵��///////////////////////
//////////////�� �� ����λ�� д�� �豸״̬��״̬�ظ���˵��///////////////////
// ��λ�������ݵ�12864Э��

// ��λ������AAN 10 AAH AAL 00 02 04 DA3 DA4 DA1 DA2 CRCL CRCH
// 12864�ط���AAN 10 AAH AAL 00 02 04 DA3 DA4 DA1 DA2 CRCL CRCH

// AAH AAL����ַ�룬�������б�30-63
// DA1 DA2 DA3 DA4:�ĸ��ֽ�Ϊ���ص�����
// CRCH CRCL��16λCRC����
// ͬ����д���ʱ����Ҫע��DA��˳��

// ���磬��Ҫ����ĳ��ѹΪ16797V�������Ƚ�16797*1000=16797000��
// �ٽ�20000ת��ΪU32������=0x01004D48������λ��Ӧ�÷�������������豸��
// AAN 10 AAH AAL 00 02 04 4D 48 01 00 CRCL CRCH

 *
 *        Version:  1.0
 *        Created:  2014��11��24�� 09ʱ40��06��
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

#include "ModBusEPS.h"
extern "C" void OutBusDebug(BYTE byBusNo, BYTE *buf, int len, int flag);


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusEPS
 *      Method:  CModbusEPS
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
CModbusEPS::CModbusEPS ()
{
	m_bLinkStatus = FALSE;
	m_bySendCount = 0;
	m_byRecvCount = 0;
	m_bySendPos = 0;
}  /* -----  end of method CModbusEPS::CModbusEPS  (constructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusEPS
 *      Method:  ~CModbusEPS
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
CModbusEPS::~CModbusEPS ()
{
}  /* -----  end of method CModbusEPS::~CModbusEPS  (destructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusEPS
 *      Method:  WhetherBufValue
 * Description:  �鿴���ձ�����Ч�� 
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModbusEPS::WhetherBufValue ( BYTE *buf, int &len )
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
}		/* -----  end of method CModbusEPS::WhetherBufValue  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusEPS
 *      Method:  ProcessYxBuf
 * Description:  ���⴦��ң������
 *       Input:	 ���ܻ����� ����
 *		Return:	 BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModbusEPS::ProcessYxBuf ( BYTE *buf, int len )
{
	WORD wPnt = 0;
	BYTE byYxValue = 0;
	BYTE byTemp;
	int i;
	char szBuf[256];
	/* ����DA1 */
	// ����DA1 bit0-bit3  0-13
	byTemp = buf[5] & 0x0f;	
	for( i = 0; i < 14; i++)
	{
		if( byTemp >= 14)
		{
			sprintf( szBuf, "yx update point=0 val=%d",  byYxValue );
			OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
			m_pMethod->SetYxData( m_SerialNo, 0, 1 );
			wPnt ++;
			continue;
		}
		if( byTemp == i )
		{
			byYxValue = 1;
		}
		else
		{
			byYxValue = 0;
		}

		sprintf( szBuf, "yx update point=%d val=%d", wPnt, byYxValue );
		OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );

		m_pMethod->SetYxData( m_SerialNo, wPnt, byYxValue );
		wPnt ++;
	}
	//���� DA1 bit4-bit5 14-17
	byTemp = ( buf[5] >> 4 ) & 0x03;
	for ( i=0; i<4; i++)
	{
		if( byTemp >= 4)
		{
			sprintf( szBuf, "yx update point=14 val=%d",  byYxValue );
			OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
			m_pMethod->SetYxData( m_SerialNo, 14, 1 );
			wPnt ++;
			continue;
		}
		if( byTemp == i )
		{
			byYxValue = 1;
		}
		else
		{
			byYxValue = 0;
		}
		
		sprintf( szBuf, "yx update point=%d val=%d", wPnt, byYxValue );
		OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );

		m_pMethod->SetYxData( m_SerialNo, wPnt, byYxValue );
		wPnt ++;
	}
	
	//���� DA1 bit6-bit7 18-21
	byTemp = ( buf[5] >> 6 ) & 0x03;
	for ( i=0; i<4; i++)
	{
		if( byTemp >= 4)
		{
			sprintf( szBuf, "yx update point=18 val=%d",  byYxValue );
			OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
			m_pMethod->SetYxData( m_SerialNo, 18, 1 );
			wPnt ++;
			continue;
		}
		if( byTemp == i )
		{
			byYxValue = 1;
		}
		else
		{
			byYxValue = 0;
		}

		sprintf( szBuf, "yx update point=%d val=%d", wPnt, byYxValue );
		OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );

		m_pMethod->SetYxData( m_SerialNo, wPnt, byYxValue );
		wPnt ++;
	}

	/* ��������DA2 buf[6] */
	//����DA2 bit0-bit1 22-25
	byTemp =  buf[6]  & 0x03;
	for ( i=0; i<4; i++)
	{
		if( byTemp >= 4)
		{
			sprintf( szBuf, "yx update point=22 val=%d",  byYxValue );
			OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
			m_pMethod->SetYxData( m_SerialNo, 22, 1 );
			wPnt ++;
			continue;
		}
		if( byTemp == i )
		{
			byYxValue = 1;
		}
		else
		{
			byYxValue = 0;
		}

		sprintf( szBuf, "yx update point=%d val=%d", wPnt, byYxValue );
		OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
		m_pMethod->SetYxData( m_SerialNo, wPnt, byYxValue );
		wPnt ++;
	}

	//����DA2 bit2-bit3 26-29
	byTemp = (buf[6] >> 2)  & 0x03;
	for ( i=0; i<4; i++)
	{
		if( byTemp >= 4)
		{
			sprintf( szBuf, "yx update point=26 val=%d",  byYxValue );
			OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
			m_pMethod->SetYxData( m_SerialNo, 26, 1 );
			wPnt ++;
			continue;
		}
		if( byTemp == i )
		{
			byYxValue = 1;
		}
		else
		{
			byYxValue = 0;
		}

		sprintf( szBuf, "yx update point=%d val=%d", wPnt, byYxValue );
		OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
		m_pMethod->SetYxData( m_SerialNo, wPnt, byYxValue );
		wPnt ++;
	}

	//����DA2 bit4-bit5 30-31
	byTemp = (buf[6] >> 4)  & 0x03;
	for ( i=0; i<2; i++)
	{
		if( byTemp >= 2)
		{
			sprintf( szBuf, "yx update point=30 val=1"  );
			OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
			m_pMethod->SetYxData( m_SerialNo, 30, 1 );
			wPnt ++;
			continue;
		}
		if( byTemp == i )
		{
			byYxValue = 1;
		}
		else
		{
			byYxValue = 0;
		}

		sprintf( szBuf, "yx update point=%d val=%d", wPnt, byYxValue );
		OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
		m_pMethod->SetYxData( m_SerialNo, wPnt, byYxValue );
		wPnt ++;
	}

	//����DA2 bit6-bit7 32-34
	byTemp = (buf[6] >> 6)  & 0x03;
	for ( i=0; i<3; i++)
	{
		if( byTemp >= 3)
		{
			sprintf( szBuf, "yx update point=32 val=%d",  byYxValue );
			OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
			m_pMethod->SetYxData( m_SerialNo, 32, 1 );
			wPnt ++;
			continue;
		}
		if( byTemp == i )
		{
			byYxValue = 1;
		}
		else
		{
			byYxValue = 0;
		}

		sprintf( szBuf, "yx update point=%d val=%d", wPnt, byYxValue );
		OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
		m_pMethod->SetYxData( m_SerialNo, wPnt, byYxValue );
		wPnt ++;
	}

	/* ����DA3 buf[3] */
	//����DA3 bit0-bit1 35-37
	byTemp = ( buf[3] )  & 0x03;
	for ( i=0; i<3; i++)
	{
		if( byTemp >= 3)
		{
			sprintf( szBuf, "yx update point=35 val=%d",  byYxValue );
			OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
			m_pMethod->SetYxData( m_SerialNo, 35, 1 );
			wPnt ++;
			continue;
		}
		if( byTemp == i )
		{
			byYxValue = 1;
		}
		else
		{
			byYxValue = 0;
		}

		sprintf( szBuf, "yx update point=%d val=%d", wPnt, byYxValue );
		OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
		m_pMethod->SetYxData( m_SerialNo, wPnt, byYxValue );
		wPnt ++;
	}

	//����DA3 bit2-bit3 38-39
	byTemp = (buf[3] >> 2)  & 0x03;
	for ( i=0; i<2; i++)
	{
		if( byTemp >= 2)
		{
			sprintf( szBuf, "yx update point=38 val=%d",  byYxValue );
			OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
			m_pMethod->SetYxData( m_SerialNo, 38, 1 );
			wPnt ++;
			continue;
		}
		if( byTemp == i )
		{
			byYxValue = 1;
		}
		else
		{
			byYxValue = 0;
		}

		sprintf( szBuf, "yx update point=%d val=%d", wPnt, byYxValue );
		OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
		m_pMethod->SetYxData( m_SerialNo, wPnt, byYxValue );
		wPnt ++;
	}

	/* ����DA4 buf[4] */
	//����DA4 bit4 bit5 bit6 bit7 40-43
	for ( i=0; i<4; i++)
	{
		byTemp = ( buf[4] >> (4 + i) ) ;
		if( byTemp & 0x01 )
		{
			byYxValue = 1;
		}
		else
		{
			byYxValue = 0;
		}

		sprintf( szBuf, "yx update point=%d val=%d", wPnt, byYxValue );
		OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );
		m_pMethod->SetYxData( m_SerialNo, wPnt, byYxValue );
		wPnt ++;
	}

	return TRUE;
}		/* -----  end of method CModbusEPS::ProcessYxBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusEPS
 *      Method:  ProcessRecvBuf
 * Description:  ������  ��������
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModbusEPS::ProcessRecvBuf ( BYTE *buf, int len )
{
	BYTE byPos;
	//�жϳ���
	if( len != 9 )
	{
		return FALSE;
	}

	byPos = GetSendPos(  );

	//λ����0 Ϊң�ţ� ����Ϊң��
	if( 0 == byPos)
	{
			
		return ProcessYxBuf( buf, len );
	}
	else
	{
		WORD wLWord;
		WORD wHWord;
		DWORD dwYcVal;
		float fYcVal;

		wHWord = MAKEWORD( buf[6], buf[5] );
		wLWord = MAKEWORD( buf[4], buf[3] );
		dwYcVal = MAKELONG( wLWord, wHWord );
		fYcVal = (float)dwYcVal;
		fYcVal = fYcVal * 0.001;

		if( fYcVal > 100000 )
		{
			return TRUE;
		}

		char szBuf[256];
		sprintf( szBuf, "yc update point=%d val=%f", byPos-1, fYcVal );
		OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen(szBuf), 2 );

		m_pMethod->SetYcData( m_SerialNo, byPos-1, fYcVal );
	}

	return TRUE;
}		/* -----  end of method CModbusEPS::ProcessRecvBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusEPS
 *      Method:  GetSendPos
 * Description:  ��ȡ����λ��
 *       Input:  void 
 *		Return:  BYTE ����λ��
 *--------------------------------------------------------------------------------------
 */
BYTE CModbusEPS::GetSendPos ( void )
{
	return ( m_bySendPos % MODBUSMASTER_EPS_MAX_POS );
}		/* -----  end of method CModbusEPS::GetSendPos  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusEPS
 *      Method:  ChangeSendPos
 * Description:  �ı䷢��λ��
 *       Input:	 void
 *		Return:  void
 *--------------------------------------------------------------------------------------
 */
void CModbusEPS::ChangeSendPos ( void )
{
	m_bySendPos = m_bySendPos++ % MODBUSMASTER_EPS_MAX_POS;

	if( m_bySendPos >= ( MODBUSMASTER_EPS_MAX_POS ) )
	{
		m_bySendPos = 0;
	}

}		/* -----  end of method CModbusEPS::ChangeSendPos  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusEPS
 *      Method:  GetProtocolBuf
 * Description:  ��ȡ���ͱ���  
 *       Input:  ������ ���� ��Ϣ����ң�ص���Ϣ���� ʼ��ΪNULL��
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModbusEPS::GetProtocolBuf ( BYTE *buf, int &len, PBUSMSG pBusMsg )
{
	if( pBusMsg != NULL )
	{
		return FALSE;	
	}

	ChangeSendPos(  );

	len = 0;

	buf[len++] = m_wDevAddr;
	buf[len++] = 0x03;

	buf[len++] = 0x00;
	buf[len++] = m_bySendPos;
	buf[len++] = 0x00;
	buf[len++] = 0x04;
	
	WORD wCRC = GetCrc( buf, len );
    buf[ len++ ] = HIBYTE(wCRC);
    buf[ len++ ] = LOBYTE(wCRC);

	m_bySendCount ++;

	return TRUE;
}		/* -----  end of method CModbusEPS::GetProtocolBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusEPS
 *      Method:  ProcessProtocolBuf
 * Description:  ������ձ���
 *       Input:  ������ ����
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModbusEPS::ProcessProtocolBuf ( BYTE *buf, int len )
{
	if ( !WhetherBufValue( buf, len ) )
	{
		char szBuf[256] = "";
		sprintf( szBuf, "%s",  "ModBusEPS recv buf err !!!\n" );
		OutBusDebug( m_byLineNo, (BYTE *)szBuf, strlen( szBuf ), 2 );

		m_byRecvCount ++;
		return FALSE;	
	}
	
	ProcessRecvBuf( buf, len );

	m_bLinkStatus = TRUE;
	m_bySendCount = 0;
	m_byRecvCount = 0;

	return TRUE;
}		/* -----  end of method CModbusEPS::ProcessProtocolBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusEPS
 *      Method:  Init
 * Description:  ��ʼ��Э��
 *       Input:  ���ߺ�
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModbusEPS::Init ( BYTE byLineNo )
{
	return TRUE;
}		/* -----  end of method CModbusEPS::Init  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusEPS
 *      Method:  TimerProc
 * Description:  ʱ�Ӵ���
 *       Input:  void
 *		Return:  void
 *--------------------------------------------------------------------------------------
 */
void CModbusEPS::TimerProc ( void )
{
	if( m_bySendCount > 3 || m_byRecvCount > 3)
	{
		m_bySendCount = 0;
		m_byRecvCount = 0;
		// m_bySendPos = 0;
		if( m_bLinkStatus  )
		{
			m_bLinkStatus = FALSE;
			OutBusDebug( m_byLineNo, (BYTE *)"CModbusEPS:unlink\n", 30, 2 );
		}
	}
}		/* -----  end of method CModbusEPS::TimerProc  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CModbusEPS
 *      Method:  GetDevCommState
 * Description:  ��ȡװ��״̬
 *       Input:  void
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CModbusEPS::GetDevCommState ( void )
{
	if ( m_bLinkStatus )
	{
		return COM_DEV_NORMAL;
	}
	else
	{
		return COM_DEV_ABNORMAL;
	}
}		/* -----  end of method CModbusEPS::GetDevCommState  ----- */




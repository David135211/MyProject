/*
 * =====================================================================================
 *
 *       Filename:  DDB.cpp
 *
 *    Description:  ˫������Э��
 *
 *        Version:  1.0
 *        Created:  2014��10��15�� 13ʱ35��00��
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
#include <unistd.h>
#include "../../BayLayer/CPublicMethod.h"
#include "DDB.h"


#define	DDBDEBUG	1				/* ������������������ڵ��Զ˿��ϴ�ӡ */
#define	DDBPRINT	1				/* �������������������UDP�˿��ϴ�ӡ */

/* in librtdb.so*/
extern "C" int  SetCurrentTime( REALTIME *pRealTime  );
extern "C" void GetCurrentTime( REALTIME *pRealTime  );
extern "C" void OutBusDebug(BYTE byBusNo, BYTE *buf, int len, int flag);
/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  CDDB
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
CDDB::CDDB ()
{
	m_byLocalStatus = STATUS_SLAVE;//��ʼ����Ϊ��
	m_byRemoteStatus = STATUS_MASTER;//Զ�̳�ʼ����Ϊ��


	m_bSwitchState = FALSE;
	m_bTimeProcCount = 0;

	m_byMachineId = IDENTITY_SINGLE;

	m_wAllYcNum = 0;
	m_wAllYxNum = 0;
	m_wAllYmNum = 0;

	m_pYcHeadAddr = 0;
	m_pYxHeadAddr = 0;
	m_pYmHeadAddr = 0;

	m_iDelayedSwitchMinute = 0;
	m_iDelayedSynSecond = 0;
	m_tmLastSwitchTime = 0;
	m_tmNowSwitchTime = 0;
	m_byQuickSwitchNum = 0;

	m_bRecvResponseSwitch = FALSE;

	// ��ʼ��Э��״̬
	InitProtocolStatus(  );
}  /* -----  end of method CDDB::CDDB  (constructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ~CDDB
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
CDDB::~CDDB ()
{
}  /* -----  end of method CDDB::~CDDB  (destructor)  ----- */


/* ********************** ���� *********************************************************/

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  DDBYxUpdate
 * Description:  //��ȡ�����ڴ��������͵���ʼλ�ú�����
 *       Input:	 ��������1��ң�� 2��ң�� 3��ң��  �����ڴ�վָ�� ��ʼλ�� ����
 *		Return:	 BOOL �����Ͳ���ʱ����FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::GetStnStartPosAndNum ( BYTE byDataType, STNPARAM *pStn, DWORD &dwStartPos, WORD &wCount )
{
	switch ( byDataType )
	{
		case 1://yx
			wCount = pStn->wDigitalSum;
			dwStartPos = pStn->dwDigitalPos;
			break;

		case 2://yc
			wCount = pStn->wAnalogSum;
			dwStartPos = pStn->dwAnalogPos;
			break;

		case 3://ym
			wCount = pStn->wPulseSum;
			dwStartPos = pStn->dwPulsePos;
			break;

		default:
			return FALSE;
			break;
	}				/* -----  end switch  ----- */

	return TRUE;
}		/* -----  end of method CDDB::DDBYxUpdate  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  GetSerialNoAndPnt
 * Description:  ͨ����ǰ���������ͺ����ݵ��λ�ã��������ź͵��
 *       Input:  byDataType:��������
 *       Input:  wPos:����λ��
 *       Input:  wSerialNo:Ҫ��õ�װ�����
 *       Input:  wPnt:Ҫ��õ�װ�õ��
 *       Input:  pStn:�м�Ĺ����ڴ�װ��ָ��
 *		Return: ������Ӧ���ͣ�byDataType����λ�ã�wPos������ʱ����false
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::GetSerialNoAndPnt (BYTE byDataType, WORD wPos, WORD &wSerialNo, WORD &wPnt, STNPARAM *pStn )
{
	int i;
	WORD wStnNum;
	WORD wCount;
	DWORD dwStartPos;

	// ������Ӧ���������ͣ�ȷ�������ĺ�����
	switch (byDataType)
	{
		case DDB_YX_DATATYPE: // yx
			{
				if( wPos > MAX_DIGITAL_SUM || wPos > m_pMethod->m_pRdbObj->m_nDigitalSum )
				{
					print ( (char *)"DDB:GetSerialNoAndPnt yx wPos is too big!!!" );
					return FALSE;
				}
			}
			break;

		case DDB_YC_DATATYPE://yc
			{
				if( wPos > MAX_ANALOG_SUM || wPos > m_pMethod->m_pRdbObj->m_nAnalogSum )
				{
					print ( (char *)"DDB:GetSerialNoAndPnt yc wPos is too big!!!" );
					return FALSE;
				}
			}
			break;

		case DDB_YM_DATATYPE://ym
			{
				if( wPos > MAX_PULSE_SUM || wPos > m_pMethod->m_pRdbObj->m_nPulseSum )
				{
					print ( (char *)"DDB:GetSerialNoAndPnt ym wPos is too big!!!" );
					return FALSE;
				}
			}
			break;
		default:
			return FALSE;
			break;
	}

	//��һ�ν��� Ѱ�ҷ���������
	if( pStn == NULL )
	{
		pStn = &m_pMethod->m_pRdbObj->m_pRTDBSpace->RTDBase.StnUnit[0];
		wStnNum = m_pMethod->m_pRdbObj->m_wStnSum;

		for ( i=0; i<wStnNum; i++, pStn++)
		{
			if ( !GetStnStartPosAndNum( byDataType, pStn, dwStartPos, wCount ) )
			{
				print ( (char *)"DDB:GetSerialNoAndPnt get stn startpos and num error!!!" );
				continue;
			}

			//������ʼλ�� С����ʼλ��+����
			if ( wPos >= dwStartPos && wPos < ( wCount + dwStartPos ) )
			{
				wSerialNo = pStn->wStnNum;
				wPnt = wPos - dwStartPos;
				break;
			}
		}
	}
	else//�Ժ��ڵ�һ�ε�pStn��������������
	{
		if ( !GetStnStartPosAndNum( byDataType, pStn, dwStartPos, wCount ) )
		{
			print ( (char *)"DDB:GetSerialNoAndPnt get stn startpos and num error!!!" );
		}

		if( wPos >= wCount + dwStartPos)
		{
			pStn ++;
		}

		wSerialNo = pStn->wStnNum;
		wPnt = wPos - dwStartPos;
	}

	return TRUE;
}		/* -----  end of method CDDB::GetSerialNoAndPnt  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  SwitchStatus
 * Description:  �л����� 1Ϊ�ӻ��� 0Ϊ������
 *       Input:	 BOOL bStatus Ϊ1ʱ���ñ���Ϊ����ģʽ
 *		Return:	 BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::SwitchStatus ( BOOL bStatus )
{
	if( bStatus )
	{
		// �����Ѿ��������򷵻�
		if( m_byLocalStatus == STATUS_MASTER )
		{
			return FALSE;
		}

		// ����Ϊ���� �Զ�Ϊ��
		m_byLocalStatus = STATUS_MASTER;
		m_byRemoteStatus = STATUS_SLAVE;

		print( (char *)"����Ϊ���� �Զ�Ϊ��\n"  );


		// if( m_bSyn )
		// {
			// m_SendStatus = NONESTATUS;
		// }
	}
	else
	{
		// �����Ѿ��Ǵӻ��򷵻�
		if( m_byLocalStatus == STATUS_SLAVE )
		{
			return FALSE;
		}

		// ����Ϊ�ӻ� �Զ�Ϊ��
		m_byLocalStatus = STATUS_SLAVE;
		m_byRemoteStatus = STATUS_MASTER;

		//
		m_bRecvResponseSwitch = TRUE;

		print( (char *)"����Ϊ�ӻ� �Զ�Ϊ��\n"  );

		// if( m_bSyn )
		// {
			// m_SendStatus = REQUEST_DATA;
			// m_byDataType = DDB_YX_DATATYPE;
			// m_wDataPos = 0;
		// }
	}
	//���±���״̬
	CPublicMethod::SetDDBSyncState( m_byLocalStatus );
	return TRUE;
}		/* -----  end of method CDDB::SwitchStatus  ----- */

BOOL CDDB::DevStateSwitchStatus ( BOOL bStatus )
{
	m_bTimeProcCount = 0;

	SwitchStatus( bStatus );

	time(&m_tmNowSwitchTime);
	time_t TimeDifference = abs(m_tmNowSwitchTime - m_tmLastSwitchTime);
	 printf("%lu = %lu -%lu  \n",TimeDifference,m_tmNowSwitchTime,m_tmLastSwitchTime);
	if( TimeDifference < m_iDelayedSynSecond+10 || m_byQuickSwitchNum == 0 )
	{
		m_tmLastSwitchTime = m_tmNowSwitchTime;
		m_byQuickSwitchNum++;
		printf("m_byQuickSwitchNum = %d\n",m_byQuickSwitchNum);
		if( m_byQuickSwitchNum > 6)
		{
			m_byQuickSwitchNum = 6;
		}
	}
	else
	{
		m_tmNowSwitchTime = 0;
		m_tmLastSwitchTime = 0;
		m_byQuickSwitchNum = 0;
	}

	InitProtocolStatus(  );

	// m_byDataType = DDB_YX_DATATYPE;//��ʼ�����ó�YX
	// m_bRemoteSyn = FALSE;
	// m_wDataPos = 0;
	// m_bIsReSend = FALSE;		//�ط���ʶλ0
	// m_byResendCount = 0;		//�ط���������
	// m_bIsSending = FALSE;		//���ͺ���1 ���պ�ֵ0
	print((char *)"~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	return TRUE;
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  GetDataNum
 * Description:  ��ȡ�����ڴ�����������
 *       Input:	 1:ң�� 2ң�� 3ң��
 *		Return:  ��Ӧ���ݵ�����
 *--------------------------------------------------------------------------------------
 */
WORD CDDB::GetDataNum ( BYTE byDataType )
{
	WORD wDataSum = 0;
	switch ( byDataType )
	{
		case 1://ң��
			wDataSum = m_pMethod->m_pRdbObj->m_nDigitalSum;
			break;

		case 2://ң��
			wDataSum = m_pMethod->m_pRdbObj->m_nAnalogSum;
			break;

		case 3://ң��
			wDataSum = m_pMethod->m_pRdbObj->m_nPulseSum;
			break;

		default:
			break;
	}				/* -----  end switch  ----- */
	return wDataSum;
}		/* -----  end of method CDDB::GetDataNum  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  GetDataHeadAddr
 * Description:	 ��ȡ�����׵�ַ
 *       Input:  1:ң���׵�ַ 2��ң���׵�ַ 3��ң���׵�ַ
 *		Return:	 ָ���׵�ַ
 *--------------------------------------------------------------------------------------
 */
DWORD CDDB::GetDataHeadAddr ( BYTE byDataType )
{
	DWORD dwHeadAddr = 0;

	switch ( byDataType )
	{
		case 1://ң��
			dwHeadAddr = (DWORD)&m_pMethod->m_pRdbObj->m_pRTDBSpace->RTDBase.DigitalTable[0] ;
			break;

		case 2://ң��
			dwHeadAddr = (DWORD)&m_pMethod->m_pRdbObj->m_pRTDBSpace->RTDBase.AnalogTable[0] ;
			break;

		case 3://ң��
			dwHeadAddr = (DWORD)&m_pMethod->m_pRdbObj->m_pRTDBSpace->RTDBase.PulseTable[0] ;
			break;

		default:
			break;
	}				/* -----  end switch  ----- */

	return dwHeadAddr;
}		/* -----  end of method CDDB::GetDataHeadAddr  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  GetCommonFrame
 * Description:  ��ȡͨ�õ�����֡buf[0]=68 buf[1]=16 buf[3]=funcode|id|status
 *       Input:	 ������ ������
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDDB::GetCommonFrame ( BYTE *buf, BYTE byFuncCode )
{
	BYTE byFrameType;

	buf[0]=0x68;
	buf[1]=0x16;

	byFrameType = byFuncCode & 0x0F;

	if (m_byMachineId == IDENTITY_B)
		byFrameType |= 0x10;   /* B->1*/

	if(m_byLocalStatus == STATUS_SLAVE)
		byFrameType |= 0x20;   /* 1*/

	buf[3] = byFrameType;
}		/* -----  end of method CDDB::GetCommonFrame  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  GetCrc
 * Description:	 ����У���� У�鷽ʽ����MMI��crcУ��
 *       Input:
 *		Return:	 WORD
 *--------------------------------------------------------------------------------------
 */
WORD CDDB::GetCrc ( BYTE *pBuf, int nLen )
{

	WORD Genpoly=0xA001;
	WORD CRC=0xFFFF;
	WORD index;
	while(nLen--)
	{
		CRC=CRC^(WORD)*pBuf++;
		for(index=0;index<8;index++)
		{
			if((CRC & 0x0001)==1)
				CRC=(CRC>>1)^Genpoly;
			else
				CRC=CRC>>1;
		}
	}

	return CRC;
}		/* -----  end of method CDDB::GetCrc  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  print
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDDB::print ( char *szBuf )
{

#ifdef  DDBDEBUG
	printf ( "%s", szBuf );
#endif     /* -----  not DDBDEBUG  ----- */

#ifdef DDBPRINT
	OutBusDebug(m_byLineNo, (BYTE *)szBuf,strlen(szBuf) , 2);
#endif


}		/* -----  end of method CDDB::print  ----- */

/* **********************������ ***************************************/
/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  YcDataDeal
 * Description:  ң�⴦����
 *       Input:  ������ ����
 *		Return:	 BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::YcDataDeal ( BYTE *buf, int len )
{
	WORD wDataNum;
	WORD wDataPos;
	DWORD dwYcVal;
	STNPARAM *pStn = NULL;
	BYTE *pointer = buf;
	WORD wSerialNo, wPnt;

	wDataNum = ( pointer[5] << 8 ) | pointer[6];
	wDataPos = ( pointer[7] << 8 ) | pointer[8];

	pointer += 9;
	while( wDataNum > 0 )
	{
		memcpy( &dwYcVal, pointer, 4 );

		if ( GetSerialNoAndPnt( 2, wDataPos, wSerialNo, wPnt, pStn  ) )
		{
			m_pMethod->SetYcData( wSerialNo, wPnt, (float)dwYcVal );
		}

		pointer += 4;
		wDataPos++;
		wDataNum--;
	}

	m_wDataPos = wDataPos;
	return TRUE;
}		/* -----  end of method CDDB::YcDataDeal  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  YxDataDeal
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::YxDataDeal ( BYTE *buf, int len )
{
	WORD wDataNum;
	WORD wDataPos;
	BYTE byTmp, i;
	BYTE *pointer = buf;
	STNPARAM *pStn = NULL;
	WORD wSerialNo, wPnt;
	BYTE byYxVal;

	wDataNum = ( pointer[5] << 8 ) | pointer[6];
	wDataPos = ( pointer[7] << 8 ) | pointer[8];

	pointer += 9;

	while( wDataNum > 0 )
	{
		byTmp = *pointer;
		for(i=0; i<8; i++)
		{
			if( byTmp & ( 0x01 << i ) )
			{
				byYxVal = 1;
			}
			else
			{
				byYxVal = 0;
			}

			if ( GetSerialNoAndPnt (1,wDataPos, wSerialNo, wPnt, pStn ) )
			{
				m_pMethod->SetYxData( wSerialNo, wPnt, byYxVal );
			}

			wDataNum -- ;
			wDataPos ++;

			if( wDataNum == 0 )
				break;
		}

		pointer ++;
	}

	m_wDataPos = wDataPos;
	return TRUE;
}		/* -----  end of method CDDB::YxDataDeal  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  YmDataDeal
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::YmDataDeal ( BYTE *buf, int len )
{
	WORD wDataNum;
	WORD wDataPos;
	DWORD dwYmVal;
	BYTE *pointer = buf;
	WORD wSerialNo, wPnt;
	STNPARAM *pStn = NULL;

	wDataNum = ( pointer[5] << 8 ) | pointer[6];
	wDataPos = ( pointer[7] << 8 ) | pointer[8];

	pointer += 9;

	while( wDataNum > 0 )
	{
		memcpy( &dwYmVal, pointer, 4 );

		if ( GetSerialNoAndPnt( 3, wDataPos, wSerialNo, wPnt, pStn  ) )
		{
			m_pMethod->SetYmData( wSerialNo, wPnt, dwYmVal );
		}
		pointer += 4;
		wDataPos++;
		wDataNum--;
	}

	m_wDataPos = wDataPos;
	return TRUE;
}		/* -----  end of method CDDB::YmDataDeal  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  TimeSyncDeal
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL  CDDB::TimeSyncDeal ( BYTE *buf, int len )
{
	REALTIME CurrTime;

	CurrTime.wYear = ( buf[5] << 8 ) | buf[6];
	CurrTime.wMonth = buf[7];
	CurrTime.wDay = buf[8];
	CurrTime.wHour = buf[9];
	CurrTime.wMinute = buf[10];
	CurrTime.wSecond = buf[11];
	CurrTime.wMilliSec = ( buf[12] << 8 ) | buf[13];

	SetCurrentTime( &CurrTime );
	return TRUE;
}		/* -----  end of method CDDB::TimeSyncDeal  ----- */


/*******************************************************************************
 * ��:CDDB
 * ������:LinkBusStatusDela
 * ��������:����״̬����
 * ����:BYTE *buf, int len
 * ����ֵ:BOOL
 ******************************************************************************/
BOOL CDDB::LinkBusStatusDeal(BYTE *buf, int len)
{
	WORD wDataNum;
	WORD wDataPos;
	BYTE *pointer = buf;

	// ����������λ��
	wDataNum = ( pointer[5] << 8 ) | pointer[6];
	wDataPos = ( pointer[7] << 8 ) | pointer[8];

	pointer += 8;

	while( wDataNum > 0 )
	{
		pointer ++;
		for (int i = 0; i < 8; i++)
		{
			BOOL bStatus = ( *pointer >> i ) & 0x01;
			CPublicMethod::SetDDBBusLinkStatus(wDataPos, bStatus);
			// printf("deal bus%d status=%d\n", wDataPos, bStatus);

			wDataPos++;
			wDataNum--;
			if( 0 == wDataNum )
			{
				break;
			}
		}
	}

	m_wDataPos = wDataPos;
    return TRUE;
}   /*-------- end class CDDB method LinkBusStatusDela -------- */

/*******************************************************************************
 * ��:CDDB
 * ������:LinkStnStatusDeal
 * ��������:װ��״̬����
 * ����:BYTE *buf, int len
 * ����ֵ:BOOL
 ******************************************************************************/
BOOL CDDB::LinkStnStatusDeal(BYTE *buf, int len)
{
	WORD wDataNum;
	WORD wDataPos;
	BYTE *pointer = buf;

	// ����������λ��
	wDataNum = ( pointer[5] << 8 ) | pointer[6];
	wDataPos = ( pointer[7] << 8 ) | pointer[8];

	pointer += 8;

	while( wDataNum > 0 )
	{
		pointer ++;
		for (int i = 0; i < 8; i++)
		{
			BOOL bStatus = ( *pointer >> i ) & 0x01;
			CPublicMethod::SetDDBStnLinkStatus(wDataPos, bStatus);
			printf("deal stn%d status=%d\n", wDataPos, bStatus);
			wDataPos++;
			wDataNum--;
			if( 0 == wDataNum )
			{
				break;
			}
		}
	}

	m_wDataPos = wDataPos;
    return TRUE;

}   /*-------- end class CDDB method LinkStnStatusDeal -------- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ProcessRecvDataBuf
 * Description:  ��������
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ProcessRecvDataBuf ( BYTE *buf, int len )
{
	BOOL bRtn = FALSE;

	switch ( buf[4] )
	{
		case DDB_YC_DATATYPE:
			bRtn = YcDataDeal( buf, len );
			if( buf[3] & 0x80 )
			{
				print( (char *)"yc end\n" );
				m_byDataType = DDB_YM_DATATYPE;
				m_wDataPos = 0;
			}
			break;

		case DDB_YX_DATATYPE:
			bRtn = YxDataDeal( buf, len );
			if( buf[3] & 0x80 )
			{
				print( (char *)"yx end\n" );
				m_byDataType = DDB_YC_DATATYPE;
				m_wDataPos = 0;
			}
			break;

		case DDB_YM_DATATYPE:
			bRtn = YmDataDeal( buf, len );
			if( buf[3] & 0x80 )
			{
				print( (char *)"ym end\n" );
				m_byDataType = DDB_LINKBUSSTATUS_DATATYPE;
				m_wDataPos = 0;
			}
			break;

		case DDB_TIME_DATATYPE:
			bRtn = TimeSyncDeal( buf, len );
			print( (char *)"timesync end\n" );
			m_byDataType = DDB_YX_DATATYPE;
			m_wDataPos = 0;
			break;


		case DDB_LINKBUSSTATUS_DATATYPE://����״̬����
			{
					print( (char *)"bus begin\n" );
				LinkBusStatusDeal( buf,len);
				if( buf[3] & 0x80 )
				{
					print( (char *)"bus end\n" );
					m_byDataType = DDB_LINKSTNSTATUS_DATATYPE;
					m_wDataPos = 0;
				}
			}
			break;

		case DDB_LINKSTNSTATUS_DATATYPE://װ��״̬����
			{
				LinkStnStatusDeal( buf, len );
				if( buf[3] & 0x80 )
				{
					print( (char *)"stn end\n" );
					m_byDataType = DDB_TIME_DATATYPE;
					m_wDataPos = 0;
				}
			}
			break;

		case DDB_YK_DATATYPE:
			print( (char *)"yk \n" );
			if( m_bIsYking )
			{
				m_bIsYking = FALSE;
				m_byDataType = m_bySaveDataType;
				// m_byDataType = DDB_YX_DATATYPE;
			}
			else
			{
				PackSendYkMsg( &buf[5] );
			}
			break;

		default:
			break;
	}				/* -----  end switch  ----- */

	return bRtn;
}		/* -----  end of method CDDB::ProcessRecvDataBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ProcessRequeStSyn
 * Description:  ����ͬ�����ݴ���
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ProcessRequestSyn ( BYTE *buf, int len )
{
	m_SendStatus = RESPONSE_SYN;
	return TRUE;
}		/* -----  end of method CDDB::ProcessRequeStSyn  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ProcessResponseSyn
 * Description:	 ��Ӧͬ�����Ĵ���
		//���Զ��δͬ�� �Լ�Ҳδͬ��������A��B�� ����Է���ͬ�� ����δͬ�� ��Է����򱾵ش� �Է����򱾵�����ԭ��
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ProcessResponseSyn ( BYTE *buf, int len )
{
/*	if( ( buf[3] >> 5 ) & 0x01 )
		m_byRemoteStatus = STATUS_SLAVE;
	else
		m_byRemoteStatus = STATUS_MASTER;
*/
	m_bRemoteSyn = buf[4];
	//δͬ��
	if( !m_bSyn )
	{
		// //�Է���ͬ��
		// if( m_bRemoteSyn )
		// {
			// //Զ�����򱾵ش�
			// if( m_byRemoteStatus == STATUS_MASTER )
			// {
				// //������Ϊ��
				// m_byLocalStatus = STATUS_SLAVE;
			// }
			// else if( m_byRemoteStatus == STATUS_SLAVE )
			// {
				// //������Ϊ��
				// m_byLocalStatus = STATUS_MASTER;
			// }
			// m_SendStatus = RESPONSE_SYN;
		// }
		// else
		// {
			// //˫����δͬ�� ����A ��B�ӵ�ԭ��
			// if( m_byMachineId == IDENTITY_A )
			// {
				// m_byLocalStatus = STATUS_MASTER;
				// m_byRemoteStatus = STATUS_SLAVE;
			// }
			// else
			// {
				// m_byRemoteStatus = STATUS_MASTER;
				// m_byLocalStatus = STATUS_SLAVE;
			// }
		// }

		print ( (char *)( "����״̬Ϊ %d\n" ));
		if( m_byLocalStatus == STATUS_SLAVE )
		{
			//������Ϊ�� ��������
			m_SendStatus = REQUEST_DATA;
		}
		else
		{
			// //����Ϊ�� �ȴ�ѯ������
			m_SendStatus = NONESTATUS;
		}
		//����Ϊң��
		m_byDataType = DDB_YX_DATATYPE;

		//����ͬ��״̬
		m_bSyn = TRUE;
		// CPublicMethod::SetDDBSyncState( m_bSyn );
	}
	else
	{
		// if( !m_bRemoteSyn )
		// {
			// m_SendStatus = RESPONSE_SYN;
		// }
		// else
		// {
			if( m_byLocalStatus == STATUS_SLAVE )
			{
				//������Ϊ�� ��������
				m_SendStatus = REQUEST_DATA;
			}
			else
			{
				//������Ӧ
				m_SendStatus = RESPONSE_SYN;
				// //����Ϊ�� �ȴ�ѯ������
				// m_SendStatus = NONESTATUS;
			}
			//����Ϊң��
			m_byDataType = DDB_YX_DATATYPE;
		// }
	}

	return TRUE;
}		/* -----  end of method CDDB::ProcessResponseSyn  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ProcessRequestData
 * Description:	 �յ��������� ��Ϊ��������ģʽ
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ProcessRequestData ( BYTE *buf, int len )
{
	m_byDataType = buf[4];
	m_SendStatus = RESPONSE_DATA;

	if( DDB_YK_DATATYPE == m_byDataType
			&& STATUS_MASTER == m_byLocalStatus )
	{
		print( (char *)"PackSendYkMsg\n" );
		PackSendYkMsg( &buf[5] );
	}
	return TRUE;
}		/* -----  end of method CDDB::ProcessRequestData  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ProcessResponseData
 * Description:  �����յ�������
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ProcessResponseData ( BYTE *buf, int len )
{
	// SwitchStatus( 1 );
	return ProcessRecvDataBuf( buf, len );
}		/* -----  end of method CDDB::ProcessResponseData  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ProcessRequestSwitch
 * Description:  ����ת��ģʽ����
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ProcessRequestSwitch ( BYTE *buf, int len )
{
	/*��ת����*/
	DevStateSwitchStatus(1);

	m_SendStatus = RESPONSE_SWITCH;

	return TRUE;
}		/* -----  end of method CDDB::ProcessRequestSwitch  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ProcessResponseSwitch
 * Description:  ����ת����Ӧ
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ProcessResponseSwitch ( BYTE *buf, int len )
{
	DevStateSwitchStatus(0);

	m_SendStatus = REQUEST_DATA;

	return TRUE;
}		/* -----  end of method CDDB::ProcessResponseSwitch  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  PackSendYkMsg
 * Description:  ��֯�ͷ���yk����
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::PackSendYkMsg ( BYTE *buf )
{
	m_YkStatus = (DDBYKSTATUS)buf[0];

	m_SaveDestYkData.byDestBusNo = buf[1];
	m_SaveDestYkData.wDestAddr = MAKEWORD( buf[3], buf[2] );
	m_SaveDestYkData.wPnt = MAKEWORD( buf[5] , buf[4]);
	m_SaveDestYkData.byVal = buf[6];

	if( STATUS_SLAVE == m_byLocalStatus )
	{
		CPublicMethod::SetDDBDevBusAndAddr( m_SaveDestYkData.byDestBusNo, m_SaveDestYkData.wDestAddr );
	}

	print((char *)"CDDB::PackSendYkMsg\n");
	switch ( m_YkStatus )
	{
		case DDB_YK_SEL:
			print((char *)"CDDB::YkSel\n");
			m_pMethod->SetYkSel( this,
					m_SaveDestYkData.byDestBusNo,
					m_SaveDestYkData.wDestAddr,
					m_SaveDestYkData.wPnt,
					m_SaveDestYkData.byVal);
			m_YkStatus = DDB_YK_SEL_CONFIRM;
			break;

		case DDB_YK_EXE:
			print((char *)"CDDB::YkExe\n");
			m_pMethod->SetYkExe( this,
					m_SaveDestYkData.byDestBusNo,
					m_SaveDestYkData.wDestAddr,
					m_SaveDestYkData.wPnt,
					m_SaveDestYkData.byVal);
			m_YkStatus = DDB_YK_EXE_CONFIRM;
			break;

		case DDB_YK_CANCEL:
			print((char *)"CDDB::YkCancel\n");
			m_pMethod->SetYkCancel( this,
					m_SaveDestYkData.byDestBusNo,
					m_SaveDestYkData.wDestAddr,
					m_SaveDestYkData.wPnt,
					m_SaveDestYkData.byVal);
			m_YkStatus = DDB_YK_CANCEL_CONFIRM;
			break;

		case DDB_YK_SEL_RTN:
			print((char *)"CDDB::YkSelRtn\n");
			m_pMethod->SetYkSelRtn( this,
					m_bySaveSrcBusNo,
					m_wSaveSrcDevAddr,
					m_SaveDestYkData.wPnt,
					m_SaveDestYkData.byVal);
			m_YkStatus = DDB_YK_SEL_RTN_CONFIRM;
			break;

		case DDB_YK_EXE_RTN:
			print((char *)"CDDB::YkExeRtn\n");
			m_pMethod->SetYkExeRtn( this,
					m_bySaveSrcBusNo,
					m_wSaveSrcDevAddr,
					m_SaveDestYkData.wPnt,
					m_SaveDestYkData.byVal);
			m_YkStatus = DDB_YK_EXE_RTN_CONFIRM;
			break;

		case DDB_YK_CANCEL_RTN:
			print((char *)"CDDB::YkCancelRtn\n");
			m_pMethod->SetYkCancelRtn( this,
					m_bySaveSrcBusNo,
					m_wSaveSrcDevAddr,
					m_SaveDestYkData.wPnt,
					m_SaveDestYkData.byVal);
			m_YkStatus = DDB_YK_CANCEL_RTN_CONFIRM;
			break;

		default:
			print((char *)"CDDB::YkError\n");
			InitProtocolStatus( );
			break;
	}				/* -----  end switch  ----- */

	if( DDB_YK_DATATYPE != m_byDataType )
		m_bySaveDataType = m_byDataType;
	m_byDataType = DDB_YK_DATATYPE;

	return TRUE;
}		/* -----  end of method CDDB::PackSendYkMsg  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  WhetherBufValue
 * Description:  �ж�103���ĵ���Ч�� �����ж�
 *       Input:  �յ��Ļ�����buf �յ������ݳ���len ��������Чλ��pos
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::WhetherBufValue (BYTE *buf, int &len, int &pos )
{
	char szBuf[256];
	BYTE *pointer = buf;
	WORD wCrc;
	BYTE byHCrc, byLCrc;
	BYTE byDataLen;
	pos = 0;

	if( buf == NULL || len <= 0 )
		return FALSE;

	while ( len > 0 )
	{
		if( 0x68 == *pointer && 0x16 == *(pointer+1))
		{
			byDataLen = *(pointer+2);
			if(byDataLen+5 > len)
			{
				len--;
				pointer++;
				pos ++;
				sprintf (szBuf, "len err len=%d byDataLen=%d\n", len, byDataLen );
				print( szBuf );
				continue;
			}
			wCrc = GetCrc( pointer+3, byDataLen);
			byHCrc = (wCrc >> 8) & 0xff;
			byLCrc = wCrc & 0xff;
			if ( byHCrc != *(pointer + 3 + byDataLen) || byLCrc != *( pointer + 4 + byDataLen ))
			{
				len--;
				pointer++;
				pos ++;
				print ( (char *)"crc err\n" );
				continue;
			}

			return TRUE;
		}
		else
		{
			len--;
			pointer++;
			pos ++;
		}
	}

	return FALSE;
}		/* -----  end of method CDDB::WhetherBufValue  ----- */



/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  JudgeStatus
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDDB::JudgeStatus ( void )
{
	if( m_byRemoteStatus == m_byLocalStatus )
	{
		if( m_byMachineId == IDENTITY_A )
		{
			SwitchStatus( 1 );
		}
		if( m_byMachineId == IDENTITY_B )
		{
			SwitchStatus( 0 );
		}
	}
}		/* -----  end of method CDDB::JudgeStatus  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  JudgeStatus
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDDB::JudgeStatus ( BYTE byRemoteByte )
{
	if( ( byRemoteByte >> 5 ) & 0x01 )
	{
		m_byRemoteStatus = STATUS_SLAVE;
	}
	else
	{
		m_byRemoteStatus = STATUS_MASTER;
	}

	JudgeStatus(  );
}		/* -----  end of method CDDB::JudgeStatus  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ProcessRecvTypeBuf
 * Description:  �����������
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ProcessRecvTypeBuf ( BYTE *buf, int len  )
{
	 //printf ( "buf[3]&0xf=%d\n", buf[3]&0xf );
	JudgeStatus( buf[3] );

	switch ( buf[3]&0x0f )
	{
		case REQUEST_SYN:
			print ( (char *)"DDB RECV REQUEST_SYN\n" );
			ProcessRequestSyn( buf, len );
			break;

		case RESPONSE_SYN:
			print ( (char *)"DDB RECV RESPONSE_SYN\n" );
			ProcessResponseSyn( buf, len );
			break;

		case REQUEST_DATA:
			// print ( (char *)"DDB RECV REQUEST_DATA" );
			if( buf[4] == DDB_YK_DATATYPE && m_bIsYking)
			{
				print( (char *)"yk rtn\n" );
				m_bIsYking = FALSE;
				m_byDataType = m_bySaveDataType;
			}
			else
			{
				ProcessRequestData( buf, len );
			}
			break;

		case RESPONSE_DATA:
			// print ( (char *)"DDB RECV RESPONSE_DATA" );
			ProcessResponseData( buf, len );
			break;

		case REQUEST_SWITCH:
			ProcessRequestSwitch( buf, len );
			break;

		case RESPONSE_SWITCH:
			ProcessResponseSwitch( buf, len );
			break;

		default:
			break;
	}				/* -----  end switch  ----- */
	return TRUE;
}		/* -----  end of method CDDB::ProcessRecvTypeBuf  ----- */


/* **********************���Ͳ��� ***************************************/

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  YcDataSend
 * Description:	 ���ڴ����ݿ��л�ȡ�վݷ���
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::YcDataSend ( BYTE *buf, int &len )
{
	WORD wYcDataNum = m_wAllYcNum;
	ANALOGITEM *pYcData = m_pYcHeadAddr;
	WORD wMaxByte;
	WORD wLeaveYcNum;
	WORD wSendNum = 0;

	GetCommonFrame(buf, RESPONSE_DATA);
	buf[4] = DDB_YC_DATATYPE;
	buf[7] = (m_wDataPos >> 8) & 0xff;
	buf[8] = m_wDataPos & 0xff;

	pYcData += m_wDataPos;
	wMaxByte = DDB_MAX_BUF_LEN - 11;
	wLeaveYcNum = wYcDataNum - m_wDataPos;
	len = 9;

	while(wMaxByte >= 4 && wLeaveYcNum > 0)
	{
		memcpy(buf+len, &(pYcData->dwRawVal), 4);
		len += 4;
		wMaxByte -= 4;

		pYcData ++;
		wLeaveYcNum --;
		wSendNum ++;
	}

	buf[5] = (wSendNum >> 8) & 0xff;
	buf[6] = wSendNum & 0xff;
	buf[2] = len-3;

	m_wDataPos += wSendNum;

	if( wLeaveYcNum == 0 )
	{
		buf[3] |= 0x80;
		m_wDataPos = 0;
	}

	return TRUE;
}		/* -----  end of method CDDB::YcDataSend  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  YxDataSend
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::YxDataSend ( BYTE *buf, int &len )
{
	WORD wYxDataNum = m_wAllYxNum;
	DIGITALITEM *pYxData = m_pYxHeadAddr;
	WORD wMaxByte;
	WORD wLeaveYxNum;
	BYTE byTmp, i;
	WORD wSendNum = 0;

	GetCommonFrame(buf, RESPONSE_DATA);
	buf[4] = DDB_YX_DATATYPE;
	buf[7] = (m_wDataPos >> 8) & 0xff;
	buf[8] = m_wDataPos & 0xff;

	pYxData += m_wDataPos;
	wMaxByte = DDB_MAX_BUF_LEN - 11;
	wLeaveYxNum = wYxDataNum - m_wDataPos;
	len = 9;

	while(wMaxByte > 0 && wLeaveYxNum > 0)
	{
		byTmp = 0x00;

		for ( i=0; i<8; i++)
		{
			if ( ( pYxData->wStatus & 0x01 ) == 1 )
			{
				byTmp |= (0x01 << i);
			}
			pYxData++;
			wLeaveYxNum--;
			wSendNum++;
			if(wLeaveYxNum == 0 )
				break;
		}

		buf[len++] = byTmp;
		wMaxByte -- ;
	}


	buf[5] = (wSendNum >> 8) & 0xff;
	buf[6] = wSendNum & 0xff;
	buf[2] = len-3;

	m_wDataPos += wSendNum ;

	if( wLeaveYxNum == 0 )
	{
		buf[3] |= 0x80;
		m_wDataPos = 0;
	}

	return TRUE;
}		/* -----  end of method CDDB::YxDataSend  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  YmDataSend
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::YmDataSend ( BYTE *buf, int &len )
{
	WORD wYmDataNum = m_wAllYmNum;
	PULSEITEM *pYmData = m_pYmHeadAddr;
	WORD wMaxByte;
	WORD wLeaveYmNum;
	WORD wSendNum = 0;

	GetCommonFrame(buf, RESPONSE_DATA);
	buf[4] = DDB_YM_DATATYPE;
	buf[7] = (m_wDataPos >> 8) & 0xff;
	buf[8] = m_wDataPos & 0xff;

	pYmData += m_wDataPos;
	wMaxByte = DDB_MAX_BUF_LEN - 11;
	wLeaveYmNum = wYmDataNum - m_wDataPos;
	len = 9;

	while(wMaxByte >= 4 && wLeaveYmNum > 0)
	{
		memcpy(buf+len, &(pYmData->dwRawVal), 4);

		len += 4;
		wMaxByte -= 4;
		pYmData ++;
		wLeaveYmNum --;
		wSendNum ++;
	}

	m_wDataPos += wSendNum;

	buf[5] = (wSendNum >> 8) & 0xff;
	buf[6] = wSendNum & 0xff;
	buf[2] = len-3;

	if ( wLeaveYmNum == 0 )
	{
		buf[3] |= 0x80;
		m_wDataPos = 0;
	}
	return TRUE;
}		/* -----  end of method CDDB::YmDataSend  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  TimeSyncSend
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::TimeSyncSend ( BYTE *buf, int &len  )
{
	REALTIME CurrTime;

	GetCurrentTime( &CurrTime );
	GetCommonFrame( buf, RESPONSE_DATA );

	buf[4] = DDB_TIME_DATATYPE;
	buf[5] = ( CurrTime.wYear >> 8) & 0xff;
	buf[6] = CurrTime.wYear & 0xff;
	buf[7] = CurrTime.wMonth;
	buf[8] = CurrTime.wDay;
	buf[9] = CurrTime.wHour;
	buf[10] = CurrTime.wMinute;
	buf[11] = CurrTime.wSecond;
	buf[12] = ( CurrTime.wMilliSec >> 8) & 0xff;
	buf[13] = ( CurrTime.wMilliSec ) & 0xff;

	len = 14;
	buf[2] = len-3;

	return TRUE;
}		/* -----  end of method CDDB::TimeSyncSend  ----- */


/*******************************************************************************
 * ��:CDDB
 * ������:LinkBusStatusSend
 * ��������:����״̬����
 * ����: BYTE *buf ���ͻ���
 * ����: int &len�����ͳ���
 * ����ֵ:BOOL
 ******************************************************************************/
BOOL CDDB::LinkBusStatusSend( BYTE *buf, int &len )
{
	BYTE byBusNum = m_pMethod->GetToTalBusNum();
	int i = 0;
	BYTE byStatus = 0xff;

	GetCommonFrame( buf, RESPONSE_DATA );

	buf[4] = DDB_LINKBUSSTATUS_DATATYPE;//�������͡�
	// ����������������һ֡����
	buf[5] = ( ( byBusNum ) >> 8) & 0xff;
	buf[6] = ( byBusNum ) & 0xff;

	// ������ʼλ�á��˴�ֻ���ǣ�
	buf[7] = (m_wDataPos >> 8) & 0xff;
	buf[8] = m_wDataPos & 0xff;

	// �ж�����
	if( MAX_LINE < byBusNum || 0 == byBusNum)
	{
		return FALSE;
	}

	len = 9;
	for (i = 0; i < byBusNum; i++)
	{
		// ÿ���ֽ��У���ͨѶ״̬����
		BOOL bStatus = m_pMethod->GetCommState(i);
		CPublicMethod::SetDDBBusLinkStatus(i, bStatus);
		// printf("bus%d status=%d\n", i, bStatus);
		if( bStatus )
		{
			byStatus |= ( ( bStatus << ( i % 8)) );
		}
		else
		{
			byStatus &= ( ( bStatus << ( i % 8)) | ( 0xff >> (8 - ( i % 8))));
		}

		if ( 0 == ( ( i+1 )%8 ) )
		{
			buf[len++] = byStatus;
			byStatus = 0xff;
		}
	}

	if( 0 != ( byBusNum % 8 ))
	{
		buf[len++] = byStatus;
	}

	if( i != byBusNum)
	{
		return FALSE;
	}

	buf[2] = len - 3;
	buf[3] |= 0x80;
	m_wDataPos = 0;

	return TRUE;
}   /*-------- end class CDDB method LinkBusStatusSend -------- */

/*******************************************************************************
 * ��:CDDB
 * ������:LinkStnStatusSend
 * ��������:װ��״̬����
 * ����: BYTE *buf ���ͻ���
 * ����: int &len�����ͳ���
 * ����ֵ:BOOL
 ******************************************************************************/
BOOL CDDB::LinkStnStatusSend(BYTE *buf, int &len)
{
	WORD wSerialNum = m_pMethod->m_pRdbObj->m_wStnSum;
	//int i = 0;
	BYTE byStatus = 0xff;
	WORD wMaxByte;
	WORD wLeaveStatusNum;
	WORD wSendNum = 0;

	GetCommonFrame( buf, RESPONSE_DATA );

	buf[4] = DDB_LINKSTNSTATUS_DATATYPE;
	// ������ʼλ�á��˴�ֻ���ǣ�
	buf[7] = (m_wDataPos >> 8) & 0xff;
	buf[8] = m_wDataPos & 0xff;

	wMaxByte = DDB_MAX_BUF_LEN - 11;
	wLeaveStatusNum = ( wSerialNum - m_wDataPos );
	len = 9;

	while(wMaxByte > 1 && wLeaveStatusNum > 0)
	{
		BOOL bStatus = m_pMethod->GetDevCommState( m_wDataPos );
		// printf("stn%d status=%d\n", m_wDataPos, bStatus);
		CPublicMethod::SetDDBStnLinkStatus(m_wDataPos, bStatus);
		if( bStatus )
		{
			byStatus |= (bStatus << ( wSendNum % 8) );
		}
		else
		{
			byStatus &= ( ( bStatus << ( wSendNum % 8)) | ( 0xff >> ( 8 - (wSendNum % 8 ))));
		}

		if ( 0 == ( ( wSendNum+1 )%8 ) )
		{
			buf[len++] = byStatus;
			byStatus = 0xff;
			wMaxByte -= 1;
		}

		m_wDataPos++;
		wSendNum ++;
		wLeaveStatusNum --;
	}

	if( 0 != ( wSendNum % 8 ))
	{
		buf[len++] = byStatus;
	}

	buf[5] = (wSendNum >> 8) & 0xff;
	buf[6] = wSendNum & 0xff;
	buf[2] = len-3;

	if ( wLeaveStatusNum == 0 )
	{
		buf[3] |= 0x80;
		m_wDataPos = 0;
	}

	return TRUE;
}   /*-------- end class CDDB method LinkStnStatusSend -------- */




/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  GetSendDataBuf
 * Description:  ��ȡ���ݱ���
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::GetSendDataBuf ( BYTE *buf, int &len )
{
	switch ( m_byDataType )
	{
		case DDB_YX_DATATYPE:
			YxDataSend( buf, len );
			break;

		case DDB_YC_DATATYPE:
			YcDataSend( buf, len );
			break;

		case DDB_YM_DATATYPE:
			YmDataSend( buf, len );
			break;

		case DDB_LINKBUSSTATUS_DATATYPE://����״̬����
			{
				LinkBusStatusSend( buf, len);
			}
			break;

		case DDB_LINKSTNSTATUS_DATATYPE://װ��״̬
			{
				LinkStnStatusSend( buf, len);
			}
			break;

		case DDB_TIME_DATATYPE:
			TimeSyncSend( buf, len );
			m_byDataType = DDB_YX_DATATYPE;
			break;

		case DDB_YK_DATATYPE:
			print( (char *)"ResponseYkData" );
			ResponseYkData( buf, len );
			break;


		default:
			break;
	}				/* -----  end switch  ----- */

	//��֯���ͽ������ô�����״̬
	m_SendStatus = NONESTATUS;
	return TRUE;
}		/* -----  end of method CDDB::GetSendDataBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  RequestSyn
 * Description:	 ����ͬ������
 *       Input:	 ���ͻ����� ����������
 *		Return:	 BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::RequestSyn ( BYTE *buf, int &len )
{
	//���ݳ���
	buf[2] = 1;
	GetCommonFrame( buf, REQUEST_SYN );

	len = 4;

	return TRUE;
}		/* -----  end of method CDDB::RequestSyn  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ResponseSyn
 * Description:	 ��Ӧͬ������
 *       Input:	 ���ͻ����� ����������
 *		Return:	 BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ResponseSyn ( BYTE *buf, int &len )
{
	GetCommonFrame( buf, RESPONSE_SYN );

	//�Ƿ�ͬ��
	if(m_bSyn)
		buf[4] = 1;
	else
		buf[4] = 0;

	buf[2] = 2;
	len = 5;

	return TRUE;
}		/* -----  end of method CDDB::ResponseSyn  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  RequestData
 * Description:  �������ݱ���j
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::RequestData ( BYTE *buf, int &len )
{
	GetCommonFrame( buf, REQUEST_DATA );

	//��������
	buf[4] = m_byDataType;

	//�յ���������
	buf[5] = (m_wDataPos >> 8) & 0xff;
	buf[6] = (m_wDataPos) & 0xff;

	buf[2] = 4;
	len = 7;

	return TRUE;
}		/* -----  end of method CDDB::RequestData  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ResponseData
 * Description:  ��Ӧ���ݱ���
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ResponseData ( BYTE *buf, int &len )
{
	return GetSendDataBuf(buf, len);
}		/* -----  end of method CDDB::ResponseData  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  RequestSwitch
 * Description:	 ����ת������
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::RequestSwitch ( BYTE *buf, int &len )
{
	GetCommonFrame( buf, REQUEST_SWITCH );
	buf[2] = 1;
	len = 4;
	//m_bSwitchSending = TRUE;
	//m_bSwitchState = TRUE;
	m_SendStatus = NONESTATUS;

	return TRUE;
}		/* -----  end of method CDDB::RequestSwitch  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ResponseSwitch
 * Description:  ��Ӧ����ת��
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ResponseSwitch ( BYTE *buf, int &len )
{
	GetCommonFrame( buf, RESPONSE_SWITCH );
	buf[2] = 1;
	len = 4;
	m_SendStatus = NONESTATUS;

	return TRUE;
}		/* -----  end of method CDDB::ResponseSwitch  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  RequestYkData
 * Description:  ����yk����
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::RequestYkData ( BYTE *buf, int &len )
{
	//��������
	GetCommonFrame( buf, m_SendStatus );

	//��������
	buf[4] = m_byDataType;

	//yk״̬
	buf[5] = m_YkStatus;

	if( STATUS_MASTER == m_byLocalStatus )
	{
		//װ�����ߺ�
		buf[6] = m_bySaveSrcBusNo;

		//װ�õ�ַ
		buf[7] = HIBYTE( m_wSaveSrcDevAddr );
		buf[8] = LOBYTE(m_wSaveSrcDevAddr );
	}
	else
	{
		//װ�����ߺ�
		buf[6] = m_SaveDestYkData.byDestBusNo;

		//װ�õ�ַ
		buf[7] = HIBYTE(m_SaveDestYkData.wDestAddr);
		buf[8] = LOBYTE(m_SaveDestYkData.wDestAddr);
	}

	//װ�õ��
	buf[9] = HIBYTE(m_SaveDestYkData.wPnt);
	buf[10] = LOBYTE(m_SaveDestYkData.wPnt);

	//ң��ֵ
	buf[11] = m_SaveDestYkData.byVal;

	//���ݳ���
	buf[2] = 9;

	len = 12;

	m_bIsYking = FALSE;					//by cyz!	��Ӹ�������˱�����m_bIsYkingʼ��Ϊtrue�����±����޷�����ң��ң��ң�����ݵ�����!
	return TRUE;
}		/* -----  end of method CDDB::RequestYkData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ResponseYkData
 * Description:  �ظ�yk����
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ResponseYkData ( BYTE *buf, int &len  )
{
	GetCommonFrame(buf, m_SendStatus);
	//��������
	buf[4] = m_byDataType;

	//yk״̬
	buf[5] = m_YkStatus;

	//���ݳ���
	buf[2] = 3;
	len = 6;

	//���ͺ������ж�״̬
	switch ( m_YkStatus )
	{
		case DDB_YK_SEL_CONFIRM:
			m_YkStatus = DDB_YK_SEL_RTN;
			break;

		case DDB_YK_EXE_CONFIRM:
			m_YkStatus = DDB_YK_EXE_RTN;
			break;

		case DDB_YK_SEL:
			m_YkStatus = DDB_YK_EXE;
			break;

		default:
			break;
	}				/* -----  end switch  ----- */

	m_byDataType = m_bySaveDataType;
	return TRUE;
}		/* -----  end of method CDDB::ResponseYkData  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ErgodicDevState
 * Description:  �����ײ�һ���������Ƿ�ȫ��װ��״̬��δ��ͨ
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ErgodicDevState()
{
	BYTE eBusNo,eDevNo;
	BYTE eBusNum = m_pMethod->GetToTalBusNum();
	for( eBusNo=0; eBusNo<eBusNum; eBusNo++ )
	{
		BYTE ProtocolType = m_pMethod->GetBusLineProtocolType( eBusNo );
		if( ProtocolType == PROTOCO_GATHER )
		{
			BYTE eDevNum = m_pMethod->GetDevNum( eBusNo );
			BYTE eAbnormalDevnum = 0;
			for( eDevNo=0; eDevNo<eDevNum; eDevNo++ )
			{
				WORD eDevaddr = m_pMethod->GetAddrByLineNoAndModuleNo ( eBusNo , eDevNo );

				if( FALSE == m_pMethod->GetDevCommState( eBusNo, eDevaddr ) )
				{
					break;
				}
				else
				{
					eAbnormalDevnum++;
				}
			}

			if( eAbnormalDevnum == eDevNum )
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  AddSendCrc
 * Description:  ��ӷ��ͱ���У��
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::AddSendCrc ( BYTE *buf, int &len )
{
	WORD wCrc;
	if ( buf[2] != len -3 )
		return FALSE;

	wCrc = GetCrc(buf+3, buf[2]);
	buf[len++] = ( wCrc >> 8 ) & 0xff;
	buf[len++] = (wCrc) &0xff;

	return TRUE;
}		/* -----  end of method CDDB::AddSendCrc  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  GetSendTypeBuf
 * Description:	 ��ȡ������������
 *       Input:	 ���ͻ����� ����������
 *		Return:	 BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::GetSendTypeBuf ( BYTE *buf, int &len )
{
	BOOL bRtn = FALSE;
	memset( buf, 0, len );
	//printf ( "m_bSwitchState=%d m_byLocalStatus=%d\n", m_bSwitchState, m_byLocalStatus );
	if( m_bSwitchState == TRUE && m_byLocalStatus == STATUS_MASTER )
	{
		m_SendStatus = REQUEST_SWITCH;
		m_bSwitchState = FALSE;
	}

	//printf ( "m_SendStatus=%d m_byDataType=%d\n", m_SendStatus, m_byDataType );
	switch ( m_SendStatus )
	{
		case REQUEST_SYN:
			print ( (char *)"DDB SEND REQUEST_SYN\n" );
			bRtn = RequestSyn(buf, len);
			break;

		case RESPONSE_SYN:
			print ( (char *)"DDB SEND RESPONSE_SYN\n" );
			bRtn = ResponseSyn(buf, len);
			break;

		case REQUEST_DATA:
			// print ( (char *)"DDB SEND REQUEST_DATA" );
			if( m_bIsYking )
			{
				if( DDB_YK_DATATYPE != m_byDataType )
					m_bySaveDataType = m_byDataType;
				m_byDataType = DDB_YK_DATATYPE;
				print( (char *)"RequestYkData\n" );
				bRtn = RequestYkData( buf, len );
			}
			else if( DDB_YK_DATATYPE == m_byDataType )
			{
				bRtn = ResponseYkData( buf, len );
			}
			else
			{
				bRtn = RequestData(buf,len);
			}
			break;

		case RESPONSE_DATA:
			// print ( (char *)"DDB SEND RESPONSE_DATA" );
			if( m_bIsYking )
			{
				if( DDB_YK_DATATYPE != m_byDataType )
					m_bySaveDataType = m_byDataType;
				m_byDataType = DDB_YK_DATATYPE;
				print( (char *)"ResponseYkData\n" );
				bRtn = RequestYkData( buf, len );
			}
			else
			{
				bRtn = ResponseData(buf,len);
			}
			break;

		case REQUEST_SWITCH:
			print ( (char *)"DDB SEND REQUEST_SWITCH\n" );
			bRtn = RequestSwitch(buf,len);
			break;

		case RESPONSE_SWITCH:
			print ( (char *)"DDB SEND RESPONSE_SWITCH\n" );
			bRtn = ResponseSwitch(buf, len);
			break;

		case NONESTATUS:
			bRtn = TRUE;
			len = 0;
			break;

		default:
			char szBuf[256];
			len = 0;
			sprintf ( szBuf, "DDB:GetProtocolBuf can't find m_SendStatus = %d\n", m_SendStatus );
			print( szBuf );
			break;
	}				/* -----  end switch  ----- */

	if( bRtn )
	{
		if( len > 0 )
			AddSendCrc(buf, len);
	}
	else
	{
		if( m_SendStatus != NONESTATUS )
		{
			char szBuf[256];
			sprintf (szBuf, "get message error!!!len=%d %.2x %.2x %.2x %.2x %.2x %.2x", len,buf[0], buf[1], buf[2],buf[3],buf[4],buf[5] );
			print( szBuf );
		}
		else
		{
			print ( (char *)"m_SendStatus = NONESTATUS" );
		}
	}

	if( m_SendStatus == NONESTATUS )
	{
		bRtn = TRUE;
	}

	return bRtn;
}		/* -----  end of method CDDB::GetSendTypeBuf  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ProcessBusMsg
 * Description:  ������Ϣ����
 *       Input:  pBusMsg:��Ϣָ��
 *				 buf�����ͻ���
 *				 len������
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ProcessBusMsg ( PBUSMSG pBusMsg, BYTE *buf, int &len )
{
	switch ( pBusMsg->byMsgType )
	{
		case YK_PROTO:	//����ң��
			{
				print( (char *)"Recv YkMsg\n" );
				ProcessYK( pBusMsg ) ;
			}
			break;

		default:
			printf ( "DDB:ProcessBusMsg can't find msgtype = %d\n", pBusMsg->byMsgType );
			return FALSE;
			break;
	}				/* -----  end switch  ----- */

	return TRUE;
}		/* -----  end of method CDDB::ProcessBusMsg  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ProcessBusMsg
 * Description:  ����ң����Ϣ
 *       Input:  ��Ϣָ��
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ProcessYK( PBUSMSG pBusMsg )
{
	if( !JudgeYkMsg( pBusMsg ) )
	{
		print( (char *)"CDDB::JudgeYkMsg error\n" );
		return FALSE;
	}

	SaveYkMsgInfo( pBusMsg );
	SetYkDataStatus( pBusMsg );
	print( (char *)"CDDB::ProcwssYk Ok\n" );

	return FALSE ;
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  JudgeYkMsg
 * Description:  �ж�YK��Ϣ�Ƿ�׼ȷ
 *       Input:  ��Ϣָ��
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::JudgeYkMsg ( PBUSMSG pBusMsg )
{
	DWORD dwYkType = pBusMsg->dwDataType ;

	if( STATUS_MASTER == m_byLocalStatus  )  //��վֻ����Rtn��Ϣ
	{
		if( YK_SEL_RTN != dwYkType
				&& YK_EXCT_RTN != dwYkType
				&& YK_CANCEL_RTN != dwYkType
				&& YK_ERROR != dwYkType)
		{
			printf ( (char *)"CDDB:JudgeYkMsg ykmsg yktype error\n" );
			return FALSE;
		}

		//û��ykѡ�񷵻�ȷ�ϵĲ��ܽ���yk����
		if( YK_SEL_RTN == dwYkType)
		{
			if( DDB_YK_SEL_RTN != m_YkStatus )
			{
				return FALSE;
			}
		}

		//û��ykִ�з���ȷ�ϵĲ��ܽ���yk����
		if( YK_EXCT_RTN == dwYkType )
		{
			if( DDB_YK_EXE_RTN != m_YkStatus )
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	else if( STATUS_SLAVE == m_byLocalStatus ) //��վֻ�����·���Ϣ
	{
		if( YK_SEL != dwYkType
				&& YK_EXCT != dwYkType
				&& YK_CANCEL != dwYkType)
		{
			printf ( (char *)"CDDB:JudgeYkMsg ykmsg yktype error\n" );
			return FALSE;
		}

		//û�о���ykѡ�񷵻�ȷ�ϵĲ��ܽ���ykִ��
		// if( YK_EXCT == dwYkType )
		// {
			// if( DDB_YK_SEL_RTN_CONFIRM != m_YkStatus  )
			// {
				// return FALSE;
			// }
		// }
		return TRUE;
	}

	printf ( "DDB::JudgeYkMsg not DDB\n" );

	return FALSE;
}		/* -----  end of method CDDB::JudgeYkMsg  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  SaveYkMsgInfo
 * Description:  ����yk��Ϣ��Ϣ
 *       Input:  ��Ϣָ��
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDDB::SaveYkMsgInfo ( PBUSMSG pBusMsg )
{
	PDDBYK_DATA pYkData = ( PDDBYK_DATA )pBusMsg->pData ;
	//����ת��Э������ߺź͵�ַ��
	m_bySaveSrcBusNo = pBusMsg->SrcInfo.byBusNo;
	m_wSaveSrcDevAddr = pBusMsg->SrcInfo.wDevNo;

	if( STATUS_MASTER == m_byLocalStatus )
	{
		print ( (char *)"SaveYkMsgInfo master not save\n" );
		return ;
	}

	// CPublicMethod::SetDDBDevBusAndAddr( m_bySaveSrcBusNo, m_wSaveSrcDevAddr );
	//����װ����Ϣ
	memcpy( &m_SaveDestYkData, pYkData, sizeof( DDBYK_DATA ) );
}		/* -----  end of method CDDB::SaveYkMsgInfo  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  SetYkDataStatus
 * Description:  ����Yk����״̬
 *       Input:  ��Ϣָ��
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDDB::SetYkDataStatus ( PBUSMSG pBusMsg )
{
	//�������еķ���״̬�����㷢��yk���ݺ��л�
	// m_bySaveDataType = m_byDataType;
	// m_byDataType = DDB_YK_DATATYPE;
	m_bIsYking = TRUE;

	DWORD dwYkType = pBusMsg->dwDataType ;

	switch ( dwYkType )
	{
		case YK_SEL:
			m_YkStatus = DDB_YK_SEL;
			break;

		case YK_EXCT:
			m_YkStatus = DDB_YK_EXE;
			break;

		case YK_CANCEL:
			m_YkStatus = DDB_YK_CANCEL;
			break;

		case YK_SEL_RTN:
			m_YkStatus = DDB_YK_SEL_RTN;
			break;

		case YK_EXCT_RTN:
			m_YkStatus = DDB_YK_EXE_RTN;
			break;

		case YK_CANCEL_RTN:
			m_YkStatus = DDB_YK_CANCEL_RTN;
			break;

		case YK_ERROR:
			m_YkStatus = DDB_YK_ERROR;
			break;

		default:
			print( (char *)"DDB:SetYkDataStatus none dwYkType" );
			m_YkStatus = DDB_YK_NONE_STATUS;
			break;
	}				/* -----  end switch  ----- */

}		/* -----  end of method CDDB::SetYkDataStatus  ----- */



/* **********************��ʼ��Э�� ***************************************/
/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  CDDB
 * Description:  ��ȡ������Ϣ
 *       Input:
 *		Return:  BOOL
 *		�����
 *		��ݣ�machineid=A����B��
 *		���ص�ַ��localip=192.168.1.200���˴�Ҳ��дΪ localNetcard=eth0 �Ǹ��ã�//�ޱ�Ҫ ����ͨ�����������жϣ�
 *		Զ�̵�ַ��localip=192.168.1.211
 *		�˿�    ��1111�������豸������һ���Ķ˿ں� ����ͨѶ��
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ReadCfgInfo (  )
{
	//BYTE bRtn;
	char szMachineBuf[256];
	char szFilename[256] = "";

	sprintf( szFilename, "%s%s", DDBPREFIXFILENAME, m_sTemplatePath );
	CProfile profile( szFilename ) ;
	if( !profile.IsValid() )
	{
		printf( "CDDB:ReadCfgInfo Open file %s Failed ! \n " , profile.m_szFileName );
		return FALSE ;
	}

	char sSect[ 200 ] = "DDB" ;
	char sKey[ 20 ][ 100 ]={ "machineid" , "localip" , "renoteip" , "port",\
							"DelayedSwitchMinute" , "DelayedSynSecond"  } ;

	//bRtn = profile.GetProfileString( sSect , sKey[ 0 ] , "NULL" , szMachineBuf , sizeof( szMachineBuf ) ) ;
	profile.GetProfileString( sSect , sKey[ 0 ] , (char *)"NULL" , szMachineBuf , sizeof( szMachineBuf ) ) ;
	if ( szMachineBuf[0] == 'A' )
	{
		m_byMachineId = IDENTITY_A;
	}
	else if( szMachineBuf[0] == 'B' )
	{
		m_byMachineId = IDENTITY_B;
	}
	else
	{
		m_byMachineId = IDENTITY_SINGLE; ;
		printf ( "CDDB:ReadCfgInfo Read m_byMachineId err !!! \n" );
		return FALSE;
	}

	m_iDelayedSwitchMinute = profile.GetProfileInt( sSect , sKey[ 4 ] , 0 ) ;
	if( m_iDelayedSwitchMinute < 10 )
	{
		m_iDelayedSwitchMinute = 10;
	}
	else if( m_iDelayedSwitchMinute > 120 )
	{
		 m_iDelayedSwitchMinute = 120 ;
	}

	m_iDelayedSynSecond = profile.GetProfileInt( sSect , sKey[ 5 ] , 0 ) ;
	if( m_iDelayedSynSecond < 10 )
	{
		m_iDelayedSynSecond = 10;
	}
	else if( m_iDelayedSynSecond > 120 )
	{
		 m_iDelayedSynSecond = 120 ;
	}

	printf ( "DelayedSynSecond=%ds DelayedSwitchMinute=%dm\n", m_iDelayedSynSecond, m_iDelayedSwitchMinute );

	// bRtn = profile.GetProfileString( sSect , sKey[ 1 ] , "NULL"  , m_szLocalIp , sizeof( m_szLocalIp ) ) ;
	// if( bRtn == 4 )
	// {
		// printf ( "CDDB:ReadCfgInfo Read m_szLocalIp err !!! \n" );
		// return FALSE;
	// }
	// bRtn = profile.GetProfileString( sSect , sKey[ 2 ] , "NULL"  , m_szRemoteIp , sizeof( m_szRemoteIp ) ) ;
	// if( bRtn == 4 )
	// {
		// printf ( "CDDB:ReadCfgInfo Read m_szRemoteIp err !!! \n" );
		// return FALSE;
	// }
	// m_dwPort = profile.GetProfileInt( sSect , sKey[ 3 ] , 0 ) ;
	// if( m_dwPort <= 1000 )
	// {
		// printf ( "CDDB:ReadCfgInfo Read port err !!! \n" );
		// return FALSE;
	// }

	printf( "localid=%d\n", m_byMachineId );
	return TRUE;
}		/* -----  end of method CDDB::ReadCfgInfo  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  InitProtocolStatus
 * Description:  ��ʼ��Э�����״̬
 *       Input:
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::InitProtocolStatus ( )
{
	if ( m_byMachineId == IDENTITY_SINGLE )
		return FALSE;

	m_bSyn = TRUE;				//����ͬ��
	m_bRemoteSyn = TRUE;		//Զ��ͬ��
	// m_byLocalStatus = STATUS_SLAVE;//��ʼ����Ϊ��
	// m_byRemoteStatus = STATUS_MASTER;//Զ�̳�ʼ����Ϊ��
	m_byDataType = DDB_YX_DATATYPE;//��ʼ�����ó�YX
	m_bLinkStatus = FALSE;		//����״̬Ϊ��
	m_SendStatus = REQUEST_SYN;	//��Ϊ��λͨ�ŵ�Ԫ
	m_dwLinkTimeOut = 0;		//���ӳ�ʱΪ0
	m_byRecvErrorCount = 0;     //���մ������0
	m_bIsReSend = FALSE;		//�ط���ʶλ0
	m_byResendCount = 0;		//�ط���������
	m_bIsSending = FALSE;		//���ͺ���1 ���պ�ֵ0
	m_bSwitchState = FALSE;     //�Ƿ�Ҫ�л�

	m_wDataPos = 0;				//������ʼλ��Ϊ0

	//����ط�������
	memset(m_byReSendBuf, 0 , DDB_MAX_BUF_LEN);
	m_wReSendLen = 0;


	//���� ͬ��״̬Ϊδͬ��
	// CPublicMethod::SetDDBSyncState( m_bSyn );



	return TRUE;
}		/* -----  end of method CDDB::InitProtocolStatus  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  InitProtocolData
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDDB::InitProtocolData ( BOOL bStatus )
{
	//��ȡ�����������׵�ַ
	m_wAllYcNum = GetDataNum( 2 );
	m_wAllYxNum = GetDataNum( 1 );
	m_wAllYmNum = GetDataNum( 3 );
	// printf ( "ycnum=%d yxnum=%d ymnum=%d\n", m_wAllYcNum, m_wAllYxNum, m_wAllYmNum );

	m_pYcHeadAddr = ( ANALOGITEM * )GetDataHeadAddr( 2 );
	m_pYxHeadAddr = ( DIGITALITEM * )GetDataHeadAddr( 1 );
	m_pYmHeadAddr = ( PULSEITEM * )GetDataHeadAddr( 3 );

	SwitchStatus( bStatus );


	// printf ( "pyc=%p yxnum=%p ymnum=%p\n", m_pYcHeadAddr, m_pYxHeadAddr, m_pYmHeadAddr );
}		/* -----  end of method CDDB::InitProtocolData  ----- */

/* ********************** ���ɻ��麯������ ***************************************/

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  TimerProc
 * Description:  ʱ�䴦���� ��Ҫ����һЩ��ʱ ���ٻ�����ʱ���йص�
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void CDDB::TimerProc (  )
{
	// if ( !m_bLinkStatus )
		// return;
	if ( m_bSyn )
	{
		JudgeStatus(  );
		// if ( m_byMachineId == IDENTITY_B && m_byLocalStatus == STATUS_MASTER && m_byRemoteStatus == STATUS_MASTER )
		// {
			// print ((char *)"\nDDB m_bSyn=TRUE m_id=B m_byLocalStatus m_byRemoteStatus = STATUS_MASTER\n" );
			// m_byLocalStatus = STATUS_SLAVE;
		// }
		// if ( m_byMachineId == IDENTITY_A && m_byLocalStatus == STATUS_SLAVE && m_byRemoteStatus == STATUS_SLAVE)
		// {
			// print ( (char *)"\nDDB m_bSyn=TRUE m_id=A m_byLocalStatus m_byRemoteStatus = STATUS_SLAVE\n" );
			// m_byLocalStatus = STATUS_MASTER;
		// }

		// CPublicMethod::SetDDBSyncState( m_byLocalStatus );
	}

	//����ͨѶͨ
	// int Interval = 250;

	//ͨѶ��ʱʱ��
	// m_dwLinkTimeOut += Interval;
	// if(m_dwLinkTimeOut >= DDB_LINK_TIMEOUT)
	// {
		// m_dwLinkTimeOut = 0;
		// if( m_bLinkStatus == TRUE )
		// {
			// if ( m_byLocalStatus == STATUS_MASTER )
			// {
				// m_SendStatus = NONESTATUS;
			// }
			// else if( m_byLocalStatus == STATUS_SLAVE )
			// {
				// SwitchStatus( 1 );
			// }
		// }
	// }


	//collect state

	if( m_byQuickSwitchNum < 6 )
	{
		if( m_byLocalStatus == STATUS_MASTER )//&& m_bSwitchSending == FALSE )
		{
			if( m_bTimeProcCount == 255)
			{
				m_bTimeProcCount = 11;
			}
			else
			{
				m_bTimeProcCount++;
			}

			if( m_bTimeProcCount >= 10)
			{
				if( FALSE == ErgodicDevState() )
				{
					m_bSwitchState = TRUE;
					// m_bIsReSend = FALSE;
					//m_byResendCount = 0;
					// m_bIsSending = FALSE;
					return ;
				}
			}
		}
	}
	else
	{
		time_t TempNowTime,TempTimeDifference;
		time(&TempNowTime);
		TempTimeDifference = abs( TempNowTime - m_tmLastSwitchTime );
		if( TempTimeDifference > m_iDelayedSwitchMinute*60 )
		{
			m_byQuickSwitchNum = 0;
		}
	}

	//���մ������
	if( m_byRecvErrorCount > DDB_MAX_ERROR_COUNT  )
	{
		char szBuf[256];
		sprintf ( szBuf, "DDB Recv err > %d ������·\n", DDB_MAX_ERROR_COUNT );
		print( szBuf );
		InitProtocolStatus(  );

		if ( m_byLocalStatus == STATUS_MASTER )
		{
			print ( (char *)"����Ϊ���� �ȴ����� \n" );
			m_SendStatus = NONESTATUS;
		}
		else
		{
			print ( (char *)"����Ϊ�ӻ� ������������ \n" );
			m_SendStatus = REQUEST_SYN;
		}
	}

	//�ط�����
	if( m_byResendCount >= DDB_MAX_RESEND_COUNT )
	{
		char szBuf[256];
		sprintf (szBuf, "DDB ReSend > %d", DDB_MAX_RESEND_COUNT );
		print( szBuf );

		InitProtocolStatus(  );

		if ( m_byLocalStatus == STATUS_MASTER )
		{
		}
		else
		{
			// //δ��ʼ��ʱA��B��
			// if( !m_bSyn )
			// {
				// if( m_byMachineId == IDENTITY_A )
				// {
					// SwitchStatus( 1 );
					// m_SendStatus = NONESTATUS;
				// }
				// else
				// {
					// m_SendStatus = REQUEST_SYN;
				// }
			// }
			// else
			// {
				// SwitchStatus( 1 );
				// m_SendStatus = NONESTATUS;
			// }
			SwitchStatus( 1 );
				// m_SendStatus = NONESTATUS;
		}

	}

}		/* -----  end of method CDDB::TimerProc  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  ProcessProtocolBuf
 * Description:	 �����յ������ݻ���
 *       Input:  ���յ������ݻ��� ���泤��
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::ProcessProtocolBuf ( BYTE *buf, int len )
{
	int pos=0;
	BOOL bRtn = FALSE;
	if( !WhetherBufValue( buf, len, pos ) )
	{
		print ( (char *)"CDDB:ProcessProtocolBuf buf Recv err!!!\n" );
		m_byRecvErrorCount ++;
		m_bIsReSend = TRUE;
		usleep(100 * 1000);
		return FALSE;
	}

	bRtn = ProcessRecvTypeBuf( &buf[pos], len );
	if( !bRtn )
	{
		print ( (char *)"DDB ProcessRecv err\n" );
		m_byRecvErrorCount ++;
		m_bIsReSend = TRUE;
	}
	else
	{
		m_byRecvErrorCount = 0;
		// m_bLinkStatus = TRUE;
		// m_dwLinkTimeOut = 0;
		m_bIsReSend = FALSE;
		m_byResendCount = 0;
		m_bIsSending = FALSE;
	}

	return bRtn;
}		/* -----  end of method CDDB::ProcessProtocolBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  GetProtocolBuf
 * Description:  ��ȡЭ�����ݻ���
 *       Input:  ������ ���������ݳ��� ������Ϣ
 *		Return:	 BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::GetProtocolBuf ( BYTE *buf, int &len, PBUSMSG pBusMsg )
{
	BOOL bRtn = TRUE;
	 // printf ( "m_SendStatus=%d m_byDataType=%d\n", m_SendStatus, m_byDataType );
	// if ( ( m_bIsReSend || m_bIsSending ) && ( m_SendStatus == REQUEST_SYN ))
	if( m_bRecvResponseSwitch == TRUE )
	{
		sleep(m_iDelayedSynSecond);
	}
	m_bRecvResponseSwitch = FALSE;

	if ( ( m_bIsReSend || m_bIsSending ) && (m_wReSendLen > 0) )
	{
		if( m_SendStatus == REQUEST_SYN || m_SendStatus == RESPONSE_SYN )
		{
			usleep( 200 * 1000 );
		}
		else
		{
			usleep( 20 * 1000 );
		}
		print ( (char *)"DDB Get Resend Buf\n" );
		len = m_wReSendLen;
		memcpy( buf, m_byReSendBuf, len );
		m_byResendCount ++;
	}
	// else if( pBusMsg != NULL && m_bLinkStatus)
	else if( pBusMsg != NULL )
	{
		if( !ProcessBusMsg( pBusMsg, buf, len ) )
			return FALSE;
	}
	else
	{
		bRtn =  GetSendTypeBuf( buf, len );

		if( bRtn )
		{
			m_byResendCount = 0;

			if ( len > DDB_MAX_BUF_LEN )
			{
				char szBuf[256];
				sprintf (szBuf, "len=%d is too big for %d", len, DDB_MAX_BUF_LEN );
				print( szBuf );
				return FALSE;
			}

			m_wReSendLen = len;
			memcpy( m_byReSendBuf, buf, m_wReSendLen );
			m_bIsSending = TRUE;
		}
	}

	return bRtn;
}		/* -----  end of method CDDB::GetProtocolBuf  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDDB
 *      Method:  Init
 * Description:	 ��ʼ��Э������
 *       Input:  ���ߺ�
 *		Return:  BOOL
 *--------------------------------------------------------------------------------------
 */
BOOL CDDB::Init ( BYTE byLineNo )
{
	if ( CPublicMethod::IsHaveDDB() )
		return FALSE;

	//�������ļ�
	if( !ReadCfgInfo() )
	{
		// printf ( "CDDB:ReadCfgInfo Err!!!\n" );
		return FALSE;
	}

	//��ʼ��״̬
	if( !InitProtocolStatus(  ) )
	{
		printf ( "CDDB:InitProtocolStatus Err\n" );
		return FALSE;
	}

	InitProtocolData( 0 );

	printf ( "DDB Init Success\n" );
	//����Э��
	CPublicMethod::SetDDBProtocol();
	CPublicMethod::SetDDBBusAndAddr( m_byLineNo , m_wDevAddr ) ;
	return TRUE;
}		/* -----  end of method CDDB::Init  ----- */

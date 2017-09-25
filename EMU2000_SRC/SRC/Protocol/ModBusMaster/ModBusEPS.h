/*
 * =====================================================================================
 *
 *       Filename:  ModBusEPS.h
 *
 *    Description:  
 
 *        Version:  1.0
 *        Created:  2014��11��24�� 09ʱ40��24��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp (), 
 *   Organization:  
 *
 *		  history:
 *
 * =====================================================================================
 */

#ifndef  MODBUSEPS_INC
#define  MODBUSEPS_INC

#include "CProtocol_ModBusMaster.h"


#define				MODBUSMASTER_EPS_MAX_POS			30		/* ���λ�� */

/*
 * =====================================================================================
 *        Class:  CModbusEPS
 *  Description:  ups��epsͨѶ�� 
 * =====================================================================================
 */
class CModbusEPS : public CProtocol_ModBusMaster
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		CModbusEPS ();                             /* constructor      */
		~CModbusEPS ();                            /* destructor       */

	virtual BOOL GetProtocolBuf( BYTE * buf , int &len , PBUSMSG pBusMsg = NULL ) ;
	virtual BOOL ProcessProtocolBuf( BYTE * buf , int len ) ;
	virtual BOOL Init( BYTE byLineNo  ) ;
	
	virtual void TimerProc() ;
	//���װ��ͨѶ״̬
	virtual BOOL GetDevCommState( ) ;

	protected:
		/* ====================  DATA MEMBERS  ======================================= */

	private:
	// �жϱ�����Ч��
	BOOL WhetherBufValue ( BYTE *buf, int &len );
	// ����ң�ű���
	BOOL ProcessYxBuf( BYTE *buf, int len );
	// ������
	BOOL ProcessRecvBuf ( BYTE *buf, int len );

	private:
	//�ı䷢��λ��
	void ChangeSendPos( void );
	//��ȡ����λ��
	BYTE GetSendPos( void );
	private:
		/* ====================  DATA MEMBERS  ======================================= */
	BOOL m_bLinkStatus;
	BYTE m_bySendCount;
	BYTE m_byRecvCount;
	BYTE m_bySendPos;

}; /* -----  end of class CModbusEPS  ----- */

#endif   /* ----- #ifndef MODBUSEPS_INC  ----- */

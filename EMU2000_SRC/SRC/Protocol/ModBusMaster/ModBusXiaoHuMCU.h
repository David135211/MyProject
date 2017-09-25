/*
 * =====================================================================================
 *
 *       Filename:  ModBusXiaoHuMCU.h
 *
 *    Description:  ���������� .cpp�д����Ӧ˵��
 *
 *        Version:  1.0
 *        Created:  2014��11��19�� 13ʱ21��18��
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


#ifndef  MODBUSXIAOHUMCU_INC
#define  MODBUSXIAOHUMCU_INC

#include "CProtocol_ModBusMaster.h"

#define		XHMCU_NONE_TROUBLE_DATATYPE		1			/* �޹��� */
#define		XHMCU_TROUBLE_DATATYPE			2			/*  ����*/



#define		XHMCU_PRINT						1			/* �Ƿ��ӡ ���δ��� ����ӡ�Ǳ�����Ϣ */

/*
 * =====================================================================================
 *        Class:  CModBusXHMCU
 *  Description:  xH 
 * =====================================================================================
 */
class CModBusXHMCU : public CProtocol_ModBusMaster
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		CModBusXHMCU ();                             /* constructor      */
		~CModBusXHMCU ();                            /* destructor       */

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
	BOOL WhetherBufValue ( BYTE *buf, int &len ,int &pos);
	// ������
	BOOL ProcessRecvBuf ( BYTE *buf, int len );
	//�����У��
	WORD GetWordSumCheck ( BYTE *pBuf, int len  );
	BYTE GetSumCheck( BYTE * pBuf , int len );
	//�����������
	BOOL ProcessTroubleData( BYTE *buf, int len );
	//����ǹ�������
	BOOL ProcessNoneTroubleData( BYTE *buf, int len );
	//��ӡ��Ϣ
	void print( char *buf ) const;
	private:
		/* ====================  DATA MEMBERS  ======================================= */
	BOOL m_bLinkStatus;
	BYTE m_bySendCount;
	BYTE m_byRecvCount;
	BYTE m_bySrcAddr;
	BYTE m_byDataType;

}; /* -----  end of class CModBusXHMCU  ----- */

#endif   /* ----- #ifndef MODBUSXIAOHUMCU_INC  ----- */

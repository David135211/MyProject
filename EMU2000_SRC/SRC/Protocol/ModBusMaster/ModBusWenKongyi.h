/*
 * =====================================================================================
 *
 *       Filename:  ModBusWenKongyi.h
 *
 *    Description:  ��ʽ�¿���ModbusЭ�� .cpp�д����Ӧ˵��
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


#ifndef  MODBUSWENKONGYI_INC
#define  MODBUSWENKONGYI_INC

#include "CProtocol_ModBusMaster.h"


/*
 * =====================================================================================
 *        Class:  CModbusWenKongYi
 *  Description:  ��ʽ�¿����� 
 * =====================================================================================
 */
class CModbusWenKongYi : public CProtocol_ModBusMaster
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		CModbusWenKongYi ();                             /* constructor      */
		~CModbusWenKongYi ();                            /* destructor       */

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
	// ������
	BOOL ProcessRecvBuf ( BYTE *buf, int len );
	private:
		/* ====================  DATA MEMBERS  ======================================= */
	BOOL m_bLinkStatus;
	BYTE m_bySendCount;
	BYTE m_byRecvCount;

}; /* -----  end of class CModbusWenKongYi  ----- */

#endif   /* ----- #ifndef MODBUSWENKONGYI_INC  ----- */

/*
 * =====================================================================================
 *
 *       Filename:  CModBusSD96E3.h
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  2017��1��9�� 11ʱ40��18��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ykk (), 
 *   Organization:  
 *
 *		  history:
 *
 * =====================================================================================
 */


#ifndef  MODBUSSD96E3_INC
#define  MODBUSSD96E3_INC

#include "CProtocol_ModBusMaster.h"


/*
 * =====================================================================================
 *        Class:  ModBusSD96-E3
 *  Description:  
 * =====================================================================================
 */
class CModBusSD96E3 : public CProtocol_ModBusMaster
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		CModBusSD96E3 ();                             /* constructor      */
		~CModBusSD96E3 ();                            /* destructor       */

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

}; /* -----  end of class CModBusSD96E3  ----- */

#endif   /* ----- #ifndef MODBUSSD96E3_INC  ----- */

/*
 * =====================================================================================
 *
 *       Filename:  ModBusXiaoDianLiu.h
 *
 *    Description:  С�����ӵ�ѡ��ModBusЭ�� .cpp�д����Ӧ˵��
 *
 *        Version:  1.0
 *        Created:  2015��04��22�� 11ʱ13��42�� 
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


#ifndef  MODBUSXIAODIANLIU_INC
#define  MODBUSXIAODIANLIU_INC

#include "CProtocol_ModBusMaster.h"

#define		XDL_PRINT		1			/*  */

/*
 * =====================================================================================
 *        Class:  CModBusXDL
 *  Description:  ��ʽ�¿����� 
 * =====================================================================================
 */
class CModBusXDL : public CProtocol_ModBusMaster
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		CModBusXDL ();                             /* constructor      */
		~CModBusXDL ();                            /* destructor       */

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
	BOOL WhetherBufValue ( BYTE *buf, int &len , int &pos);
	// ������
	BOOL ProcessRecvBuf ( BYTE *buf, int len );
	// ��ӡ����
	void print( char *buf );
	private:
		/* ====================  DATA MEMBERS  ======================================= */
	BOOL m_bLinkStatus;
	BYTE m_bySendCount;
	BYTE m_byRecvCount;

}; /* -----  end of class CModBusXDL  ----- */

#endif   /* ----- #ifndef MODBUSXIAODIANLIU_INC  ----- */

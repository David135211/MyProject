/*
 * =====================================================================================
 *
 *       Filename:  ModBusLekuThermometer.h
 *
 *    Description:  �Ϻ��ֿ��ѹ���õ����¶ȼ�
				 .cpp�д����Ӧ˵��
 *
 *        Version:  1.0
 *        Created:  2016��04��21�� 09ʱ45��18��
 *       Revision:  none
 *       Compiler:  
 *
 *         Author:  ykk 
 *   Organization:  
 *
 *		  history:
 *
 * =====================================================================================
 */


#ifndef  MODULE_LEKUTHERMOMETER_INC
#define  MODULE_LEKUTHERMOMETER_INC

#include "CProtocol_ModBusMaster.h"

/*
 * =====================================================================================
 *        Class:  ModBusLekuThermometer
 *  Description:  xH 
 * =====================================================================================
 */
class ModBusLekuThermometer : public CProtocol_ModBusMaster
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		ModBusLekuThermometer ();                             /* constructor      */
		~ModBusLekuThermometer ();                            /* destructor       */

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
	//��ӡ��Ϣ
	void print( char *buf ) const;
	private:
		/* ====================  DATA MEMBERS  ======================================= */
	BOOL m_bLinkStatus;
	BYTE m_bySendCount;
	BYTE m_byRecvCount;


}; /* -----  end of class ModBusLekuThermometer  ----- */

#endif   /* ----- #MODULE_LEKUTHERMOMETER_INC  ----- */

/*
 * =====================================================================================
 *
 *       Filename:  ModBusLiuLiangJi.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016��05��20�� 09ʱ45��18��
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


#ifndef  MODULE_LIULIANGJI_INC
#define  MODULE_LIULIANGJI_INC

#include "CProtocol_ModBusMaster.h"

/*
 * =====================================================================================
 *        Class:  ModBusLiuLiangJi
 *  Description:  xH 
 * =====================================================================================
 */
class ModBusLiuLiangJi : public CProtocol_ModBusMaster
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		ModBusLiuLiangJi ();                             /* constructor      */
		~ModBusLiuLiangJi ();                            /* destructor       */

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


}; /* -----  end of class ModBusLiuLiangJi  ----- */

#endif   /* ----- #MODULE_LIULIANGJI_INC  ----- */

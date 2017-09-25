/*
 * =====================================================================================
 *
 *       Filename:  Dlt645_2007.h
 *
 *    Description:  dlt645 2007�汾Э��
 *
 *        Version:  1.0
 *        Created:  2014��11��10�� 14ʱ12��46��
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

#ifndef  DLT645_2007_INC
#define  DLT645_2007_INC

#include "CProtocol_Dlt645.h"


/*
 * =====================================================================================
 *        Class:  CDlt645_2007
 *  Description:  
 * =====================================================================================
 */
class CDlt645_2007 : public CProtocol_Dlt645
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		CDlt645_2007 ();                             /* constructor      */
		~CDlt645_2007 ();                            /* destructor       */
		//ʱ�䴦����
		virtual void    TimerProc( void );
		//��ʼ��Э������
		virtual BOOL Init( BYTE byLineNo );
		//��ȡЭ�����ݻ���
		virtual BOOL GetProtocolBuf( BYTE * buf , int &len , PBUSMSG pBusMsg = NULL ) ;
		//�����յ������ݻ��� 
		virtual BOOL ProcessProtocolBuf( BYTE * buf , int len ) ;
		//����ͨѶ״̬ 
		virtual BOOL GetDevCommState( void ) ;
		//������������
		virtual BOOL RequestReadData( BYTE *buf, int &len );
		//����ң������
		virtual BOOL ProcessYcData( const BYTE *buf, int len );
		//����ң������
		virtual BOOL ProcessYmData( const BYTE *buf, int len );
		//��ʱ����
		virtual BOOL TimeSync( BYTE *buf, int &len );
		
	public:
		/* ====================  METHODS  ============================================ */


	public:
	protected:
		//��ȡװ������Ϊ���ص�ַ
		BOOL GetDevNameToAddr( void );
		/* ====================  DATA MEMBERS  ======================================= */

	private:
		/* ====================  METHODS  ============================================ */
		//�Ƿ��ʱ
		BOOL IsTimeToSync( void );
		//��ʼ��Э��״̬����
		BOOL InitProtocolStatus( void );
		//��ȡ����
		BOOL GetSendBuf( BYTE *buf, int &len );
		//������
		BOOL ProcessBuf( const BYTE *buf, int len );
	
	private:
		BOOL m_bLinkTimeSyn;	//��������ʱ��ʱһ��
		BOOL m_bLinkStatus;		//����״̬
		BOOL m_bIsSending;		//�Ƿ����ڷ���
		BOOL m_bIsReSend;		//�Ƿ��ط�
		BOOL m_bIsNeedResend;	//�Ƿ���Ҫ�ط�
		BOOL m_bTimeSynFlag;	//��ʱ��ʶ


		BYTE m_byResendCount;			//�ط�����
		BYTE m_byReSendBuf[DLT645_MAX_BUF_LEN];//�ط�����
		BYTE m_byReSendLen;				//�ط����泤��

		BYTE m_byRecvErrorCount;        //���մ������
		/* ====================  DATA MEMBERS  ======================================= */

}; /* -----  end of class CDlt645_2007  ----- */

#endif   /* ----- #ifndef DLT645_2007_INC  ----- */

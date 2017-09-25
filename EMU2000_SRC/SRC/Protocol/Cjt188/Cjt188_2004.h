/*
 * =====================================================================================
 *
 *       Filename:  Cjt188_2004.h
 *
 *    Description:  Cjt188_2004�汾Э��
 *
 *        Version:  1.0
 *        Created:  2015��03��12�� 10ʱ30��56�� 
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

#ifndef  CJT188_2007_INC
#define  CJT188_2007_INC

#include "CProtocol_Cjt188.h"


/*
 * =====================================================================================
 *        Class:  CCjt188_2004
 *  Description:  
 * =====================================================================================
 */
class CCjt188_2004 : public CProtocol_Cjt188
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		CCjt188_2004 ();                             /* constructor      */
		~CCjt188_2004 ();                            /* destructor       */
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
		
	public:
		/* ====================  METHODS  ============================================ */


	public:
	protected:
		/* ====================  DATA MEMBERS  ======================================= */
		//������������
		virtual BOOL RequestReadData( BYTE *buf, int &len );
		//10-19h
		BOOL ProcessDataT1 ( const BYTE *buf, int len  );
		//20-29h
		BOOL ProcessDataT2 ( const BYTE *buf, int len  );
		//30-39h
		BOOL ProcessDataT3 ( const BYTE *buf, int len  );
		//�����������
		virtual BOOL ProcessReadData( const BYTE *buf, int len );
		//��ʱ����
		virtual BOOL TimeSync( BYTE *buf, int &len );
		//��ʼ��������Ϣ
		virtual void InitSendCfgInfo( void );

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
		//��ȡװ������Ϊ���ص�ַ
		BOOL GetDevNameToAddr( void );
	
	private:
		BOOL m_bLinkTimeSyn;	//��������ʱ��ʱһ��
		BOOL m_bLinkStatus;		//����״̬
		BOOL m_bIsSending;		//�Ƿ����ڷ���
		BOOL m_bIsReSend;		//�Ƿ��ط�
		BOOL m_bIsNeedResend;	//�Ƿ���Ҫ�ط�
		BOOL m_bTimeSynFlag;	//��ʱ��ʶ


		BYTE m_byResendCount;			//�ط�����
		BYTE m_byReSendBuf[CJT188_MAX_BUF_LEN];//�ط�����
		BYTE m_byReSendLen;				//�ط����泤��

		BYTE m_byRecvErrorCount;        //���մ������
		/* ====================  DATA MEMBERS  ======================================= */

}; /* -----  end of class CCjt188_2004  ----- */

#endif   /* ----- #ifndef CJT188_2004_INC  ----- */

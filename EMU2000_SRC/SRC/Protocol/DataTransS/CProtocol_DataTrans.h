/*
 * =====================================================================================
 *
 *       Filename:  CProtocol_DataTrans.h
 *
 *    Description:  ����ESD�Զ�����ʷ�����ϴ� 
 *
 *        Version:  1.0
 *        Created:  2015��06��09�� 14ʱ22��18��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp
 *   Organization:  
 *
 *		  history:	Time						Author			version			Description
 *					2015-06-09 14:25:52         mengqp			1.0				created
 *
 * =====================================================================================
 */


#ifndef  CPROTOCOL_DATATRANS_INC
#define  CPROTOCOL_DATATRANS_INC

/* #####   HEADER FILE INCLUDES   ################################################### */

#include "../../share/CMethod.h"
#include "../../share/Rtu.h"

/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ################################### */
#define		DATATRANSSPREFIXFILENAME			"/mynand/config/DataTrans/"	/* 104�̶�·�� */

// #define		ESD_DATATRANS_PRINT			/* �ն˴�ӡ */
//#define		ESD_DATATRANS_DEBUG			/* UDP�˿ڴ�ӡ */   //undefined by cyz!

#define		DATATRANS_MAX_YC_NUM			4096			/* ���ң������ */
#define		DATATRANS_MAX_YX_NUM			8192			/* ���ң������ */
#define		DATATRANS_MAX_YM_NUM			1024			/* ���ң������ */
#define		DATATRANS_MAX_BUF_LEN			256				/* ��󻺳����� */
#define		DATATRANS_MAX_SEND_COUNT		3				/* ����ʹ��� */




#define			DATATRANS_LINK_STATE			0x00000001		/* ���� */
#define			DATATRANS_RESEND_STATE			0x00000002		/* �ط� */
#define			DATATRANS_YC_STATE				0x00000004		/*  YC*/
#define			DATATRANS_YC_OVER_STATE			0x00000008		/*  YC����*/
#define			DATATRANS_YX_STATE				0x00000010		/*  YX*/
#define			DATATRANS_YX_OVER_STATE			0x00000020		/*  YX����*/
#define			DATATRANS_YM_STATE				0x00000040		/*  YM*/
#define			DATATRANS_YM_OVER_STATE			0x00000080		/*  YM ����*/
#define			DATATRANS_CHANGE_YX_STATE		0x00000100		/*  �仯YX*/
#define			DATATRANS_HEARTBEAT_STATE		0x00000200		/*  ���� */
#define			DATATRANS_WRITEFILE_STATE		0x00000400		/*  д�ļ� */
#define			DATATRANS_WRITEFILE_OVER_STATE	0x00000800		/*  д�ļ����� */
#define			DATATRANS_WRITEFILE_HEAD_STATE	0x00001000		/*  д�ļ�HEAD */
#define			DATATRANS_WRITEFILE_YC_STATE	0x00002000		/*  д�ļ�YC */
#define			DATATRANS_WRITEFILE_YX_STATE	0x00004000		/*  д�ļ�YX */
#define			DATATRANS_WRITEFILE_YM_STATE	0x00008000		/*  д�ļ�YM */

/* #####   TYPE DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ######################### */
//������ʷ�������ݵ�����
typedef enum 
{
	ESD_DATATRANS_YC_DATATYPE,
	ESD_DATATRANS_YX_DATATYPE,
	ESD_DATATRANS_YM_DATATYPE
}ESD_DATATRANS_DATATYPE;				/* ----------  end of enum ESD_DATATRANS_DATATYPE  ---------- */

/* #####   DATA TYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */

/*
 * =====================================================================================
 *        Class:  CProtocol_DataTrans
 *  Description:  �Զ�����ʷ������
 * =====================================================================================
 */
class CProtocol_DataTrans : public CRtuBase
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		CProtocol_DataTrans ();                             /* constructor      */
		virtual ~CProtocol_DataTrans ();                            /* destructor       */

		/* ====================  METHODS	   ======================================= */
		//��ȡЭ�鱨��
		virtual BOOL GetProtocolBuf( BYTE * buf , int &len , PBUSMSG pBusMsg = NULL );
		//����Э�鱨��
		virtual BOOL ProcessProtocolBuf( BYTE * buf , int len );
		//��ʼ������ģ��
		virtual BOOL Init( BYTE byLineNo  ) ;
		//ʱ�䴦����
		virtual void TimerProc(){ return;  } 
		//�㲥��Ϣ����
		virtual BOOL BroadCast( BYTE * buf , int &len ){return FALSE;} 
		//�жϱ��ĺ�����
		virtual BOOL WhetherBufValid( const BYTE *buf, int &len, int &pos );

		/* ====================  MUTATORS      ======================================= */

		/* ====================  OPERATORS     ======================================= */


	protected:
		/* ====================  METHODS  ======================================= */
		// ����״̬ 
		void SetState ( DWORD dwState );
		// ȡ��״̬
		void UnsetState ( DWORD dwState );
		// �鿴�Ƿ��и�����
		BOOL IsHaveState ( DWORD dwState ) const;
		// ������
		void OpenLink ( void );
		// �ر�����
		void CloseLink ( void );
		
		/* ====================  DATA MEMBERS  ======================================= */
		char m_sMasterAddr[ 200 ] ;//�����������
		char m_szPrintBuf[256];
		void print( const char *szBuf );   //�ڲ���ӡ����
		DWORD m_ProtocolState;

	private:
		/* ====================  METHODS	   ======================================= */
		//��ȡ�����е�װ������
		BOOL GetDevData( void ) ;
		//���������ļ�����
		BOOL ProcessFileData( CProfile &profile );
		//����ģ��
		BOOL CreateModule( int iModule ,
				char * sMasterAddr ,
				int iAddr , 
				char * sName , 
				char * stplatePath);
		//��ʼ��ģ�����
		BOOL InitModule( CProtocol_DataTrans * pProtocol ,
				int iModule , 
				char * sMasterAddr ,
				int iAddr ,
				char * sName ,
				char * stplatePath );
		/* ====================  DATA MEMBERS  ======================================= */

}; /* -----  end of class CProtocol_DataTrans  ----- */


#endif   /* ----- #ifndef CPROTOCOL_DATATRANS_INC  ----- */

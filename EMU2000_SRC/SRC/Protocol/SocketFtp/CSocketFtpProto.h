/*
 * =====================================================================================
 *
 *       Filename:  CSocketFtpProto.h
 *
 *    Description:  Э���� 
 *
 *        Version:  1.0
 *        Created:  2015��09��24�� 11ʱ55��51��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp
 *   Organization:  
 *
 *		  history:
 *
 * =====================================================================================
 */

#ifndef  CSOCKETFTPPROTO_INC
#define  CSOCKETFTPPROTO_INC

#include "../../share/typedef.h"
#include "CSocketFtpFile.h"
#include "../../share/md5.h"

#define					FTP_MAX_SEND_LEN			1028			/* ����ͳ��� */

/*
 * =====================================================================================
 *        Class:  CSocketFtpProto
 *  Description:  
 * =====================================================================================
 */
class CSocketFtpProto
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		CSocketFtpProto ();                             /* constructor      */
		~CSocketFtpProto ();                            /* destructor       */

	public:
		//����Э�鱨��
		BOOL ProcessProtoSendBuf ( BYTE *buf, int len );
		//��ʼ��Э��
		BOOL Init ( void  );


	protected:
		/* ====================  DATA MEMBERS  ======================================= */

	private:

		//�����ļ�����
		void SetFileName ( const char *pchFileName );

		//���ô���λ
		void SetErrorBit ( void );
		//���ûظ�λ
		void SetResponseBit ( void );
		// �����ļ�����λ 
		void SetFileBit ( void );
		// �Ƿ����ļ�װ̬
		BOOL isFileState ( void ) const;
		//�����ϴ�����λ
		void SetDownLoadBit ( void );
		//�Ƿ����ϴ�װ̬
		BOOL IsLoad ( void ) const;
		// ���ù���λ 
		void SetFuncCode ( BYTE byFunc );
		// ��ù����� 
		BYTE GetFuncCode ( void ) const;
		//�����ļ�ͷ
		void AddFrameHead ( void );

		//�Ƿ����֡��ʽ
		BOOL IsFrameFormat ( BYTE *buf, int len );
		//����װ̬�ֽ�
		BOOL ProcessStateByte ( BYTE byState );
		//�����ϴ�����
		BOOL ProcessLoadData ( BYTE *buf, int len );
		//������������
		BOOL ProcessDownData ( BYTE *buf, int len );
		//�����ϴ����� ��ʼ���䷢��
		BOOL PLD_BeginTrans ( void );
		//�����ϴ�����1 �ļ��б���Ϣ����
		BOOL PLD_FileListInfo ( void );
		//�����ϴ�����1 �ļ���Ϣ����
		BOOL PLD_FileInfo ( void );
		//�����ϴ������ļ�
		BOOL PLD_File ( void );
		//�����ϴ�����md5
		BOOL PLD_MD5 ( void );
		//�����ϴ�����15
		BOOL PLD15 ( void );
		//�����ϴ�����6
		BOOL PLD6 ( void );
		//�����������ݽ�������
		BOOL PDD_endTrans ( void );
		//�������������ļ��б�
		BOOL PDD_FileListInfo ( void );
		//�������������ļ�
		BOOL PDD_FileInfo ( void );
		//������������6
		BOOL PDD6 ( const char *chMd5Buf );


		/* ====================  DATA MEMBERS  ======================================= */

	public:
		/* ====================  DATA MEMBERS  ======================================= */
		BYTE m_bySendBuf[ FTP_MAX_SEND_LEN ];
		WORD m_wSendLen;
		CSocketFtpFile m_FtpFile;
		BOOL m_bReboot;                         /* �Ƿ����� */

	private:
		/* ====================  DATA MEMBERS  ======================================= */
		char m_chFileName[256];
		DWORD m_dwFileSize;

		FTP_FILE_TYPE m_FileType;
		UINT m_uiReadPos;
		WORD m_wFileNum;

		enum m_eFuncCode                        /* ������ */
		{
			NONE_FUNC = 0,                      /* ��Ч�Ĺ��� */
			START_TRANS = 1,                    /* ��ʼ���� */
			END_TRANS = 2,                      /* �������� */
			FILE_LIST_INFO = 3,                 /* �ļ��б���Ϣ */
			FILE_INFO = 4,                      /* �ļ���Ϣ */
			START_DOWN = 5,                     /* ��ʼ���� */
			MD5 = 6,                            /* md5 */
			REBOOT = 7,                         /* ���� */
			RECV_CONFIG = 15                    /* ȷ�Ͻ��� */
		};


}; /* -----  end of class CSocketFtpProto  ----- */


#endif   /* ----- #ifndef CSOCKETFTPPROTO_INC  ----- */


#ifndef CPROTOCOL_LFP_H
#define CPROTOCOL_LFP_H

#include "../../share/CProtocol.h"
#include "../../share/CMethod.h"
#include <time.h>
#include <sys/time.h>

using namespace std;

#define LFPPREFIXFILENAME				"/myapp/config/LFP/template/"  /* LFP�̶�����ǰ׺ */

// #define	LFPPRINT			/* ��ӡ���ն� */
#define	LFPDEBUG			/* ��ӡ������ */

#define	LFP_READDATA_DATATYPE		1				/* ����ǰ������������ */
#define	LFP_TIME_DATATYPE			2				/* ��ʱ�������� */


#define	LFP_MAX_BUF_LEN			256			/* ��󻺴������� */
#define	LFP_MAX_RESEND_COUNT		3			/* ����ط�����*/
#define	LFP_MAX_RECV_ERR_COUNT	3			/* �����մ������*/

extern "C" void GetCurrentTime( REALTIME *pRealTime );

/*
 * =====================================================================================
 *        Class:  CLFPCfgInfo
 *  Description:  ������Ϣ
 * =====================================================================================
 */
class CProtocol_LFP : public CProtocol
{
    public:
        CProtocol_LFP();
        virtual ~CProtocol_LFP();

	protected://Э��ͨ�ú���
		//��ȡ�����ļ����װ������
		BOOL GetDevData( ) ;
		//���������ļ�����
		BOOL ProcessFileData( CProfile &profile );
		//����ģ��
		BOOL CreateModule( int iModule , int iSerialNo , WORD iAddr , char * sName , char * stplatePath ) ;
	public:
		/* �麯�� */
		//��ʼ������
		virtual BOOL Init( BYTE byLineNo  ) ;
		//��ȡУ���
		virtual WORD GetCrc( BYTE * pBuf , int len );

	protected:
		//��ӡ������Ϣ
		void print( const char *szBuf, int len=0 );
};


#endif // CPROTOCOL_LFP_H


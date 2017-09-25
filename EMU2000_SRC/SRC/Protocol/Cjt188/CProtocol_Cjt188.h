
#ifndef CPROTOCOL_CJT188_H
#define CPROTOCOL_CJT188_H

#include "../../share/CProtocol.h"
#include "../../share/CMethod.h"
#include <time.h>
#include <sys/time.h>

using namespace std;

#define CJT188PREFIXFILENAME				"/myapp/config/Cjt188/template/"  /* Cjt188�̶�����ǰ׺ */

// #define	CJT188PRINT			/* ��ӡ���ն� */
#define	CJT188DEBUG			/* ��ӡ������ */

#define	CJT188_READDATA_DATATYPE		1				/* ����ǰ������������ */
#define	CJT188_TIME_DATATYPE			2				/* ��ʱ�������� */


#define	CJT188_MAX_BUF_LEN			256			/* ��󻺴������� */
#define	CJT188_MAX_RESEND_COUNT		3			/* ����ط�����*/
#define	CJT188_MAX_RECV_ERR_COUNT	3			/* �����մ������*/

extern "C" void GetCurrentTime( REALTIME *pRealTime );

/*
 * =====================================================================================
 *        Class:  CJT188CfgInfo
 *  Description:  ������Ϣ
 * =====================================================================================
 */
class Cjt188CfgInfo
{
	public:
		Cjt188CfgInfo ()                             /* constructor */
		{
			byDataType = 0;
			byDI0 = 0;
			byDI1 = 0;
			byCycle = 1;
		}
	
		BYTE byDataType;	//�������� Cjt188_YC_DATATYPE��ң�� Cjt188_YM_DATATYPE:ң��
		BYTE byDI0;			//���ݱ�ʶ0
		BYTE byDI1;			//���ݱ�ʶ1
		BYTE byCycle;		//ѭ������
}; /* -----  end of class Cjt188CfgInfo  ----- */


class CProtocol_Cjt188 : public CProtocol
{
    public:
        CProtocol_Cjt188();
        virtual ~CProtocol_Cjt188();

		/* �麯�� */
		//��ȡЭ�鱨��
		virtual BOOL GetProtocolBuf( BYTE * buf , int &len , PBUSMSG pBusMsg = NULL );
		//����Э�鱨��
		virtual BOOL ProcessProtocolBuf( BYTE * buf , int len );
		//��ʼ������
		virtual BOOL Init( BYTE byLineNo  ) ;
		//�㲥����
		virtual BOOL BroadCast( BYTE * buf , int &len ) ;
		//ʱ�䴦��
		virtual void TimerProc(){ return;  } 

	public:
		BYTE m_bySendPos;	//���Ͳ���
		BYTE m_byDataType;	//��������
		vector <Cjt188CfgInfo> m_CfgInfo; //������Ϣ
		char m_szPrintBuf[256];	//��ӡ������
		BYTE m_bySlaveAddr[7];
		BYTE  m_byMeterType;
		BYTE m_byFENum;
		BYTE m_bySer;
	
	protected://�麯��
		//��ȡУ���
		virtual BYTE GetCs( const BYTE * pBuf , int len );
		//�жϱ�����Ч��
		virtual BOOL WhetherBufValue(const BYTE *buf, int &len, int &pos );
		//��ȡ�����ļ�
		virtual BOOL ReadCfgInfo( void );
		//�ƶ�����λ��
		virtual BYTE ChangeSendPos( void );

	protected:
		//��ӡ������Ϣ
		void print( const char *szBuf, int len=0 );

	protected://Э��ͨ�ú���
		//��ȡ�����ļ����װ������
		BOOL GetDevData( ) ;
		//���������ļ�����
		BOOL ProcessFileData( CProfile &profile );
		//����ģ��
		BOOL CreateModule( int iModule , int iSerialNo , WORD iAddr , char * sName , char * stplatePath ) ;
		// �ַ���ת16����
		DWORD atoh ( char *szBuf, BYTE len, BYTE byFlag );
		//16����תBCD�� 
		BYTE HexToBcd( BYTE c );
		//BCD��ת16����
		BYTE BcdToHex( BYTE c );

};


#endif // CPROTOCOL_Cjt188_H


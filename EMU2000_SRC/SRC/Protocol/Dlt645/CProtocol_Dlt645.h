
#ifndef CPROTOCOL_Dlt645_H
#define CPROTOCOL_Dlt645_H

#include "../../share/CProtocol.h"
#include "../../share/CMethod.h"
#include <time.h>
#include <sys/time.h>

using namespace std;

#define DLT645PREFIXFILENAME				"/mynand/config/Dlt645/template/"  /* Dlt645�̶�����ǰ׺ */

//#define	DLT645PRINT			/* ��ӡ���ն� */
//#define	DLT645DEBUG			/* ��ӡ������ */

#define	DLT645_YC_DATATYPE		2				/* ң���������� */
#define	DLT645_YM_DATATYPE		4				/* ң���������� */
#define	DLT645_TIME_DATATYPE	3				/* ң���������� */


#define	DLT645_MAX_BUF_LEN			256			/* ��󻺴������� */
#define	DLT645_MAX_RESEND_COUNT		3			/* ����ط�����*/
#define	DLT645_MAX_RECV_ERR_COUNT	3			/* �����մ������*/

extern "C" void GetCurrentTime( REALTIME *pRealTime );

/*
 * =====================================================================================
 *        Class:  Dlt645CfgInfo
 *  Description:  ������Ϣ
 * =====================================================================================
 */
class Dlt645CfgInfo
{
	public:
		Dlt645CfgInfo ()                             /* constructor */
		{
			byDataType = 0;
			byDI0 = 0;
			byDI1 = 0;
			byDI2 = 0;
			byDI3 = 0;
			byDataNum = 0;
			byStartIndex = 0;
			byDataFormat = 0;
			byDataLen = 0;
			byFENum = 0;
			byCycle = 1;
		}
	
		BYTE byDataType;	//�������� Dlt645_YC_DATATYPE��ң�� Dlt645_YM_DATATYPE:ң��
		BYTE byDI0;			//���ݱ�ʶ0
		BYTE byDI1;			//���ݱ�ʶ1
		BYTE byDI2;			//���ݱ�ʶ2		
		BYTE byDI3;			//���ݱ�ʶ3
		BYTE byDataNum;		//��������
		BYTE byStartIndex;  //��ʼ���
		BYTE byDataFormat;	//���ݸ�ʽ
		BYTE byDataLen;		//���ݳ���
		BYTE byFENum;		//0xfe �ĸ���
		BYTE byCycle;		//ѭ������
}; /* -----  end of class Dlt645CfgInfo  ----- */


class CProtocol_Dlt645 : public CProtocol
{
    public:
        CProtocol_Dlt645();
        virtual ~CProtocol_Dlt645();
		virtual BOOL GetProtocolBuf( BYTE * buf , int &len , PBUSMSG pBusMsg = NULL );
		virtual BOOL ProcessProtocolBuf( BYTE * buf , int len );
		virtual BOOL Init( BYTE byLineNo  ) ;
		//��ȡУ���
		virtual BYTE GetCs( const BYTE * pBuf , int len );
		virtual BOOL BroadCast( BYTE * buf , int &len ) ;
		virtual void TimerProc(){ return;  } 
		//�жϱ�����Ч��
		virtual BOOL WhetherBufValue(const BYTE *buf, int &len, int &pos );
		//��ȡ�����ļ�
		virtual BOOL ReadCfgInfo( void );
		//����̶���ʽ����
		virtual BOOL CalFormatData( const BYTE *buf, BYTE byDataFormat, BYTE byDataLen, DWORD &dwData );
		virtual BYTE ChangeSendPos( void );

	public:
		BYTE m_bySendPos;	//���Ͳ���
		BYTE m_byDataType;	//��������
		vector <Dlt645CfgInfo> m_CfgInfo; //������Ϣ
		char m_szPrintBuf[256];	//��ӡ������
		BYTE m_bySlaveAddr[6];

	protected:
		BOOL GetDevData( ) ;
		void print( const char *szBuf, int len=0 );
	protected:
		BOOL ProcessFileData( CProfile &profile );
		BOOL CreateModule( int iModule , int iSerialNo , WORD iAddr , char * sName , char * stplatePath ) ;
		DWORD atoh ( char *szBuf );
		DWORD atoh ( char *szBuf , BYTE len, BYTE byFlag);
		//�ַ���ת16����
		// DWORD atoh ( char *szBuf );

	private:
		//16����תBCD��
		// BYTE HexToBcd ( BYTE c );
		//��m_bySlaveAddr��ֵ
		BOOL ReadCfgSlaveAddr( char *szLineBuf );
		//������ֵ
		BOOL ReadCfgVal( char *szLineBuf );
};


#endif // CPROTOCOL_Dlt645_H


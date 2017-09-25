#ifndef CPROTOCOL_IEC101S_H
#define CPROTOCOL_IEC101S_H

#include "../../share/Rtu.h"
#include "../../share/CMethod.h"
#include "../../share/typedef.h"



// #define		IEC101S_PRINT			/* ��ӡ���ն� */
#define		IEC101S_DEBUG			/* ��ӡ������ */

#define	IEC101SPREFIXFILENAME			"/mynand/config/IEC101Slave/"	/* 104�̶�·�� */
#define MODULE_IEC101S_2002		1  //��׼101 2002��   
#define MODULE_IEC101S_1997		2  //��׼101 1997��   


#define	IEC101S_TOTAL_CALL		0x00000001				/* ���ٻ� */
#define	IEC101S_TOTAL_YX		0x00000002				/* ң�� */
#define	IEC101S_TOTAL_YC	    0x00000004				/* ң�� */
#define	IEC101S_TOTAL_YM		0x00000008				/* ң�� */
#define	IEC101S_CALL_YM			0x00000010				/* �ٻ�ң�� */
#define	IEC101S_TIME_SYNC		0x00000020				/* ʱ��ͬ�� */
#define	IEC101S_YK_SEL			0x00000040				/* ң��ѡ�� */
#define	IEC101S_YK_EXE			0x00000080				/* ң��ִ�� */
#define	IEC101S_YK_CANCEL		0x00000100				/* ң��ȡ�� */
#define	IEC101S_TOTAL_CALL_END  0x00000200				/* ���ٻ����� */
#define	IEC101S_CALL_YM_END	    0x00000400				/* �ٻ�ң������ */
#define	IEC101S_TIME_SYNC_END   0x00000800				/* ��ʱ���� */
#define	IEC101S_SPECIAL_DATA	0x00001000				/* �������ݽ׶� ��ָ���ٶ�ʱң���Ƚ׶� */
#define	IEC101S_CHANGE_YX	    0x00002000				/* �仯ң�� */
#define	IEC101S_CHANGE_YC	    0x00004000				/* �仯ң�� */
#define	IEC101S_SOE_YX			0x00008000				/* ң���¼� */


// typedef enum _IEC101SSENDFLAG
// {
	// NULL_FLAG,				//��ʶ�ÿ�
	// TOTAL_CALL_BEGIN,       //���ٻ���ʼ
	// TOTAL_CALL_YX,			//���ٻ�ң��
	// TOTAL_CALL_YC,			//���ٻ�ң��
	// TOTAL_CALL_YM_BEGIN,    //���ٻ�ң����ʼ
	// TOTAL_CALL_YM,			//���ٻ�ң��

	// TIME_SYNC,				//ʱ�ӱ�ʶ

	// YK_BEGIN,				//ң�ر�ʶ
	// YK_RETURN,              //ң�ط���
// }IEC101SSENDFLAG;

typedef enum _IEC101SSENDSTATUS
{	
	/* �Զ���  */
	NULL_STATUS,            //�޷���״̬
	RESEND,					//�ط�״̬
	LEVEL1_DATA,				//һ������
	LEVEL2_DATA,				//��������
	TOTAL_CALL,				//���ٻ�
	TIME_SYNC,				//��ʱ
	CALL_YM,				//�ٻ�ң��
	YK_RTN_DATA,			//ң�ط�������
	LINK_STATUS,		   //��Ӧ֡ ��·״̬��Ҫ�����
	RECOGNITION,		   //ȷ��֡ �Ͽ�  0
	DENY_RECOGNITION,      //ȷ��֡ ���Ͽ�:δ�յ����ģ� ��·æ 1
	USER_DATA,			   //��Ӧ֡ �û�����
	NONE_USER_DATA,		   //��Ӧ֡ ���Ͽ� ������������

	// /* �̶�֡ */

	// /* �仯֡ */
	// M_SP_NA_1,			   //������Ϣ	ASDU1
	// M_SP_TA_1,			   //��ʱ��ĵ�����Ϣ ASDU2
	// M_DP_NA_1,			   //˫����Ϣ	ASDU3 
	// M_DP_TA_1,			   //��ʱ���˫����Ϣ ASDU4
	// // M_ST_NA_1,			   //��λ��ASDU5 
	// // M_ST_TA_1,			   //��ʱ��Ĳ�λ����Ϣ ASDU6 
	// // M_BO_NA_1,			   //32���ش� ASDU7
	// // M_BO_TA_1,			   //��ʱ���32���ش� ASDU8
	// M_ME_NA_1,			   //����ֵ ��һ��ֵ ASDU9
	// M_ME_TA_1,			   //����ֵ ��ʱ��Ĺ�һ��ֵ ASDU10
	// M_ME_NB_1,			   //����ֵ ��Ȼ�ֵASDU11
	// M_ME_TB_1,			   //����ֵ ��ʱ��ı�Ȼ�ֵASDU12
	// M_ME_NC_1,			   //����ֵ �̸�����ASDU13
	// M_ME_TC_1,			   //����ֵ ��ʱ��Ķ̸�����ASDU14
	// M_IT_NA_1,			   //�ۼ��� ASDU15
	// M_IT_TA_1,			   //��ʱ����ۼ���ASDU16
	// // M_EP_TA_1,			   //��ʱ��ļ̵����¼�ASDU17
	// // M_EP_TB_1,			   //��ʱ��ļ̵������������¼�ASDU18
	// // M_EP_TC_1,			   //��ʱ��ĳ��������ϢASDU19
	// // M_PS_NA_1,			   //����λ������鵥����ϢASDU20
	// M_ME_ND_1,			   //����ֵ,����Ʒ������ֵ�ù�һ��ֵASDU21
	// M_SP_TB_1,			   //��CP56Time2aʱ��ĵ�����Ϣ ASDU30          
	// M_DP_TB_1,             //��CP56Time2aʱ���˫����Ϣ ASDU31         
	// // M_ST_TB_1,             //��CP56Time2aʱ��Ĳ�λ����Ϣ   ASDU32 
	// // M_BO_TB_1,             //��CP56Time2aʱ���32���ش� ASDU33    
	// M_ME_TD_1,             //��CP56Time2aʱ��Ĳ���ֵ, ��һ��ֵ ASDU34
	// M_ME_TE_1,             //��CP56Time2aʱ��Ĳ���ֵ, ��Ȼ�ֵ ASDU35
	// M_ME_TF_1,             //��CP56Time2aʱ��Ĳ���ֵ, �̸����� ASDU36
	// M_IT_TB_1,             //��CP56Time2aʱ����ۼ���           ASDU37
	// // M_EP_TD_1,             //��CP56Time2aʱ��ļ̵籣���豸�¼�ASDU38
	// // M_EP_TE_1,             //��CP56Time2aʱ��ļ̵籣���豸���������¼� ASDU39
	// // M_EP_TF_1,             //��CP56Time2aʱ��ļ̵籣���豸���������·��ϢASDU40 
	// // 
	// //
	
}IEC101SSENDSTATUS;				/* ----------  end of enum IEC101SSENDSTATUS  ---------- */

class CProtocol_IEC101S : public CRtuBase
{
    public:
    CProtocol_IEC101S();
    virtual ~CProtocol_IEC101S();
	BOOL GetDevData( );
	BOOL ProcessFileData( CProfile &profile );
	BOOL CreateModule( int iModule ,char * sMasterAddr , WORD iAddr , char * sName , char * stplatePath ) ;
	BOOL InitIEC101S_Module( CProtocol_IEC101S * pProtocol , int iModule , char * sMasterAddr , WORD iAddr , char * sName , char * stplatePath );
	virtual BOOL Init( BYTE byLineNo );
	//��ȡУ���
	virtual BYTE GetCs( const BYTE *pBuf, int len );
	//�жϱ��ĺ�����
	virtual BOOL WhetherBufValid( const BYTE *buf, int &len );
	//�жϽ��ձ�ʶλ
	virtual BOOL ProcessJudgeFlag( BYTE c );


	protected:
	char m_sMasterAddr[ 200 ] ;//�����������
	void print( const char *szBuf, int len=0 );   //�ڲ���ӡ����

	public:
		char m_szPrintBuf[256];
		BOOL m_bFcb;
		IEC101SSENDSTATUS m_SendStatus;
		// IEC101SSENDFLAG m_SendFlag;
		DWORD m_dwSendFlag;
    private:
};

#endif // CPROTOCOL_IEC101S_H

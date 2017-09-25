/********************************************************************
*  ��Ϣ�������ṹ����
********************************************************************/
#ifndef  _MSGDEF_H_
#define  _MSGDEF_H_

/*******************************************************************/
#define MSG_NAME_LEN  24  /*��Ϣ���Ƴ���*/
#define MSG_BODY_LEN  512 /*��Ϣ���ݳ���*/
#define MSG_POOL_SUM  64  /*��Ϣ��������*/
#define MSG_SLOT_SUM  16  /*��Ϣ�������*/

/*������Ϣ*/
#define MSGSET_RAW_DATA     0   /*ԭʼ����, ң������ң������SOE�����������������ݵ�*/
#define MSGSET_CTRL_ECHO    1   /*������Ӧ, ң�ء�����������                     */
#define MSGSET_DEVS_DATA    2   /*װ������, ��������ֵ��¼���������               */
#define MSGSET_FES_NOTIFY   3   /*ǰ��(�����)����, ͨ�����豸ͨѶ״̬             */
#define MSGSET_FES_REQUEST  4   /*ǰ��(�����)����                                 */

/*������Ϣ*/
#define MSGSET_CTRL_DATA    8   /*��������, ң�ء�����������      */
#define MSGSET_DEVS_COMM    9   /*װ�ò�ѯ, ��������ֵ��¼���������*/
#define MSGSET_FES_RECEIVE  10  /*ǰ��(�����)������Ϣ              */
#define MSGSET_FES_SWITCH   11  /*ǰ��(�����)�л���Ϣ              */

/*******************************************************************/
#pragma pack(1)

/*��Ϣ�ṹ*/
typedef struct
{
	int   nSrcKey;        /*��Դ��ʶ*/
	short nActive;        /*���־*/
	unsigned short  wLevel;         /*���ȼ���*/
	unsigned short  wTypes;         /*��Ϣ����*/
	unsigned short  wMsgLen;        /*��Ϣ����*/
	unsigned char   byBuff[MSG_BODY_LEN]; /*��Ϣ����*/
} MSGITEM;

/*��Ϣ����*/
typedef struct tagMSGLIST
{
	int    self;  /*�����*/
	int    next;  /*�����*/
} MSGLIST;

/*������Ϣ��*/
typedef struct tagPROCSLOT
{
	char  szProcName[MSG_NAME_LEN]; /*��������*/
	int   nStatus;   /*ͨ��״̬*/
	int   nProcKey;  /*���̱�ʶ*/
	int   dwMsgCtrl; /*���Ŀ���*/
	int   nMsgPos;   /*��Ϣλ��*/
} PROCSLOT;

/*��Ϣ����*/
typedef struct
{
	int  nQuality;	           /*�ռ�״̬*/
	int  nFreeNum;	           /*��������*/
	int  nFreePos, nFreeTail;  /*����λ��*/
	MSGLIST  vect[MSG_POOL_SUM]; /*��Ϣ�����ռ�*/
	MSGITEM  pool[MSG_POOL_SUM]; /*��Ϣ����ռ�*/
	PROCSLOT slot[MSG_SLOT_SUM]; /*��Ϣͨ���ռ�*/
} MSGSTORE;

#pragma pack()

/*******************************************************************/
#endif   /*_MSGDEF_H_*/

/********************************************************************
*  ���ݿⳣ�����ṹ����
********************************************************************/
#ifndef  _RTDB_H_
#define  _RTDB_H_

#include "msgdef.h"
#include "typedef.h"

#define SHM_STRUCT_VER  0x02000001
/*******************************************************************/
#define STN_NAME_SIZE 16
#define PNT_CODE_SIZE 24
#define PNT_NAME_SIZE 48

#define MAX_STN_SUM  600
// #define MAX_ANALOG_SUM  8192
// #define MAX_DIGITAL_SUM 8192
#define MAX_ANALOG_SUM  16384
#define MAX_DIGITAL_SUM 16384
#define MAX_PULSE_SUM   10240
#define MAX_RELAY_SUM   1024
#define MAX_ADJUST_SUM   256

#define SOE_QUEUE_SUM 256  /*SOE���г���*/
#define AIE_QUEUE_SUM 256  /*AIE���г���*/

/*ģ��������������*/
#define AICTRL_ENABLE     0x0001     /* �Զ�/�ֶ� */
#define AICTRL_POLARITY   0x0002     /* ˫/������ */

/*����������������*/
#define DICTRL_ENABLE     0x0001     /* �Զ�/�ֶ� */
#define DICTRL_DOUBLEBIT  0x0002     /* ˫λң��  */
#define DICTRL_OPPOSITE   0x0004     /* ״̬ȡ��  */
#define DICTRL_AUTORESET  0x0008     /* �Զ�����  */
/*������״̬������*/
#define DISTATUS_VALUE    0x0003     /* ֵ(˫λB0,B1) */
#define DISTATUS_VALID    0x0100     /* ��Ч/��Ч���� */
#define DISTATUS_ALARMING 0x0200     /* ��λ������־  */
/*������״̬��־*/
#define DIVALUE_FALSE1	0
#define DIVALUE_ON		1
#define DIVALUE_OFF		2
#define DIVALUE_FALSE2	3

/*******************************************************************/
#pragma pack(1)

/*ACSIʱ��*/
typedef struct
{
	unsigned SecondSinceEpoch:32;
	unsigned FractionOfSecond:24;
	unsigned TimeQuality:8;
}ACSI_TIMESTAMP;

/*SOE�ṹ*/
typedef struct
{
	LONG  lTime;		/*��ʱ��*/
	WORD  wMiSecond;    /*������*/
	BYTE  byState;      /*״ֵ̬*/
	WORD  wStnID;      /*վ���*/
	WORD  wPntNum;      /*����*/
	WORD  wAttrib;      /*������ 0=״̬��λ  1=SOE��Ϣ  2=��ʱ���λ*/
} SOEITEM;

/*�仯ң��ṹ*/
typedef struct
{
	BYTE  byType;    /*���ͣ�����*/
	WORD  wStnID;   /*վ���*/
	WORD  wPntNum;   /*����*/
	int32 dwValue;	 /*��ǰֵ: ������*/
	float fValue;	 /*��ǰֵ: ����ֵ*/
} AIEITEM;

/*****************************************************************************/
/*ģ������ṹ*/
typedef struct tagAI_ITEM
{
    WORD  wPntID;	  /*���ʶ*/
    BYTE  byType;     /*����: 0=��ѹ,1=����,2=�й�,3=�޹�,4=��������,5=Ƶ��,6=�¶�*/
    BYTE  byUnit;     /*��λ*/
    char  szName[PNT_NAME_SIZE]; /*�������*/
    int   iTransNum;  /*ȫ�ֱ��*/
	WORD  wPntCtrl;	  /*�������*/
	WORD  wThreshold; /*�仯��ֵ*/
    float fRatio;     /*ת��ϵ��*/
    float fOffset;    /*���ƫ��*/
    
    int32 dwRawVal; /*ԭʼֵ*/
    float fRealVal; /*һ��ֵ*/
	ACSI_TIMESTAMP ACSITime; /*ACSIʱ��*/
} ANALOGITEM;

/*��������ṹ*/
typedef struct tagDI_ITEM
{
    WORD  wPntID;	  /*���ʶ*/
    BYTE  byType;     /*����:0=����,1=���뵶բ,2=�ӵص�բ,3=�����ź�(һ��/Ԥ��/�¹�),4=״̬�ź�,5=λ����Ϣ,6=�����ź�*/
    BYTE  byAttr;     /*����:*/
    char  szName[PNT_NAME_SIZE]; /*�������*/
	int   iTransNum;  /*ȫ�ֱ��*/
    WORD  wPntCtrl;	  /*�������*/
	short wEvtCode;   /*�����*/
	WORD  wReserve;   /*����  */
    WORD  wStatus;    /*��״̬*/
	ACSI_TIMESTAMP ACSITime; /*ACSIʱ��*/
} DIGITALITEM;

/*���ܵ�ṹ*/
typedef struct tagPI_ITEM
{
    WORD  wPntID;	  /*���ʶ*/
    BYTE  byType;     /*������*/
    BYTE  byAttr;     /*������*/
    char  szName[PNT_NAME_SIZE]; /*�������*/
    int   iTransNum;  /*ȫ�ֱ��*/
	WORD  wPntCtrl;	  /*�������*/
	WORD  wReserve;   /*����    */
	float fRatio;     /*ת������*/
    DWORD dwRawVal;   /*���ֵ*/
	ACSI_TIMESTAMP ACSITime; /*ACSIʱ��*/
} PULSEITEM;

/*������ṹ*/
typedef struct tagDO_ITEM
{
    WORD  wPntID;	  /*���ʶ*/
    BYTE  byType;     /*����:*/
    BYTE  byAttr;     /*����:*/
    char  szName[PNT_NAME_SIZE]; /*������*/
    int   iTransNum;  /*ȫ�ֱ��*/
    WORD  wPntCtrl;	  /*������*/
    WORD  wStatus;    /*״̬��*/
} RELAYITEM;

/*�趨��ṹ*/
typedef struct tagDA_ITEM
{
    WORD  wPntID;	  /*���ʶ*/
    BYTE  byType;     /*����:*/
    BYTE  byAttr;     /*����:*/
    char  szName[PNT_NAME_SIZE]; /*������*/
    int   iTransNum;  /*ȫ�ֱ��*/
	float fRatio;     /*ת��ϵ��*/
    float fOffset;    /*���ƫ��*/
    int32 dwRawVal;   /*ԭʼֵ*/
	float fRealVal;   /*һ��ֵ*/
} ADJUSTITEM;

/*վ��װ�����Խṹ*/
typedef struct
{
	WORD  wStnNum;  /*���*/
	WORD  wStatus;  /*״̬*/
    char  szStnName[STN_NAME_SIZE];
	/*�������*/
    WORD  wAnalogSum;
    WORD  wDigitalSum;
    WORD  wRelaySum;
	WORD  wPulseSum;
	WORD  wAdjustSum;
	WORD  wBak1Sum;
	WORD  wBak2Sum;
	WORD  wBak3Sum;
	/*���λ��*/
	DWORD  dwAnalogPos;
	DWORD  dwDigitalPos;
	DWORD  dwRelayPos;
	DWORD  dwPulsePos;
	DWORD  dwAdjustPos;
	DWORD  dwBak1Pos;
	DWORD  dwBak2Pos;
	DWORD  dwBak3Pos;
    /*���ݱ�ָ��*/  
    ANALOGITEM   *pAnalogTab;
	DIGITALITEM  *pDigitalTab;
	RELAYITEM	 *pRelayTab;
	PULSEITEM	 *pPulseTab;
    void  *pTabRes0;
    void  *pTabRes1;
    void  *pTabRes2;
    void  *pTabRes3;
} STNPARAM;

/*���ݿ�ṹ*/
typedef struct
{
	/*��վ����*/
	STNPARAM  StnUnit[MAX_STN_SUM];
	/*���ռ�*/
	ANALOGITEM   AnalogTable[MAX_ANALOG_SUM];
	DIGITALITEM  DigitalTable[MAX_DIGITAL_SUM];
	RELAYITEM    RelayTable[MAX_RELAY_SUM];
	PULSEITEM    PulseTable[MAX_PULSE_SUM];
}RTDBASE;

/*��Ҫ����*/
typedef struct
{
	WORD   wState;		/*���ݿ��־  */
	WORD   wStnSum;     /*վ��װ������*/
	int32  nAnalogSum;  /*ģ��������  */
	int32  nDigitalSum; /*����������  */
	int32  nRelaySum;   /*����������  */
	int32  nPulseSum;   /*����������  */
	int32  nAdjustSum;  /*���������  */
	int32  nSpare1;		/*����*/
	int32  nSpare2;		/*����*/
	int32  nSpare3;		/*����*/
	int32  nSOEWritePos; /*SOE����дλ��*/
	int32  nAIEWritePos; /*AIE����дλ��*/
	int32  nSpare4;		/*����*/
	int32  nSpare5;		/*����*/
	int32  nSpare6;		/*����*/
	int32  nRunMode;	/*���з�ʽ(0:���� 1:�ȱ�)*/
	int32  nIsDuty;		/*����״̬(0:���� 1:ֵ��)*/
}SYSINFO;

/*�����ڴ�ռ�ṹ*/
typedef struct
{
	DWORD    dwAllSize;	/*ȫ���ռ䳤��*/
	DWORD    dwExtSize;	/*���ӿռ䳤��*/
	DWORD    dwQuality;	/*Ʒ�ʱ�־*/
	DWORD    dwEdition;	/*�ṹ�汾*/
	SYSINFO  sysInfo;   /*��Ҫ��Ϣ*/
	RTDBASE  RTDBase;	/*���ݿ�ռ�*/
	SOEITEM  soeArray[SOE_QUEUE_SUM]; /*SOE�ռ�*/
	AIEITEM  aieArray[AIE_QUEUE_SUM]; /*AIE�ռ�*/
	MSGSTORE msgStore;  /*��Ϣ����*/
    /*�����ṹ*/
	void    *pExtAddr;  /*���ӿռ�λ��*/
}SHM_SPACE;

#pragma pack()

/*******************************************************************/
#endif   /*_RTDB_H*/

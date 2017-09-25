/********************************************************************
*  数据库常数及结构定义
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

#define SOE_QUEUE_SUM 256  /*SOE队列长度*/
#define AIE_QUEUE_SUM 256  /*AIE队列长度*/

/*模拟量控制字属性*/
#define AICTRL_ENABLE     0x0001     /* 自动/手动 */
#define AICTRL_POLARITY   0x0002     /* 双/单极性 */

/*数字量控制字属性*/
#define DICTRL_ENABLE     0x0001     /* 自动/手动 */
#define DICTRL_DOUBLEBIT  0x0002     /* 双位遥信  */
#define DICTRL_OPPOSITE   0x0004     /* 状态取反  */
#define DICTRL_AUTORESET  0x0008     /* 自动复归  */
/*数字量状态字属性*/
#define DISTATUS_VALUE    0x0003     /* 值(双位B0,B1) */
#define DISTATUS_VALID    0x0100     /* 有效/无效数据 */
#define DISTATUS_ALARMING 0x0200     /* 变位报警标志  */
/*数字量状态标志*/
#define DIVALUE_FALSE1	0
#define DIVALUE_ON		1
#define DIVALUE_OFF		2
#define DIVALUE_FALSE2	3

/*******************************************************************/
#pragma pack(1)

/*ACSI时标*/
typedef struct
{
	unsigned SecondSinceEpoch:32;
	unsigned FractionOfSecond:24;
	unsigned TimeQuality:8;
}ACSI_TIMESTAMP;

/*SOE结构*/
typedef struct
{
	LONG  lTime;		/*秒时标*/
	WORD  wMiSecond;    /*毫秒数*/
	BYTE  byState;      /*状态值*/
	WORD  wStnID;      /*站编号*/
	WORD  wPntNum;      /*点编号*/
	WORD  wAttrib;      /*点属性 0=状态变位  1=SOE信息  2=带时标变位*/
} SOEITEM;

/*变化遥测结构*/
typedef struct
{
	BYTE  byType;    /*类型：备用*/
	WORD  wStnID;   /*站编号*/
	WORD  wPntNum;   /*点编号*/
	int32 dwValue;	 /*当前值: 生数据*/
	float fValue;	 /*当前值: 工程值*/
} AIEITEM;

/*****************************************************************************/
/*模拟量点结构*/
typedef struct tagAI_ITEM
{
    WORD  wPntID;	  /*点标识*/
    BYTE  byType;     /*类型: 0=电压,1=电流,2=有功,3=无功,4=功率因数,5=频率,6=温度*/
    BYTE  byUnit;     /*单位*/
    char  szName[PNT_NAME_SIZE]; /*测点名称*/
    int   iTransNum;  /*全局编号*/
	WORD  wPntCtrl;	  /*点控制字*/
	WORD  wThreshold; /*变化阈值*/
    float fRatio;     /*转换系数*/
    float fOffset;    /*零点偏移*/
    
    int32 dwRawVal; /*原始值*/
    float fRealVal; /*一次值*/
	ACSI_TIMESTAMP ACSITime; /*ACSI时间*/
} ANALOGITEM;

/*数字量点结构*/
typedef struct tagDI_ITEM
{
    WORD  wPntID;	  /*点标识*/
    BYTE  byType;     /*类型:0=开关,1=隔离刀闸,2=接地刀闸,3=保护信号(一般/预告/事故),4=状态信号,5=位置信息,6=控制信号*/
    BYTE  byAttr;     /*属性:*/
    char  szName[PNT_NAME_SIZE]; /*测点名称*/
	int   iTransNum;  /*全局编号*/
    WORD  wPntCtrl;	  /*点控制字*/
	short wEvtCode;   /*点编码*/
	WORD  wReserve;   /*保留  */
    WORD  wStatus;    /*点状态*/
	ACSI_TIMESTAMP ACSITime; /*ACSI时间*/
} DIGITALITEM;

/*电能点结构*/
typedef struct tagPI_ITEM
{
    WORD  wPntID;	  /*点标识*/
    BYTE  byType;     /*点类型*/
    BYTE  byAttr;     /*点属性*/
    char  szName[PNT_NAME_SIZE]; /*测点名称*/
    int   iTransNum;  /*全局编号*/
	WORD  wPntCtrl;	  /*点控制字*/
	WORD  wReserve;   /*保留    */
	float fRatio;     /*转换倍率*/
    DWORD dwRawVal;   /*表计值*/
	ACSI_TIMESTAMP ACSITime; /*ACSI时间*/
} PULSEITEM;

/*开出点结构*/
typedef struct tagDO_ITEM
{
    WORD  wPntID;	  /*点标识*/
    BYTE  byType;     /*类型:*/
    BYTE  byAttr;     /*属性:*/
    char  szName[PNT_NAME_SIZE]; /*点名称*/
    int   iTransNum;  /*全局编号*/
    WORD  wPntCtrl;	  /*控制字*/
    WORD  wStatus;    /*状态字*/
} RELAYITEM;

/*设定点结构*/
typedef struct tagDA_ITEM
{
    WORD  wPntID;	  /*点标识*/
    BYTE  byType;     /*类型:*/
    BYTE  byAttr;     /*属性:*/
    char  szName[PNT_NAME_SIZE]; /*点名称*/
    int   iTransNum;  /*全局编号*/
	float fRatio;     /*转换系数*/
    float fOffset;    /*零点偏移*/
    int32 dwRawVal;   /*原始值*/
	float fRealVal;   /*一次值*/
} ADJUSTITEM;

/*站或装置属性结构*/
typedef struct
{
	WORD  wStnNum;  /*编号*/
	WORD  wStatus;  /*状态*/
    char  szStnName[STN_NAME_SIZE];
	/*测点数量*/
    WORD  wAnalogSum;
    WORD  wDigitalSum;
    WORD  wRelaySum;
	WORD  wPulseSum;
	WORD  wAdjustSum;
	WORD  wBak1Sum;
	WORD  wBak2Sum;
	WORD  wBak3Sum;
	/*测点位置*/
	DWORD  dwAnalogPos;
	DWORD  dwDigitalPos;
	DWORD  dwRelayPos;
	DWORD  dwPulsePos;
	DWORD  dwAdjustPos;
	DWORD  dwBak1Pos;
	DWORD  dwBak2Pos;
	DWORD  dwBak3Pos;
    /*数据表指针*/  
    ANALOGITEM   *pAnalogTab;
	DIGITALITEM  *pDigitalTab;
	RELAYITEM	 *pRelayTab;
	PULSEITEM	 *pPulseTab;
    void  *pTabRes0;
    void  *pTabRes1;
    void  *pTabRes2;
    void  *pTabRes3;
} STNPARAM;

/*数据库结构*/
typedef struct
{
	/*厂站参数*/
	STNPARAM  StnUnit[MAX_STN_SUM];
	/*点表空间*/
	ANALOGITEM   AnalogTable[MAX_ANALOG_SUM];
	DIGITALITEM  DigitalTable[MAX_DIGITAL_SUM];
	RELAYITEM    RelayTable[MAX_RELAY_SUM];
	PULSEITEM    PulseTable[MAX_PULSE_SUM];
}RTDBASE;

/*主要参数*/
typedef struct
{
	WORD   wState;		/*数据库标志  */
	WORD   wStnSum;     /*站或装置数量*/
	int32  nAnalogSum;  /*模拟量总数  */
	int32  nDigitalSum; /*数字量总数  */
	int32  nRelaySum;   /*开出量总数  */
	int32  nPulseSum;   /*电能量总数  */
	int32  nAdjustSum;  /*设点量总数  */
	int32  nSpare1;		/*备用*/
	int32  nSpare2;		/*备用*/
	int32  nSpare3;		/*备用*/
	int32  nSOEWritePos; /*SOE队列写位置*/
	int32  nAIEWritePos; /*AIE队列写位置*/
	int32  nSpare4;		/*备用*/
	int32  nSpare5;		/*备用*/
	int32  nSpare6;		/*备用*/
	int32  nRunMode;	/*运行方式(0:并列 1:热备)*/
	int32  nIsDuty;		/*运行状态(0:备用 1:值班)*/
}SYSINFO;

/*共享内存空间结构*/
typedef struct
{
	DWORD    dwAllSize;	/*全部空间长度*/
	DWORD    dwExtSize;	/*附加空间长度*/
	DWORD    dwQuality;	/*品质标志*/
	DWORD    dwEdition;	/*结构版本*/
	SYSINFO  sysInfo;   /*主要信息*/
	RTDBASE  RTDBase;	/*数据库空间*/
	SOEITEM  soeArray[SOE_QUEUE_SUM]; /*SOE空间*/
	AIEITEM  aieArray[AIE_QUEUE_SUM]; /*AIE空间*/
	MSGSTORE msgStore;  /*消息队列*/
    /*其他结构*/
	void    *pExtAddr;  /*附加空间位置*/
}SHM_SPACE;

#pragma pack()

/*******************************************************************/
#endif   /*_RTDB_H*/

/*
 * =====================================================================================
 *
 *       Filename:  gDataType.h
 *
 *    Description:  ����Э���������� 
 *
 *        Version:  1.0
 *        Created:  2014��07��17�� 15ʱ08��42��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp (), 
 *        Company:  esdtek
 *
 * =====================================================================================
 */

#ifndef  _GDATATYPE_H__
#define  _GDATATYPE_H__

#include "./typedef.h"
#include <string.h>

#define	MAX_DATA_LEN		512			/*  */

#define	MAX_UNPROCESSBUF_LEN		1024		/*  */
#define YC_TYPE		1
#define YX_TYPE		2
#define YM_TYPE		3

//typedef struct _tagRTDBDATA
//{
//    DWORD dwAddrID;         /*��ַ��ʶ(4�ֽ�)*/
//    WORD  wDataLen;         /*���ݳ���(2�ֽ�)*/
//    BYTE  byTypeID;         /*��������(1�ֽ�)*/
//    BYTE  byDevNum;         /*�豸���(1�ֽ�)*/
//    BYTE  byDataBuf[MAX_DATA_LEN]; /*��������*/
//} RTDBDATA;


/*  typedef struct _tagTIMEDATA
{
	unsigned int MiSec:10;
	unsigned int Second:6;
	unsigned int Minute:6;
	unsigned int Hour:5;
	unsigned int Day:5;
	unsigned int Month:4;
	unsigned int Year:12;
} TIMEDATA;

typedef struct _tagREALTIME {
	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDayOfWeek;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;
	unsigned short wMilliSec;
} REALTIME;
*/

typedef struct _tagUNPROCESSBUF
{
	int len;
	BYTE buf[MAX_UNPROCESSBUF_LEN];
}UNPROCESSBUF;			/* ----------  end of struct YC_DATA  ---------- */

# pragma pack(1)
/*-----------------------------------------------------------------------------
 *  �������Ͷ��� ���޸�
 *----------------------------------------------------------------------------*/
typedef struct tagYcData 
{
	WORD wSerialNo;
	WORD wPnt;
	float fYcValue;
	int32 YcValue;
	BYTE byYcType;	//0����Ȼ�ֵ 1����һ��ֵ2����ʱ��ı�Ȼ�ֵ 3����ʱ��Ĺ�һ��ֵ
	WORD MilSecond;
	BYTE Second;
	BYTE Minute;
	BYTE Hour;
	BYTE Day;
	BYTE Month;
	WORD Year;
	
	tagYcData( )
	{
		wSerialNo = 0xFFFF ;
		wPnt = 0xFFFF;
		fYcValue = 0 ;
		YcValue = 0 ;
		byYcType = 0 ;
		MilSecond = 0 ;
		Second = 0 ;
		Minute = 0 ;
		Hour = 0 ;
		Day = 0 ;
		Month = 0 ;
		Year = 0 ;
	}

}YC_DATA , *PYC_DATA;			/* ----------  end of struct YC_DATA  ---------- */


typedef struct tagYxData
{
	WORD wSerialNo;
	WORD wPnt;
	BYTE YxValue;
	BYTE byYxType;	//0������ң��1��˫��ң��2����ʱ��ĵ���ң��3����ʱ���˫��ң��
	WORD MilSecond;
	BYTE Second;
	BYTE Minute;
	BYTE Hour;
	BYTE Day;
	BYTE Month;
	WORD Year;

	tagYxData( )
	{
		wSerialNo = 0xFFFF ;
		wPnt = 0xFFFF;
		YxValue = 0 ;
		byYxType = 0 ;
		MilSecond = 0 ;
		Second = 0 ;
		Minute = 0 ;
		Hour = 0 ;
		Day = 0 ;
		Month = 0 ;
		Year = 0 ;
	}

}YX_DATA, *PYX_DATA;				/* ----------  end of struct YX_DATA  ---------- */


typedef struct tagYmData
{
	WORD wPnt;
	float YmValue;
	WORD wSerialNo;
	
	tagYmData( )
	{
		 wPnt = 0 ;
		 YmValue = 0 ;
		 wSerialNo = 0 ;
	}

}YM_DATA , *PYM_DATA;				/* ----------  end of struct YM_DATA  ---------- */


typedef struct tagYkData
{
	WORD wPnt;
	BYTE byVal;
	BYTE byType;	//0������ң�� 1��˫��ң��

	tagYkData( )
	{
		wPnt = 0 ;
		byVal = 0 ;
		byType = 0 ;
	}
}YK_DATA , *PYK_DATA;

typedef struct _DDBYK_DATA
{
	BYTE byDestBusNo ;
	WORD wDestAddr ;
	WORD wPnt ;
	BYTE byVal ;
	BYTE byType ;
	_DDBYK_DATA( )
	{
		byDestBusNo = 0 ;
		wDestAddr = 0 ;
		wPnt = 0 ;
		byVal = 0 ;
		byType = 0 ;
	}
}DDBYK_DATA , *PDDBYK_DATA;

typedef struct tagDzData
{
	WORD wPnt;
	BYTE byType; //0:byte 1:word 2:dword 3:float
	BYTE byVal[4];

	tagDzData()
	{
		wPnt = 0;
		byType = 0;
		memset( byVal, 0, 4 );
	}

}DZ_DATA, *PDZ_DATA;				/* ----------  end of struct tagDzData  ---------- */

# pragma pack()

#endif   /* ----- #ifndef GDATATYPE_H__  ----- */

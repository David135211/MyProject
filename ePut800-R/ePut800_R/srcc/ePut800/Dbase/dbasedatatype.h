/*
 * =====================================================================================
 *
 *       Filename:  dbasedatatype.h
 *
 *    Description:  存储数据类型 
 *
 *        Version:  1.0
 *        Created:  2015年03月19日 11时56分31秒
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

#ifndef  DBASEDATATYPE_INC
#define  DBASEDATATYPE_INC


/* #####   HEADER FILE INCLUDES   ################################################### */
#include "Typedef.h"




/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ################################### */

// #define		TABLE_TYPE_HISTORY		01			/* 历史表类型 */
// #define		TABLE_TYPE_INFORECORD	02			/* 信息记录表类型 */
#define		MAX_DBASE_NAME_LEN						128					/* 最长的数据库名字长度 */
#define		MAX_DBASE_HISTORY_TABLE_NAME_LEN		64					/* 最长的数据库历史表名字长度 */


//历史数据表结构( 无名字 )
#define	SOESTRUCT	"(ID INTEGER PRIMARY KEY AUTOINCREMENT,\
                          BusNo int16_t	,	     	  \
						  DevNo int16_t	,			\
						  DataType int8_t,			\
						  Location char,			\
						  Content char,				\
						  DateTime char)"			



/* #####   TYPE DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ######################### */



/* #####   DATA TYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */
#pragma pack( 1 )

typedef struct _SoeData
{
	WORD BusNo;
		WORD DevNo;
        BYTE DataType;                           /* 数据类型 */
        char Location[32];                       /* 发生位置 */
        char Content[64];                        /* 内容 */
        char DateTime[32];                     /* 时间 */
	_SoeData()
    {
        BusNo = 0;
        DevNo = 0;
        DataType = 0xFF;
        memset( Location , 0 , sizeof(Location) );
        memset( Content , 0 , sizeof(Content) );
        memset( DateTime , 0 , sizeof(DateTime) );
    }
}SOEDATA;				/* ----------  end of struct _HistoryData  ---------- */
#pragma pack(  )


#endif   /* ----- #ifndef DBASEDATATYPE_INC  ----- */


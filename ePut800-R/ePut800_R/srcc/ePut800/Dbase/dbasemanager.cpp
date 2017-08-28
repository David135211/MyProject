/*
 * =====================================================================================
 *
 *       Filename:  dbasemanager.cpp
 *
 *    Description:  数据据的管理方法类
 *
 *        Version:  1.0
 *        Created:  2015年03月19日 09时11分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp
 *   Organization:  esdtek
 *
 *		  history:  Time							Author	 Version	Description
 *					2015年03月30日 09时33分06秒     mengqp	 1.0		created
 * =====================================================================================
 */

/* #####   HEADER FILE INCLUDES   ################################################### */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dbasemanager.h"
#include "dbasemanager_soe.h"


/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ################################### */

/* 此处增加管理方法模块宏 */
#define	DBMANAGER_MOUDLE_SOE			1			/* 关于SOE数据库管理模块 */

/* #####   TYPE DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ######################### */

/* #####   DATA TYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */

/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ################################ */

/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ############################ */

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ##################### */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager
 *      Method:  CDBaseManager
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
CDBaseManager::CDBaseManager ()
{
	//初始化本地变量
	memset( m_szDBPathName, 0, MAX_DBASE_NAME_LEN );
	memset( m_szDBTableName, 0,MAX_DBASE_HISTORY_TABLE_NAME_LEN );
	m_byMoudle = 0;
	strcpy( m_szDBPathName, "/media/mmcblk0p1/ePut800Soe.db");
	strcpy( m_szDBTableName, "Soe");
	m_byMoudle = DBMANAGER_MOUDLE_SOE;

	printf ( "CreateDBMoudle constructor\n" );
}  /* -----  end of method CDBaseManager::CDBaseManager  (constructor)  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager
 *      Method:  ~CDBaseManager
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
CDBaseManager::~CDBaseManager ()
{
	printf ( "CDBaseManager desstructor\n" );
	//析构内存
	if( NULL != m_pMoudle )
	{
		delete m_pMoudle;
		m_pMoudle = NULL;
	}
}  /* -----  end of method CDBaseManager::~CDBaseManager  (destructor)  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager
 *      Method:  Init
 * Description:  初始化协议，在创建该类后第一次使用时使用该函数
 *       Input:	 pRdbObj:共享内存管理指针
 *				 pBusManager:总线管理指针
 *		Return:  正确初始化类：库模块指针
 *				 错误初始化类：NULL
 *--------------------------------------------------------------------------------------
 */
CDBaseManager * CDBaseManager::Init ()
{
	// //输入参数判断
	// if( NULL == pRdbObj || NULL == pBusManager )
	// {
	// 	printf ( "CDBaseManager Init NULL=Input\n" );
	// 	return NULL;
	// }

	// //指针传递
	// m_pRdbObj = pRdbObj;
	// m_pBusManager = pBusManager;

	//配置文件读取
	if( !ReadCfgInfo(  ) )
	{
		return NULL;
	}

	//创建模块
	return CreateDBMoudle(  );
}		/* -----  end of method CDBaseManager::Init  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager
 *      Method:  ReadCfgInfo
 * Description:  读配置信息 在初始化数据管理模块模块时使用
 *       Input:  void
 *		Return:  正确读到配置信息：TRUE
 *				 读配置信息发生错误：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL CDBaseManager::ReadCfgInfo ( void )
{
	// //初始模块设置
	// strcpy( m_szDBPathName, "testdb.db" );
	// strcpy( m_szDBTableName, "historytable" );
	// m_bySaveDateLen = 14;
	// m_dwSaveInterval = 60;
	// m_byMoudle=0;

	// FILE *fp = NULL;
	// char szLineBuf[1024];
	// char szKey[5][64] = {"saveinterval=",
	// 	"savedatelen=",
	// 	"dbtablename=",
	// 	"dbasename=",
	// 	"moudle="};
	// int iKeyLen[5]={13,12,12,10,7};

	// /* 打开配置文件 */
	// fp = fopen( DBASECFGPATHNAME, "r" );
	// if( NULL == fp )
	// {
	// 	printf ( "CDBaseManager ReadConfig Open file %s err !!! \n",
	// 			DBASECFGPATHNAME );
	// 	return FALSE;
	// }
	// else
	// {
	// 	printf ( "CDBaseManager ReadConfig Open file %s OK !!! \n",
	// 			DBASECFGPATHNAME );
	// }

	// /* 逐行分析每行数据 */
	// while( NULL != fgets( szLineBuf, sizeof( szLineBuf ), fp ) )
	// {
	// 	// rtrim( szLineBuf );
	// 	if ( '#' == szLineBuf[0] || ';' == szLineBuf[0] )
	// 	{
	// 		continue;
	// 	}

	// 	if( 0 == strncmp( szLineBuf, szKey[0], iKeyLen[0]) )
	// 	{
	// 		m_dwSaveInterval = atoi( szLineBuf + iKeyLen[0] );
	// 		if( 60 > m_dwSaveInterval )
	// 		{
	// 			m_dwSaveInterval = 60;
	// 		}
	// 	}
	// 	else if( 0 == strncmp( szLineBuf, szKey[1], iKeyLen[1] ) )
	// 	{
	// 		m_bySaveDateLen = atoi( szLineBuf + iKeyLen[1] );
	// 		if( m_bySaveDateLen > MAX_SAVE_DATE_LEN )
	// 		{
	// 			m_bySaveDateLen = MAX_SAVE_DATE_LEN;
	// 		}
	// 	}
	// 	else if( 0 == strncmp( szLineBuf, szKey[2], iKeyLen[2] ) )
	// 	{
	// 		strcpy( m_szDBTableName, ( szLineBuf + iKeyLen[2] ) );
	// 	}
	// 	else if( 0 == strncmp( szLineBuf, szKey[3] , iKeyLen[3] ))
	// 	{
	// 		strcpy( m_szDBPathName, (  szLineBuf + iKeyLen[3]) );
	// 	}
	// 	else if( 0 == strncmp( szLineBuf, szKey[4] , iKeyLen[4] ))
	// 	{
	// 		m_byMoudle = atoi( szLineBuf + iKeyLen[4] );
	// 	}

	// }

	// //打印数据 测试读的正确性
	// printf ( "CDBaseManager ReadConfig dbase:%s table:%s day:%d interval:%lu moudle=%d\n",
	// 		m_szDBPathName, m_szDBTableName, m_bySaveDateLen, m_dwSaveInterval, m_byMoudle);

	// fclose( fp );		//关闭文件

	return TRUE;
}		/* -----  end of method CDBaseManager::ReadCfgInfo  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager
 *      Method:  CreateDBMoudle
 * Description:  创建数据库管理模块 读配置正确后使用
 *       Input:  void
 *		Return:  正确创建模块：库模块指针
 *				 创建模块错误：NULL
 *--------------------------------------------------------------------------------------
 */
CDBaseManager * CDBaseManager::CreateDBMoudle ( void )
{
	switch ( m_byMoudle )
	{
		case DBMANAGER_MOUDLE_SOE:
			{
				//创建模块
				CDBaseManager_SOE *pMoudle = new CDBaseManager_SOE;
				if( NULL == pMoudle )
				{
					printf ( "CDBaseManager CreateDBMoudle new err!!!\n" );
					return NULL;
				}

				//赋初值
				strcpy( pMoudle->m_szDBPathName, m_szDBPathName );
				strcpy( pMoudle->m_szDBTableName, m_szDBTableName );
				// pMoudle->m_bySaveDateLen = m_bySaveDateLen;
				// pMoudle->m_dwSaveInterval = m_dwSaveInterval;
				// pMoudle->m_pRdbObj = m_pRdbObj;
				// pMoudle->m_pBusManager = m_pBusManager;
				pMoudle->m_byMoudle = m_byMoudle;

				//初始化
				if( pMoudle->Init(  ) )
				{
					printf ( "create eem moudle OK\n" );
					m_pMoudle = ( CDBaseManager_SOE * )pMoudle;
					return m_pMoudle;
				}

				delete pMoudle;
				printf ( "create eem moudle Error\n" );
				return NULL;
			}
			break;


		default:
			printf ( "not find dbase moudle\n" );
			break;
	}				/* -----  end switch  ----- */
	return NULL;
}		/* -----  end of method CDBaseManager::CreateDBMoudle  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager
 *      Method:  SelectHistoryDataByTime
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
int CDBaseManager::SelectDataByTime ( char *pszBeginTime, char *pszEndTime, BYTE DataType )
{
	printf ( "CDBaseManager SelectHistoryDataBytime pszBeginTime=%s pszEndTime=%s\n" ,
			pszBeginTime, pszEndTime);
	return -1;
}		/* -----  end of method CDBaseManager::SelectHistoryDataByTime  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager
 *      Method:  UpdateHistoryData
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDBaseManager::UpdateData ( void* data )
{
	printf ( "CDBaseManager UpdateHistoryData\n" );
	return FALSE;
}		/* -----  end of method CDBaseManager::UpdateHistoryData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager
 *      Method:  InsertHistoryData
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDBaseManager::InsertData ( void* data )
{
	printf ( "CDBaseManager InsertHistoryData\n" );
	return FALSE;
}		/* -----  end of method CDBaseManager::InsertHistoryData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager
 *      Method:  DeleteDB
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDBaseManager::DeleteData ( void )
{
	return FALSE;
}		/* -----  end of method CDBaseManager::DeleteDB  ----- */

/*******************************************************************************
 * 类:CDBaseManager
 * 函数名:ClearData
 * 功能描述:销毁该类
 * 参数:void
 * 返回值:BOOL
 ******************************************************************************/
void CDBaseManager::ClearData(void)
{
	// return FALSE;
}   /*-------- end class CDBaseManager method ClearData -------- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager
 *      Method:  GetData
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
void * CDBaseManager::GetData( DWORD pos )
{
	return NULL;
}		/* -----  end of method CDBaseManager::GetHistoryData  ----- */


/*******************************************************************************
 * 类:CDBaseManager
 * 函数名:SelectDataByBusLineAndAddr( WORD wBusLine, WORD wAddr )
 * 功能描述:总线，地址查询数据
 * 参数:总线地址
 * 返回值:int
 ******************************************************************************/
int  CDBaseManager::SelectDataByBusLineAndAddr( WORD wBusLine, WORD wAddr, BYTE DataType )
{
	return 0;
}   /*-------- end class CDBaseManager method SelectDataByBusLineAndAddr( WORD wBusLine, WORD wAddr ) -------- */


/*******************************************************************************
 * 类:CDBaseManager
 * 函数名:SelectAllData
 * 功能描述:查询所有数据
 * 参数:void
 * 返回值:int 返回所有的数据数量
 ******************************************************************************/
int CDBaseManager::SelectAllData(BYTE DataType)
{
	return 0;
}   /*-------- end class CDBaseManager method SelectAllData -------- */

/*******************************************************************************
 * 类:CDBaseManager
 * 函数名:SelectFirstPageOfAllData
 * 功能描述:查找所有数据的第一页
 * 参数: int datanum 第一页的数量数
 * 返回值:int 返回的数据数量
 ******************************************************************************/
int CDBaseManager::SelectFirstPageOfAllData( DWORD datanum, BYTE DataType)
{
	return 0;
}   /*-------- end class CDBaseManager method SelectFirstPageOfAllData -------- */

/*******************************************************************************
 * 类:CDBaseManager
 * 函数名:SelectFirstPageOfDataByTime
 * 功能描述:按时间查询第一页的数据
 * 参数: int datanum, char *pszBeginTime,pszEndTime 数据数量 起始时间 结束时间
 * 返回值:int  返回数据数量
 ******************************************************************************/
int  CDBaseManager::SelectFirstPageOfDataByTime( DWORD datanum, char *pszBeginTime,char *pszEndTime, BYTE DataType)
{
	return 0;
}   /*-------- end class CDBaseManager method SelectFirstPageOfDataByTime -------- */


/*******************************************************************************
 * 类:CDBaseManager
 * 函数名:SelectFirstPageOfDataByBusLineAndAddr( int pagenum, WORD wBusLine, WORD wAddr )
 * 功能描述:按总线地址查询第一页数据
 * 参数:datanum 第一页的数量数量
 * 参数:wBusLine 总线号
 * 参数:wAddr 地址号
 * 返回值:int 返回数据数量
 ******************************************************************************/
int  CDBaseManager::SelectFirstPageOfDataByBusLineAndAddr( DWORD datanum, WORD wBusLine, WORD wAddr, BYTE DataType )
{
	return 0;

}   /*-------- end class CDBaseManager method SelectFirstPageOfDataByBusLineAndAddr( int pagenum, WORD wBusLine, WORD wAddr ) -------- */

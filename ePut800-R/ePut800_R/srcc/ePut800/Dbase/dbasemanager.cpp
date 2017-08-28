/*
 * =====================================================================================
 *
 *       Filename:  dbasemanager.cpp
 *
 *    Description:  ���ݾݵĹ�������
 *
 *        Version:  1.0
 *        Created:  2015��03��19�� 09ʱ11��52��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp
 *   Organization:  esdtek
 *
 *		  history:  Time							Author	 Version	Description
 *					2015��03��30�� 09ʱ33��06��     mengqp	 1.0		created
 * =====================================================================================
 */

/* #####   HEADER FILE INCLUDES   ################################################### */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dbasemanager.h"
#include "dbasemanager_soe.h"


/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ################################### */

/* �˴����ӹ�����ģ��� */
#define	DBMANAGER_MOUDLE_SOE			1			/* ����SOE���ݿ����ģ�� */

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
	//��ʼ�����ر���
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
	//�����ڴ�
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
 * Description:  ��ʼ��Э�飬�ڴ���������һ��ʹ��ʱʹ�øú���
 *       Input:	 pRdbObj:�����ڴ����ָ��
 *				 pBusManager:���߹���ָ��
 *		Return:  ��ȷ��ʼ���ࣺ��ģ��ָ��
 *				 �����ʼ���ࣺNULL
 *--------------------------------------------------------------------------------------
 */
CDBaseManager * CDBaseManager::Init ()
{
	// //��������ж�
	// if( NULL == pRdbObj || NULL == pBusManager )
	// {
	// 	printf ( "CDBaseManager Init NULL=Input\n" );
	// 	return NULL;
	// }

	// //ָ�봫��
	// m_pRdbObj = pRdbObj;
	// m_pBusManager = pBusManager;

	//�����ļ���ȡ
	if( !ReadCfgInfo(  ) )
	{
		return NULL;
	}

	//����ģ��
	return CreateDBMoudle(  );
}		/* -----  end of method CDBaseManager::Init  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager
 *      Method:  ReadCfgInfo
 * Description:  ��������Ϣ �ڳ�ʼ�����ݹ���ģ��ģ��ʱʹ��
 *       Input:  void
 *		Return:  ��ȷ����������Ϣ��TRUE
 *				 ��������Ϣ��������FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL CDBaseManager::ReadCfgInfo ( void )
{
	// //��ʼģ������
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

	// /* �������ļ� */
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

	// /* ���з���ÿ������ */
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

	// //��ӡ���� ���Զ�����ȷ��
	// printf ( "CDBaseManager ReadConfig dbase:%s table:%s day:%d interval:%lu moudle=%d\n",
	// 		m_szDBPathName, m_szDBTableName, m_bySaveDateLen, m_dwSaveInterval, m_byMoudle);

	// fclose( fp );		//�ر��ļ�

	return TRUE;
}		/* -----  end of method CDBaseManager::ReadCfgInfo  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager
 *      Method:  CreateDBMoudle
 * Description:  �������ݿ����ģ�� ��������ȷ��ʹ��
 *       Input:  void
 *		Return:  ��ȷ����ģ�飺��ģ��ָ��
 *				 ����ģ�����NULL
 *--------------------------------------------------------------------------------------
 */
CDBaseManager * CDBaseManager::CreateDBMoudle ( void )
{
	switch ( m_byMoudle )
	{
		case DBMANAGER_MOUDLE_SOE:
			{
				//����ģ��
				CDBaseManager_SOE *pMoudle = new CDBaseManager_SOE;
				if( NULL == pMoudle )
				{
					printf ( "CDBaseManager CreateDBMoudle new err!!!\n" );
					return NULL;
				}

				//����ֵ
				strcpy( pMoudle->m_szDBPathName, m_szDBPathName );
				strcpy( pMoudle->m_szDBTableName, m_szDBTableName );
				// pMoudle->m_bySaveDateLen = m_bySaveDateLen;
				// pMoudle->m_dwSaveInterval = m_dwSaveInterval;
				// pMoudle->m_pRdbObj = m_pRdbObj;
				// pMoudle->m_pBusManager = m_pBusManager;
				pMoudle->m_byMoudle = m_byMoudle;

				//��ʼ��
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
 * ��:CDBaseManager
 * ������:ClearData
 * ��������:���ٸ���
 * ����:void
 * ����ֵ:BOOL
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
 * ��:CDBaseManager
 * ������:SelectDataByBusLineAndAddr( WORD wBusLine, WORD wAddr )
 * ��������:���ߣ���ַ��ѯ����
 * ����:���ߵ�ַ
 * ����ֵ:int
 ******************************************************************************/
int  CDBaseManager::SelectDataByBusLineAndAddr( WORD wBusLine, WORD wAddr, BYTE DataType )
{
	return 0;
}   /*-------- end class CDBaseManager method SelectDataByBusLineAndAddr( WORD wBusLine, WORD wAddr ) -------- */


/*******************************************************************************
 * ��:CDBaseManager
 * ������:SelectAllData
 * ��������:��ѯ��������
 * ����:void
 * ����ֵ:int �������е���������
 ******************************************************************************/
int CDBaseManager::SelectAllData(BYTE DataType)
{
	return 0;
}   /*-------- end class CDBaseManager method SelectAllData -------- */

/*******************************************************************************
 * ��:CDBaseManager
 * ������:SelectFirstPageOfAllData
 * ��������:�����������ݵĵ�һҳ
 * ����: int datanum ��һҳ��������
 * ����ֵ:int ���ص���������
 ******************************************************************************/
int CDBaseManager::SelectFirstPageOfAllData( DWORD datanum, BYTE DataType)
{
	return 0;
}   /*-------- end class CDBaseManager method SelectFirstPageOfAllData -------- */

/*******************************************************************************
 * ��:CDBaseManager
 * ������:SelectFirstPageOfDataByTime
 * ��������:��ʱ���ѯ��һҳ������
 * ����: int datanum, char *pszBeginTime,pszEndTime �������� ��ʼʱ�� ����ʱ��
 * ����ֵ:int  ������������
 ******************************************************************************/
int  CDBaseManager::SelectFirstPageOfDataByTime( DWORD datanum, char *pszBeginTime,char *pszEndTime, BYTE DataType)
{
	return 0;
}   /*-------- end class CDBaseManager method SelectFirstPageOfDataByTime -------- */


/*******************************************************************************
 * ��:CDBaseManager
 * ������:SelectFirstPageOfDataByBusLineAndAddr( int pagenum, WORD wBusLine, WORD wAddr )
 * ��������:�����ߵ�ַ��ѯ��һҳ����
 * ����:datanum ��һҳ����������
 * ����:wBusLine ���ߺ�
 * ����:wAddr ��ַ��
 * ����ֵ:int ������������
 ******************************************************************************/
int  CDBaseManager::SelectFirstPageOfDataByBusLineAndAddr( DWORD datanum, WORD wBusLine, WORD wAddr, BYTE DataType )
{
	return 0;

}   /*-------- end class CDBaseManager method SelectFirstPageOfDataByBusLineAndAddr( int pagenum, WORD wBusLine, WORD wAddr ) -------- */

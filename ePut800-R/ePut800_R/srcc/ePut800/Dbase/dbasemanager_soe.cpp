#include "dbasemanager_soe.h"
#include <sys/time.h>

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  dbase_history_data_process
 *  Description:  选择历史数据处理
 *		  Param:  data:自定义参数 此处指传过来的dbasemanager指针
 *				  n_column:信息段
 *				  column_name:信息名称
 *				  column_value:信息值
 *		 Return:
 * =====================================================================================
 */
static int dbase_select_history_data_process ( void * data, int n_column, char ** column_value, char ** column_name )
{
	SOEDATA TempData;
	CDBaseManager_SOE * m_pCDBase = (CDBaseManager_SOE *)data;
	if( NULL == m_pCDBase )
	{
		return -1;
	}

	if( NULL== &m_pCDBase->InterruptSelect )
	{
		return -1;
	}

	if( m_pCDBase->InterruptSelect == TRUE )
	{
		printf("Callback routine requested an abort \n");
		return SQLITE_ABORT;
	}
	if( m_pCDBase == NULL )
	{
		printf("dbase_select_history_data_callback parameter data = NULL\n");
		return -1;
	}

	if( n_column > 7 )
	{
		printf ( "dbase_select_history_data_callback n_column > 6\n" );
		return -1;
	}

	TempData.BusNo = atoi( column_value[1] );
	TempData.DevNo = atoi( column_value[2] );
	TempData.DataType = atoi( column_value[3] );
	strcpy( TempData.Location , column_value[4] );
	strcpy( TempData.Content , column_value[5] );
	strcpy( TempData.DateTime , column_value[6] );

	m_pCDBase->m_SoeData->push_back( TempData );

	m_pCDBase->CurrentAllPage = (( m_pCDBase->m_SoeData->size() - 1 ) / 8) +1;

	// if( m_pCDBase->FirstSelectState == TRUE )
	// {
		// if( m_pCDBase->m_SoeData->size() == 8 )
		// {
			// return SQLITE_ABORT;
		// }
	// }

	return 0;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  dbase_select_history_data_by_busno_devno_process
 *  Description:  选择历史数据处理
 *		  Param:  data:自定义参数 此处指传过来的dbasemanager指针
 *				  n_column:信息段
 *				  column_name:信息名称
 *				  column_value:信息值
 *		 Return:
 * =====================================================================================
 */
static int dbase_select_history_data_by_busno_devno_process ( void * data, int n_column, char ** column_value, char ** column_name )
{
	SOEDATA TempData;
	CDBaseManager_SOE * m_pCDBase = (CDBaseManager_SOE *)data;

	/*if( m_pCDBase->InterruptSelect == TRUE )
	{
		printf("Callback routine requested an abort \n");
		return SQLITE_ABORT;
	}*/
	if( m_pCDBase == NULL )
	{
		printf("dbase_select_history_data_callback parameter data = NULL\n");
		return -1;
	}

	if( n_column > 6 )
	{
		printf ( "dbase_select_history_data_callback n_column > 6\n" );
		return -1;
	}

	TempData.BusNo = atoi( column_value[1] );
	TempData.DevNo = atoi( column_value[2] );
	TempData.DataType = atoi( column_value[3] );
	strcpy( TempData.Location , column_value[4] );
	strcpy( TempData.Content , column_value[5] );
	strcpy( TempData.DateTime , column_value[6] );

	m_pCDBase->m_SoeData->push_back( TempData );

	return 0;
}


CDBaseManager_SOE::CDBaseManager_SOE()
{
	m_pCDBase = NULL;
	m_SoeData = NULL;
	InterruptSelect = FALSE;
	FirstSelectState = TRUE;
	memset(m_szDBPathName,0,sizeof(m_szDBPathName));
	strcpy(m_szDBPathName,SOEDBASEPATH);
	memset(m_szDBTableName,0,sizeof(m_szDBTableName));
	strcpy(m_szDBTableName,SOEDBTABLENAME);
	CurrentAllPage = 1;

	//RealTimeData.InitVector( 20151026 );
}

CDBaseManager_SOE::~CDBaseManager_SOE()
{
	//关闭数据库
	if( m_pCDBase->IsDBExist(  ) )
	{
		m_pCDBase->CloseDB(  );
	}

	//删除数据库类
	if( NULL != m_pCDBase)
	{
		delete m_pCDBase;
		m_pCDBase = NULL;
	}

	if( NULL != m_SoeData )
	{
		m_SoeData->clear(  );
		delete m_SoeData;
		m_SoeData = NULL;
	}
}

BOOL CDBaseManager_SOE::Init()
{
	if( m_pCDBase == NULL )
	{
		m_pCDBase = new sqlite3data();
		if( m_pCDBase == NULL )
		{
			printf("CDBaseManager_SOE new error!\n");
			return FALSE;
		}
	}

	if( !CreateDB() || !CreateDBTableAndIndex() )
	{
		printf("CDBaseManager_SOE open fail!\n");
	}
	if( NULL == m_SoeData )
	{
		m_SoeData = new vector<SOEDATA>;
		if( NULL == m_SoeData )
		{
			printf ( "CDBaseManager_SOE InitVector m_SoeData new err!!!\n" );
		}
	}


	return TRUE;
}

BOOL CDBaseManager_SOE::CreateDB()
{
	m_pCDBase->CreateDB( m_szDBPathName );
	if( !m_pCDBase->IsDBExist() )
	{
		printf("dbase_soe.db create fail!\n");
		return FALSE;
	}
	return TRUE;
}

BOOL CDBaseManager_SOE::CloseDB()
{
	if( !m_pCDBase->CloseDB() )
	{
		printf("dbase_soe.db close fail!\n");
		return FALSE;
	}
	return TRUE;
}

BOOL CDBaseManager_SOE::DeleteDB()
{
	m_pCDBase->DeleteDB( m_szDBPathName );
	return TRUE;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager_SOE
 *      Method:  CreateDBTableAndIndex
 * Description:  创建表及其索引
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
BOOL CDBaseManager_SOE::CreateDBTableAndIndex()
{
	char TempBuf[256] = "";
	BOOL bRtn = FALSE;

	//创建历史数据表
	//组表结构
	sprintf( TempBuf, "%s%s", m_szDBTableName , SOESTRUCT );
	m_pCDBase->CreateDBTable( m_szDBTableName, TempBuf );
	bRtn = m_pCDBase->IsDBTableExist( m_szDBTableName );
	if ( !bRtn )
	{
		printf ( "CDBaseManager_SOE Init CreateDBTable %s err!!!\n",
				m_szDBTableName);
		return FALSE;
	}

	//创建历史表索引 根据DataType,DateTime
	//组索引名
	sprintf( TempBuf, "%s.index", m_szDBTableName );
	bRtn = m_pCDBase->CreateDBTableIndex( m_szDBTableName ,
			TempBuf ,
			SOEDBTABLEINDEXDATA	);
	if( !bRtn )
	{
		printf ( "CDBaseManager_SOE Init CreateDBTable DataType_DataTime Index %s err!!!\n",
				m_szDBTableName);
		return FALSE;
	}

	sprintf( TempBuf, "%s1.index", m_szDBTableName );
	bRtn = m_pCDBase->CreateDBTableIndex( m_szDBTableName ,
			TempBuf ,
			SOEDBTABLEINDEXNO);
	if( !bRtn )
	{
		printf ( "CDBaseManager_SOE Init CreateDBTable BusNo_DevNo Index %s err!!!\n",
				m_szDBTableName);
		return FALSE;
	}

	sprintf( TempBuf, "%s2.index", m_szDBTableName );
	bRtn = m_pCDBase->CreateDBTableIndex( m_szDBTableName ,
			TempBuf ,
			SOEDBTABLEDATATIME);
	if( !bRtn )
	{
		printf ( "CDBaseManager_SOE Init CreateDBTable BusNo_DevNo Index %s err!!!\n",
				m_szDBTableName);
		return FALSE;
	}

	sprintf( TempBuf, "%s3.index", m_szDBTableName );
	bRtn = m_pCDBase->CreateDBTableIndex( m_szDBTableName ,
										  TempBuf ,
										  "ID");
	if( !bRtn )
	{
		printf ( "CDBaseManager_SOE Init CreateDBTable BusNo_DevNo Index %s err!!!\n",
				 m_szDBTableName);
		return FALSE;
	}

	sprintf( TempBuf, "%s4.index", m_szDBTableName );
	bRtn = m_pCDBase->CreateDBTableIndex( m_szDBTableName ,
                                          TempBuf ,
                                          SOEDBTABLEDATATYPE);
	if( !bRtn )
	{
		printf ( "CDBaseManager_SOE Init CreateDBTable datatype Index %s err!!!\n",
                 m_szDBTableName);
		return FALSE;
	}
	return TRUE;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager_SOE
 *      Method:  InsertData
 * Description:  插入历史数据
 *       Input:	 InsertBuf 已组织完的插入的数据
 *		Return:  插入成功：TRUE
 *				 插入失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL CDBaseManager_SOE::InsertData ( void *data )
{
	string InsertBuf;
	char szTmpBuf[1024];
	// BOOL bRtn = FALSE;
	SOEDATA *pdata = (SOEDATA *)data;

	// if( ( DateTime == NULL ) || ( 0 == strlen( DateTime ) ) )
	// 	return FALSE;

	sprintf( szTmpBuf,
		"insert into %s(ID, BusNo , DevNo , DataType , Location , Content , DateTime )\
	values (NULL, %u, %u, %u,'%s','%s','%s' );\n",
			 m_szDBTableName, pdata->BusNo , pdata->DevNo , pdata->DataType , pdata->Location , pdata->Content , pdata->DateTime );

	InsertBuf += szTmpBuf;

	return m_pCDBase->InsertDBData( m_szDBTableName , InsertBuf );
}

/*******************************************************************************
 * 类:CDBaseManager_SOE
 * 函数名:SelectAllData
 * 功能描述:查询所有数据
 * 参数:void
 * 返回值:int 返回所有的数据数量
 ******************************************************************************/
int CDBaseManager_SOE::SelectAllData( BYTE DataType )
{
	char SelectBuf[256] =  "";
    sprintf( SelectBuf, "where DataType=%d order by ID DESC", DataType );
	m_SoeData->clear();

	if( !m_pCDBase->IsDBExist() )
	{
		printf("dbase_soe.db create fail!\n");
		return 0;
	}
	//组织语句和执行
	// sprintf( SelectBuf, "order by ID DESC",  SOEDBTABLEDATATIME );

	if( !m_pCDBase->SelectDBData( m_szDBTableName,
				SelectBuf,
				dbase_select_history_data_process,
				this) )
	{
		printf ( "CDBaseManager_SOE SelectDataByDataType err!!!\n" );
		return -1;
	}

	return m_SoeData->size();
}   /*-------- end class CDBaseManager_SOE method SelectAllData -------- */

/*******************************************************************************
 * 类:CDBaseManager_SOE
 * 函数名:SelectFirstPageOfAllData
 * 功能描述:查找所有数据的第一页
 * 参数: DWORD datanum 第一页的数量数
 * 返回值:int 返回的数据数量
 ******************************************************************************/
int CDBaseManager_SOE::SelectFirstPageOfAllData( DWORD datanum, BYTE DataType)
{
	char SelectBuf[256] = "";
	m_SoeData->clear();

	if( !m_pCDBase->IsDBExist() )
	{
		printf("dbase_soe.db create fail!\n");
		return 0;
	}
	//组织语句和执行
	sprintf( SelectBuf, "where DataType=%d order by ID DESC limit %lu", DataType,  datanum );

	if( !m_pCDBase->SelectDBData( m_szDBTableName,
								  SelectBuf,
								  dbase_select_history_data_process,
								  this) )
	{
		printf ( "CDBaseManager_SOE SelectDataByDataType err!!!\n" );
		return -1;
	}

	return m_SoeData->size();
}   /*-------- end class CDBaseManager_SOE method SelectFirstPageOfAllData -------- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager_SOE
 *      Method:  SelectDataByBusNoDevNo
 * Description:
 *       Input:
 *		Return:  成功：返回查询到数据的数量
 *				 失败：返回-1
 *--------------------------------------------------------------------------------------
 */
int CDBaseManager_SOE::SelectDataByBusLineAndAddr ( WORD wBusLine , WORD wAddr , BYTE DataType)
{
	char SelectBuf[256] = "";
	m_SoeData->clear();

	if( !m_pCDBase->IsDBExist() )
	{
		printf("dbase_soe.db create fail!\n");
		return 0;
	}
	//组织语句和执行
	sprintf( SelectBuf, "where %s=%u and %s=%u and DataType=%d order by ID DESC",DataType , SOEDBTABLEBUSNO ,
			 wBusLine , SOEDBTABLEDEVNO , wAddr  );

	if( !m_pCDBase->SelectDBData( m_szDBTableName,
								  SelectBuf,
								  dbase_select_history_data_by_busno_devno_process,
								  this) )
	{
		printf ( "CDBaseManager_SOE SelectDataByDataType err!!!\n" );
		return -1;
	}
	return m_SoeData->size();
}

/*******************************************************************************
 * 类:CDBaseManager_SOE
 * 函数名:SelectFirstPageOfDataByBusLineAndAddr( int pagenum, WORD wBusLine, WORD wAddr )
 * 功能描述:按总线地址查询第一页数据
 * 参数:datanum 第一页的数量数量
 * 参数:wBusLine 总线号
 * 参数:wAddr 地址号
 * 返回值:int 返回数据数量ome text.

 ******************************************************************************/
int  CDBaseManager_SOE::SelectFirstPageOfDataByBusLineAndAddr( DWORD datanum, WORD wBusLine, WORD wAddr, BYTE DataType )
{
	char SelectBuf[256] = "";
	m_SoeData->clear();

	if( !m_pCDBase->IsDBExist() )
	{
		printf("dbase_soe.db create fail!\n");
		return 0;
	}
	//组织语句和执行
	sprintf( SelectBuf, "where %s=%u and %s=%u and DataType=%d order by ID DESC limit %lu", SOEDBTABLEBUSNO ,
			 wBusLine , SOEDBTABLEDEVNO , wAddr ,  datanum );

	if( !m_pCDBase->SelectDBData( m_szDBTableName,
								  SelectBuf,
								  dbase_select_history_data_by_busno_devno_process,
								  this) )
	{
		printf ( "CDBaseManager_SOE SelectDataByDataType err!!!\n" );
		return -1;
	}
	return m_SoeData->size();

}   /*-------- end class CDBaseManager_SOE method SelectFirstPageOfDataByBusLineAndAddr( int pagenum, WORD wBusLine, WORD wAddr ) -------- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CDBaseManager_SOE
 *      Method:  SelectInfoRecordDataByTime
 * Description:  根据类型选择信息数据
 *       Input:  datatype 数据类型
 *		Return:  成功：返回信息数目
 *				 失败：-1
 *--------------------------------------------------------------------------------------
 */
int CDBaseManager_SOE::SelectDataByTime ( char *pszBeginTime, char *pszEndTime, BYTE DataType )
{
	char SelectBuf[256] = "";
	m_SoeData->clear();
	InterruptSelect = FALSE;

	if( !m_pCDBase->IsDBExist() )
	{
		printf("dbase_soe.db create fail!\n");
		return 0;
	}
	//组织语句和执行
	sprintf( SelectBuf, "where DataType=%d and %s between datetime('%s') and datetime('%s') ", DataType,
			SOEDBTABLEDATATIME , pszBeginTime , pszEndTime );

	// if( datatype != ALLSOE )
	// {
	// 	char TempBuf[256] = "";
	// 	strcpy( TempBuf , SelectBuf );
	// 	memset( SelectBuf , 0 , sizeof( SelectBuf ) );
	// 	sprintf( SelectBuf, "%s and %s=%u", TempBuf , SOEDBTABLEDATATYPE , datatype );
	// }

	char TempOrderBuf[256] = " order by ID DESC";
	// if( FirstSelect )
	// {
	// 	sprintf( TempOrderBuf, " order by %s DESC limit 8", SOEDBTABLEDATATIME );
	// }
	// else
	// {
	// sprintf( TempOrderBuf, " order by %s DESC", SOEDBTABLEDATATIME );
	// }

	strcat( SelectBuf , TempOrderBuf );

	if( !m_pCDBase->SelectDBData( m_szDBTableName,
				SelectBuf,
				dbase_select_history_data_process,
				this) )
	{
		printf ( "CDBaseManager_SOE SelectDataByDataType err!!!\n" );
		return -1;
	}

	return m_SoeData->size();
}

/*******************************************************************************
 * 类:CDBaseManager_SOE
 * 函数名:SelectFirstPageOfDataByTime
 * 功能描述:按时间查询第一页的数据
 * 参数: int datanum, char *pszBeginTime,pszEndTime 数据数量 起始时间 结束时间
 * 返回值:int  返回数据数量
 ******************************************************************************/
int  CDBaseManager_SOE::SelectFirstPageOfDataByTime( DWORD datanum, char *pszBeginTime, char* pszEndTime, BYTE DataType)
{
	char SelectBuf[256] = "";
	m_SoeData->clear();
	InterruptSelect = FALSE;

	if( !m_pCDBase->IsDBExist() )
	{
		printf("dbase_soe.db create fail!\n");
		return 0;
	}
	//组织语句和执行
	sprintf( SelectBuf, "where DataType = %d and %s between datetime('%s') and datetime('%s')",DataType,
			SOEDBTABLEDATATIME , pszBeginTime , pszEndTime );


	char TempOrderBuf[256] = "";
	sprintf( TempOrderBuf, " order by ID DESC limit %lu", datanum );

	strcat( SelectBuf , TempOrderBuf );

	if( !m_pCDBase->SelectDBData( m_szDBTableName,
				SelectBuf,
				dbase_select_history_data_process,
				this) )
	{
		printf ( "CDBaseManager_SOE SelectDataByDataType err!!!\n" );
		return -1;
	}

	return m_SoeData->size();
}   /*-------- end class CDBaseManager_SOE method SelectFirstPageOfDataByTime -------- */

void * CDBaseManager_SOE::GetData ( DWORD pos )
{
	DWORD size = m_SoeData->size();
	if( pos >= size )
		return NULL;

	SOEDATA * TempData = &m_SoeData->at(pos);
	return TempData;
}

void CDBaseManager_SOE::ClearData ( )
{
	m_SoeData->clear();
}

#include "sqlite3dbase.h"
#include <stdlib.h>


sqlite3* sqlite3data::m_pDBase = NULL;				/* 初始化为NULL */
static pthread_mutex_t gDBMutex ;             /* 设置线程索，防止数据库操作并行任务  */
static BOOL gIsDBMutexFlag = FALSE;

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  InitMutex
 *  Description:  在未被初始化的时候进行初始化
 *		  Param:  void
 *		 Return:  初始化失败：FALSE
 *				  初始化成功或已经被初始化：TRUE
 * =====================================================================================
 */
static BOOL InitMutex ( void )
{
        //初始化线程锁
        if( !gIsDBMutexFlag )
        {
                int iRtn;
                iRtn = pthread_mutex_init( &gDBMutex, NULL );
                if ( 0 != iRtn )
                {
                        printf ( "sqlite3data gDBMutex init error\n" );
                        return FALSE;
                }
                printf ( "sqlite3data gDBMutex init Ok\n" );
                gIsDBMutexFlag = TRUE;
        }

        return TRUE;
}		/* -----  end of function InitMutex  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  IsDBMutexExist
 *  Description:  查看数据库线程锁是否初始化，gIsDBMutexFlag 是初始化标志位
 *		  Param:  void
 *		 Return:  已经被初始化：TRUE
 *				  未被初始化：	FALSE
 * =====================================================================================
 */
static BOOL IsDBMutexExist ( void )
{
        if( !gIsDBMutexFlag )
        {
                return InitMutex(  );
        }

        return gIsDBMutexFlag;
}		/* -----  end of function IsDBMutexExist  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  SelectTable
 *  Description:  查看是否已建立某个数据库
 *		  Param:  data:通过函数传入的处定义的参数
 *				  n_columu:搜索到一条信息的字段数
 *				  column：字段值
 *				  column_name:字段名
 *		 Return:
 * =====================================================================================
 */
static int SelectTable ( void * data, int n_column, char ** column_value, char ** column_name )
{
        BOOL* bRtn = (BOOL *)data;
        if( n_column > 0 )
        {
                *bRtn = TRUE;
        }
        else
        {
                *bRtn = FALSE;
        }

        return 0;
}		/* -----  end of function SelectTable  ----- */
/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  sqlite3data
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
sqlite3data::sqlite3data ()
{
	memset( m_szDBName, 0, MAX_DBASE_NAME_LEN );

}  /* -----  end of method sqlite3data::sqlite3data  (constructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  ~sqlite3data
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
sqlite3data::~sqlite3data ()
{

}  /* -----  end of method sqlite3data::~sqlite3data  (destructor)  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  CreateDB
 * Description:  当没有数据库时， 创建数据库
 *       Input:  szDBPathName:数据库的保存路径及名字
 *		Return:  成功创建: 创建数据库指针:
 *				 失败创建：NULL
 *--------------------------------------------------------------------------------------
 */
void *sqlite3data::CreateDB( char *szDBPathName )
{
    if ( NULL == szDBPathName )
    {
            printf ( "sqlite3data CreateDB m_szDBPathName == NULL\n" );
            return NULL;
    }
    // printf ( "szDBPathName=%s\n", szDBPathName );

    if( !IsDBExist(  ) )
    {
            int iRtn;
            iRtn = sqlite3_open( szDBPathName, &m_pDBase );
            if( iRtn )
            {
                    printf ( "sqlite3data can't open db %s\n", szDBPathName );
                    sqlite3_close( m_pDBase );
					m_pDBase = NULL;
                    return NULL;
            }
            iRtn = sqlite3_exec(m_pDBase,"PRAGMA synchronous = FULL; ",0,0,0);
            if( iRtn != SQLITE_OK )
            {
                    printf ( "sqlite3data synchronous err\n" );
            }
            printf ( "sqlite3data open db %s OK\n", szDBPathName );
            strcpy( m_szDBName, szDBPathName );
    }
    else
    {
            printf ( "sqlite3data open db %s OK is already exist\n", m_szDBName );
    }

    printf ( "sqlite3data CreateDB %s\n", szDBPathName );
    return m_pDBase;
}		/* -----  end of method sqlite3data::CreateDB  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  DeleteDB
 * Description:  当有数据库时， 删除数据库
 *       Input:  szDBPathName:数据库的
 *		Return:  TRUE:成功删除
 *			   	 FALSE:失败删除
 *--------------------------------------------------------------------------------------
 */
BOOL sqlite3data::DeleteDB ( char *szDBPathName )
{
    char szTmpBuf[256];

    //组织shell命令，删除相应数据库
    sprintf( szTmpBuf, "rm %s -rf", szDBPathName );
    system( szTmpBuf );

    //重新将数据库指针置为空
    m_pDBase = NULL;
    printf ( "sqlite3data DeleteDB %s\n", szDBPathName );

    return TRUE;
}		/* -----  end of method sqlite3data::DeleteDB  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  OpenDB
 * Description:  当存在数据库时 打开数据库
 *       Input:  void:默认是已经创建的数据库
 *		Return:  打开成功：返回当前数据库指针
 *				 打开失败：返回NULL
 *--------------------------------------------------------------------------------------
 */
void* sqlite3data::OpenDB ( void )
{
        return m_pDBase;
}		/* -----  end of method sqlite3data::OpenDB  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  CloseDB
 * Description:  当打开数据库时 关闭数据库
 *       Input:  void:默认是已经创建的数据库
 *		Return:  关成功：TRUE
 *		         关失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL sqlite3data::CloseDB ( void )
{
    if( IsDBExist(  ) )
    {
            sqlite3_close( m_pDBase );
            m_pDBase = NULL;
            printf ( "sqlite3data CloseDB\n" );
            return TRUE;
    }

    return FALSE;
}		/* -----  end of method sqlite3data::CloseDB  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  CreateDBTable
 * Description:  当存在已打开数据库时 创建表
 *       Input:  szTableName 创建表的名字
 *				 szTableStruct 表结构
 *		Return:  创建表成功：TRUE
 *				 创建表失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL sqlite3data::CreateDBTable ( char *szTableName, char *szTableStruct )
{
	char szTmpBuf[1024];
	char *szErrBuf;
	int iRtn;

	//判断param
	if( NULL == szTableName || NULL == szTableStruct )
	{
		printf ( "CSqliteDBase CreateDBTable err NULL==(Tablename) || NULL=(TableStruct)\n");
		return FALSE;
	}

	//判断数据库和线程锁是否存在
	if( !IsDBExist(  ) || !IsDBMutexExist(  ) )
	{
		return FALSE;
	}
	
	/* 建表 */
	//命令组织
	sprintf( szTmpBuf, "create table %s", szTableStruct );
	//创建
	pthread_mutex_lock( &gDBMutex );
	iRtn = sqlite3_exec( m_pDBase, szTmpBuf, NULL, NULL, &szErrBuf );
	pthread_mutex_unlock( &gDBMutex );
	if(iRtn != SQLITE_OK )
	{
		printf( "CSqliteDBase Create table error code:%d: cause%s\n",
				iRtn, szErrBuf );
		pthread_mutex_lock( &gDBMutex );
		sqlite3_free( szErrBuf );
		pthread_mutex_unlock( &gDBMutex );
		//iRtn=1 时是已经创建该表的情况
		if( 1 != iRtn )
		{
			return FALSE;
		}
	}


	printf ( "CSqliteDBase CReateDBTaBle %s\n", szTableName );
	return TRUE;
}		/* -----  end of method sqlite3data::CreateDBTable  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  CreateDBTableIndex
 * Description:  根据表名称、索引表名字和索引创建索引表
 *       Input:  szTableName:已存在的表名称
 *				 szIndexName:要创建的索引表名称
 *				 szIndex：	 按照此索引创建索引表
 *		Return:  创建表索引成功：TRUE
 *		         创建表索引失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL sqlite3data::CreateDBTableIndex ( char *szTableName, char *szIndexName, const char *szIndex )
{
    char szTmpBuf[1024];
    char *szErrBuf;
    int iRtn;

    //判断param
    if( NULL == szTableName
                    || NULL == szIndexName
                    || NULL == szIndex)
    {
            printf ( "sqlite3data CreateDBTableIndex param error!!!\n" );
            return FALSE;
    }

    //判断数据库,数据表和线程锁是否存在
    if( !IsDBExist(  )
            || !IsDBMutexExist(  )
            || !IsDBTableExist( szTableName ) )
    {
            return FALSE;
    }

    /* 建表 */
    //命令组织
    sprintf( szTmpBuf, "create index '%s' on %s( %s )", szIndexName, szTableName, szIndex );
    //创建
    pthread_mutex_lock( &gDBMutex );
    iRtn = sqlite3_exec( m_pDBase, szTmpBuf, NULL, NULL, &szErrBuf );
    pthread_mutex_unlock( &gDBMutex );
    if(iRtn != SQLITE_OK )
    {
            printf( "sqlite3data create index error code:%d: cause%s\n",
                            iRtn, szErrBuf );
            pthread_mutex_lock( &gDBMutex );
            sqlite3_free( szErrBuf );
            pthread_mutex_unlock( &gDBMutex );
            //iRtn=1 是已有索引的情况
            if( 1 != iRtn )
            {
                    return FALSE;
            }
    }


    printf ( "sqlite3data CreateDBTableIndex %s %s %s\n",
                    szTableName, szIndexName, szIndex);
    return TRUE;
}		/* -----  end of method sqlite3data::CreateDBTableIndex  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  QuickInsertHistoryDBData
 * Description:
 *       Input:
 *		Return:
 *--------------------------------------------------------------------------------------
 */
// BOOL sqlite3data::QuickInsertHistoryDBData ( char *szTableName,
//          vector <HISTORYDATA>*pHistoryData ,
//          unsigned int uiInterval)
// {
//     return TRUE;
// }		/* -----  end of method sqlite3data::QuickInsertHistoryDBData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  InsertDBData
 * Description:  将数据插入数据库
 *       Input:  szTableName:已存在的表名称
 *				 InsertBuf:  要插入的数据 涉及到事物性插入 为string
 *		Return:  插入表数据成功：TRUE
 *				 插入表数据失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL sqlite3data::InsertDBData (char *szTableName,  string &InsertBuf )
{
    char *szErrBuf;
    int iRtn;
    string InsertStr;
    InsertStr.clear(  );

    //判断param
    if( NULL == szTableName )
    {
            printf ( "sqlite3data InsertDBData param error\n" );
            return FALSE;
    }

    //判断数据库和线程锁是否存在
    if( !IsDBTableExist( szTableName ) )
    {
            return FALSE;
    }

    /* 建表 */

//    if( 1 >= InsertBuf.size(  ) )
//    {
            //printf ( "sqlite3data InsertStr %s\n", szTableName );
            InsertStr += InsertBuf;
//   }
/*    else
    {
            //printf ( "sqlite3data InsertStr Translation %s\n", szTableName );
            InsertStr += "begin;\n";
            InsertStr += InsertBuf;
            InsertStr += "commit;";
    }*/
    //printf ( "insertbuf=%s\n", InsertStr.c_str(  ) );
    //创建
    pthread_mutex_lock( &gDBMutex );
    iRtn = sqlite3_exec( m_pDBase, InsertStr.c_str(), 0, 0, &szErrBuf );
    pthread_mutex_unlock( &gDBMutex );
    if(iRtn != SQLITE_OK )
    {
            printf( "sqlite3data table insert data error code:%d: cause%s\n",
                            iRtn, szErrBuf );
            pthread_mutex_lock( &gDBMutex );
            sqlite3_free( szErrBuf );
            pthread_mutex_unlock( &gDBMutex );
            return FALSE;
    }

    InsertStr.clear(  );

    // printf ( "sqlite3data InsertDBData %s\n", szTableName );
    return TRUE;
}		/* -----  end of method sqlite3data::InsertDBData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  DeleteDBData
 * Description:  将数据从数据库中删除
 *       Input:  szTableName:已存在的表名称
 *               DeleteBuf:  要删除的数据
 *		Return:  删除表数据成功：TRUE
 *		         删除表数据失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL sqlite3data::DeleteDBData ( char *szTableName, void *DeleteBuf )
{
    char szTmpBuf[1024];
    char *szErrBuf;
    int iRtn;

    //判断数据库,数据表和线程锁是否存在
    if( !IsDBExist(  )
            || !IsDBMutexExist(  )
            || !IsDBTableExist( szTableName ) )
    {
            return FALSE;
    }

    /* 建表 */
    //命令组织
    sprintf( szTmpBuf, "delete from %s %s",
                    szTableName, (char *)DeleteBuf );
    // printf ( "szTmpBuf = %s\n", szTmpBuf );
    //创建
    pthread_mutex_lock( &gDBMutex );
    iRtn = sqlite3_exec( m_pDBase, szTmpBuf, NULL, NULL, &szErrBuf );
    pthread_mutex_unlock( &gDBMutex );
    if(iRtn != SQLITE_OK )
    {
            printf( "sqlite3data table delete data code:%d: cause%s\n",
                            iRtn, szErrBuf );
            pthread_mutex_lock( &gDBMutex );
            sqlite3_free( szErrBuf );
            pthread_mutex_unlock( &gDBMutex );
            return FALSE;
    }


    // printf ( "sqlite3data DeleteDBData %s\n", szTableName );
    return TRUE;
}		/* -----  end of method sqlite3data::DeleteDBData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  SelectDBData
 * Description:  从数据表中查询数据
 *       Input:  szTableName:已存在的表名称
 *               SelectBuf:  要查询的数据
 *		Return:  查询表数据成功：TRUE
 *		         查询表数据失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL sqlite3data::SelectDBData( char *szTableName,
                void *SelectBuf,
                int( *func )( void * data
                        ,int n_column,
                        char ** column_value,
                        char ** column_name ),
                void *data)
// BOOL sqlite3data::SelectDBData ( char *szTableName, void *SelectBuf )
{
    char szTmpBuf[1024];
    char *szErrBuf;
    int iRtn;


    //判断数据库,数据表和线程锁是否存在
    if( !IsDBExist(  )
            || !IsDBMutexExist(  )
            || !IsDBTableExist( szTableName ) )
    {
            return FALSE;
    }

    /* 建表 */
    //命令组织
    sprintf( szTmpBuf, "select * from %s %s",
                    szTableName, (char *)SelectBuf );
    // printf ( "szTmpBuf =%s\n", szTmpBuf );
    //创建
    pthread_mutex_lock( &gDBMutex );
    iRtn = sqlite3_exec( m_pDBase, szTmpBuf, func, data, &szErrBuf );
    pthread_mutex_unlock( &gDBMutex );
    if(iRtn != SQLITE_OK )
    {
    		if( iRtn != SQLITE_ABORT )
	            printf( "sqlite3data table select data code:%d: cause%s\n",
                            iRtn, szErrBuf );
            pthread_mutex_lock( &gDBMutex );
            sqlite3_free( szErrBuf );
            pthread_mutex_unlock( &gDBMutex );
			if( iRtn == SQLITE_ABORT )
				return TRUE;
            return FALSE;
    }

    // printf ( "sqlite3data SelectBuf %s\n", szTableName );
    return TRUE;
}		/* -----  end of method sqlite3data::SelectDBData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  UpdateDBData
 * Description:  修改数据表中的数据
 *       Input:  szTableName:已存在的表名称
 *               UpdateBuf:  要修改的数据
 *		Return:  修改表数据成功：TRUE
 *		         修改表数据失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL sqlite3data::UpdateDBData ( char *szTableName, void *UpdateBuf )
{
    return TRUE;
}		/* -----  end of method sqlite3data::UpdateDBData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  IsDBExist
 * Description:  数据库是否存在
 *       Input:  无
 *		Return:  存在：TRUE
 *				 不存在：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL sqlite3data::IsDBExist ( void )
{
    //检测数据库指针
    if( NULL == m_pDBase )
    {
            printf ( "sqlite3data DB %s is not exist\n", m_szDBName );
            return FALSE;
    }

    return TRUE;
}		/* -----  end of method sqlite3data::IsDBExist  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  sqlite3data
 *      Method:  IsDBTableExist
 * Description:  数据表是否存在
 *       Input:  szTableName:要查询的数据表名称
 *		Return:  存在：TRUE
 *		         不存在：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL sqlite3data::IsDBTableExist ( char *szTableName )
{
    char szTmpBuf[1024];
    char *szErrBuf;
    int iRtn;
    BOOL bRtn = FALSE;

    //判断数据库和线程锁是否存在
    if( !IsDBExist(  ) || !IsDBMutexExist(  ) )
    {
        return FALSE;
    }
    /* 建表 */
    //命令组织
    sprintf( szTmpBuf, "select count(*) from sqlite_master \
                where type = 'table' and name='%s'",
                szTableName);
                // "11");
    //创建
    pthread_mutex_lock( &gDBMutex );
    iRtn = sqlite3_exec( m_pDBase, szTmpBuf, SelectTable, &bRtn, &szErrBuf );
    pthread_mutex_unlock( &gDBMutex );

    if( iRtn == SQLITE_OK )
    {
        return TRUE;
    }
    else
    {
        printf ( "sqlite3data table %s not exist err code=%d err = %s\n",
                        szTableName, iRtn, szErrBuf);
        pthread_mutex_lock( &gDBMutex );
        sqlite3_free( szErrBuf );
        pthread_mutex_unlock( &gDBMutex );
        return FALSE;
    }


    return FALSE;
}		/* -----  end of method sqlite3data::IsDBTableExist  ----- */

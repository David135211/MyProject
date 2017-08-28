#include "database.h"

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Database
 *      Method:  Database
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
Database::Database ()
{
        printf ( "Database constructor\n" );
}  /* -----  end of method Database::Database  (constructor)  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Database
 *      Method:  ~Database
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
Database::~Database ()
{
        printf ( "Database destructor\n" );
}  /* -----  end of method Database::~Database  (destructor)  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  Database
 *      Method:  CreateDB
 * Description:  当没有数据库时， 创建数据库
 *       Input:  szDBPathName:数据库的保存路径及名字
 *		Return:  成功创建: 创建数据库指针:
 *				 失败创建：NULL
 *--------------------------------------------------------------------------------------
 */
void *Database::CreateDB ( char *szDBPathName )
{
        printf ( "Database CreateDB %s\n", szDBPathName );
        return NULL;
}		/* -----  end of method Database::CreateDB  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  Database
 *      Method:  DeleteDB
 * Description:  当有数据库时， 删除数据库
 *       Input:  szDBPathName:数据库的
 *		Return:  TRUE:成功删除
 *			   	 FALSE:失败删除
 *--------------------------------------------------------------------------------------
 */
BOOL Database::DeleteDB ( char *szDBPathName )
{
        printf ( "Database DeleteDB %s\n", szDBPathName );
        return FALSE;
}		/* -----  end of method Database::DeleteDB  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  Database
 *      Method:  OpenDB
 * Description:  当存在数据库时 打开数据库
 *       Input:  void:默认是已经创建的数据库
 *		Return:  打开成功：返回当前数据库指针
 *				 打开失败：返回NULL
 *--------------------------------------------------------------------------------------
 */
void* Database::OpenDB ( void )
{
        printf ( "Database OpenDB\n" );
        return NULL;
}		/* -----  end of method Database::OpenDB  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Database
 *      Method:  CloseDB
 * Description:  当打开数据库时 关闭数据库
 *       Input:  void:默认是已经创建的数据库
 *		Return:  关成功：TRUE
 *		         关失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL Database::CloseDB ( void )
{
        printf ( "Database CloseDB\n" );
        return FALSE;
}		/* -----  end of method Database::CloseDB  ----- */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  Database
 *      Method:  CreateDBTable
 * Description:  当存在已打开数据库时 创建表
 *       Input:  szTableName 创建表的名字
 *		Return:  创建表成功：TRUE
 *				 创建表失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL Database::CreateDBTable ( char *szTableName, char *szTableStruct )
{
        printf ( "Database CReateDBTaBle %s\n", szTableName );
        return FALSE;
}		/* -----  end of method Database::CreateDBTable  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Database
 *      Method:  CreateDBTableIndex
 * Description:  根据表名称、索引表名字和索引创建索引表
 *       Input:  szTableName:已存在的表名称
 *				 szIndexName:要创建的索引表名称
 *				 szIndex：	 按照此索引创建索引表
 *		Return:  创建表索引成功：TRUE
 *		         创建表索引失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL Database::CreateDBTableIndex ( char *szTableName, char *szIndexName, const char *szIndex )
{
        printf ( "Database CreateDBTableIndex %s %s %s\n",
                        szTableName, szIndexName, szIndex);
        return FALSE;
}		/* -----  end of method Database::CreateDBTableIndex  ----- */

// BOOL Database::QuickInsertHistoryDBData ( char *szTableName, vector <HISTORYDATA>*pHistoryData, unsigned int uiInterval  )
// {
//         printf ( "Database QuickInsertHistoryDBData  %s\n",
//                         szTableName);
//         return FALSE;
// }

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Database
 *      Method:  InsertDBData
 * Description:  将数据插入数据库
 *       Input:  szTableName:已存在的表名称
 *				 InsertBuf:  要插入的数据
 *		Return:  插入表数据成功：TRUE
 *				 插入表数据失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL Database::InsertDBData (char *szTableName,  string &InsertBuf )
{
        printf ( "Database InsertDBData %s\n", szTableName );
        return FALSE;
}		/* -----  end of method Database::InsertDBData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Database
 *      Method:  DeleteDBData
 * Description:  将数据从数据库中删除
 *       Input:  szTableName:已存在的表名称
 *               InsertBuf:  要删除的数据
 *		Return:  删除表数据成功：TRUE
 *		         删除表数据失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL Database::DeleteDBData ( char *szTableName, void *DeleteBuf )
{
        printf ( "Database DeleteDBData %s\n", szTableName );
        return FALSE;
}		/* -----  end of method Database::DeleteDBData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Database
 *      Method:  SelectDBData
 * Description:  从数据表中查询数据
 *       Input:  szTableName:已存在的表名称
 *               InsertBuf:  要查询的数据
 *		Return:  查询表数据成功：TRUE
 *		         查询表数据失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL Database::SelectDBData( char *szTableName,
                void *SelectBuf,
                int( *func )( void * data
                        ,int n_column,
                        char ** column_value,
                        char ** column_name ),
                void *data)
// BOOL Database::SelectDBData ( char *szTableName, void *SelectBuf )
{
        printf ( "Database SelectBuf %s\n", szTableName );
        return FALSE;
}		/* -----  end of method Database::SelectDBData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Database
 *      Method:  UpdateDBData
 * Description:  修改数据表中的数据
 *       Input:  szTableName:已存在的表名称
 *               InsertBuf:  要修改的数据
 *		Return:  修改表数据成功：TRUE
 *		         修改表数据失败：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL Database::UpdateDBData ( char *szTableName, void *UpdateBuf )
{
        printf ( "Database UpdateBuf %s\n", szTableName );
        return FALSE;
}		/* -----  end of method Database::UpdateDBData  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Database
 *      Method:  IsDBExist
 * Description:  数据库是否存在
 *       Input:  无
 *		Return:  存在：TRUE
 *				 不存在：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL Database::IsDBExist ( void )
{
        printf ( "Database IsDBExist\n" );
        return FALSE;
}		/* -----  end of method Database::IsDBExist  ----- */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Database
 *      Method:  IsDBTableExist
 * Description:  数据表是否存在
 *       Input:  szTableName:要查询的数据表名称
 *		Return:  存在：TRUE
 *		         不存在：FALSE
 *--------------------------------------------------------------------------------------
 */
BOOL Database::IsDBTableExist ( char *szTableName )
{
        printf ( "Database IsDBTableExist %s\n", szTableName );
        return FALSE;
}		/* -----  end of method Database::IsDBTableExist  ----- */

#ifndef SQLITE3DATA_H
#define SQLITE3DATA_H

#include "sqlite3.h"
#include "database.h"
#include "dbasedatatype.h"



class sqlite3data : public Database
{

public:
    sqlite3data();
    virtual ~sqlite3data();

public:
        /* ====================  METHODS     ======================================= */
        //创建数据库
        virtual void *CreateDB( char *szDBPathName );
        //删除数据库
        virtual BOOL DeleteDB( char *szDBPathName );
        //打开数据库
        virtual void* OpenDB( void );
        //关闭数据库
        virtual BOOL CloseDB( void );
        //创建表
        virtual BOOL CreateDBTable( char *szTableName, char *szTableStruct );
        //创建索引
        virtual BOOL CreateDBTableIndex( char *szTableName, char *szIndexName, const char *szIndex );
        //插入表数据
        virtual BOOL InsertDBData( char *szTableName, string &InsertBuf );
        // //快速插入历史数据
        // virtual BOOL QuickInsertHistoryDBData ( char *szTableName, vector <HISTORYDATA>*pHistoryData, unsigned int uiInterval  );
        //删除表数据
        virtual BOOL DeleteDBData( char *szTableName, void *DeleteBuf );
        //查询表
        // virtual BOOL SelectDBData( char *szTableName, void *SelectBuf );
        virtual BOOL SelectDBData( char *szTableName,
                        void *SelectBuf,
                        int( *func )( void * data
                                ,int n_column,
                                char ** column_value,
                                char ** column_name ),
                        void *data);
        //更新数据库数据
        virtual BOOL UpdateDBData( char *szTableName, void *UpdateBuf );

        //是否已存在数据库
        virtual BOOL IsDBExist( void );
        //表是否存在
        virtual BOOL IsDBTableExist( char *szTableName );


protected:

private:
        /* ====================  VARIABLES   ======================================= */
        static sqlite3 *m_pDBase;				/* 数据库指针， 只被打开一次 */
        char m_szDBName[MAX_DBASE_NAME_LEN];
};

#endif // SQLITE3DATA_H

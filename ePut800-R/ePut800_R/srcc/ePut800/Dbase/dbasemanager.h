/*
 * =====================================================================================
 *
 *       Filename:  dbasemanager.h
 *
 *    Description:  数据库管理类 父类
 *
 *        Version:  1.0
 *        Created:  2015年03月18日 14时39分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mengqp
 *   Organization:  esdtek
 *
 *		  history:  Time							Author	 Version	Description
 *					2015年03月18日 14时41分05秒	    mengqp   1.0		create
 *
 * =====================================================================================
 */

#ifndef  DBASEMANAGER_INC
#define  DBASEMANAGER_INC
/* #####   HEADER FILE INCLUDES   ################################################### */
#include "../Typedef.h"
#include "dbasedatatype.h"

#define DBASE_SOE_DATATYPE       1  // soe 数据类型
#define DBASE_YXALARM_DATATYPE       2  // YXALARM 数据类型

/* 此处增加管理方法模块宏 */
#define	DBMANAGER_MOUDLE_SOE			1			/* 关于SOE数据库管理模块 */
/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ################################### */


/* #####   TYPE DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ######################### */

/* #####   DATA TYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */

/*
 * =====================================================================================
 *        Class:  CDBaseManager
 *  Description:
 * =====================================================================================
 */
class CDBaseManager
{
  public:
	/* ====================  LIFECYCLE     ======================================= */
	CDBaseManager ();                             /* constructor      */
	virtual ~CDBaseManager ();                            /* destructor       */
	/* ====================  METHODS       ======================================= */
	//初始化
	CDBaseManager *Init( void );
	// 查询所有数据
	virtual int SelectAllData( BYTE DataType);
	// 查询所有数据的第一页数据
	virtual int SelectFirstPageOfAllData(DWORD datanum, BYTE DataType);
	// 按时间查询数据
	virtual int SelectDataByTime ( char *pszBeginTime, char *pszEndTime, BYTE DataType );
	// 按时间查询数据的第一页数据
	virtual int SelectFirstPageOfDataByTime ( DWORD datanum, char *pszBeginTime, char *pszEndTime, BYTE DataType );
	// 按总线和地址查询数据
	virtual int SelectDataByBusLineAndAddr ( WORD wBusLine , WORD wAddr , BYTE DataType);
	// 按总线和地址查询数据第一页数据
	virtual int SelectFirstPageOfDataByBusLineAndAddr ( DWORD datanum,WORD wBusLine , WORD wAddr, BYTE DataType );
	// 插入数据
	virtual BOOL InsertData ( void *data );
	//更新历史数据
	virtual BOOL UpdateData( void *data );
	// 删除数据
	virtual BOOL DeleteData( void );
	//破坏
	virtual void ClearData( void );

  public:
	// 获取数据
	virtual void* GetData ( DWORD pos );

  public:
	/* ====================  DATA MEMBERS  ======================================= */
	char m_szDBPathName[MAX_DBASE_NAME_LEN]; /* 数据库路径名称 */
	char m_szDBTableName[MAX_DBASE_HISTORY_TABLE_NAME_LEN]; /* 历史数据表名称 */
	// BYTE m_bySaveDateLen;                   /* 保存的时间长度 */
	BYTE m_byMoudle;  //模块号  可以根据模块号，选择用那个数据库
	/* ====================  METHODS       ======================================= */
	CDBaseManager *m_pMoudle;               /* 模块指针 */
  protected:
	/* ====================  METHODS       ======================================= */

  private:
	/* ====================  METHODS       ======================================= */
	//读取配置文件
	BOOL ReadCfgInfo( void );
	//创建数据库模块
	CDBaseManager *CreateDBMoudle( void );
	/* ====================  DATA MEMBERS  ======================================= */

}; /* -----  end of class CDBaseManager  ----- */


/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ################################ */

/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ############################ */

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ##################### */



#endif   /* ----- #ifndef DBASEMANAGER_INC  ----- */

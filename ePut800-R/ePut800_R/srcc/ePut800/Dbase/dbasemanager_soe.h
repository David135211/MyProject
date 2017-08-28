#ifndef		DBASEMANGER_SOE_H
#define		DBASEMANGER_SOE_H

#include <pthread.h>
#include <stdlib.h>
#include "sqlite3dbase.h"
#include "dbasemanager.h"

// "/media/mmcblk0p1/SoeDbase.db"		//"SoeDbase.db"
#define SOEDBASEPATH			"./SoeDbase.db"
#define SOEDBTABLENAME			"SoeTable"
#define SOEDBREALTIMETABLENAME	"RealTimeSoeTable"
#define SOEDBTABLEINDEXDATA		"DataType,DateTime"
#define SOEDBTABLEINDEXNO		"BusNo,DevNo"
#define SOEDBTABLEDATATYPE		"DataType"
#define SOEDBTABLEDATATIME		"DateTime"
#define SOEDBTABLEBUSNO			"BusNo"
#define SOEDBTABLEDEVNO			"DevNo"
#define SOEDBTABLEPOINTNO		"PointNo"
#define REALTIMEDBTABLINDEX		"BusNo,DevNo,PointNo"




class CDBaseManager_SOE : public CDBaseManager
{
  public :
	CDBaseManager_SOE();
	~CDBaseManager_SOE();

	// 初始化
	virtual BOOL Init( void );

	// 查询所有数据
	virtual int SelectAllData( BYTE DataType);
	// 查询所有数据的第一页数据
	virtual int SelectFirstPageOfAllData(DWORD datanum, BYTE DataType );
	// 按时间查询数据
	virtual int SelectDataByTime ( char *pszBeginTime, char *pszEndTime , BYTE DataType);
	// 按时间查询数据的第一页数据
	virtual int SelectFirstPageOfDataByTime (DWORD datanum, char *pszBeginTime, char *pszEndTime , BYTE DataType);
	// 按总线和地址查询数据
	virtual int SelectDataByBusLineAndAddr ( WORD wBusLine , WORD wAddr , BYTE DataType);
	// 按总线和地址查询数据第一页数据
	virtual int SelectFirstPageOfDataByBusLineAndAddr (DWORD datanum, WORD wBusLine , WORD wAddr , BYTE DataType);
	// 插入数据
	virtual BOOL InsertData ( void *data );
	// 获取数据
	virtual void* GetData ( DWORD Pos );
	// 清空数据
	virtual void ClearData( void );

  private :
	// 创建数据库
	BOOL CreateDB( void );
	// 关闭数据库
	BOOL CloseDB( void );
	// 删除数据库
	BOOL DeleteDB( void );
	// 创建表和索引
	BOOL CreateDBTableAndIndex( void );

  public:
	/* ====================  DATA MEMBERS  ======================================= */
	Database *m_pCDBase;                      /* 数据库指针 调用数据库函数 */
	pthread_t m_InsertHistoryThreadId;      /* 插入历史数据线程 */


  public:
	BOOL InterruptSelect;
	BOOL FirstSelectState;
	WORD CurrentAllPage;
	vector<SOEDATA> * m_SoeData;		//查询历史数据容器

};

#endif

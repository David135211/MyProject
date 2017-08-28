#ifndef CPAGE_DATAQUERY_H
#define CPAGE_DATAQUERY_H


#include "cpage.h"

#include <QVector>
#include <QTreeView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QToolBox>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QDialog>
#include <QObject>
#include <QDateTime>


#define     DATAQUERY_REALDATA   0x01  /* 实时数据 */
#define     DATAQUERY_SOE        0x02  /* SOE 数据 */
#define     DATAQUERY_YXALARM        0x03  /* 遥信报警 */

// 每页的最大数量
#define ONE_PAGE_DATANUM  14


/* 实时数据结构 */
typedef struct _QUERY_REALTIME_DATA
{
    WORD index; // 数据序号从0开始
    QString content; //数据栏内容

    /* 通过这两项可以获得数据 */
    BYTE type;  // 数据类型
    WORD busline; // 总线
    WORD addr; //地址
    WORD point; // 数据点号

    QString val;

    QString pos;  // 数据位置

    _QUERY_REALTIME_DATA()
    {
        busline = 0xffff;
        addr = 0;
        point = 0;
        index = 0;
        type = 0;
        content = QString("");
        pos = QString("");
        val= QString("");
    }

}QUERY_REALTIME_DATA;

/* 实时数据页结构 */
typedef struct _QUERY_REALTIME_PAGE
{
    int datanum;  // 数据数量
    QVector<QUERY_REALTIME_DATA> data;  // 页数据
}QUERY_REALTIME_PAGE;

/* 实时数据回路结构 */
typedef struct _QUERY_REALTIME_CIRCUIT
{
    WORD index;  // 本回路序号
    char name[100];  // 名字

    int pagenum;  // 页数量
    QVector<QUERY_REALTIME_PAGE> page;  // 页数据
}QUERY_REALTIME_CIRCUIT;
/* 实时数据柜结构 */
typedef struct _QUERY_REALTIME_CABINET
{
    WORD index;     // 柜号
    char name[100];  // 名字

    int pagenum;
    QVector<QUERY_REALTIME_PAGE> page;

    int circuitnum;
    QVector<QUERY_REALTIME_CIRCUIT> circuit;
}QUERY_REALTIME_CABINET ;

/* 树属性 */
typedef struct _QUERY_TREE_ITEM_ATTR
{
	int index;	//当前树序号
	BOOL parent;  //是否是父节点
	int parent_index;  //如果不是父节点，父节点的序号
	QString name;	   //当前名字
	QString parent_name;  //父节点名字

	_QUERY_TREE_ITEM_ATTR()
	{
		index = 0;
		parent = TRUE;
		parent_index = 0;
		name = QString("");
		parent_name = QString("");
	}
}QUERY_TREE_ITEM_ATTR;


class CPage_DataQuery : public CPage
{
    Q_OBJECT

public:
    explicit CPage_DataQuery( BYTE byType );
    virtual BOOL AddData( LPVOID pVoid ) ;
    virtual BOOL DrawPage( QPainter * pPainter );
    virtual BOOL mousePressEvent(QMouseEvent *pEvent );
    ~CPage_DataQuery();
    // virtual BOOL ProcessPageDown( );
    // virtual BOOL ProcessPageUp( );

  private: // 初始化界面
    // 清空数据
    void ClearData();
    // 初始化界面
    BOOL InitUi( void );
    // 删除Ui界面
    void DeleteUi( void );

    // 初始化图片
    BOOL InitUiPics( void );
    // 删除图片
    // void DeletePics( void );
    // 初始化位置信息
    BOOL InitUiPosition( void );
    // 删除位置信息
    // void DeleteUiPosition( void );
    // 初始化数据表
    BOOL InitDataTable( void );
    // 删除数据表
    void DeleteDataTable(void);

    // 初始化柜表
    BOOL InitCabinetTable(void );
    // 初始化回路表
    BOOL InitCircuitTable(PCABINET pCabinet,
                          QUERY_REALTIME_CABINET *pcabinetable);

    // 初始化页数表
    BOOL InitPageTable( PCABINET pCabinet,
                        QUERY_REALTIME_CABINET *pcabinetable,
                        QUERY_REALTIME_CIRCUIT *pcircuitable,
                        BYTE flag);


    // 初始化页数据表数据
    BOOL InitPageTableData(PSINGLECIRCUIT pCircuit,
                           QVector<QUERY_REALTIME_PAGE> *ppage,
                           BYTE flag
                           );
    // 初始化实时数据查询树
    BOOL InitUiTreeView( void );
    // 删除实时数据查询树
    void DeleteUiTreeView(void );
    // 根椐配置初始化树
    BOOL InitUiTreeViewbyCfg( QStandardItemModel *pModle );

    // 初始化抽屉
    BOOL InitUiSoeToolBox( void );
    // 删除抽屉
    void DeleteUiSoeToolBox( void );
    // 初始化按时间查询
    BOOL InitUiSoeToolBoxByTime(void);
    // 删除按时间查询
    void DeleteUiSoeToolBoxByTime( void );
    // 初始化按总线查询
    BOOL InitUiSoeToolBoxByBusLine(void);
    // 删除按总线查询
    void DeleteUiSoeToolBoxByBusLine( void );

  private: // 部件功能
    // 设置当前的提示信息
    void SetTipsStr( QString str);
    // 获取当前提示信息
    QString GetTipsStr( void );
    // 画提示信息
    void DrawTips( QPainter *pPainter );

    // 设置当前页数
    void SetCurrentPage( DWORD dwCurrentPage );
    // 获得当前页数
    DWORD GetCurrentPage( void );
    // 设置总页数
    void SetTotalPage( DWORD dwTotalPage );
    // 获得总页数
    DWORD GetTotalPage( void );
    // 画页数信息
    void DrawPages( QPainter *pPainter);

    //页数增加
    void PageUp();
    // 页数减少
    void PageDown();

    // 设置查询类型
    void SetQueryType( BYTE type );
    // 获取查询类型
    BYTE GetQueryType( void );
    // 画查询类型图片不显示
    void DrawQueryTypePicsNone( QPainter *pPainter );
    // 画相应的查询类型图片
    void DrawQueryTypePic( QPainter *pPainter, BYTE type, BOOL flag );
    // 画查询类型图片
    void DrawQueryTypePics( QPainter *pPainter );

    // 设置数据
    void SetQueryData( BYTE index, QUERY_REALTIME_DATA data );
    // 获取数据
    QUERY_REALTIME_DATA *GetQueryData( BYTE index );
	// 画数据头
	void DrawDataHead( QPainter *pPainter );
    // 画序号区
    void DrawDataIndex( QPainter *pPainter, BYTE index );
    // 画数据内容或名称
    void DrawDataContent( QPainter *pPainter, BYTE index , QString content);
    // 画数据当前状态或数值
    void DrawDataValue( QPainter *pPainter, BYTE index, QString val);
    // 画数据位置
    void DrawDataPosition( QPainter *pPainter, BYTE index,QString position );
    // 设置Soe数据
    void SetSoeData( BYTE index, SOEDATA data );
    // 获取Soe数据
    SOEDATA *GetSoeData( BYTE index );
    // 画数据 调用上 4 个
    void DrawData( QPainter *pPainter );

	// 设置树属性
	void SetTreeItemAttr( QUERY_TREE_ITEM_ATTR attr);
	// 获取树属性
	QUERY_TREE_ITEM_ATTR *GetTreeItemAttr(void);
	// 设置树数据
	void SetTreeItemData( void );
	// 设置Soe显示数据
	void SetDisplaySoe(void);
  private:  // 点击功能
    // 获取区域功能
    BYTE GetFuncAreaType( QPoint pt);

    // 显示相应插件
    void ShowQueryTypeUi( void );
    // 隐藏所有Ui
    void HideQueryTypeUi( void );

  private:	//数据库
	void QueryAllSoe( void );

  private slots:
    // 槽 实时数据树的点击项改变
    void TreeItemChange(const QModelIndex &index);
    // 槽 起始时间改变
	void BeginTimeChanged(const QDateTime &datetime);
    // 槽 结束时间改变
	void EndTimeChanged(const QDateTime &datetime);
    // 槽 结束时间改变
	void SlotQuerySoeByTime( void );


private:
    // SOE图片
    QString m_strPicSoeSelect;    //选中图片
    QString m_strPicSoeNoneSelect;//未选中图片

    // 实时数据图片
    QString m_strPicRealTimeSelect; // 选中图片
    QString m_strPicRealTimeNoneSelect; // 未选中图片

    // 实时数据图片
    QString m_strPicYxAlarmSelect; // 选中图片
    QString m_strPicYxAlarmNoneSelect; // 未选中图片

    // 提示信息位置
    QRect m_rcToolTips;
    // 页数信息区域
    QRect m_rcPageInfo;
    // 功能区域
    QRect m_rcFunction[7];
    enum rcNavigation
    {
        // 功能区域
        PAGEUP = 0,// 上翻页
        PAGEDOWN,  // 下翻页
        BACKHOME,  // 主菜单
        QUERYSOE,  // 查询soe
        QUERYREALDATA, // 查询实时数据
        LOGIN, // 登陆
        QUERYYXALARM  // 遥信报警
    };

    // 数据显示区
    QRect m_rcDataDiaplay[15][4]; // m_rcDataDiaplay[0] 第0行数组为名称

    // 柜表
    // QVector<QUERY_REALTIME_DATA> **m_vectorCabinetTable;
    // 柜表数量
    // WORD m_wCabineNum;
    // 回路表
    // QVector<QUERY_REALTIME_DATA> ***m_vectorCircuitTable;
    // 回路数量
    // WORD m_wCircuitNum;

    QVector<QUERY_REALTIME_CABINET> m_vectorCabinetTable;
    QUERY_REALTIME_DATA m_CurrentData[ONE_PAGE_DATANUM];
    BYTE m_byCurrentDataNum;

	// SOE 数据
	SOEDATA m_CurrentSoe[ONE_PAGE_DATANUM];
    BYTE m_byCurrentSoeNum;
	BOOL m_bSelectAllSoe;


    // 定义实时数据树
    QTreeView *m_treeRealTimeData;
    //定义树形模块
    QStandardItemModel *m_pModel;
	// 树属性
	QUERY_TREE_ITEM_ATTR m_treeAttr;

    // Soe抽屉
    QToolBox *m_toolBoxSoe;
    // 按时间查询的widget
    QWidget *m_widgetSoeBytime;
    // 起始时间
    QPushButton *m_buttonTimeBegin;
    QDialog *m_dialogTimeBegin;  // 起始时间窗口
    QDateTimeEdit *m_dtBegin;    // 起始时间
    QLineEdit *m_editBeginYear;  // 显示起始时间年
    QLabel *m_labelBeginYear;    // 起始年
    QLineEdit *m_editBeginMonth; // 显示起始时间月
    QLabel *m_labelBeginMonth;   // 起始月
    QLineEdit *m_editBeginDay;   // 显示起始时间日
    QLabel *m_labelBeginDay;     // 起始日
    QLineEdit *m_editBeginHour;  // 显示起始时间时
    QLabel *m_labelBeginHour;    // 起始时
    QLineEdit *m_editBeginMinute;  // 显示起始时间分
    QLabel *m_labelBeginMinute;    // 起始分
    QLineEdit *m_editBeginSecond;  // 显示起始时间秒
    QLabel *m_labelBeginSecond;    // 起始秒

    // 结束时间
    QPushButton *m_buttonTimeEnd;
    QDialog *m_dialogTimeEnd; // 结束时间窗口
    QDateTimeEdit *m_dtEnd;   // 结束时间
    QLineEdit *m_editEndYear; // 显示结束时间年
    QLabel *m_labelEndYear;   // 结束年
    QLineEdit *m_editEndMonth;// 显示结束时间月
    QLabel *m_labelEndMonth;  // 结束月
    QLineEdit *m_editEndDay;  // 显示结束时间日
    QLabel *m_labelEndDay;    // 结束日
    QLineEdit *m_editEndHour; // 显示结束时间时
    QLabel *m_labelEndHour;   // 结束时
    QLineEdit *m_editEndMinute;  // 显示结束时间分;
    QLabel *m_labelEndMinute;   // 结束分
    QLineEdit *m_editEndSecond;  // 显示结束时间秒;
    QLabel *m_labelEndSecond;   // 结束秒
    // 确定键
    QPushButton *m_dtOk;

    // 按总线地址查询的widget
    QWidget *m_widgetSoeByBusLine;
    // 总线
    QLabel *m_labelBusLine;
    QComboBox *m_boxBusLine;
    // 地址
    QLabel *m_labelAddr;
    QComboBox *m_boxAddr;
    // 点号
    QLabel *m_labelPoint;
    QComboBox *m_boxPoint;
    // 确定键
    QPushButton *m_BusLineOk;

  private:
    // 提示信息字符
    QString m_strTips;
    // 当前页数信息
    DWORD m_dwCurrentPage;
    // 当前soe页数信息
    DWORD m_dwSoeCurrentPage;
    // 当前遥信报警页数信息
    DWORD m_dwYxAlarmCurrentPage;
    // 当前总页数信息
    DWORD m_dwTotalPage;
    // 当前soe总页数信息
    DWORD m_dwSoeTotalPage;
    // 当前Yx报警总数信息
    DWORD m_dwYxAlarmTotalPage;
    // 当前的查询类型
    BYTE m_byCurrentQueryType;
	// 查询到的Soe数量
	DWORD m_dwSoeNum ;
	// 查询到的YxAlarm数量
	DWORD m_dwYxAlarmNum ;

};

#endif // CPAGE_DATAQUERY_H

#include "cpage_dataquery.h"
#include <QMessageBox>
#include <stdio.h>
#include <string.h>
#include <QDebug>

static BOOL LOG = 1;
static BOOL PRINT = 1;
/*******************************************************************************
 * 函数名:print
 * 功能描述:打印信息
 * 参数:char *buf
 * 返回值:void
 ******************************************************************************/
static void print( char *buf)
{
	if( PRINT )
	{
		printf("%s\n", buf);
	}

	if( LOG )
	{

	}
}   /*-------- end print -------- */


CPage_DataQuery::CPage_DataQuery(BYTE byType ):CPage( byType )
{
	// 初始化变量 具体每个变量代表的函义在.h中查找
	m_strPicSoeNoneSelect = "";
	m_strPicSoeNoneSelect = "";

	m_strPicRealTimeSelect = "";
	m_strPicRealTimeNoneSelect = "";

	m_strPicYxAlarmSelect = "";
	m_strPicYxAlarmNoneSelect = "";

	// QRect 不进行初始化了，初始化在InitUi里面
	// m_rcToolTips = QRect(0, 0, 0 , 0);
	// m_rcPageInfo = QRect(0, 0, 0 , 0);
	// m_rcFunction[PAGEUP] = QRect(0, 0, 0 , 0);
	// m_rcFunction[PAGEDOWN] = QRect(0, 0, 0 , 0);
	// m_rcFunction[BACKHOME] = QRect(0, 0, 0 , 0);
	// m_rcFunction[QUERYSOE] = QRect(0, 0, 0 , 0);
	// m_rcFunction[QUERYREALDATA] = QRect(0, 0, 0 , 0);

	m_vectorCabinetTable.clear();
	// for (unsigned int i=0; i < ONE_PAGE_DATANUM; i++)
	// {
	//  m_CurrentData[i] = NULL;
	// }

	m_treeRealTimeData = NULL;
	m_pModel = NULL;

	m_toolBoxSoe = NULL;
	m_widgetSoeBytime = NULL;
	m_dialogTimeBegin = NULL;
	m_dtBegin = NULL;
	m_dialogTimeEnd = NULL;
	m_dtEnd = NULL;
	m_dtOk = NULL;
	m_widgetSoeByBusLine = NULL;
	m_labelBusLine = NULL;
	m_boxBusLine = NULL;
	m_labelAddr = NULL;
	m_boxAddr = NULL;
	m_labelPoint = NULL;
	m_boxPoint = NULL;

	m_strTips = QString( "" );
	m_dwCurrentPage = 0;
	m_dwTotalPage = 0;
	m_dwSoeCurrentPage = 0;
	m_dwSoeTotalPage = 0;
	m_byCurrentQueryType = 0;

	SetQueryType(DATAQUERY_REALDATA);

	// 初始化值
	m_bSelectAllSoe = TRUE;
	m_buttonTimeBegin = NULL;
	m_editBeginYear = NULL;
	m_labelBeginYear = NULL;
	m_editBeginMonth = NULL;
	m_labelBeginMonth = NULL;
	m_editBeginDay = NULL;
	m_labelBeginDay = NULL;
	m_editBeginHour = NULL;
	m_labelBeginHour = NULL;
	m_editBeginMinute = NULL;
	m_labelBeginMinute = NULL;
	m_editBeginSecond = NULL;
	m_labelBeginSecond = NULL;
	m_buttonTimeEnd = NULL;
	m_editEndYear = NULL;
	m_labelEndYear = NULL;
	m_editEndMonth = NULL;
	m_labelEndMonth = NULL;
	m_editEndDay = NULL;
	m_labelEndDay = NULL;
	m_editEndHour = NULL;
	m_labelEndHour = NULL;
	m_editEndMinute = NULL;
	m_labelEndMinute = NULL;
	m_editEndSecond = NULL;
	m_labelEndSecond = NULL;
	m_BusLineOk = NULL;


}

CPage_DataQuery::~CPage_DataQuery()
{
	DeleteUi();
}


BOOL CPage_DataQuery::AddData( LPVOID pVoid )
{
	if (NULL == pVoid)
	{
		// 此处不加任何东西
	}
	// 初始化界面
	InitUi( );
	return TRUE ;
}

BOOL CPage_DataQuery::DrawPage( QPainter * pPainter )
{
	CPage::DrawPage( pPainter ) ;

	DrawTips(pPainter);

	DrawPages(pPainter );
	DrawData(pPainter);


	DrawQueryTypePics(pPainter);
	ShowQueryTypeUi();


	return TRUE;
}




BOOL CPage_DataQuery::mousePressEvent(QMouseEvent *pEvent )
{
	QPoint pt(pEvent->pos());

	// update time
	AfxUpdateTimeoutTime();

	BYTE type = GetFuncAreaType(pt);
	switch (type)
	{
		case PAGEUP: // 页增加
			{
				PageUp();
			}
			break;

		case PAGEDOWN://页减少
			{
				PageDown();
			}
			break;

		case BACKHOME://返回主页
			{
				// 隐藏所有ui
				HideQueryTypeUi();
				m_bSelectAllSoe = TRUE;
				AfxSetCurPageIndex( Widget::PAGE_HOME );
			}
			break;

		case QUERYSOE://选择soe
			{
				SetQueryType(DATAQUERY_SOE);
			}
			break;

		case QUERYREALDATA://选择实时数据
			{
				SetQueryType(DATAQUERY_REALDATA);
			}
			break;

		/*lel*/
#if 0
		case LOGIN: //
			{
				Widget * TempWidget = AfxGetWidget();
				TempWidget->LoginDlg->show();
			}
			break;
#endif
		/*end*/
		case QUERYYXALARM://选择soe
			{
				SetQueryType(DATAQUERY_YXALARM);
			}
			break;

		default:
			break;
	}

	// if( CPage::mousePressEvent( pEvent ) )
	// {
	//     AfxRefreshScreen();
	//     return TRUE ;
	// }

	AfxRefreshScreen( );
	return  TRUE ;
}

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:InitUi
 * 功能描述:初始化Ui
 * 参数:BOOL
 * 返回值:void
 ******************************************************************************/
BOOL CPage_DataQuery::InitUi(void)
{
	m_strTips = "";

	// 初始化界面图片
	if ( !InitUiPics() )
	{
		print( (char *)"InitUiPics error" );
		return FALSE;
	}

	// 初始化位置信息
	if ( !InitUiPosition() )
	{
		print( (char *)"InitUiPosition error" );
		return FALSE;
	}

	// 初始化数据表
	if ( !InitDataTable() )
	{
		print( (char *)"InitUiDataTable error" );
		// return FALSE;
	}

	// // 初始化实时数据树
	if( !InitUiTreeView() )
	{
		print( (char *)"InitUiTreeView error" );
		return FALSE;
	}

	// 初始化抽屉
	if( !InitUiSoeToolBox())
	{
		print( (char *)"InitUiSoeToolBox error" );
		return FALSE;
	}


	print( (char *)"InitUi ok" );
	return TRUE;
}   /*-------- end class CPage_DataQuery method InitUi -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DeleteUi
 * 功能描述:删除Ui界面
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DeleteUi(void)
{
	// 删除抽屉数据
	DeleteUiSoeToolBox();

	// 删除树数据
	DeleteUiTreeView();

	// 删除表数据
	DeleteDataTable();

	print( (char*) "DeleteUi Ok");

}   /*-------- end class CPage_DataQuery method DeleteUi -------- */


/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:InitUiPics
 * 功能描述:初始化界面图片
 * 参数:void
 * 返回值:BOOL
 ******************************************************************************/
BOOL CPage_DataQuery::InitUiPics(void)
{
	// 设置本页的图片路径，该图片在 CPage::DrawPage( pPainter )加载
	m_strPic = "/usr/share/ePut800/Pics/ePut800-DataQuery.jpg";

	//设置SOE图片路径
	m_strPicSoeSelect = "/usr/share/ePut800/Pics/ePut800-DataQuery-SOE-Select.jpg";
	m_strPicSoeNoneSelect = "/usr/share/ePut800/Pics/ePut800-DataQuery-SOE-NoneSelect.jpg";

	//设置实时数据图片路径
	m_strPicRealTimeSelect = "/usr/share/ePut800/Pics/ePut800-DataQuery-RealTimeData-Select.jpg";
	m_strPicRealTimeNoneSelect = "/usr/share/ePut800/Pics/ePut800-DataQuery-RealTimeData-NoneSelect.jpg";

	/*lel*/
#if 0
	//设置遥信报警图片路径
	m_strPicYxAlarmSelect = "/usr/share/ePut800/Pics/ePut800-DataQuery-YxAlarm-Select.jpg";
	m_strPicYxAlarmNoneSelect = "/usr/share/ePut800/Pics/ePut800-DataQuery-YxAlarm-NoneSelect.jpg";
#else
	//设置状态信息图片路径
	m_strPicYxAlarmSelect = "/usr/share/ePut800/Pics/ePut800-DataQuery-StatusInfo-Select.jpg";
	m_strPicYxAlarmNoneSelect = "/usr/share/ePut800/Pics/ePut800-DataQuery-StatusInfo-NoneSelect.jpg";
#endif
	/*end*/

	print( (char *)"InitUiPics ok" );
	return TRUE;
}   /*-------- end class CPage_DataQuery method InitUiPics -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:InitUiPosition
 * 功能描述:初始化位置信息
 * 参数:void
 * 返回值:BOOL
 ******************************************************************************/
BOOL CPage_DataQuery::InitUiPosition(void)
{
	m_rcToolTips = QRect( 150, 27, 200, 32 );
	m_rcPageInfo = QRect( 380, 27, 140, 32 );

	m_rcFunction[PAGEDOWN] = QRect( 532, 23, 68, 48 );
	m_rcFunction[PAGEUP] = QRect(615, 23, 68, 48 );
	m_rcFunction[BACKHOME] = QRect( 698, 23, 68, 48 );
	m_rcFunction[QUERYREALDATA] = QRect(34 , 88, 100, 38);
	m_rcFunction[LOGIN] = QRect( 646 ,558 , 154 ,42);
	/*lel*/
#if 1
	m_rcFunction[QUERYYXALARM] = QRect( 150, 88, 100, 38);
	m_rcFunction[QUERYSOE] = QRect( 265, 88, 100, 38);
#else
	m_rcFunction[QUERYSOE] = QRect( 150, 88, 100, 38);
	m_rcFunction[QUERYYXALARM] = QRect( 265, 88, 100, 38);
#endif

	//序号 名称
	m_rcDataDiaplay[0][0] = QRect(145, 137, 64, 29) ;
	// 内容 名称
	m_rcDataDiaplay[0][1] = QRect(209, 137, 339, 29) ;
	// 数据名称
	m_rcDataDiaplay[0][2] = QRect(546, 137, 150, 29) ;
	// 位置名称
	m_rcDataDiaplay[0][3] = QRect(696, 137, 70, 29) ;
	for (int i = 1; i < 15; i++)
	{
		//序号
		m_rcDataDiaplay[i][0] = QRect(145, 140 + i * 25, 64, 25 ) ;
		// 内容
		m_rcDataDiaplay[i][1] = QRect(209, 140 + i * 25, 339, 25 ) ;
		// 数据
		m_rcDataDiaplay[i][2] = QRect(546, 140 + i * 25, 150, 25 ) ;
		// 位置
		m_rcDataDiaplay[i][3] = QRect(696, 140 + i * 25, 70, 25 ) ;
	}

	print( (char *)"InitUiPosition ok" );
	return TRUE;

}   /*-------- end class CPage_DataQuery method InitUiPosition -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:InitDataTable
 * 功能描述:初始化数据表 供InitUi调用
 * 参数:void
 * 返回值:BOOL
 ******************************************************************************/
BOOL CPage_DataQuery::InitDataTable(void)
{
	if( !InitCabinetTable(  ) )
	{
		print( (char *)"InitUiDataTable error" );
		return FALSE;
	}

	print( (char *)"InitUiDataTable ok" );
	return TRUE;
	// InitCircuitTable(  );

}   /*-------- end class CPage_DataQuery method InitDataTable -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DeleteDataTable
 * 功能描述:清除数据表
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DeleteDataTable(void)
{
	int iCabinetNum = m_vectorCabinetTable.size();

	for (int i=iCabinetNum-1; i>=0; i--)
	{
		// 清除柜页数据
		for (int j=m_vectorCabinetTable[i].pagenum-1; j>=0; j--)
		{
			// 清除data
			m_vectorCabinetTable[i].page[j].data.clear();
		}
		// 清除page
		m_vectorCabinetTable[i].page.clear();

		// 清除回路数据
		for (int k=m_vectorCabinetTable[i].circuitnum-1; k>=0; k--)
		{
			// 清除page
			for (int kk=m_vectorCabinetTable[i].circuit[k].pagenum-1; kk >=0; kk--)
			{
				// 清除data
				m_vectorCabinetTable[i].circuit[k].page[kk].data.clear();
			}

			// 清除page
			m_vectorCabinetTable[i].circuit[k].page.clear();
		}
		m_vectorCabinetTable[i].circuit.clear();
	}

	// 清除柜数据
	m_vectorCabinetTable.clear();

	print( (char *)"DeleteUiDataTable ok" );

}   /*-------- end class CPage_DataQuery method DeleteDataTable -------- */


/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:InitCabinetTable
 * 功能描述:初始化柜表 供InitDataTable调用
 * 参数:void
 * 返回值:BOOL
 ******************************************************************************/
BOOL CPage_DataQuery::InitCabinetTable(void)
{
	// 获取柜信息指针
	const QCAB_ARRAY &cabineArray = AfxGetCabinetArray();
	// 柜数量
	int iCabinetNum = cabineArray.size();

	if( 0 >= iCabinetNum )
	{
		char buf[128];

		sprintf(buf, "InitCabinetTable find  iCabinetNum=%d\n", iCabinetNum);
		print( buf );

		return FALSE;
	}

	// 轮询柜
	for (int i=0; i<iCabinetNum; i++)
	{
		PCABINET pCabinet = AfxGetCabinet( i );
		if (NULL == pCabinet )
		{
			char buf[128];

			sprintf(buf, "InitCabinetTable find cabinet%d NULL \n", i);
			print( buf );

			return FALSE;
		}

		QUERY_REALTIME_CABINET cabinettable;  // 定义一个实时数据库结构

		// 序号
		cabinettable.index = pCabinet->wSerialNo;
		// 名字
		strcpy( cabinettable.name, pCabinet->sName);

		// 初始化柜页数
		if (!InitPageTable(pCabinet, &cabinettable, NULL, 1))
		{
			print((char *)"InitPageTable error");
			return FALSE;
		}


		// 初始化回路表
		if( !InitCircuitTable( pCabinet, &cabinettable))
		{
			print((char *)"InitCircuitTable error");
			return FALSE;
		}

		m_vectorCabinetTable.push_back( cabinettable );
	}

	print( (char *)"InitCabinetTable ok" );
	return TRUE;
}   /*-------- end class CPage_DataQuery method InitCabinetTable -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:InitCircuitTable
 * 功能描述:初始化回路表 InitDataTable调用
 * 参数:PCABINET pCabinet  柜数据指针
 * 参数:QUERY_REALTIME_CABINET *pcabinetable 本地要存的柜结构体
 * 返回值:BOOL
 ******************************************************************************/
BOOL CPage_DataQuery::InitCircuitTable( PCABINET pCabinet,
		QUERY_REALTIME_CABINET *pcabinetable)
{
	int iCircuitNum = pCabinet->circuitArray.size();

	// 回路数量
	pcabinetable->circuitnum = iCircuitNum;

	QSCIRCUIT_MAP_ITOR iter;

	// for (iter = pCabinet->circuitArray.begin(); iter != pCabinet->circuitArray.end(); iter++)
	// {
	//  PSINGLECIRCUIT pCircuit = iter.value();
	//  if (NULL == pCircuit )
	//  {
	//      char buf[128];

	//      sprintf( buf, "InitPageTable find cabinet%d NULL\n", pcabinetable->index );
	//      print( buf );

	//      return FALSE;
	//  }
	// 初始化每条回路
	for (iter = pCabinet->circuitArray.begin(); iter != pCabinet->circuitArray.end(); iter++)
		// for(int i=0; i<iCircuitNum; i++)
	{
		// PSINGLECIRCUIT pCircuit = pCabinet->circuitArray[i];
		PSINGLECIRCUIT pCircuit = iter.value();
		QUERY_REALTIME_CIRCUIT circuittbale;

		if ( NULL == pCircuit )
		{
			char buf[128];

			sprintf(buf, "InitCircuitTable:: ");
			print( buf );

			return FALSE;
		}

		circuittbale.index = pCircuit->wSerialNo; // 序号
		strcpy(circuittbale.name, pCircuit->sName);  // 名字

		// 初始化回路页表
		if( !InitPageTable( pCabinet, pcabinetable, &circuittbale, 0))
		{
			print( (char *)"InitPageTable error");
			return FALSE;
		}

		pcabinetable->circuit.push_back( circuittbale ) ;
	}

	print( (char *)"InitCircuitTable ok" );
	return TRUE;
}   /*-------- end class CPage_DataQuery method InitCircuitTable -------- */


/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:InitPageTable
 * 功能描述:初始化页数表
 * 参数:PCABINET pCabinet  柜数据指针
 * 参数:QUERY_REALTIME_CABINET *pcabinetable 本地要存的柜结构体
 * 参数:QUERY_REALTIME_CIRCUIT *pcircuitable 本地要存的回路结构体 当初始化柜页数时为NULL
 * 参数:BYTE flag 当flag为1时为柜页数，当flag为0时为回路页数
 * 返回值:BOOL
 ******************************************************************************/
BOOL CPage_DataQuery::InitPageTable( PCABINET pCabinet,
		QUERY_REALTIME_CABINET *pcabinetable,
		QUERY_REALTIME_CIRCUIT *pcircuitable,
		BYTE flag )
{
	if( NULL == pcabinetable )
	{
		print( (char *)"InitPageTable: NULL == pcabinetable " );
		return FALSE;
	}
	// 初始化回路页数表
	if( 0 == flag)
	{
		if ( NULL == pcircuitable )
		{
			print( (char *)"InitPageTable: NULL == pcircuitable " );
			return FALSE;
		}

		PSINGLECIRCUIT pCircuit = AfxGetSingleCircuit( pcircuitable->index );
		if (NULL == pCircuit)
		{
			char buf[128];

			sprintf(buf, "InitPageTable find circuit%d NULL\n", pcircuitable->index);
			print( buf );

			return FALSE;
		}

		// 初始化页数据
		InitPageTableData(pCircuit, &pcircuitable->page,flag);

		pcircuitable->pagenum = pcircuitable->page.size();
		// printf("InitPageTable::circuit%d pagenum=%d\n", pcircuitable->index, pcircuitable->pagenum);
	}
	else if ( 1 == flag)  // 初始化柜页数表
	{
		QSCIRCUIT_MAP_ITOR iter;

		for (iter = pCabinet->circuitArray.begin(); iter != pCabinet->circuitArray.end(); iter++)
		{
			PSINGLECIRCUIT pCircuit = iter.value();
			if (NULL == pCircuit )
			{
				char buf[128];

				sprintf( buf, "InitPageTable find cabinet%d NULL\n", pcabinetable->index );
				print( buf );

				return FALSE;
			}

			// 初始化页数据
			if( !InitPageTableData(pCircuit, &pcabinetable->page, flag))
			{
				print( (char *)"InitPageTableData error");
				return FALSE;
			}

		}



		// int iCircuitNum = pCabinet->circuitArray.size();
		// 初始化每条回路
		// for(int i=0; i<iCircuitNum; i++)
		// {
		//     PSINGLECIRCUIT pCircuit = pCabinet->circuitArray[i];
		//     if (NULL == pCircuit )
		//     {
		//      char buf[128];

		//         sprintf( buf, "InitPageTable find cabinet%d i=%d NULL\n", pcabinetable->index , i);
		//      print( buf );

		//      return FALSE;
		//     }

		//     // 初始化页数据
		//     InitPageTableData(pCircuit, &pcabinetable->page, flag);

		// }

		pcabinetable->pagenum = pcabinetable->page.size();
		// printf("InitPageTable::cabinet%d pagenum=%d\n",
		//        pcabinetable->index,
		//        pcabinetable->pagenum);
	}

	print( (char *)"InitPageTable ok" );
	return TRUE;
}   /*-------- end class CPage_DataQuery method InitPageTable -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:InitPageTableData
 * 功能描述:初始化页数据表数据
 * 参数: PSINGLECIRCUIT pCircuit,  // 回路指针
 QVector<QUERY_REALTIME_PAGE> *ppage  // 页指针
 BYTE flag 当flag为1时为柜页数，当flag为0时为回路页数
 * 返回值:BOOL
 ******************************************************************************/
BOOL CPage_DataQuery::InitPageTableData( PSINGLECIRCUIT pCircuit, QVector<QUERY_REALTIME_PAGE> *ppage, BYTE flag)
{
	int ycsum = 0;
	int yxsum = 0;
	int ymsum = 0;
	int sum;
	QUERY_REALTIME_PAGE page;
	page.datanum = 0;


	if ( 1 == flag )
	{
		if( !ppage->isEmpty()  )
		{
			page.datanum = ppage->last().datanum;
			if ( page.datanum < 14 )
			{
				page = ppage->last();
				ppage->remove( ppage->size() - 1);
			}
		}
	}

	// 加入yc数据
	ycsum = pCircuit->ycArray.size();
	yxsum = pCircuit->yxArray.size();
	ymsum = pCircuit->ymArray.size();
	sum = ycsum + yxsum + ymsum;

	for (int i=0; i<sum; i++)
	{
		QUERY_REALTIME_DATA data;
		QString strBusNo ;
		QString strAddr ;
		QString strPntNo ;
		QString strName;

		data.index = i;  // 序号
		// data.point = i;                  // 本指针点号

		if ( i < ycsum )
		{
			PYCST pyc = (PYCST)pCircuit->ycArray.at( i );
			strBusNo = QString::number(pyc->wBusNo + 1, 10);
			strAddr = QString::number(pyc->wAddr, 10);
			strPntNo = QString::number(pyc->wPntNo + 1, 10);
			// strName = UTF8("遥测")+ QString::number(i+1, 10) + UTF8(":") + UTF8(pyc->chName);
			strName =  UTF8(pyc->chName);
			data.type = DataType::YC_DTYPE;  // 类型
			data.busline = pyc->wBusNo;      // 总线
			data.addr = pyc->wAddr;          // 地址
			data.point = pyc->wPntNo;        // 点号
		}
		if ( i>=ycsum && i < ycsum + yxsum )
		{
			PYXST pyx = (PYXST)pCircuit->yxArray.at( i - ycsum );
			strBusNo = QString::number(pyx->wBusNo+1, 10);
			strAddr = QString::number(pyx->wAddr, 10);
			strPntNo = QString::number(pyx->wPntNo+1, 10);
			// strName = UTF8("遥信")+ QString::number(i+1-ycsum, 10) + UTF8(":") + UTF8(pyx->chName);
			strName =  UTF8(pyx->chName);
			data.type = DataType::YX_DTYPE;  // 类型
			data.busline = pyx->wBusNo;      // 总线
			data.addr = pyx->wAddr;          // 地址
			data.point = pyx->wPntNo;        // 点号
		}
		if ( i>=ycsum+yxsum  )
		{
			PYMST pym = (PYMST)pCircuit->ymArray.at( i - ycsum - yxsum );
			strBusNo = QString::number(pym->wBusNo+1, 10);
			strAddr = QString::number(pym->wAddr, 10);
			strPntNo = QString::number(pym->wPntNo+1, 10);
			// strName = UTF8("遥脉")+ QString::number(i+1-ycsum-yxsum, 10) + UTF8(":") + UTF8(pym->chName);
			strName = UTF8(pym->chName);
			data.type = DataType::YM_DTYPE;  // 类型
			data.busline = pym->wBusNo;      // 总线
			data.addr = pym->wAddr;          // 地址
			data.point = pym->wPntNo;        // 点号
		}

		// 位置信息
		data.pos = UTF8("P")
			+ strBusNo
			+ UTF8(",")
			+ strAddr
			+ UTF8(",")
			+ strPntNo;

		// 内容信息
		if( 1 == flag )
		{
			// data.content = UTF8("回路")
			//                + QString::number(pCircuit->wSerialNo + 1)
			//                + strName;
			data.content = pCircuit->sName
				+ strName;
		}

		if( 0 == flag )
		{
			data.content = strName;
		}

		page.data.push_back(data);
		page.datanum ++;
		if ( ONE_PAGE_DATANUM == page.datanum )
		{
			ppage->push_back( page );
			page.datanum = 0;
			page.data.clear();
		}

	}

	// 增加上没有到一页数量的
	if( page.datanum != 0)
	{
		ppage->push_back( page );
	}

	print( (char *)"InitPageTableData ok" );
	return TRUE;
}   /*-------- end class CPage_DataQuery method InitPageTableData -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:InitUiTreeView
 * 功能描述:初始化实时数据查询树
 * 参数:void
 * 返回值:BOOL
 ******************************************************************************/
BOOL CPage_DataQuery::InitUiTreeView(void)
{
	// 定义部分数据字体
	QFont font;
	font.setPixelSize( 20 );

	// 定义树的基本属性
	m_treeRealTimeData = new QTreeView( (QWidget *)AfxGetWidget());
	m_treeRealTimeData->setGeometry(35, 135, 114, 385);
	m_treeRealTimeData->setFont( font );

	//定义树结构
	m_treeRealTimeData->setHeaderHidden(true);
	m_treeRealTimeData->header()->setResizeMode( QHeaderView::ResizeToContents );
	m_treeRealTimeData->header()->setStretchLastSection( false );

	//定义模块
	m_pModel = new QStandardItemModel( m_treeRealTimeData  );

	//模块头
	m_pModel->setHorizontalHeaderLabels(QStringList()<<QString(UTF8( "实时数据" )));

	// 根据配置初始化树
	BOOL bRtn = InitUiTreeViewbyCfg( m_pModel);

	//设置模块
	m_treeRealTimeData->setModel(m_pModel);
	m_treeRealTimeData->hide(  );
	// m_treeRealTimeData->show(  );

	connect( m_treeRealTimeData,
			SIGNAL( clicked( QModelIndex  ) ),
			this,
			SLOT( TreeItemChange( QModelIndex ) ) );

	if( bRtn )
	{
		TreeItemChange( m_pModel->index( 0, 0, QModelIndex(  ) ));
	}
	print( (char *)"InitUiTreeView ok" );
	return TRUE;

}   /*-------- end class CPage_DataQuery method InitUiTreeView -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DeleteUiTreeView
 * 功能描述:删除实时数据查询树
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DeleteUiTreeView(void)
{
	int iModelRowCount = m_pModel->rowCount( );

	// 删除树下的每一项数据
	for (int i=iModelRowCount-1; i>=0; i--)
	{
		QStandardItem *pItemVec = m_pModel->item(i, 0);
		if (NULL == pItemVec)
		{
			continue;
		}
		int iItemRowCount = pItemVec->rowCount();

		for (int j = iItemRowCount-1; j>=0; j--)
		{
			QStandardItem *pItemC = pItemVec->child(j, 0);
			if( NULL != pItemC )
			{
				delete pItemC;
				pItemC              = NULL;
			}

		}

		if( NULL != pItemVec )
		{
			delete pItemVec;
			pItemVec            = NULL;
		}
	}

	// 删除模块
	if( NULL != m_pModel )
	{
		delete m_pModel;
		m_pModel        = NULL;
	}

	// 删除树
	if( NULL != m_treeRealTimeData )
	{
		delete m_treeRealTimeData;
		m_treeRealTimeData      = NULL;
	}


	print( (char *)"DeleteUiTreeView ok" );
}   /*-------- end class CPage_DataQuery method DeleteUiTreeView -------- */


/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:InitUiTreeViewbyCfg
 * 功能描述:根据配置初始化树
 * 参数: QStandardItemModel *pModle
 * 返回值:BOOL
 ******************************************************************************/
BOOL CPage_DataQuery::InitUiTreeViewbyCfg( QStandardItemModel *pModel )
{
	// 获得柜的数量
	int iCabinetNum = m_vectorCabinetTable.size();
	if ( 0 == iCabinetNum )
	{
		return FALSE;
	}

	//初始化柜
	for( int i=0; i<iCabinetNum; i++ )
	{
		// 柜名字
		QString strCabinetName = UTF8( m_vectorCabinetTable[i].name );
		QStandardItem* pItemVec = new QStandardItem( strCabinetName );

		// 不可编缉
		pItemVec->setEditable( false );
		// 加入行
		pModel->appendRow( pItemVec  );


		for( int j=0; j< m_vectorCabinetTable[i].circuitnum; j++ )
		{
			QVector<QUERY_REALTIME_CIRCUIT> circuit = m_vectorCabinetTable[i].circuit;

			// 名字
			QString str = UTF8( circuit[j].name );
			QStandardItem* pItemC = new QStandardItem(str);

			// 不可编缉
			pItemC->setEditable( false );
			// 加入行
			pItemVec->appendRow(pItemC);
		}
	}

	print( (char *)"InitUiTreeViewbyCfg ok" );
	return TRUE;
}   /*-------- end class CPage_DataQuery method InitUiTreeViewbyCfg -------- */


/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:InitUiSeoToolBox
 * 功能描述:初始化SOE抽屉
 * 参数:void
 * 返回值:BOOL
 ******************************************************************************/
BOOL CPage_DataQuery::InitUiSoeToolBox(void)
{
	// 定义抽屉
	m_toolBoxSoe = new QToolBox( (QWidget *)AfxGetWidget());
	// 设置位置
	m_toolBoxSoe->setGeometry(35, 135, 112, 383);

	// 初始化按时间查询
	InitUiSoeToolBoxByTime();
	m_toolBoxSoe->addItem( m_widgetSoeBytime, UTF8("按时间查询"));

	/* 目前这个还没有完成，第一个版本不用这个，先不做*/
	// 按总线查询
	// InitUiSoeToolBoxByBusLine();
	// m_toolBoxSoe->addItem(m_widgetSoeByBusLine, UTF8("按总线查询"));
	// m_toolBoxSoe->hide();
	// m_toolBoxSoe->show();

	print( (char *)"InitUiSoeToolBox ok" );
	return TRUE;

}   /*-------- end class CPage_DataQuery method InitUiSoeToolBox -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DeleteUiSoeToolBox
 * 功能描述:删除抽屉
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DeleteUiSoeToolBox(void)
{
	// 删除总线查询
	DeleteUiSoeToolBoxByBusLine();

	// 删除时间查询
	DeleteUiSoeToolBoxByTime();

	if( NULL != m_toolBoxSoe )
	{
		delete m_toolBoxSoe;
		m_toolBoxSoe        = NULL;
	}

	print( (char *)"DeleteUiSoeToolBox ok" );
}   /*-------- end class CPage_DataQuery method DeleteUiSoeToolBox -------- */




/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:InitUiSoeToolBoxByTime
 * 功能描述:初始化按时间查询
 * 参数:void
 * 返回值:BOOL
 ******************************************************************************/
BOOL CPage_DataQuery::InitUiSoeToolBoxByTime(void)
{
	// 定义部分数据字体
	QFont font;
	font.setPixelSize( 35 );

	// 定义按时间查询的窗口
	m_widgetSoeBytime = new QWidget( (QWidget *)AfxGetWidget());

	//定义起始时间窗口
	m_dialogTimeBegin = new QDialog( (QWidget *)AfxGetWidget() );
	m_dialogTimeBegin->setFixedSize( QSize(540, 110 ));
	m_dialogTimeBegin->setWindowTitle( UTF8("起始时间设置"));
	m_dialogTimeBegin->setWindowFlags( Qt::WindowCloseButtonHint
			| Qt::Window );

	// 起始时间控件
	m_dtBegin = new QDateTimeEdit( m_dialogTimeBegin );
	m_dtBegin->setGeometry(10, 10, 520, 100);
	m_dtBegin->setDisplayFormat("yyyy年 MM月 dd日 hh时 mm分 ss秒");
	m_dtBegin->setFont( font );
	connect(m_dtBegin,SIGNAL(dateTimeChanged(const QDateTime)), this , SLOT(BeginTimeChanged(QDateTime )));
	m_dialogTimeBegin->hide();

	// 定义结束时间窗口
	m_dialogTimeEnd = new QDialog( (QWidget *)AfxGetWidget() );
	m_dialogTimeEnd->setFixedSize( QSize(540, 110 ));
	m_dialogTimeEnd->setWindowTitle( UTF8("结束时间设置"));
	m_dialogTimeEnd->setWindowFlags( Qt::WindowCloseButtonHint
			| Qt::Window );

	// 结束时间控件
	m_dtEnd = new QDateTimeEdit( m_dialogTimeEnd );
	m_dtEnd->setGeometry(10, 10, 520, 100);
	m_dtEnd->setDisplayFormat("yyyy年 MM月 dd日 hh时 mm分 ss秒");
	m_dtEnd->setFont( font );
	connect(m_dtEnd,SIGNAL(dateTimeChanged(QDateTime )), this , SLOT(EndTimeChanged(QDateTime)));
	// m_dtEnd->setDateTime(QDateTime::currentDateTime());
	m_dialogTimeEnd->hide();

	// 起始时间文字
	m_buttonTimeBegin = new QPushButton( m_widgetSoeBytime );
	m_buttonTimeBegin->setGeometry(5, 10, 60, 20);
	m_buttonTimeBegin->setText( UTF8("起始时间") );
	connect(m_buttonTimeBegin, SIGNAL(released()), m_dialogTimeBegin,SLOT(exec()));

	// m_labelTimeBegin = new QLabel(   UTF8("起始时间"), m_widgetSoeBytime);
	// m_labelTimeBegin->setGeometry(5, 20, 60, 20);


	// 起始年
	m_editBeginYear = new QLineEdit (m_widgetSoeBytime );
	m_editBeginYear->setGeometry(5, 30, 40, 20);
	m_editBeginYear->setText("2000");

	m_labelBeginYear = new QLabel( UTF8("年"), m_widgetSoeBytime);
	m_labelBeginYear->setGeometry(50,30,30,20);

	// 起始月
	m_editBeginMonth = new QLineEdit(m_widgetSoeBytime);
	m_editBeginMonth->setGeometry(5, 50, 40, 20);
	m_editBeginMonth->setText("10");

	m_labelBeginMonth = new QLabel( UTF8("月"), m_widgetSoeBytime);
	m_labelBeginMonth->setGeometry(50,50,30,20);

	// 起始日
	m_editBeginDay = new QLineEdit(m_widgetSoeBytime);
	m_editBeginDay->setGeometry(5, 70, 40, 20);
	m_editBeginDay->setText("10");

	m_labelBeginDay = new QLabel( UTF8("日"), m_widgetSoeBytime);
	m_labelBeginDay->setGeometry(50,70,30,20);

	// 起始时
	m_editBeginHour = new QLineEdit(m_widgetSoeBytime);
	m_editBeginHour->setGeometry(5, 90, 40, 20);
	m_editBeginHour->setText("10");

	m_labelBeginHour = new QLabel( UTF8("时"), m_widgetSoeBytime);
	m_labelBeginHour->setGeometry(50,90,30,20);

	// 起始分
	m_editBeginMinute = new QLineEdit(m_widgetSoeBytime);
	m_editBeginMinute->setGeometry(5, 110, 40, 20);
	m_editBeginMinute->setText("10");

	m_labelBeginMinute = new QLabel( UTF8("分"), m_widgetSoeBytime);
	m_labelBeginMinute->setGeometry(50,110,30,20);

	// 起始秒
	m_editBeginSecond = new QLineEdit(m_widgetSoeBytime);
	m_editBeginSecond->setGeometry(5, 130, 40, 20);
	m_editBeginSecond->setText("10");

	m_labelBeginSecond = new QLabel( UTF8("秒"), m_widgetSoeBytime);
	m_labelBeginSecond->setGeometry(50,130,30,20);

	// 结束时间文字
	m_buttonTimeEnd = new QPushButton( m_widgetSoeBytime );
	m_buttonTimeEnd->setGeometry(5, 150, 60, 20);
	m_buttonTimeEnd->setText( UTF8("结束时间") );
	connect(m_buttonTimeEnd, SIGNAL(released()), m_dialogTimeEnd,SLOT(exec()));
	// m_labelTimeEnd = new QLabel(   UTF8("结束时间"), m_widgetSoeBytime);
	// m_labelTimeEnd->setGeometry(5, 160, 60, 20);

	// 结束年
	m_editEndYear = new QLineEdit (m_widgetSoeBytime );
	m_editEndYear->setGeometry(5, 170, 40, 20);
	m_editEndYear->setText("2000");

	m_labelEndYear = new QLabel( UTF8("年"), m_widgetSoeBytime);
	m_labelEndYear->setGeometry(50,170,30,20);

	// 结束月
	m_editEndMonth = new QLineEdit(m_widgetSoeBytime);
	m_editEndMonth->setGeometry(5, 190, 40, 20);
	m_editEndMonth->setText("10");

	m_labelEndMonth = new QLabel( UTF8("月"), m_widgetSoeBytime);
	m_labelEndMonth->setGeometry(50,190,30,20);

	// 结束日
	m_editEndDay = new QLineEdit(m_widgetSoeBytime);
	m_editEndDay->setGeometry(5, 210, 40, 20);
	m_editEndDay->setText("10");

	m_labelEndDay = new QLabel( UTF8("日"), m_widgetSoeBytime);
	m_labelEndDay->setGeometry(50,210,30,20);

	// 结束时
	m_editEndHour = new QLineEdit(m_widgetSoeBytime);
	m_editEndHour->setGeometry(5, 230, 40, 20);
	m_editEndHour->setText("10");

	m_labelEndHour = new QLabel( UTF8("时"), m_widgetSoeBytime);
	m_labelEndHour->setGeometry(50,230,30,20);

	// 结束分
	m_editEndMinute = new QLineEdit(m_widgetSoeBytime);
	m_editEndMinute->setGeometry(5, 250, 40, 20);
	m_editEndMinute->setText("10");

	m_labelEndMinute = new QLabel( UTF8("分"), m_widgetSoeBytime);
	m_labelEndMinute->setGeometry(50,250,30,20);

	// 结束秒
	m_editEndSecond = new QLineEdit(m_widgetSoeBytime);
	m_editEndSecond->setGeometry(5, 270, 40, 20);
	m_editEndSecond->setText("10");

	m_labelEndSecond = new QLabel( UTF8("秒"), m_widgetSoeBytime);
	m_labelEndSecond->setGeometry(50,270,30,20);

	m_dtOk = new QPushButton( m_widgetSoeBytime);
	m_dtOk->setGeometry(5,300,70,20);
	m_dtOk->setText( UTF8("查询"));
	connect(m_dtOk,SIGNAL(released()),this,SLOT(SlotQuerySoeByTime()));

	m_dtBegin->setDateTime(QDateTime::currentDateTime());
	m_dtEnd->setDateTime(QDateTime::currentDateTime());
	print( (char *)"InitUiSoeToolBoxByTime ok" );
	return TRUE;
}   /*-------- end class CPage_DataQuery method InitUiSoeToolBoxByTime -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DeleteUiSoeToolBoxByTime
 * 功能描述:删除按时间查询
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DeleteUiSoeToolBoxByTime(void)
{
	// 删除查询确定按钮
	if( NULL != m_dtOk )
	{
		delete m_dtOk;
		m_dtOk      = NULL;
	}

	// 删除结束秒
	if( NULL != m_editEndSecond )
	{
		delete m_editEndSecond;
		m_editEndSecond     = NULL;
	}
	if( NULL != m_labelEndSecond )
	{
		delete m_labelEndSecond;
		m_labelEndSecond        = NULL;
	}

	// 删除结束分
	if( NULL != m_editEndMinute )
	{
		delete m_editEndMinute;
		m_editEndMinute     = NULL;
	}
	if( NULL != m_labelEndMinute )
	{
		delete m_labelEndMinute;
		m_labelEndMinute        = NULL;
	}

	// 删除结束时
	if( NULL != m_editEndHour )
	{
		delete m_editEndHour;
		m_editEndHour       = NULL;
	}
	if( NULL != m_labelEndHour )
	{
		delete m_labelEndHour;
		m_labelEndHour      = NULL;
	}

	// 删除结束日
	if( NULL != m_editEndDay )
	{
		delete m_editEndDay;
		m_editEndDay        = NULL;
	}
	if( NULL != m_labelEndDay )
	{
		delete m_labelEndDay;
		m_labelEndDay       = NULL;
	}

	// 删除结束月
	if( NULL != m_editEndMonth )
	{
		delete m_editEndMonth;
		m_editEndMonth      = NULL;
	}
	if( NULL != m_labelEndMonth )
	{
		delete m_labelEndMonth;
		m_labelEndMonth     = NULL;
	}

	// 删除结束年
	if( NULL != m_editEndYear )
	{
		delete m_editEndYear;
		m_editEndYear       = NULL;
	}
	if( NULL != m_labelEndYear )
	{
		delete m_labelEndYear;
		m_labelEndYear      = NULL;
	}

	// 删除开始秒
	if( NULL != m_editBeginSecond )
	{
		delete m_editBeginSecond;
		m_editBeginSecond       = NULL;
	}
	if( NULL != m_labelBeginSecond )
	{
		delete m_labelBeginSecond;
		m_labelBeginSecond      = NULL;
	}

	// 删除开始分
	if( NULL != m_editBeginMinute )
	{
		delete m_editBeginMinute;
		m_editBeginMinute       = NULL;
	}
	if( NULL != m_labelBeginMinute )
	{
		delete m_labelBeginMinute;
		m_labelBeginMinute      = NULL;
	}

	// 删除开始时
	if( NULL != m_editBeginHour )
	{
		delete m_editBeginHour;
		m_editBeginHour     = NULL;
	}
	if( NULL != m_labelBeginHour )
	{
		delete m_labelBeginHour;
		m_labelBeginHour        = NULL;
	}

	// 删除开始日
	if( NULL != m_editBeginDay )
	{
		delete m_editBeginDay;
		m_editBeginDay      = NULL;
	}
	if( NULL != m_labelBeginDay )
	{
		delete m_labelBeginDay;
		m_labelBeginDay     = NULL;
	}

	// 删除开始月
	if( NULL != m_editBeginMonth )
	{
		delete m_editBeginMonth;
		m_editBeginMonth        = NULL;
	}
	if( NULL != m_labelBeginMonth )
	{
		delete m_labelBeginMonth;
		m_labelBeginMonth       = NULL;
	}

	// 删除开始年
	if( NULL != m_editBeginYear )
	{
		delete m_editBeginYear;
		m_editBeginYear     = NULL;
	}
	if( NULL != m_labelBeginYear )
	{
		delete m_labelBeginYear;
		m_labelBeginYear        = NULL;
	}

	// 删除结束button
	if( NULL != m_buttonTimeEnd )
	{
		delete m_buttonTimeEnd;
		m_buttonTimeEnd     = NULL;
	}
	// 删除开始button
	if( NULL != m_buttonTimeBegin )
	{
		delete m_buttonTimeBegin;
		m_buttonTimeBegin       = NULL;
	}

	// 删除结束控件
	if( NULL != m_dtEnd )
	{
		delete m_dtEnd;
		m_dtEnd     = NULL;
	}
	// 删除开始时间控件
	if( NULL != m_dtBegin )
	{
		delete m_dtBegin;
		m_dtBegin       = NULL;
	}

	//删除结束窗口
	if( NULL != m_dialogTimeEnd )
	{
		delete m_dialogTimeEnd;
		m_dialogTimeEnd     = NULL;
	}
	// 删除起始控件
	if( NULL != m_dialogTimeBegin )
	{
		delete m_dialogTimeBegin;
		m_dialogTimeBegin       = NULL;
	}

	// 删除按时间查询
	if( NULL != m_widgetSoeBytime )
	{
		delete m_widgetSoeBytime;
		m_widgetSoeBytime       = NULL;
	}

	print( (char *)"DeleteUiSoeToolBoxByTime");

}   /*-------- end class CPage_DataQuery method DeleteUiSoeToolBoxByTime -------- */


/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:InitUiSoeToolBoxByBusLine
 * 功能描述:初始化按总线查询
 * 参数:void
 * 返回值:BOOL
 ******************************************************************************/
BOOL CPage_DataQuery::InitUiSoeToolBoxByBusLine(void)
{
	// 定义按总线查询窗口
	m_widgetSoeByBusLine = new QWidget( (QWidget *) AfxGetWidget());

	// 总线
	m_labelBusLine = new QLabel( UTF8("通道号"), m_widgetSoeByBusLine );
	m_labelBusLine->setGeometry(5, 20, 70, 20 );

	m_boxBusLine = new QComboBox( m_widgetSoeByBusLine );
	m_boxBusLine->setGeometry(5, 40, 70, 20);

	// 增加相应的总线内容
	QBUS_ARRAY *pBusArray = AfxGetBusArray();
	if ( NULL == pBusArray )
	{
		return FALSE;
	}

	// 一共有多少个总线
	DWORD busSize = pBusArray->size();
	// 遍历每条总线，进行添加
	for( DWORD i=0; i<busSize; i++)
	{
		QBUS_ARRAY::iterator itor =  pBusArray->find( i ) ;
		if( itor == pBusArray->end() )
		{
			break;
		}

		PBUS_LINEST pBusLine = itor.value() ;
		// 找出使用中的总线
		if ( BUS_PAUSE != pBusLine->byBusType )
		{
			m_boxBusLine->addItem( QString::number( pBusLine->wBusNo + 1) );
		}

	}
	// m_boxBusLine->addItem("1" );
	// m_boxBusLine->addItem("2" );

	// 地址
	m_labelAddr = new QLabel( UTF8("设备地址"), m_widgetSoeByBusLine );
	m_labelAddr->setGeometry(5, 60, 70, 20 );

	m_boxAddr = new QComboBox( m_widgetSoeByBusLine );
	m_boxAddr->setGeometry(5, 80, 70, 20);
	m_boxAddr->addItem("1" );
	m_boxAddr->addItem("2" );

	// 点号
	m_labelPoint = new QLabel( UTF8("点号"), m_widgetSoeByBusLine );
	m_labelPoint->setGeometry(5, 100, 70, 20 );

	m_boxPoint = new QComboBox( m_widgetSoeByBusLine );
	m_boxPoint->setGeometry(5, 120, 70, 20);
	m_boxPoint->addItem("1" );
	m_boxPoint->addItem("2" );

	m_BusLineOk = new QPushButton( m_widgetSoeByBusLine );
	m_BusLineOk->setGeometry(5, 150, 70, 20);
	m_BusLineOk->setText(UTF8("查询"));

	print( (char *)"InitUiSoeToolBoxByTime ok" );
	return TRUE;
}   /*-------- end class CPage_DataQuery method InitUiSoeToolBoxByBusLine -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DeleteUiSoeToolBoxByBusLine
 * 功能描述:删除按总线查询数据
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DeleteUiSoeToolBoxByBusLine(void)
{
	// 删除查询按钮
	if( NULL != m_BusLineOk )
	{
		delete m_BusLineOk;
		m_BusLineOk     = NULL;
	}

	// 删除点号数据
	if( NULL != m_boxPoint )
	{
		m_boxPoint->clear();
		delete m_boxPoint;
		m_boxPoint      = NULL;
	}
	if( NULL != m_labelPoint )
	{
		delete m_labelPoint;
		m_labelPoint        = NULL;
	}

	// 删除地址
	if( NULL != m_boxAddr )
	{
		m_boxAddr->clear();
		delete m_boxAddr;
		m_boxAddr       = NULL;
	}
	if( NULL != m_labelAddr )
	{
		delete m_labelAddr;
		m_labelAddr     = NULL;
	}

	// 删除总线
	if( NULL != m_boxBusLine )
	{
		m_boxBusLine->clear();
		delete m_boxBusLine;
		m_boxBusLine        = NULL;
	}
	if( NULL != m_labelBusLine )
	{
		delete m_labelBusLine;
		m_labelBusLine      = NULL;
	}

	// 删除按总线查询数据窗口
	if( NULL != m_widgetSoeByBusLine )
	{
		delete m_widgetSoeByBusLine;
		m_widgetSoeByBusLine        = NULL;
	}

	print( (char *)"DeleteUiSoeToolBoxByBusLine ok");

}   /*-------- end class CPage_DataQuery method DeleteUiSoeToolBoxByBusLine -------- */



/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:SetTipsStr
 * 功能描述:设置提示信息
 * 参数: QString str
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::SetTipsStr( QString str)
{
	m_strTips = str;
}   /*-------- end class CPage_DataQuery method SetTipsStr -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:GetTipsStr
 * 功能描述:获得提示信息
 * 参数:void
 * 返回值:QString
 ******************************************************************************/
QString CPage_DataQuery::GetTipsStr(void)
{
	return m_strTips;
}   /*-------- end class CPage_DataQuery method GetTipsStr -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DrawTips
 * 功能描述:画提示信息
 * 参数: QPainter *pPainter
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DrawTips( QPainter *pPainter )
{
	pPainter->save();

	QFont font ;
	font.setPixelSize( 23 );
	pPainter->setFont(font);
	QPen pen( QColor( 255 , 255 , 255 , 255 ) );
	pPainter->setPen( pen );

	if( DATAQUERY_REALDATA == GetQueryType( ) )
	{
		pPainter->drawText( m_rcToolTips , Qt::AlignLeft , GetTipsStr( ));
	}
	else
	{
		pPainter->drawText( m_rcToolTips , Qt::AlignLeft , QString(""));
	}

	pPainter->restore();

}   /*-------- end class CPage_DataQuery method DrawTips -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:SetCurrentPage
 * 功能描述:画当前页数
 * 参数: DWORD dwCurrentPage
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::SetCurrentPage( DWORD dwCurrentPage)
{
	if ( DATAQUERY_SOE == GetQueryType() )
	{
		m_dwSoeCurrentPage = dwCurrentPage;
		SetDisplaySoe();
	}
	if ( DATAQUERY_YXALARM == GetQueryType() )
	{
		m_dwYxAlarmCurrentPage = dwCurrentPage;
		SetDisplaySoe();
	}
	if ( DATAQUERY_REALDATA == GetQueryType() )
	{
		m_dwCurrentPage = dwCurrentPage;
		// 设置树数据
		SetTreeItemData();
	}

}   /*-------- end class CPage_DataQuery method SetCurrentPage -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:GetCurrentPage
 * 功能描述:获得当前页数
 * 参数:void
 * 返回值:DWORD  页数返回值
 ******************************************************************************/
DWORD CPage_DataQuery::GetCurrentPage(void)
{
	if ( DATAQUERY_SOE == GetQueryType() )
	{
		return m_dwSoeCurrentPage;
	}
	if ( DATAQUERY_YXALARM == GetQueryType() )
	{
		return m_dwYxAlarmCurrentPage;
	}
	if ( DATAQUERY_REALDATA == GetQueryType() )
	{
		return m_dwCurrentPage;
	}

	return 0;
}   /*-------- end class CPage_DataQuery method GetCurrentPage -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:SetTotalPage
 * 功能描述:设置总页数
 * 参数: DWORD dwTotalPage  要设置的页数值
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::SetTotalPage( DWORD dwTotalPage )
{
	if ( DATAQUERY_SOE == GetQueryType() )
	{
		m_dwSoeTotalPage = dwTotalPage;
	}
	if ( DATAQUERY_YXALARM == GetQueryType() )
	{
		m_dwYxAlarmTotalPage = dwTotalPage;
	}
	if ( DATAQUERY_REALDATA == GetQueryType() )
	{
		m_dwTotalPage = dwTotalPage;
	}
}   /*-------- end class CPage_DataQuery method SetTotalPage -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:GetTotalPage
 * 功能描述:获取总页数
 * 参数:void
 * 返回值:DWORD 返回总页数值
 ******************************************************************************/
DWORD CPage_DataQuery::GetTotalPage(void)
{
	if ( DATAQUERY_SOE == GetQueryType() )
	{
		return m_dwSoeTotalPage;
	}
	if ( DATAQUERY_YXALARM == GetQueryType() )
	{
		return m_dwYxAlarmTotalPage;
	}
	if ( DATAQUERY_REALDATA == GetQueryType() )
	{
		return m_dwTotalPage;
	}

	return 0;
}   /*-------- end class CPage_DataQuery method GetTotalPage -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DrawPages
 * 功能描述:画页数信息
 * 参数:QPainter *pPainter  画笔
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DrawPages(QPainter *pPainter )
{
	DWORD cur_page = GetCurrentPage();
	DWORD total_page = GetTotalPage();
	static bool page_error = false;

	if ( 0 == cur_page || 0 == total_page )
	{
		return;
	}
	if (cur_page > total_page && !page_error)
	{
		page_error = true;
		return;
	}

	if ( 0 == cur_page )
	{
		cur_page = 1;
		SetCurrentPage(1);
	}

	page_error = false;
	QString str = QString::number(cur_page, 10)  + "/" + QString::number(total_page, 10) + UTF8( "页");

	pPainter->save();

	QFont font ;
	font.setPixelSize( 23 );
	pPainter->setFont(font);
	QPen pen( QColor( 255 , 255 , 255 , 255 ) );
	pPainter->setPen( pen );

	pPainter->drawText( m_rcPageInfo , Qt::AlignLeft , str );

	pPainter->restore();

}   /*-------- end class CPage_DataQuery method DrawPages -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:PageUp
 * 功能描述:页数增加
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::PageUp(void)
{
	DWORD page = GetCurrentPage();

	page ++;
	if ( page > GetTotalPage() )
	{
		page = 1;
	}

	SetCurrentPage( page );
}   /*-------- end class CPage_DataQuery method PageUp -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:PageDown
 * 功能描述:页数减少
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::PageDown(void)
{
	DWORD page = GetCurrentPage();
	page--;

	if ( page < 1 )
	{
		page = GetTotalPage();
	}

	SetCurrentPage( page );
}   /*-------- end class CPage_DataQuery method PageDown -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:SetQueryType
 * 功能描述:设置查询类型
 * 参数: BYTE type  目前仅有实时数据(DATAQUERY_REALTIME)和SOE(DATAQUERY_SOE)两种类型
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::SetQueryType( BYTE type )
{
	m_byCurrentQueryType = type;
	if ( DATAQUERY_YXALARM ==  type)
	{
		// SetCurrentPage(1);
		// SetTotalPage(1);
		// AfxRefreshScreen();
		QueryAllSoe();
	}
	if ( DATAQUERY_SOE ==  type)
	{
		QueryAllSoe();
	}
	if ( DATAQUERY_REALDATA == type )
	{

	}
}   /*-------- end class CPage_DataQuery method SetQueryType -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:GetQueryType
 * 功能描述:获取查询类型
 * 参数:void
 * 返回值:BYTE 目前仅有实时数据(DATAQUERY_REALTIME)和SOE(DATAQUERY_SOE)两种类型
 ******************************************************************************/
BYTE CPage_DataQuery::GetQueryType(void)
{
	return m_byCurrentQueryType;
}   /*-------- end class CPage_DataQuery method GetQueryType -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DrawQueryTypePicsNone
 * 功能描述:画所有查询类型图片不显示
 * 参数: QPainter *pPainter  画笔
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DrawQueryTypePicsNone( QPainter *pPainter )
{
	DrawQueryTypePic(pPainter, DATAQUERY_REALDATA, FALSE);
	DrawQueryTypePic(pPainter, DATAQUERY_SOE, FALSE);
	DrawQueryTypePic(pPainter, DATAQUERY_YXALARM, FALSE);

}   /*-------- end class CPage_DataQuery method DrawQueryTypePicsNone -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DrawQueryTypePic
 * 功能描述:画相应的查询类型的图片
 * 参数: QPainter *pPainter, BYTE type, BOOL flag
 * pPainter 画笔
 * type 查询类型 目前仅有实时数据(DATAQUERY_REALTIME)和SOE(DATAQUERY_SOE)两种类型
 * flag 是选中还是未选中的标识位
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DrawQueryTypePic( QPainter *pPainter, BYTE type, BOOL flag)
{
	int size = 0;
	QString str = "";
	QPoint pt(0, 0);

	if( NULL == pPainter )
	{
		return ;
	}

	switch (type)
	{
		case DATAQUERY_REALDATA: // 实时数据
			{
				size = m_strPicRealTimeNoneSelect.size() ;
				str = m_strPicRealTimeNoneSelect;
				if ( flag )
				{
					size = m_strPicRealTimeSelect.size() ;
					str = m_strPicRealTimeSelect;
				}

				pt.setX(m_rcFunction[QUERYREALDATA].x());
				pt.setY( m_rcFunction[QUERYREALDATA].y());

			}
			break;

		case DATAQUERY_SOE://soe 数据
			{
				size = m_strPicSoeNoneSelect.size() ;
				str = m_strPicSoeNoneSelect;
				if ( flag )
				{
					size = m_strPicSoeSelect.size() ;
					str = m_strPicSoeSelect;
				}

				pt.setX(m_rcFunction[QUERYSOE].x());
				pt.setY( m_rcFunction[QUERYSOE].y());

			}
			break;

		case DATAQUERY_YXALARM:// YX报警
			{
				size = m_strPicYxAlarmNoneSelect.size() ;
				str = m_strPicYxAlarmNoneSelect;
				if ( flag )
				{
					size = m_strPicYxAlarmSelect.size() ;
					str = m_strPicYxAlarmSelect;
				}

				pt.setX(m_rcFunction[QUERYYXALARM].x());
				pt.setY( m_rcFunction[QUERYYXALARM].y());

			}
			break;
		default:
			break;
	}

	if( size <= 0 )
	{
		return;
	}
	QPixmap pix;

	bool bFlag = pix.load( str );
	if( bFlag ==  false )
	{
		return;
	}
	pPainter->drawPixmap( pt , pix );

	pix.detach();

}   /*-------- end class CPage_DataQuery method DrawQueryTypePic -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DrawQueryTypePics
 * 功能描述:画查询类型图片 供画图那边直接掉用
 * 参数:QPainter *pPainter 画笔
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DrawQueryTypePics( QPainter *pPainter)
{
	DrawQueryTypePicsNone( pPainter );
	DrawQueryTypePic(pPainter, GetQueryType(), TRUE);
}   /*-------- end class CPage_DataQuery method DrawQueryTypePics -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:SetQueryData
 * 功能描述:设置数据
 * 参数:BYTE index 数据序号
 QUERY_REALTIME_DATA data 数据
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::SetQueryData(BYTE index, QUERY_REALTIME_DATA data)
{
	if( index >= ONE_PAGE_DATANUM )
	{
		return;
	}

	m_byCurrentDataNum = index + 1;


	// qDebug() << data.val;
	m_CurrentData[index].index = data.index;
	m_CurrentData[index].content = data.content;
	m_CurrentData[index].type = data.type;
	m_CurrentData[index].busline = data.busline;
	m_CurrentData[index].addr = data.addr;
	m_CurrentData[index].point = data.point;
	m_CurrentData[index].val = data.val;
	m_CurrentData[index].pos = data.pos;
	// m_CurrentData[index].val.clear();
	// qDebug() << m_CurrentData[index].val;
	// memcpy( &m_CurrentData[index], &data, sizeof(QUERY_REALTIME_DATA) );
	// qDebug() << m_CurrentData[index].val;
	// printf("cur = %d %d %d %d %d %d\n", index,m_CurrentData[index].index, m_CurrentData[index].type, m_CurrentData[index].busline,m_CurrentData[index].addr, m_CurrentData[index].point);

}   /*-------- end class CPage_DataQuery method SetQueryData -------- */


/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:GetQueryData
 * 功能描述:获取某行数据
 * 参数: BYTE index 行
 * 返回值:QUERY_REALTIME_DATA * 返回数据指针
 ******************************************************************************/
QUERY_REALTIME_DATA *CPage_DataQuery::GetQueryData( BYTE index)
{
	if( index >= m_byCurrentDataNum )
	{
		return NULL;
	}

	if( NULL == &m_CurrentData[index])
	{
		return NULL;
	}
	// printf("pyx->byValue index=%d\n", index);

	switch ( m_CurrentData[index].type)
	{
		case DataType::YC_DTYPE: // 遥测类型
			{
				PYCST pyc = AfxGetYc( m_CurrentData[index].busline,
						m_CurrentData[index].addr,
						m_CurrentData[index].point);

				if ( NULL == pyc )
				{
					return NULL;
				}
				m_CurrentData[index].val = QString("%1").arg(pyc->fVal_1Time, 0, 'f', 3);
			}
			break;

		case DataType::YX_DTYPE://遥信
			{
				PYXST pyx = AfxGetYx( m_CurrentData[index].busline,
						m_CurrentData[index].addr,
						m_CurrentData[index].point);
				if (NULL == pyx)
				{
					return NULL;
				}

				// m_CurrentData[index].val = QString("%1").arg(pyx->byValue);
				if ( 1 == pyx->byValue )
				{
					m_CurrentData[index].val = UTF8("合");
				}
				else
				{
					m_CurrentData[index].val = UTF8("分");
				}
			}
			break;


		case DataType::YM_DTYPE://遥脉
			{
				PYMST pym = AfxGetYm( m_CurrentData[index].busline,
						m_CurrentData[index].addr,
						m_CurrentData[index].point);
				if (NULL == pym)
				{
					return NULL;
				}
				m_CurrentData[index].val = QString("%1").arg((DWORD)pym->dVal_1Time, 0, 'f', 0);

			}
			break;
		default:
			break;
	}

	return &m_CurrentData[index] ;
}   /*-------- end class CPage_DataQuery method GetQueryData -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:SetSoeData
 * 功能描述: 设置SOE数据
 * 参数: BYTE index, SOEDATA data
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::SetSoeData( BYTE index, SOEDATA data)
{
	if( index >= ONE_PAGE_DATANUM )
	{
		return;
	}

	m_byCurrentSoeNum = index + 1;

	m_CurrentSoe[index].BusNo = data.BusNo;
	m_CurrentSoe[index].DevNo = data.DevNo;
	m_CurrentSoe[index].DataType = data.DataType;
	strcpy(m_CurrentSoe[index].Location,data.Location);
	strcpy(m_CurrentSoe[index].Content,data.Content);
	strcpy(m_CurrentSoe[index].DateTime,data.DateTime);

}   /*-------- end class CPage_DataQuery method SetSoeData -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:GetSoeData
 * 功能描述:
 * 参数: BYTE index
 * 返回值:SOEDATA *
 ******************************************************************************/
SOEDATA * CPage_DataQuery::GetSoeData( BYTE index)
{
	if( index >= m_byCurrentSoeNum )
	{
		return NULL;
	}

	if( NULL == &m_CurrentSoe[index])
	{
		return NULL;
	}

	return &m_CurrentSoe[index];
}   /*-------- end class CPage_DataQuery method GetSoeData -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DrawDataHead
 * 功能描述:画数据头
 * 参数:QPainter *pPainter
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DrawDataHead(QPainter *pPainter)
{
	pPainter->save();

	pPainter->drawText( m_rcDataDiaplay[0][0] , Qt::AlignCenter , UTF8("序号"));
	pPainter->drawText( m_rcDataDiaplay[0][1] , Qt::AlignCenter , UTF8("内容"));
	pPainter->drawText( m_rcDataDiaplay[0][3] , Qt::AlignCenter , UTF8("位置"));

	if ( DATAQUERY_REALDATA == GetQueryType())
	{
		pPainter->drawText( m_rcDataDiaplay[0][2] , Qt::AlignCenter , UTF8("数值"));
	}

	if( DATAQUERY_SOE == GetQueryType() )
	{
		pPainter->drawText( m_rcDataDiaplay[0][2] , Qt::AlignCenter , UTF8("时间"));
	}

	if( DATAQUERY_YXALARM == GetQueryType() )
	{
		pPainter->drawText( m_rcDataDiaplay[0][2] , Qt::AlignCenter , UTF8("时间"));
	}

	pPainter->restore();

}   /*-------- end class CPage_DataQuery method DrawDataHead -------- */


/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DrawDataIndex
 * 功能描述:画序号
 * 参数: QPainter *pPainter 画笔
 BYTE index 序号
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DrawDataIndex( QPainter *pPainter, BYTE index )
{
	DWORD pagenum = GetCurrentPage();
	pPainter->save();

	// QFont font ;
	// font.setPixelSize( 23 );
	// pPainter->setFont(font);
	// QPen pen( QColor( 255 , 255 , 255 , 255 ) );
	// pPainter->setPen( pen );

	DWORD current_index = index + (pagenum - 1 ) * ONE_PAGE_DATANUM;
	pPainter->drawText( m_rcDataDiaplay[index][0] , Qt::AlignCenter , QString::number(current_index,10));

	pPainter->restore();


}   /*-------- end class CPage_DataQuery method DrawDataIndex -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DrawDataContent
 * 功能描述:画数据内容
 * 参数: QPainter *pPainter 画笔
 BYTE index 序号
 QString content 对应内容
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DrawDataContent( QPainter *pPainter, BYTE index, QString content)
{
	pPainter->save();

//	printf("%s %d\n", __func__, __LINE__);
//	qDebug(content.toAscii().data());
	pPainter->drawText( m_rcDataDiaplay[index][1] , Qt::AlignCenter , content);

	pPainter->restore();

}   /*-------- end class CPage_DataQuery method DrawDataContent -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DrawDataValue
 * 功能描述:画数据或当前显示的状态
 * 参数: QPainter *pPainter, BYTE index, QString val
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DrawDataValue( QPainter *pPainter, BYTE index, QString val)
{
	pPainter->save();

	pPainter->drawText( m_rcDataDiaplay[index][2] , Qt::AlignCenter , val);

	pPainter->restore();

}   /*-------- end class CPage_DataQuery method DrawDataValue -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DrawDataPosition
 * 功能描述:画位置
 * 参数: QPainter *pPainter, BYTE index, QString pos
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DrawDataPosition( QPainter *pPainter, BYTE index, QString pos)
{
	pPainter->save();

	pPainter->drawText( m_rcDataDiaplay[index][3] , Qt::AlignCenter , pos);

	pPainter->restore();

}   /*-------- end class CPage_DataQuery method DrawDataPosition -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:DrawData
 * 功能描述:
 * 参数: QPainter *pPainter
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::DrawData( QPainter *pPainter )
{
	DrawDataHead( pPainter );

	if ( DATAQUERY_REALDATA == GetQueryType())
	{
		// for (BYTE i=1; i<=ONE_PAGE_DATANUM; i++)
		for (BYTE i=1; i<=m_byCurrentDataNum; i++)
		{
			QUERY_REALTIME_DATA *pdata = GetQueryData( i - 1 );
			/*lel*/
		//	if (NULL == pdata || pdata->content == NULL)
			/*end*/
			if (NULL == pdata )
			{
				break;
			}

			DrawDataIndex( pPainter,  i );
			DrawDataContent( pPainter, i, pdata->content);
			DrawDataValue(pPainter, i, pdata->val );
			DrawDataPosition(pPainter, i, pdata->pos );
		}
	}

	if( DATAQUERY_SOE == GetQueryType() || DATAQUERY_YXALARM == GetQueryType() )
	{
		// 第一次进入时需要重新查一下所有数据
		if( m_bSelectAllSoe )
		{
			QueryAllSoe();
			m_bSelectAllSoe = FALSE;

		}
		for (BYTE i=1; i<=m_byCurrentSoeNum; i++)
		{
			SOEDATA *pSoe = GetSoeData( i - 1 );
			if (NULL == pSoe )
			{
				break;
			}

			DrawDataIndex( pPainter, i  );
			DrawDataContent( pPainter, i, UTF8(pSoe->Content ));
			DrawDataValue(pPainter, i, UTF8(pSoe->DateTime ) );
			DrawDataPosition(pPainter, i, UTF8(pSoe->Location ) );
		}
	}

}   /*-------- end class CPage_DataQuery method DrawData -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:GetFuncAreaType
 * 功能描述:获取功能区域
 * 参数: QPoint pt
 * 返回值:BYTE
 ******************************************************************************/
BYTE CPage_DataQuery::GetFuncAreaType( QPoint pt)
{
	int size = sizeof( m_rcFunction ) / sizeof( m_rcFunction[ 0 ] ) ;
	for (int i=0; i < size; i++)
	{
		if ( m_rcFunction[i].contains(pt) )
		{
			return i;
		}
	}

	return 0xff;
}   /*-------- end class CPage_DataQuery method GetFuncAreaType -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:ShowQueryTypeUi
 * 功能描述:显示相应功能的Ui
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::ShowQueryTypeUi(void)
{
	BYTE type = GetQueryType();
	switch (type)
	{
		case DATAQUERY_SOE: // soe
		case DATAQUERY_YXALARM://遥信报警
			{
				if ( !m_toolBoxSoe->isHidden() )
				{
					return;
				}
				// 隐藏所有ui
				HideQueryTypeUi();
				m_toolBoxSoe->show();
			}
			break;

		case DATAQUERY_REALDATA://实时数据
			{
				if ( !m_treeRealTimeData->isHidden() )
				{
					return;
				}
				// 隐藏所有ui
				HideQueryTypeUi();
				m_treeRealTimeData->show();
			}
			break;
		default:
			break;
	}
}   /*-------- end class CPage_DataQuery method ShowQueryTypeUi -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:HideQueryTypeUi
 * 功能描述:隐藏所有ui
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::HideQueryTypeUi(void)
{
	m_treeRealTimeData->hide();
	m_toolBoxSoe->hide();
}   /*-------- end class CPage_DataQuery method HideQueryTypeUi -------- */


/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:TreeItemChange
 * 功能描述:槽， 实时数据树的项目改变
 * 参数:const QModelIndex &index 点击项
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::TreeItemChange( const QModelIndex &index)
{
	QStandardItem *pItem = m_pModel->itemFromIndex(index);
	QString strTips = "";
	QVector<QUERY_REALTIME_PAGE> page ;
	QUERY_TREE_ITEM_ATTR attr;


	// 当前index
	int current_index = pItem->row();
	// 树属性
	attr.index = current_index;

	if (NULL != pItem->parent())
	{
		QStandardItem *pItemP = pItem->parent();
		int parent_index = pItemP->row();

		// // 组织tips
		// strTips = UTF8("柜")
		//           + QString::number(parent_index + 1, 10)
		//           + "-"
		//           + UTF8("回路")
		//           + QString::number(current_index + 1, 10) ;

		// // 设置总页数
		// SetTotalPage(m_vectorCabinetTable[parent_index].circuit[current_index].pagenum);

		// // 页数据指针
		// page = m_vectorCabinetTable[parent_index].circuit[current_index].page;

		// 是父节点
		attr.parent = FALSE;
		attr.parent_index = parent_index;
		attr.name = pItem->text();
		attr.parent_name = pItemP->text();


	}
	else
	{
		// // 组织tips
		// strTips = UTF8("柜")
		//           + QString::number(current_index + 1, 10) ;
		// // 设置总页数
		// SetTotalPage(m_vectorCabinetTable[current_index].pagenum);

		// // 页数据指针
		// page = m_vectorCabinetTable[current_index].page;

		// 是父节点
		attr.parent = TRUE;
		attr.parent_index = 0;
		attr.name = pItem->text();

	}


	// 设置树属性
	SetTreeItemAttr( attr );
	// 设置提示信息
	// SetTipsStr(strTips);
	// 设置当前页为第一页
	SetCurrentPage(1);

	// 设置树数据
	// SetTreeItemData();
	// for (int i=0; i<page[0].datanum; i++)
	// {
	//     SetQueryData(i, page[0].data.value(i));
	// }


	// AfxRefreshScreen( );
}   /*-------- end class CPage_DataQuery method TreeItemChange -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:SetTreeItemAttr
 * 功能描述:设置树属性
 * 参数:QUERY_TREE_ITEM_ATTR attr
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::SetTreeItemAttr(QUERY_TREE_ITEM_ATTR attr)
{
	m_treeAttr.index = attr.index;
	m_treeAttr.parent = attr.parent;
	m_treeAttr.parent_index = attr.parent_index;
	m_treeAttr.name = attr.name;
	m_treeAttr.parent_name = attr.parent_name;
}   /*-------- end class CPage_DataQuery method SetTreeItemAttr -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:GetTreeItemAttr
 * 功能描述:获得树属性
 * 参数:void
 * 返回值:QUERY_TREE_ITEM_ATTR
 ******************************************************************************/
QUERY_TREE_ITEM_ATTR *CPage_DataQuery::GetTreeItemAttr(void)
{
	return &m_treeAttr;
}   /*-------- end class CPage_DataQuery method GetTreeItemAttr -------- */


/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:SetTreeItemData()
 * 功能描述:设置树数据
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::SetTreeItemData(void)
{
	QString strTips = "";
	QVector<QUERY_REALTIME_PAGE> page ;
	QUERY_TREE_ITEM_ATTR *pTreeAttr;
	int current_index;
	DWORD pagenum = GetCurrentPage();

	pTreeAttr = GetTreeItemAttr();
	current_index = pTreeAttr->index;

	if ( !pTreeAttr->parent)
	{
		int parent_index = pTreeAttr->parent_index;

		// 组织tips
		strTips = pTreeAttr->parent_name // UTF8("柜")
			// + QString::number(parent_index + 1, 10)
			+ "->"
			// + UTF8("回路")
			+ pTreeAttr->name;
		// + QString::number(current_index + 1, 10) ;

		// 设置总页数
		SetTotalPage(m_vectorCabinetTable[parent_index].circuit[current_index].pagenum);

		// 页数据指针
		page = m_vectorCabinetTable[parent_index].circuit[current_index].page;
	}
	else
	{
		// 组织tips
		strTips = pTreeAttr->name; //UTF8("柜");
		// + QString::number(current_index + 1, 10) ;

		// 设置总页数
		SetTotalPage(m_vectorCabinetTable[current_index].pagenum);

		// 页数据指针
		page = m_vectorCabinetTable[current_index].page;
	}

	// 设置提示信息
	SetTipsStr(strTips);

	if( 0 == page.size() )
	{
		m_byCurrentDataNum = 0;

		SetTotalPage( 1 );
		AfxRefreshScreen( );
		return;
	}

	for (int i=0; i<page[pagenum-1].datanum; i++)
	{
		SetQueryData(i, page[pagenum-1].data.value(i));
	}


	AfxRefreshScreen( );

}   /*-------- end class CPage_DataQuery method SetTreeItemData() -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:SetDisplaySoe
 * 功能描述:设置Soe显示数据
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::SetDisplaySoe(void)
{
	BYTE soenum = 0;
	DWORD dwDataTotalNum;

	CDBaseManager *pDBase = AfxGetDBase();
	if ( NULL == pDBase )
	{
		print((char *)"NULL == pDBase\n");
		return;
	}

	DWORD pagenum = GetCurrentPage();
	if ( DATAQUERY_SOE == GetQueryType())
	{
		dwDataTotalNum = m_dwSoeNum;
	}
	else if ( DATAQUERY_YXALARM == GetQueryType())
	{
		dwDataTotalNum = m_dwYxAlarmNum;
	}
	// 查看数量
	if (( pagenum-1 ) * ONE_PAGE_DATANUM > dwDataTotalNum )
	{
		return;
	}

	if ( pagenum * ONE_PAGE_DATANUM > dwDataTotalNum )
	{
		soenum = dwDataTotalNum - (( pagenum-1 ) * ONE_PAGE_DATANUM);
	}
	else
	{
		soenum = ONE_PAGE_DATANUM;
	}

	// 没有查到任何数据
	if( 0 == soenum )
	{
		m_byCurrentSoeNum = 0;
	}

	for (BYTE i=0;  i < soenum; i++)
	{
		SOEDATA *pSoe = (SOEDATA *)pDBase->GetData( (pagenum - 1) * ONE_PAGE_DATANUM + i);
		if( NULL == pSoe )
		{
			printf("NULL == pSoe\n");
			break;
		}

		SetSoeData(i, *pSoe);
	}


}   /*-------- end class CPage_DataQuery method SetDisplaySoe -------- */


/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:BeginTimeChanged
 * 功能描述:槽 起始时间改变
 * 参数: const QDateTime &datetime 时间
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::BeginTimeChanged( const QDateTime &datetime)
{
	QDate date = datetime.date();
	QTime time = datetime.time();

	m_editBeginYear->setText( QString::number(date.year(), 10 ));
	m_editBeginMonth->setText(QString::number(date.month(), 10));
	m_editBeginDay->setText(QString::number(date.day(), 10));
	m_editBeginHour->setText(QString::number(time.hour(), 10));
	m_editBeginMinute->setText(QString::number(time.minute(), 10));
	m_editBeginSecond->setText(QString::number(time.second(), 10));
}   /*-------- end class CPage_DataQuery method BeginTimeChanged -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:EndTimeChanged
 * 功能描述:槽 结束时间改变
 * 参数: const QDateTime &datetime 时间
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::EndTimeChanged( const QDateTime &datetime)
{
	QDate date = datetime.date();
	QTime time = datetime.time();

	m_editEndYear->setText( QString::number(date.year(), 10 ));
	m_editEndMonth->setText(QString::number(date.month(), 10));
	m_editEndDay->setText(QString::number(date.day(), 10));
	m_editEndHour->setText(QString::number(time.hour(), 10));
	m_editEndMinute->setText(QString::number(time.minute(), 10));
	m_editEndSecond->setText(QString::number(time.second(), 10));
}   /*-------- end class CPage_DataQuery method EndTimeChanged -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:SlotQuerySoeByTime
 * 功能描述:槽 按时间查询soe
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::SlotQuerySoeByTime(void)
{
	char chBeginTime[256] = "";	 //起始时间
	char chEndTime[256] = "";	 //结束时间

	sprintf( chBeginTime,"%.2d-%.2d-%.2d %.2d:%.2d:%.2d",
			m_dtBegin->date().year(),
			m_dtBegin->date().month(),
			m_dtBegin->date().day(),
			m_dtBegin->time().hour(),
			m_dtBegin->time().minute(),
			m_dtBegin->time().second()
		   );

	sprintf( chEndTime,"%.2d-%.2d-%.2d %.2d:%.2d:%.2d",
			m_dtEnd->date().year(),
			m_dtEnd->date().month(),
			m_dtEnd->date().day(),
			m_dtEnd->time().hour(),
			m_dtEnd->time().minute(),
			m_dtEnd->time().second()
		   );

	print((char *)chBeginTime);
	print((char *)chEndTime);

	CDBaseManager *pDBase = AfxGetDBase();
	if ( NULL == pDBase )
	{
		print((char *)"NULL == pDBase\n");
		return;
	}

	if ( DATAQUERY_SOE == GetQueryType() )
	{
		m_dwSoeNum = pDBase->SelectFirstPageOfDataByTime(ONE_PAGE_DATANUM, chBeginTime, chEndTime, DBASE_SOE_DATATYPE);
		SetCurrentPage(1);
		SetTotalPage(1);
		AfxRefreshScreen();
		if( m_dwSoeNum < ONE_PAGE_DATANUM )
		{
			return;
		}
		m_dwSoeNum = pDBase->SelectDataByTime(chBeginTime, chEndTime, DBASE_SOE_DATATYPE);
		DWORD totalpage = m_dwSoeNum / ONE_PAGE_DATANUM;
		if( 0 != m_dwSoeNum % ONE_PAGE_DATANUM )
		{
			totalpage++;
		}

		SetTotalPage( totalpage);
	}

	if ( DATAQUERY_YXALARM == GetQueryType() )
	{
		m_dwYxAlarmNum = pDBase->SelectFirstPageOfDataByTime(ONE_PAGE_DATANUM, chBeginTime, chEndTime, DBASE_YXALARM_DATATYPE);
		SetCurrentPage(1);
		SetTotalPage(1);
		AfxRefreshScreen();
		if( m_dwYxAlarmNum < ONE_PAGE_DATANUM )
		{
			return;
		}
		m_dwYxAlarmNum = pDBase->SelectDataByTime(chBeginTime, chEndTime, DBASE_YXALARM_DATATYPE);
		DWORD totalpage = m_dwYxAlarmNum / ONE_PAGE_DATANUM;
		if( 0 != m_dwYxAlarmNum % ONE_PAGE_DATANUM )
		{
			totalpage++;
		}

		SetTotalPage( totalpage);
	}
}   /*-------- end class CPage_DataQuery method SlotQuerySoeByTime -------- */

/*******************************************************************************
 * 类:CPage_DataQuery
 * 函数名:QueryAllSoe
 * 功能描述:查询所有数据
 * 参数:void
 * 返回值:void
 ******************************************************************************/
void CPage_DataQuery::QueryAllSoe(void)
{
	CDBaseManager *pDBase = AfxGetDBase();
	if ( NULL == pDBase )
	{
		print((char *)"NULL == pDBase\n");
		return;
	}

	if ( DATAQUERY_SOE == GetQueryType() )
	{
		m_dwSoeNum = pDBase->SelectFirstPageOfAllData(ONE_PAGE_DATANUM,DBASE_SOE_DATATYPE);
		SetCurrentPage(1);
		SetTotalPage(1);
		AfxRefreshScreen();
		if( m_dwSoeNum < ONE_PAGE_DATANUM )
		{
			return;
		}
		m_dwSoeNum = pDBase->SelectAllData( DBASE_SOE_DATATYPE );
		DWORD totalpage = m_dwSoeNum / ONE_PAGE_DATANUM;
		if( 0 != m_dwSoeNum % ONE_PAGE_DATANUM )
		{
			totalpage++;
		}

		SetTotalPage( totalpage);
	}
	else if ( DATAQUERY_YXALARM == GetQueryType() )
	{
		m_dwYxAlarmNum = pDBase->SelectFirstPageOfAllData(ONE_PAGE_DATANUM,DBASE_YXALARM_DATATYPE);
		SetCurrentPage(1);
		SetTotalPage(1);
		AfxRefreshScreen();
		if( m_dwYxAlarmNum < ONE_PAGE_DATANUM )
		{
			return;
		}
		m_dwYxAlarmNum = pDBase->SelectAllData( DBASE_YXALARM_DATATYPE );
		DWORD totalpage = m_dwYxAlarmNum / ONE_PAGE_DATANUM;
		if( 0 != m_dwYxAlarmNum % ONE_PAGE_DATANUM )
		{
			totalpage++;
		}

		SetTotalPage( totalpage);
	}

}   /*-------- end class CPage_DataQuery method QueryAllSoe -------- */

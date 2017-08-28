#include "cpage_syspic.h"
#include "structure.h"
#include "../Element/ceelment.h"

const QString Path="/usr/share/ePut800/Pics/";				//of mine!

CPage_SysPic::CPage_SysPic( BYTE byType ):CPage( byType ),m_MAX_PAGE( 20 )
{/*{{{*/
	m_strPic += "ePut800-SysPic.jpg";
	m_TotalPage = 0 ;
	m_CurPage = 0 ;
	/*lel*/
#if 1
	//	m_strBackground += "timg0.jpg";
	//	pixmap.load(m_strBackground);
	byPrePage = GetCurPage();
	bg_flag = 0;
#else
	m_strBackgroundpaste = m_strBackground;
	for(int i = 0; i < 10; i++){
		m_strBackgroundpaste += "time"
			QPIXMAP_MAP[i].load()
	}
#endif
	/*end*/
}/*}}}*/

CPage_SysPic::~CPage_SysPic()
{
	pixmap.detach();
}

BOOL CPage_SysPic::AddData( LPVOID pVoid )
{/*{{{*/
	//AddData In future from File

	//Use Test Data Just Now
	//AddTestData( );

	if( !pVoid )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	QPAGE_FILEST_ARRAY * pPageEleArray = NULL ;
	pPageEleArray = ( QPAGE_FILEST_ARRAY * )pVoid ;
	AddTestData( pPageEleArray );
	AddTestDataAttribute();
	SetCurPage( 0 ) ;
	return FALSE ;
}/*}}}*/

void CPage_SysPic::AddTestDataAttribute()
{/*{{{*/
	Widget * tmpWidget = AfxGetWidget();
	int AllElementSize = tmpWidget->m_objAllElement.size();
	if( !AllElementSize )
	{
		//Q_ASSERT( FALSE ) ;
		return ;
	}

	QELEMENT_PICOBJ_MAP::iterator Begin_itor =  tmpWidget->m_objAllElement.begin() ;
	QELEMENT_PICOBJ_MAP::iterator End_itor =  tmpWidget->m_objAllElement.end() ;

	for( ;Begin_itor != End_itor ; Begin_itor++ )
	{
		CElement * tmpElement = Begin_itor.value() ;
		if( !tmpElement || tmpElement->m_wObjectID != Begin_itor.key( ) )
			return ;

		if( tmpElement->m_wLinkObjectId == 0 )//默认对象ID为0，既没有关联对象
		{
			tmpElement->InitData();
			continue;
		}
		PST_SYSPAGE tmpSysPage = GetSysPage_Strcut(tmpElement->m_wPageNo);
		switch( tmpElement->GetObjectType() )
		{
			case CElement::LINKER://链接线
			case CElement::TRANSFORMER ://变压器
			case CElement::CT_SIGN ://CT
			case CElement::PT_SIGN ://PT
			case CElement::LIGHTNING_PROTECTOR ://避雷针
				//start
				//		case CElement::IMAGE_FOR_AI:		/*	遥测身边图片!	*/
				//		//事实证明，这两个不需要添加
				//		case CElement::ALARM_FOR_AI:		/*	遥测关联的报警图元!	*/
				//end
				{
					if( tmpSysPage->m_AllElementNodeToPicId_Map.find(tmpElement->m_wLinkObjectId)
							== tmpSysPage->m_AllElementNodeToPicId_Map.end() )
					{
						PPICNODE tmpNewPicNode = new PICNODE;
						tmpNewPicNode->PicNode = tmpElement->m_wLinkObjectId;
						tmpSysPage->m_AllElementNodeToPicId_Map[tmpElement->m_wLinkObjectId]=tmpNewPicNode;
					}
					PPICNODE tmpPicNode = tmpSysPage->m_AllElementNodeToPicId_Map[tmpElement->m_wLinkObjectId];
					tmpPicNode->AddTheNodePicId(tmpElement->m_wObjectID);
					tmpElement->m_wObjectNodeL = tmpElement->m_wLinkObjectId;
					tmpElement->InitData();
					continue;
				}
			default:
				break;

		}
		PPIC_ATTRIBUT_FST  tmpAttributFst = tmpWidget->m_objAllElementAttribute_Map[tmpElement->m_wLinkObjectId];
		if( tmpAttributFst == NULL )
		{
			printf("图元Id=%d,找不到其关联对象Id=%d\n",tmpElement->m_wObjectID,tmpElement->m_wLinkObjectId);
			tmpElement->InitData();
			continue;
		}
		switch( tmpElement->GetObjectType() )
		{
			case CElement::GENERATRIX : //母线
			case CElement::POWER : //电源
				{
					if( tmpSysPage->m_AllElementNodeToPicId_Map.find(tmpAttributFst->wObjectNodeL)
							== tmpSysPage->m_AllElementNodeToPicId_Map.end() )
					{
						PPICNODE tmpNewPicNode = new PICNODE;
						tmpNewPicNode->PicNode = tmpAttributFst->wObjectNodeL;
						tmpSysPage->m_AllElementNodeToPicId_Map[tmpAttributFst->wObjectNodeL]=tmpNewPicNode;
					}
					PPICNODE tmpPicNode = tmpSysPage->m_AllElementNodeToPicId_Map[tmpAttributFst->wObjectNodeL];
					tmpPicNode->AddTheNodePicId(tmpElement->m_wObjectID);
					tmpElement->m_wObjectNodeL = tmpAttributFst->wObjectNodeL;
					if( CElement::POWER == tmpElement->GetObjectType() )
						tmpSysPage->m_PowerElementNodeToPicId_Array.push_back(tmpPicNode);
					break;
				}
			case CElement::BREAKER:/*断路器*/
			case CElement::CONTACTOR:/*接触器*/
			case CElement::LOAD_BREAKER_2POS:/*两工位负荷开关*/
			case CElement::LOAD_BREAKER_3POS:/*三工位负荷开关*/
			case CElement::SWITCH_2POS:/*两工位刀闸*/
			case CElement::SWITCH_3POS:/*三工位刀闸*/
			case CElement::SWITCH_CONNECT_EARTH:/*接地刀闸*/
				{
					if( tmpSysPage->m_AllElementNodeToPicId_Map.find(tmpAttributFst->wObjectNodeL)
							== tmpSysPage->m_AllElementNodeToPicId_Map.end() )
					{
						PPICNODE tmpNewPicNodeL = new PICNODE;
						tmpNewPicNodeL->PicNode = tmpAttributFst->wObjectNodeL;
						tmpSysPage->m_AllElementNodeToPicId_Map[tmpAttributFst->wObjectNodeL]=tmpNewPicNodeL;
					}
					PPICNODE tmpPicNodeL = tmpSysPage->m_AllElementNodeToPicId_Map[tmpAttributFst->wObjectNodeL];
					tmpPicNodeL->AddTheNodePicId(tmpElement->m_wObjectID);
					tmpElement->m_wObjectNodeL = tmpAttributFst->wObjectNodeL;

					if( tmpSysPage->m_AllElementNodeToPicId_Map.find(tmpAttributFst->wObjectNodeR)
							== tmpSysPage->m_AllElementNodeToPicId_Map.end() )
					{
						PPICNODE tmpNewPicNodeR = new PICNODE;
						tmpNewPicNodeR->PicNode = tmpAttributFst->wObjectNodeR;
						tmpSysPage->m_AllElementNodeToPicId_Map[tmpAttributFst->wObjectNodeR]=tmpNewPicNodeR;
					}
					PPICNODE tmpPicNodeR = tmpSysPage->m_AllElementNodeToPicId_Map[tmpAttributFst->wObjectNodeR];
					tmpPicNodeR->AddTheNodePicId(tmpElement->m_wObjectID);
					tmpElement->m_wObjectNodeR = tmpAttributFst->wObjectNodeR;
					break;
				}
			default:
				break;
		}

		int AttributeSize = tmpAttributFst->DataArray.size();
	//	printf("%s %d AttributeSize = %d\n", __func__, __LINE__, AttributeSize);
		int AttributeNo=0;
		//		short relanum = tmpElement->relanum - 1;
		//	for(AttributeNo = 0; AttributeNo < AttributeSize; AttributeNo++, relanum++)
		for(AttributeNo = 0; AttributeNo < AttributeSize; AttributeNo++)
		{
			PPIC_ATTRIBUTE_DATA_FST tmpAttributeFst = tmpAttributFst->DataArray.at(AttributeNo);
			if( tmpElement->GetObjectType()==CElement::ANALOG_DATA_SIGN ||
					tmpElement->GetObjectType()==CElement::RECT_INDICATOR_SIGN ||
					tmpElement->GetObjectType()==CElement::CIRCLE_INDICATOR_SIGN)// || 			06-10
				//					tmpElement->GetObjectType() == CElement::IMAGE_FOR_AI)		//start end
				//				tmpElement->GetObjectType() == CElement::ALARM_FOR_AI)		//start end
			{
				if( tmpElement->m_wDirect != tmpAttributeFst->byAttributeDataByte )
					continue;
			}
			PPIC_ATTRIBUTE_DATA_ELEMENT tmpElementAttribute = new PIC_ATTRIBUTE_DATA_ELEMENT;
			tmpElementAttribute->wBusNo = tmpAttributeFst->wBusNo;
			tmpElementAttribute->wAddr = tmpAttributeFst->wAddr;
			tmpElementAttribute->wPntNo = tmpAttributeFst->wPntNo;
			tmpElementAttribute->byAttributeDataByte = tmpAttributeFst->byAttributeDataByte;
			tmpElementAttribute->byDataType = tmpAttributeFst->byDataType;
			//start
			if(tmpElement->GetObjectType() == CElement::IMAGE_FOR_AI){
				short relanum = tmpElement->relanum - 1;
			//	printf("%s %d tmpElement->relanum = %d\n", __func__, __LINE__, tmpElement->relanum);
				WORD pageno = tmpElement->m_wPageNo;			//
				QMap<BYTE, QVector<Alarm> > alarmobj = tmpWidget->alarmobj;
				//	if(alarmobj[pageno].size() <= relanum + 1){
				if(alarmobj[pageno].size() > 0){
					//	printf("-----------------------------------	%s %d %d %d	-------------------------------------------\n", __func__, __LINE__, relanum, pageno);
					tmpElementAttribute->RelaYk.id = alarmobj[pageno].at(relanum).id;
					tmpElementAttribute->RelaYk.toppole = alarmobj[pageno].at(relanum).toppole;
					tmpElementAttribute->RelaYk.top = alarmobj[pageno].at(relanum).top;
					tmpElementAttribute->RelaYk.valuetop = alarmobj[pageno].at(relanum).valuetop;
					tmpElementAttribute->RelaYk.valuebottom = alarmobj[pageno].at(relanum).valuebottom;
					tmpElementAttribute->RelaYk.bottom = alarmobj[pageno].at(relanum).bottom;
					tmpElementAttribute->RelaYk.bottompole = alarmobj[pageno].at(relanum).bottompole;
					tmpElementAttribute->RelaYk.busno = alarmobj[pageno].at(relanum).busno;
					tmpElementAttribute->RelaYk.devaddr = alarmobj[pageno].at(relanum).devaddr;
					tmpElementAttribute->RelaYk.point = alarmobj[pageno].at(relanum).point;
				//	tmpElementAttribute->RelaYk.reservation = alarmobj[pageno].at(relanum).reservation;
				//	printf("%s %d reservation = %d id = %d\n", __func__, __LINE__, tmpElementAttribute->RelaYk.reservation, tmpElementAttribute->RelaYk.id);
					/*lel*/
#if 1
				//	if(tmpElementAttribute->RelaYk.reservation == 2){
					if(tmpElement->w_primitivetype == TEMPERATURE_HUMIDITY_SENSOR){
			//			printf("%s %d ########\n", __func__, __LINE__);
						tmpElementAttribute->RelaYk1.id = alarmobj[pageno].at(relanum + 1).id;
						tmpElementAttribute->RelaYk1.toppole = alarmobj[pageno].at(relanum + 1).toppole;
						tmpElementAttribute->RelaYk1.top = alarmobj[pageno].at(relanum + 1).top;
						tmpElementAttribute->RelaYk1.valuetop = alarmobj[pageno].at(relanum + 1).valuetop;
						tmpElementAttribute->RelaYk1.valuebottom = alarmobj[pageno].at(relanum + 1).valuebottom;
						tmpElementAttribute->RelaYk1.bottom = alarmobj[pageno].at(relanum + 1).bottom;
						tmpElementAttribute->RelaYk1.bottompole = alarmobj[pageno].at(relanum + 1).bottompole;
						tmpElementAttribute->RelaYk1.busno = alarmobj[pageno].at(relanum + 1).busno;
						tmpElementAttribute->RelaYk1.devaddr = alarmobj[pageno].at(relanum + 1).devaddr;
						tmpElementAttribute->RelaYk1.point = alarmobj[pageno].at(relanum + 1).point;
					//	tmpElementAttribute->RelaYk1.reservation = alarmobj[pageno].at(relanum + 1).reservation;
					}
#endif
					/*end*/
#if 0
					printf("****	%d	****\n", alarmobj[pageno].at(relanum).id);
					printf("****	%d	****\n", alarmobj[pageno].at(relanum).toppole);
					printf("****	%d	****\n", alarmobj[pageno].at(relanum).top);
					printf("****	%d	****\n", alarmobj[pageno].at(relanum).valuetop);
					printf("****	%d	****\n", alarmobj[pageno].at(relanum).valuebottom);
					printf("****	%d	****\n", alarmobj[pageno].at(relanum).bottom);
					printf("****	%d	****\n", alarmobj[pageno].at(relanum).bottompole);
					printf("****	%d	****\n", alarmobj[pageno].at(relanum).busno);
					printf("****	%d	****\n", alarmobj[pageno].at(relanum).devaddr);
					printf("****	%d	****\n", alarmobj[pageno].at(relanum).point);
					printf("****	%d	****\n", alarmobj[pageno].at(relanum).reservation);
#endif
					//					break;
				}
				//			}
			}else{															//对于无关联的数据，给他们一个默认值吧，反正也不会被执行!
				tmpElementAttribute->RelaYk.id = 0;
				tmpElementAttribute->RelaYk.toppole = (WORD)-1;
				tmpElementAttribute->RelaYk.top = ((WORD)-1) * 5 / 4;
				tmpElementAttribute->RelaYk.valuetop = ((WORD)-1) / 2;
				tmpElementAttribute->RelaYk.valuebottom = -((WORD)-1) / 2;
				tmpElementAttribute->RelaYk.bottom = -((WORD)-1) * 5 / 4;
				tmpElementAttribute->RelaYk.bottompole = -65536;
				tmpElementAttribute->RelaYk.busno = 254;
				tmpElementAttribute->RelaYk.devaddr = 254;
				tmpElementAttribute->RelaYk.point = 254;
			//	tmpElementAttribute->RelaYk.reservation = 0;
			}
			//end

			PDataType tmpData = AfxGetData( tmpElementAttribute->byDataType , tmpElementAttribute->wBusNo ,
					tmpElementAttribute->wAddr , tmpElementAttribute->wPntNo ) ;
			if( tmpData == NULL )
			{
				printf("找不到类型=%d  P%d,%d,%d(类型：1-Yc,2-Yx,3-Ym,4-Yk)\n",
						tmpElementAttribute->byDataType , tmpElementAttribute->wBusNo ,
						tmpElementAttribute->wAddr , tmpElementAttribute->wPntNo);
			}
			else
			{
				strcpy(tmpAttributeFst->name,tmpData->chName);
				strcpy(tmpElementAttribute->name,tmpData->chName);
			}
			switch( tmpElementAttribute->byDataType )
			{
				case PIC_ATTRIBUTE_DATA_FST::YX_DTYPE:
					tmpElement->Yx_ElementAttributeArray.push_back(tmpElementAttribute);
					break;
				case PIC_ATTRIBUTE_DATA_FST::YC_DTYPE:
					/*lel*/
#if 0
					//	目的是让一个图元只关联一个遥测
					//	if(tmpElement->GetObjectType() != CElement::IMAGE_FOR_AI)				//06-10
					tmpElement->Yc_ElementAttributeArray.push_back(tmpElementAttribute);
					//	else if(tmpElement->m_wDirect == tmpAttributeFst->byAttributeDataByte)	//06-10
					//		tmpElement->Yc_ElementAttributeArray.push_back(tmpElementAttribute);//06-10
#else
					tmpElement->Yc_ElementAttributeArray.push_back(tmpElementAttribute);
#endif
					/*end*/
					break;
				case PIC_ATTRIBUTE_DATA_FST::YM_DTYPE:
					tmpElement->Ym_ElementAttributeArray.push_back(tmpElementAttribute);
					break;
				case PIC_ATTRIBUTE_DATA_FST::YK_DTYPE:
					tmpElement->Yk_ElementAttributeArray.push_back(tmpElementAttribute);
					break;
				default:
					break;
			}
		}
		tmpElement->InitData();
	}
}/*}}}*/

void CPage_SysPic::AddTestData( QPAGE_FILEST_ARRAY * pArray )
{/*{{{*/
	if( !pArray )
	{
		Q_ASSERT( FALSE );
		return ;
	}

	int size = pArray->size() ;
	WORD wCurPage = (WORD)-1 ;
	for( BYTE i = 0 ; i < size ; i++ )
	{
		PPAGE_FST pPageFst = (*pArray)[ i ] ;
		if( !pPageFst )
		{
			Q_ASSERT( FALSE ) ;
			return ;
		}

		WORD wPageNo = pPageFst->wPageNo ;
		WORD wEleNum = pPageFst->wEleNumber ;
		WORD wSizeEle = pPageFst->ElementArray.size( ) ;
		if( wCurPage == ( WORD )-1 )
			wCurPage = wPageNo ;

		if( wSizeEle == 0 )
			return ;

		if( wSizeEle != wEleNum )
			return ;

		ST_SYSPAGE * pSysPage = new ST_SYSPAGE ;
		pSysPage->byPageNo = wPageNo  ;
		int strLen = strlen( pPageFst->pPageTitle ) ;
		memcpy( pSysPage->strPicTitle , pPageFst->pPageTitle ,strLen ) ;
		//start
		strLen = strlen(pPageFst->pWallName);
		memcpy(pSysPage->pWall_SysPage, pPageFst->pWallName, strLen);
		//end
		/*lel*/
		pSysPage->pWareHome = pPageFst->pWareHome;
		/*end*/

		for( BYTE m = 0 ; m < wEleNum ; m++ )
		{
			PELE_FST pEle_fst =  pPageFst->ElementArray[ m ] ;
			if( !pEle_fst )
			{
				Q_ASSERT( FALSE );
				return ;
			}
			CElement *pElement = NULL ;
			//	pElement = CElement::CreateElement( pEle_fst->wElementType ) ;
			pElement = CElement::CreateElement( pEle_fst) ;
			if( !pElement )
				return ;
			//start
			if(pEle_fst->wElementType == 4114)						//IMAGE_FOR_AI 
				AfxGetWidget()->yc_ykvec.push_back(pElement);
			else if(pEle_fst->wElementType == 4115)					//ALARM_FOR_AI
				AfxGetWidget()->yx_ykvec.push_back(pElement);
			//end

			int width = pEle_fst->wRight - pEle_fst->wLeft ;
			int height = pEle_fst->wBottom - pEle_fst->wTop ;
			QRect rc( pEle_fst->wLeft , pEle_fst->wTop , width , height ) ;
			pElement->Create( rc ) ;
			pElement->m_wObjectID     = pEle_fst->wObjectID ;
			pElement->m_wLinkObjectId = pEle_fst->wLinkObjectId ;
			pElement->m_wPageNo       = pEle_fst->wPageNo ;
			pElement->m_wDirect       = pEle_fst->wDirect ;
			pElement->w_top           = pEle_fst->wTop;
			pElement->w_bottom        = pEle_fst->wBottom;
			pElement->w_left          = pEle_fst->wLeft;
			pElement->w_right         = pEle_fst->wRight;
			pElement->w_primitivetype = pEle_fst->wPrimitiveType;
			pElement->w_elementtype   = pEle_fst->wElementType;
			strcpy(pElement->m_text, pEle_fst->text);
			strcpy(pElement->m_picname, pEle_fst->picname);
			strcpy(pElement->m_picintroduce, pEle_fst->picintroduce);
			/*end*/
			pElement->SetObjectType( pEle_fst->wElementType );
			//start
			pElement->relanum = pEle_fst->relanum;
			//end
			pSysPage->m_EleArray.push_back( pElement );
			AfxGetWidget()->m_objAllElement[ pElement->m_wObjectID ] = pElement;
		}

		//		printf("^^^^^^^^^^^^^^^^\n");
		m_sysPageArray[ pSysPage->byPageNo ] = pSysPage ;

		if( m_TotalPage <= m_MAX_PAGE )
			m_TotalPage++ ;
	}
}/*}}}*/

BYTE CPage_SysPic::GetTotalPageNum( )
{/*{{{*/
	if( m_TotalPage == 0 )
		m_TotalPage = 1;
	return m_TotalPage ;
}/*}}}*/
#if 0
void CPage_SysPic::AddTestData( )
{/*{{{*/
	//construct buffers For per page
	ST_SYSPAGE * pSysPage = new ST_SYSPAGE ;
	pSysPage->byPageNo = 1 ;

	for( BYTE i = 0 ; i < 11 + 7 ; i++ )
	{
		CElement * pEle = NULL ;
		//创建各种图元
		if( 0 == i )
			pEle = CElement::CreateElement( CElement::GENERATRIX );
		else if( 1 == i )
			pEle = CElement::CreateElement( CElement::LINKER ) ;
		else if( 2 == i )
			pEle = CElement::CreateElement( CElement::POWER );
		else if( 3 == i )
			pEle = CElement::CreateElement( CElement::TRANSFORMER );
		else if( 4 == i )
			pEle = CElement::CreateElement( CElement::CT_SIGN );
		else if( 5 == i )
			pEle = CElement::CreateElement( CElement::PT_SIGN ) ;
		else if( 6 == i )
			pEle = CElement::CreateElement( CElement::ANALOG_DATA_SIGN );
		else if( 7 == i )
			pEle = CElement::CreateElement( CElement::TEXT_SIGN );
		else if( 8 == i )
			pEle = CElement::CreateElement( CElement::CIRCLE_INDICATOR_SIGN );
		else if( 9 == i )
			pEle = CElement::CreateElement( CElement::RECT_INDICATOR_SIGN );
		else if( 10 == i )
			pEle = CElement::CreateElement( CElement::LIGHTNING_PROTECTOR );
		else if( 11 == i )
			pEle = CElement::CreateElement( CElement::BREAKER ) ;
		else if( 12 == i )
			pEle = CElement::CreateElement( CElement::CONTACTOR );
		else if ( 13 == i )
			pEle = CElement::CreateElement( CElement::LOAD_BREAKER_2POS );
		else if ( 14 == i )
			pEle = CElement::CreateElement( CElement::LOAD_BREAKER_3POS );
		else if ( 15 == i )
			pEle = CElement::CreateElement( CElement::SWITCH_2POS );
		else if ( 16 == i )
			pEle = CElement::CreateElement( CElement::SWITCH_3POS ) ;
		else if ( 17 == i )
			pEle = CElement::CreateElement( CElement::SWITCH_CONNECT_EARTH );
		//start
		//添加不添加无所谓，这个函数并没有被调用!
		else if(i == 18)
			pEle = CElement::CreateElement(CElement::IMAGE_FOR_AI);
		else if(i == 19)
			pEle = CElement::CreateElement(CElement::ALARM_FOR_AI);
		//end
		else
			Q_ASSERT( FALSE ) ;

		if( pEle )
		{
			WORD wHigh = i / 5 ;
			WORD wWidh = i % 5 ;
			WORD wWidth = 100 ;
			WORD wHeight = 120 ;

			pEle->Create( QRect( 10 + wWidth * wWidh,79 + wHigh * wHeight , wWidth ,wHeight ) ) ;
			pSysPage->m_EleArray.push_back( pEle );
		}
	}

	m_sysPageArray[ pSysPage->byPageNo ] = pSysPage ;
	SetCurPage( pSysPage->byPageNo ) ;
	/*
	   int size1 = pSysPage->m_EleArray.size() ;
	   int size2 = m_sysPageArray.size() ;
	   int size3 = 0 ;
	   */
}/*}}}*/
#endif
BOOL CPage_SysPic::DrawPage( QPainter * pPainter )
{/*{{{*/
	CPage::DrawPage( pPainter ) ;
	DrawCurPageNumber( pPainter ) ;
	DrawPageTitle( pPainter ) ;

	BYTE byCurPage = GetCurPage() ;
	PST_SYSPAGE pSysPage = GetSysPage_Strcut( byCurPage ) ;
	//	printf("%s %d byCurPage = %d\n", __func__, __LINE__, byCurPage);
	if( !pSysPage )
	{
		//		printf("!!!!!!!!!!!!!!!!!!!!\n");
		//Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	if( pSysPage->byPageNo != byCurPage )
	{
		//		printf("@@@@@@@@@@@@@@@@@@@@\n");
		Q_ASSERT( FALSE );
		return FALSE ;
	}
	/*lel*/
#if 1
	/*保证该界面背景图只有第一次的时候load加载，之后就只是drawPixmap*/
	//start
	QString backGroundPage = Path + (QString)(pSysPage->pWall_SysPage);
	//	printf("%s %d pSysPage->pWall_SysPage = %s\n", __func__, __LINE__, pSysPage->pWall_SysPage);
	//	qDebug(backGroundPage.toAscii().data());
	if(byPrePage != pSysPage->byPageNo){
		byPrePage = pSysPage->byPageNo;
		bg_flag = 0;
	}
	else if(bg_flag == 0){
		//	QPixmap pixmap;
		switch(pSysPage->byPageNo){							//预置10张壁纸，超出的部分使用默认壁纸!/*{{{*/
			case 0:
				pixmap.load(backGroundPage);
				break;
			case 1:
				pixmap.load(backGroundPage);
				break;
			case 2:
				pixmap.load(backGroundPage);
				break;
			case 3:
				pixmap.load(backGroundPage);
				break;
			case 4:
				pixmap.load(backGroundPage);
				break;
			case 5:
				pixmap.load(backGroundPage);
				break;
			case 6:
				pixmap.load(backGroundPage);
				break;
			case 7:
				pixmap.load(backGroundPage);
				break;
			case 8:
				pixmap.load(backGroundPage);
				break;
			case 9:
				pixmap.load(backGroundPage);
				break;
			default:
				pixmap.load("/usr/share/ePut800/Pics/timg0.jpg");
				break;
		}/*}}}*/
		bg_flag = 1;
		//	pPainter->drawPixmap(0, 67, 800, 490, pixmap);
	}
	//	pPainter->drawPixmap(0, 67, 800, 490, pixmap);
	//	pPainter->drawPixmap(0, 67, 800, 532, pixmap);
	pPainter->drawPixmap(0, 72, 800, 527, pixmap);
	if(pSysPage->pWareHome == 2){
	//	printf("!!!!!!!!!!!!!!!!!!!\n");
		pPainter->setPen(QPen(Qt::gray, 5, Qt::SolidLine));
		pPainter->drawLine(0, 333, 800, 338);
	}
	else if(pSysPage->pWareHome == 3){
	//	printf("###################\n");
		pPainter->setPen(QPen(Qt::gray, 5, Qt::SolidLine));
		pPainter->drawLine(0, 240, 800, 245);
		pPainter->drawLine(0, 416, 800, 421);
	}
	//	pixmap.detach();
	//	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@444444444\n");
	//end
#else

	pPainter->drawPixmap(0, 67, 800, 490, pixmap);
#endif
	/*end*/
	int size = pSysPage->m_EleArray.size() ;
	for( int i = 0 ; i < size ; i++ ){
		CElement * pEle = pSysPage->m_EleArray[ i ] ;
		if( !pEle )
		{
			Q_ASSERT( FALSE ) ;
			continue ;
		}

		pEle->DrawElemnt( pPainter ) ;
		DrawText(pEle, pPainter);
	}

	//	printf("###########################################\n");
	return TRUE ;
}/*}}}*/

void CPage_SysPic::DrawText(CElement *pEle, QPainter *pPainter)
{
	QFont font;
	font.setPixelSize(11);
	pPainter->setFont(font);
	pPainter->setPen(Qt::black);
	pPainter->drawText(QRect(QPoint(pEle->w_left - 10, pEle->w_top - 15), QPoint(pEle->w_right + 10, pEle->w_top)), Qt::AlignCenter, pEle->m_picintroduce);
	if(pEle->w_elementtype == 4114){
		switch(pEle->w_primitivetype){
			case LIQUID_LEVEL_SENSOR:
				memset(sPrimitive, 0, sizeof(sPrimitive));
				sprintf(sPrimitive, "%0.2f m", pEle->f_valued);
				pPainter->drawText(QRect(QPoint(pEle->w_left - 10, pEle->w_bottom), QPoint(pEle->w_right + 10, pEle->w_bottom + 15)), Qt::AlignCenter, sPrimitive);
				break;

			case TEMPERATURE_HUMIDITY_SENSOR:
				memset(sPrimitive, 0, sizeof(sPrimitive));
				sprintf(sPrimitive, "%0.2f C", pEle->f_valued);
				pPainter->drawText(QRect(QPoint(pEle->w_left - 10, pEle->w_bottom), QPoint(pEle->w_right + 10, pEle->w_bottom + 15)), Qt::AlignCenter, sPrimitive);

				memset(sPrimitive, 0, sizeof(sPrimitive));
				sprintf(sPrimitive, "%0.1f %%", pEle->f_valued1);
				pPainter->drawText(QRect(QPoint(pEle->w_left - 10, pEle->w_bottom + 15), QPoint(pEle->w_right + 10, pEle->w_bottom + 30)), Qt::AlignCenter, sPrimitive);
				break;

			case GAS_DETECTOR:
				memset(sPrimitive, 0, sizeof(sPrimitive));
				sprintf(sPrimitive, "%0.2f ppm", pEle->f_valued);
				pPainter->drawText(QRect(QPoint(pEle->w_left - 10, pEle->w_bottom), QPoint(pEle->w_right + 10, pEle->w_bottom + 15)), Qt::AlignCenter, sPrimitive);
				break;

			default:
				break;
		}
	}
	else if(pEle->w_elementtype == 4115){
		switch(pEle->w_primitivetype){
			case COVERS:
				if(pEle->w_blink_flag == 0)
					pPainter->drawText(QRect(QPoint(pEle->w_left - 10, pEle->w_bottom), QPoint(pEle->w_right + 10, pEle->w_bottom + 15)), Qt::AlignCenter, "开启");
				else
					pPainter->drawText(QRect(QPoint(pEle->w_left - 10, pEle->w_bottom), QPoint(pEle->w_right + 10, pEle->w_bottom + 15)), Qt::AlignCenter, "关闭");
				break;

			case DRAIN_PUMP:
				if(pEle->w_blink_flag == 0)
					pPainter->drawText(QRect(QPoint(pEle->w_left - 10, pEle->w_bottom), QPoint(pEle->w_right + 10, pEle->w_bottom + 15)), Qt::AlignCenter, "运行");
				else
					pPainter->drawText(QRect(QPoint(pEle->w_left - 10, pEle->w_bottom), QPoint(pEle->w_right + 10, pEle->w_bottom + 15)), Qt::AlignCenter, "停止");
				break;

			default:
				break;
		}
	}
}

void CPage_SysPic::DrawPageTitle( QPainter *pPainter )
{/*{{{*/
	if( !pPainter )
	{
		Q_ASSERT( FALSE );
		return ;
	}

	BYTE byCurPage = GetCurPage() ;
	PST_SYSPAGE pPage = GetSysPage_Strcut( byCurPage ) ;
	if( !pPage )
	{
		Q_ASSERT( FALSE );
		return ;
	}

	pPainter->save();

	QPen pen ;
	pen.setStyle( Qt::SolidLine );
	pen.setColor( QColor( 255 , 255 , 255 , 255 ));
	pPainter->setPen( pen );

	QFont font ;
	font.setPixelSize( 30 );
	font.setBold( TRUE );
	pPainter->setFont( font );

	QRect rc( QPoint( 124 , 18 ) , QPoint( 419 , 65 )) ;
	pPainter->drawText( rc , Qt::AlignCenter , pPage->strPicTitle );

	pPainter->restore();
}/*}}}*/

void CPage_SysPic::DrawCurPageNumber( QPainter * pPainter )
{/*{{{*/
	if( !pPainter )
	{
		Q_ASSERT( FALSE ) ;
		return ;
	}

	char buf[ 100 ] ;
	memset( buf , 0 , sizeof( buf ) ) ;
	sprintf( buf , "%02d/%02d" , GetCurPage() + 1, GetTotalPageNum() ) ;

	QString strText ;
	strText = buf ;
	QRect rc( QPoint( 435 - 10 , 27) ,QPoint( 527 -10 , 62 )) ;

	pPainter->save();

	QPen pen ;
	pen.setStyle( Qt::SolidLine );
	pen.setColor( QColor( 255 , 0 , 0 , 255 ));
	pPainter->setPen( pen );

	QFont font ;
	font.setPixelSize( 30 );
	font.setBold( TRUE );
	pPainter->setFont( font );

	pPainter->drawText( rc , strText );

	pPainter->restore();
}/*}}}*/

PST_SYSPAGE CPage_SysPic::GetSysPage_Strcut( BYTE byPageIndex )
{/*{{{*/
	if( byPageIndex > m_MAX_PAGE ){
		//		printf("############\n");
		return NULL ;
	}

	int size = m_sysPageArray.size() ;
	if( !size ){
		//		printf("&&&&&&&&&&&&&\n");
		return NULL ;
	}

	PST_SYSPAGE pSt = m_sysPageArray[ byPageIndex ];
	if( !pSt )
	{
		//		printf("$$$$$$$$$$$$$\n");
		Q_ASSERT( FALSE ) ;
		return NULL ;
	}

	return pSt ;
}/*}}}*/

BOOL CPage_SysPic::SetCurPage( BYTE byPageID )
{/*{{{*/
	if( byPageID > m_MAX_PAGE )
		return FALSE ;

	m_CurPage = byPageID ;

	PST_SYSPAGE PCurPage =  GetSysPage_Strcut(m_CurPage);
	if( PCurPage == NULL )
		return FALSE ;
	int PowerNodeNum = PCurPage->m_PowerElementNodeToPicId_Array.size();
	CElement * tmpElement = NULL;
	for(int i=0;i<PowerNodeNum;i++)
	{
		int ThisNodePowerNum = PCurPage->m_PowerElementNodeToPicId_Array.at(i)->PicId_Array.size();
		for( int j=0;j<ThisNodePowerNum;j++ )
		{
			tmpElement = AfxGetWidget()->m_objAllElement[PCurPage->m_PowerElementNodeToPicId_Array.at(i)->PicId_Array.at(j)];
			if( tmpElement->GetObjectType() == CElement::POWER )
				AfxRefreshPicColor(PCurPage,tmpElement);
		}
	}
	return TRUE ;
}/*}}}*/

BYTE CPage::GetCurPage( )
{/*{{{*/
	return m_CurPage ;
}/*}}}*/

BOOL CPage_SysPic::mousePressEvent(QMouseEvent *pEvent )
{/*{{{*/
	if( CPage::mousePressEvent( pEvent ) )
	{
		AfxRefreshScreen();
		return TRUE ;
	}

	//prcess element operation
	ProcessElementOpt( pEvent ) ;

	AfxRefreshScreen();
	return TRUE ;
}/*}}}*/

BOOL CPage_SysPic::ProcessElementOpt( QMouseEvent *pEvent )
{/*{{{*/
	if( !pEvent )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	BYTE byPageIndex = GetCurPage() ;
	PST_SYSPAGE pPage = GetSysPage_Strcut( byPageIndex ) ;
	if( !pPage )
	{
		Q_ASSERT( FALSE ) ;
		return FALSE ;
	}

	QPoint pt = pEvent->pos() ;
	int size = pPage->m_EleArray.size() ;
	printf("%s %d size = %d\n", __func__, __LINE__, size);
	for( int i = 0 ; i < size ; i++ )
	{
		CElement * pElement = pPage->m_EleArray[ i ] ;
		if( !pElement )
		{
			Q_ASSERT( FALSE ) ;
			return FALSE ;
		}

		QRect rc( pElement->GetQRect() ) ;
		if( rc.contains( pt ) )
		{
			pElement->ProcessMouseDown( pEvent ) ;
			return TRUE ;
		}
	}

	return FALSE ;
}/*}}}*/

BOOL CPage_SysPic::ProcessPageDown( )
{/*{{{*/
	BYTE byCurPage = GetCurPage() ;
	if( byCurPage < m_TotalPage - 1 )
		byCurPage++ ;
	else
		byCurPage = 0 ;

	SetCurPage( byCurPage ) ;

	return TRUE ;
}/*}}}*/

BOOL CPage_SysPic::ProcessPageUp( )
{/*{{{*/
	BYTE byCurPage = GetCurPage() ;
	if( byCurPage > 0 )
		byCurPage-- ;
	else
		byCurPage = 0 ;

	SetCurPage( byCurPage ) ;
	return TRUE ;
}/*}}}*/

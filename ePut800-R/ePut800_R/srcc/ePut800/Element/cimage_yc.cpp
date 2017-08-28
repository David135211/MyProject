#include "cimage_yc.h"
const QString Path = "/usr/share/ePut800/Pics/";

CImage_Yc::CImage_Yc()
{/*{{{*/
	m_wObjectType = IMAGE_FOR_AI;
	m_rcDraw = QRect( 0 , 0 , 0 , 0 );
	blink_flag = 0;
	/*lel*/
#if 1
//	flag_temper_humi = 0;
#endif
	/*end*/
	yxstatus = 0;
	yxstatus1 = 0;
	box = new QMessageBox(AfxGetWidget());
//	pixmap.load(Path + QString(m_text));					//配置文件中text字段复用!
}/*}}}*/

/*lel*/
#if 1
CImage_Yc::CImage_Yc(QString m_picname)
{/*{{{*/
	m_wObjectType = IMAGE_FOR_AI;
	m_rcDraw = QRect( 0 , 0 , 0 , 0 );
	blink_flag = 0;
//	flag_temper_humi = 0;
	yxstatus = 0;
	yxstatus1 = 0;
	box = new QMessageBox(AfxGetWidget());
//	pixmap.load(Path + QString(m_text));					//配置文件中text字段复用!
	pixmap.load(Path + QString(m_picname));					//配置文件中text字段复用!
}/*}}}*/
#endif
/*end*/

CImage_Yc::~CImage_Yc()
{
	pixmap.detach();
}

BOOL CImage_Yc::DrawElemnt( QPainter *pPainter )
{/*{{{*/
	if( !pPainter )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}

	pPainter->save();

	if(blink_flag == 2){
	//	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		pPainter->setBrush(QBrush(Qt::red));
		pPainter->setPen(Qt::red);
	}else if(blink_flag == 1){
		pPainter->setBrush(QBrush(Qt::yellow));
		pPainter->setPen(Qt::yellow);
	}else{
	//	printf("################################\n");
	/*lel*/
#if 0
		pPainter->setBrush(QBrush(Qt::green));
		pPainter->setPen(Qt::green);
#else
		pPainter->setPen(Qt::NoPen);
#endif
	/*end*/
	}
	pPainter->drawRect(m_rcDraw);
	/*lel*/
#if 0
	QPixmap pixmap;
//	pixmap.load(Path + QString(m_text));					//配置文件中text字段复用!
	pixmap.load(Path + QString(m_picname));					//配置文件中text字段复用!
#else
	pPainter->drawPixmap(m_rcDraw, pixmap);
//	printf("%s %d m_picintroduce = %s\n", __func__, __LINE__, m_picintroduce);
//	DrawText(pPainter, m_picintroduce, m_rcLoginInfo);
	/*end*/
#endif
#if 0
	QImage image;
//	image.load(Path + QString(m_text));
	image.load(Path + QString(m_picname));
	pPainter->drawImage(m_rcDraw, image);
	image.detach();
#endif
	pPainter->restore();
	return TRUE ;
}/*}}}*/

BOOL CImage_Yc::InitData( )
{/*{{{*/
	InitYkData();
	return FALSE ;
}/*}}}*/

BOOL CImage_Yc::Create( QRect rc )
{/*{{{*/
	m_rcDraw = rc ;
	return TRUE ;
}/*}}}*/

BOOL CImage_Yc::CompareAttributeData( )
{/*{{{*/
	int AttributeDataSize = Yc_ElementAttributeArray.size();	//不可理喻!
	/*lel*/
#if 0
	if( AttributeDataSize != 1 )								//数据是在哪里关联的!
		return FALSE;
#endif
	/*end*/
	WORD DevState = COMM_ABNORMAL;
	PPIC_ATTRIBUTE_DATA_ELEMENT tmpData = Yc_ElementAttributeArray.at(0);

	AfxGetDevCommState(tmpData->wBusNo, tmpData->wAddr, DevState);
	if(DevState == COMM_ABNORMAL){
		blink_flag = 0;
		printf("---- %s %d ----\n", __func__, __LINE__);
		qDebug() << "----设备状态error!----";
		return FALSE;
	}
	if( tmpData->byDataType == PIC_ATTRIBUTE_DATA_ELEMENT::YC_DTYPE )
	{
		float fVal = 0;
		AfxGetYc(tmpData->wBusNo, tmpData->wAddr, tmpData->wPntNo, fVal);
	//	printf("#####%s %d wBusNo = %d wAddr = %d wPntNo = %d fVal = %f\n", __func__, __LINE__, tmpData->wBusNo, tmpData->wAddr, tmpData->wPntNo, fVal);
		tmpData->dbVal = fVal;
		valued = tmpData->RelaYk.valuebottom + (float)(tmpData->RelaYk.valuetop - tmpData->RelaYk.valuebottom) / (20 - 4) * fabs(fVal - 4);
		f_valued = valued;
	//	printf("##### %s %d abs(fval - 4) = %f\n", __func__, __LINE__, fabs(fVal - 4));
		printf("##### %s %d valued = %f, valuetop = %d, valuebottom = %d, fVal = %f toppole = %d, bottompole = %d, top = %d, bottom = %d\n", __func__, __LINE__, valued, tmpData->RelaYk.valuetop, tmpData->RelaYk.valuebottom, fVal, tmpData->RelaYk.toppole, tmpData->RelaYk.bottompole, tmpData->RelaYk.top, tmpData->RelaYk.bottom);
		if((valued >= tmpData->RelaYk.toppole) || (valued <= tmpData->RelaYk.bottompole))		//越上限极红色!
			blink_flag = 2;
		else if((valued >= tmpData->RelaYk.top) || (valued <= tmpData->RelaYk.bottom))			//越上限黄色!
			blink_flag = 1;
		else																					//正常范围绿色!
			blink_flag = 0;
	}
	/*lel*/
#if 1
//	if(tmpData->RelaYk.reservation == 2){
	if(w_primitivetype == CPage::TEMPERATURE_HUMIDITY_SENSOR){
	//	flag_temper_humi = 1;
		blink_flag_humi = blink_flag;
		PPIC_ATTRIBUTE_DATA_ELEMENT tmpData1 = Yc_ElementAttributeArray.at(1);

		AfxGetDevCommState(tmpData1->wBusNo, tmpData1->wAddr, DevState);
		if(DevState == COMM_ABNORMAL){
			blink_flag = 0;
			printf("---- %s %d ----\n", __func__, __LINE__);
			qDebug() << "----设备状态error!----";
			return FALSE;
		}
		if( tmpData1->byDataType == PIC_ATTRIBUTE_DATA_ELEMENT::YC_DTYPE )
		{
			float fVal = 0;
			AfxGetYc(tmpData1->wBusNo, tmpData1->wAddr, tmpData1->wPntNo, fVal);
			tmpData1->dbVal = fVal;
			valued1 = tmpData1->RelaYk1.valuebottom + (float)(tmpData1->RelaYk1.valuetop - tmpData1->RelaYk1.valuebottom) / (20 - 4) * fabs(fVal - 4);
			f_valued1 = valued1;
		//	printf("##### %s %d abs(fval - 4) = %f\n", __func__, __LINE__, fabs(fVal - 4));
			printf("##### %s %d valued1 = %f, valuetop = %d, valuebottom = %d, fVal = %f toppole = %d, bottompole = %d, top = %d, bottom = %d\n", __func__, __LINE__, valued1, tmpData1->RelaYk1.valuetop, tmpData1->RelaYk1.valuebottom, fVal, tmpData1->RelaYk1.toppole, tmpData1->RelaYk1.bottompole, tmpData1->RelaYk1.top, tmpData1->RelaYk1.bottom);
			if((valued1 >= tmpData1->RelaYk1.toppole) || (valued1 <= tmpData1->RelaYk1.bottompole))		//越上限极红色!
				blink_flag = 2;
			else if((valued1 >= tmpData1->RelaYk1.top) || (valued1 <= tmpData1->RelaYk1.bottom))			//越上限黄色!
				blink_flag = 1;
			else																					//正常范围绿色!
				blink_flag = 0;
		}
		blink_flag = (blink_flag_humi > blink_flag) ? blink_flag_humi : blink_flag;
	}
	else
	//	flag_temper_humi = 0;
#else
#endif
	/*end*/
//	printf("blink_flag = %d\n", blink_flag);
#if 0
	if(Yx_ElementAttributeArray.size() < 2){
	//	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
		blink_flag = 0;
		return FALSE;
	}
#endif
	tmpData = Yx_ElementAttributeArray.at(0);
	if(tmpData->byDataType == PIC_ATTRIBUTE_DATA_ELEMENT::YX_DTYPE){
		if(tmpData->byAttributeDataByte == 1){						//受控
			AfxGetYx(tmpData->wBusNo, tmpData->wAddr, tmpData->wPntNo, yxstatus);
			printf("\n########	%d %d %d yx1:%d	########\n", tmpData->wBusNo, tmpData->wAddr, tmpData->wPntNo, yxstatus);
		}
	}
	tmpData = Yx_ElementAttributeArray.at(1);
	if(tmpData->byDataType == PIC_ATTRIBUTE_DATA_ELEMENT::YX_DTYPE){
		if(tmpData->byAttributeDataByte == 2){						//主控
			AfxGetYx(tmpData->wBusNo, tmpData->wAddr, tmpData->wPntNo, yxstatus1);
			printf("########	%d %d %d yx2:%d	########\n", tmpData->wBusNo, tmpData->wAddr, tmpData->wPntNo, yxstatus1);
			tmpData->dbVal = yxstatus1;
		}
	}

	/*
	 * 1.	预置	要考虑是否需要预置
	 *	AfxGetWidget()->YkDlg->slots_YkPrepareBtn_clicked();
	 * 2.	执行合
	 *	AfxGetWidget()->YkDlg->slots_YkExecBtn_clicked();
	 */
	printf("********	%d	%d	********\n\n", yxstatus, yxstatus1);
	if((yxstatus == 0) && (yxstatus1 == 1) && (blink_flag == 2)){						//合闸!
//	if((yxstatus == 0) && (yxstatus1 == 1)){						//合闸!
		//	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
			SetAction(BREAKER_CLOSE);
			AfxGetWidget()->YkDlg->slots_YkPrepareBtn_clicked();
			sleep(2);						//预置和执行之间隔了100ms，这个时间够不够，反正已经影响操控了!
			AfxGetWidget()->YkDlg->slots_YkExecBtn_clicked();
	}
	/*
	 * 1.	预置	要考虑是否需要预置
	 *	AfxGetWidget()->YkDlg->slots_YkPrepareBtn_clicked();
	 * 2.	执行分
	 *	AfxGetWidget()->YkDlg->slots_YkExecBtn_clicked();
	 */
	if((yxstatus == 1) && (yxstatus1 == 0) && (blink_flag != 2)){						//分闸
			SetAction(BREAKER_OPEN);
			AfxGetWidget()->YkDlg->slots_YkPrepareBtn_clicked();
			sleep(2);						//预置和执行之间隔了100ms，这个时间够不够，反正已经影响操控了!
			AfxGetWidget()->YkDlg->slots_YkExecBtn_clicked();
	}
	return FALSE;
}/*}}}*/

BOOL CImage_Yc::ProcessMouseDown(QMouseEvent *pEvent)
{/*{{{*/
	qDebug() << "ProcessMouseDown";
	memset(str, 0, sizeof(str));
	//	AfxGetWidget()->update(m_rcDraw);	//06-12
	switch(w_primitivetype){
		case CPage::LIQUID_LEVEL_SENSOR:
			sprintf(str, "%.2f m", valued);
			break;

		case CPage::TEMPERATURE_HUMIDITY_SENSOR:
			sprintf(str, "温度:%.2f C 湿度:%.1f %%", valued, valued1);
			break;

		case CPage::GAS_DETECTOR:
			sprintf(str, "%.2f ppm", valued);
			break;

		default:
			break;
	}

	box->setText(str);
	box->move(pEvent->globalPos());
	//	box.exec();
	box->show();						//关闭时出问题，使用时倒是没有问题!这跟new有关! 上午有问题，下午正常，不可理喻!
//	AfxGetWidget()->update(m_rcDraw);	//06-12
//	QMessageBox::information(AfxGetWidget(), "wahaha", str);		//为什么没有使用exec或者show函数可以直接显示!
	return TRUE;
}/*}}}*/

void CImage_Yc::SetAction(BYTE Action)
{/*{{{*/
	BYTE byYkSetState = FALSE;

	switch( Action ){
	case BREAKER_OPEN:
		YkActionType = YKACTIONOPEN;			//0
		if( YkData[YkActionType].byAction == YK_OPEN )
			byYkSetState = TRUE;
		break;
	case BREAKER_CLOSE:
		YkActionType = YKACTIONCLOSE;			//1
		if( YkData[YkActionType].byAction == YK_CLOSE )
			byYkSetState = TRUE;
		break;
	default:
		break;
	}

	if( !byYkSetState ){
		//		QMessageBox::warning(AfxGetWidget() , UTF8("警告") , UTF8("没有配置对应操作遥控点"),QMessageBox::Yes );
		qDebug() << "装置状态不通!";
		return ;
	}
	else
		AfxGetWidget()->YkDlg->tShowParameter(TWOPOS,YkData[YkActionType], NULL);
}/*}}}*/

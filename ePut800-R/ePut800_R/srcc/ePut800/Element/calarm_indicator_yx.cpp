#include "calarm_indicator_yx.h"
const QString Path = "/usr/share/ePut800/Pics/";

CAlarm_indicator_yx::CAlarm_indicator_yx()
{/*{{{*/
	m_wObjectType = LINKER;
	m_rcDraw = QRect( 0 , 0 , 0 , 0 );
	byVal1 = 0;
	byVal = 0;
	blink_flag = 2;
//	box = new QMessageBox(AfxGetWidget());
	//	box = new QMessageBox(QMessageBox::Information, "数据", str, QMessageBox::Yes, this);
}/*}}}*/

CAlarm_indicator_yx::CAlarm_indicator_yx(QString m_picname)
{/*{{{*/
	m_wObjectType = LINKER;
	m_rcDraw = QRect( 0 , 0 , 0 , 0 );
	byVal1 = 0;
	byVal = 0;
	blink_flag = 2;
	pixmap.load(Path + QString(m_picname));
//	box = new QMessageBox(AfxGetWidget());
	//	box = new QMessageBox(QMessageBox::Information, "数据", str, QMessageBox::Yes, this);
}/*}}}*/

CAlarm_indicator_yx::~CAlarm_indicator_yx()
{
	//	delete box;
	pixmap.detach();
}

BOOL CAlarm_indicator_yx::DrawElemnt( QPainter *pPainter )
{/*{{{*/
	if( !pPainter )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}


	pPainter->save();
	if(blink_flag == 0){
		pPainter->setPen(Qt::green);
		pPainter->setBrush(Qt::green);
	}
	else{
		/*lel*/
#if 0
		pPainter->setPen(Qt::green);
		pPainter->setBrush(Qt::green);
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
	pPainter->drawPixmap(m_rcDraw, pixmap);
	pixmap.detach();
#else
	pPainter->drawPixmap(m_rcDraw, pixmap);
#endif
	/*end*/
	pPainter->restore();
	return TRUE ;
}/*}}}*/

BOOL CAlarm_indicator_yx::InitData( )
{/*{{{*/
	InitYkData();
	return FALSE ;
}/*}}}*/

BOOL CAlarm_indicator_yx::Create( QRect rc )
{/*{{{*/
	m_rcDraw = rc ;
	return TRUE ;
}/*}}}*/

BOOL CAlarm_indicator_yx::CompareAttributeData( )
{/*{{{*/
	int AttributeDataSize = Yx_ElementAttributeArray.size();
	if( AttributeDataSize < 2){
		byVal1 = 0;
		qDebug() << "遥信点数少于两个";
		return FALSE;
	}

	int AttributeDataNo = 0;
	WORD DevState = COMM_ABNORMAL;
	for( AttributeDataNo = 0; AttributeDataNo < AttributeDataSize; AttributeDataNo++ )
	{
		PPIC_ATTRIBUTE_DATA_ELEMENT tmpData = Yx_ElementAttributeArray.at(AttributeDataNo);
		switch( tmpData->byDataType )
		{
		case PIC_ATTRIBUTE_DATA_ELEMENT::YX_DTYPE:
			if( tmpData->byAttributeDataByte == 1 )				//位号1 for controled
			{
				AfxGetDevCommState(tmpData->wBusNo, tmpData->wAddr, DevState);
				if(DevState == COMM_ABNORMAL){
					byVal1 = 0;
					printf("---- %s %d ----\n", __func__, __LINE__);
					qDebug() << "-设备状态error!-";
					return FALSE;
				}
				AfxGetYx(tmpData->wBusNo,tmpData->wAddr,tmpData->wPntNo,byVal);
				//				tmpData->dbVal = byVal;
			}
			if( tmpData->byAttributeDataByte == 2 )				//位号2 for control
			{
				AfxGetDevCommState(tmpData->wBusNo, tmpData->wAddr, DevState);
				if(DevState == COMM_ABNORMAL){
					byVal1 = 0;
					printf("---- %s %d ----\n", __func__, __LINE__);
					qDebug() << "-设备状态error!-";
					return FALSE;
				}
				AfxGetYx(tmpData->wBusNo,tmpData->wAddr,tmpData->wPntNo,byVal1);
				tmpData->dbVal = byVal1;
			}
			break;
		default:
			continue;
		}
	}

	if(blink_flag != 1){
		if((byVal == 0) &&(byVal1 == 1)){
			SetAction(BREAKER_CLOSE);
			AfxGetWidget()->YkDlg->slots_YkPrepareBtn_clicked();
			usleep(2000000);
			AfxGetWidget()->YkDlg->slots_YkExecBtn_clicked();
			blink_flag = 1;
		}
	}
	if(blink_flag != 0){
		if((byVal == 1) && (byVal1 == 0)){
			SetAction(BREAKER_OPEN);
			AfxGetWidget()->YkDlg->slots_YkPrepareBtn_clicked();
			usleep(2000000);
			AfxGetWidget()->YkDlg->slots_YkExecBtn_clicked();
			blink_flag = 0;
		}
	}
	if(((byVal == 0) && (byVal1 == 0)) || ((byVal == 1) && (byVal1 == 1)))
		blink_flag = 2;
	w_blink_flag = blink_flag;
	return FALSE;
}/*}}}*/

BOOL CAlarm_indicator_yx::ProcessMouseDown(QMouseEvent *pEvent)
{/*{{{*/
#if 0
	qDebug() << "ProcessMouseDown";
	memset(str, 0, sizeof(str));
	sprintf(str, "%f", valued);
	box->setText(str);
	box->move(pEvent->globalPos());
	box->show();
#endif
	return TRUE;
}/*}}}*/

void CAlarm_indicator_yx::SetAction(BYTE Action)
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

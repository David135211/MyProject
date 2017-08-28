#include "cykdlg.h"
#include "cglobal.h"
#include "./Pages/cpage.h"
#include "./Element/ceelment.h"
CYkDlg::CYkDlg(QWidget *parent) :
	QDialog(parent)
{/*{{{*/
	YkLabel = new QLabel(UTF8("开关操作"));
	YkLabel->setAlignment( Qt::AlignCenter );
	YkRtnLabel = new QLabel(UTF8("尚无返回信息"));
	YkRtnLabel->setAlignment( Qt::AlignCenter );
	YkPrepareBtn = new QPushButton(UTF8("遥控预置"));
	YkExecBtn = new QPushButton(UTF8("遥控执行"));
	YkCancelBtn = new QPushButton(UTF8( "遥控取消" ) );

	YkEarthPrepareBtn = new QPushButton(UTF8("遥控接地预置"));
	YkEarthExecBtn = new QPushButton(UTF8("遥控接地执行"));
	YkEarthCancelBtn = new QPushButton(UTF8( "遥控接地取消" ) );

	QGridLayout *YkLabelgirdLayout = new QGridLayout;
	YkLabelgirdLayout->addWidget(YkLabel,0,0,1,1);
	QGridLayout *YkRtnLabelgirdLayout = new QGridLayout;
	YkRtnLabelgirdLayout->addWidget(YkRtnLabel,0,0,1,1);

	QHBoxLayout * YkBtnLayout = new QHBoxLayout;
	YkBtnLayout->setSpacing(60);
	YkBtnLayout->addWidget(YkPrepareBtn);
	YkBtnLayout->addWidget(YkExecBtn);
	YkBtnLayout->addWidget(YkCancelBtn);

	QHBoxLayout * YkEarthBtnLayout = new QHBoxLayout;
	YkEarthBtnLayout->addWidget(YkEarthPrepareBtn);
	YkEarthBtnLayout->addWidget(YkEarthExecBtn);
	YkEarthBtnLayout->addWidget(YkEarthCancelBtn);

	QVBoxLayout * dlgLayout = new QVBoxLayout;
	dlgLayout->setMargin(20);
	dlgLayout->setSpacing(20);
	dlgLayout->addLayout(YkLabelgirdLayout);
	dlgLayout->addLayout(YkBtnLayout);
	//if( TwoOrThree == THREEPOS )
	//dlgLayout->addLayout(YkEarthBtnLayout);
	dlgLayout->addLayout(YkRtnLabelgirdLayout);
	setLayout(dlgLayout);
	hide();

	YkExecBtn->setDisabled(TRUE);
	YkCancelBtn->setDisabled(TRUE);

	connect(YkPrepareBtn , SIGNAL(clicked()) , this , SLOT(slots_YkPrepareBtn_clicked()));
	connect(YkExecBtn , SIGNAL(clicked()) , this , SLOT(slots_YkExecBtn_clicked()));
	connect(YkCancelBtn , SIGNAL(clicked()) , this , SLOT(slots_YkCancelBtn_clicked()));
	connect(AfxGetGlobal() , SIGNAL(Signal_YkRtnInRealThread(unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned char,unsigned short)) ,
			this , SLOT(slot_YkRtnInRealThread(unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned char,unsigned short)));
	connect(AfxGetGlobal() , SIGNAL(Signal_YkRtnTimeOut()), this , SLOT(slot_YkTimeOut()) );
	setFixedSize(400,300);
	setWindowFlags(Qt::WindowCloseButtonHint| Qt::Window );
}/*}}}*/

void CYkDlg::slots_YkPrepareBtn_clicked()
{/*{{{*/
	YkDlgData.byAction = YK_PREPARE;				//YkDlgData:的值在tShowParameter()函数中被赋值!
//	printf(" slots_YkPrepareBtn_clicked  p %d %d\n ",YkDlgData.wBusNoDest ,YkDlgData.wAddrDest);
	WORD DevState = COMM_ABNORMAL;
	AfxGetDevCommState(YkDlgData.wBusNoDest ,YkDlgData.wAddrDest,DevState);
//	printf(" %s %d wBusNoDest = %d wAddrDest = %d wPntNoDest = %d wBusNoSrc = %d wAddrSrc = %d wPntNoSrc = %d YK_PREPARE = %d wVal = %d\n ", __func__, __LINE__, YkDlgData.wBusNoDest, YkDlgData.wAddrDest, YkDlgData.wPntNoDest, YkDlgData.wBusNoSrc, YkDlgData.wAddrSrc, YkDlgData.wPntNoSrc, YK_PREPARE, YkDlgData.wVal);
//	DevState = COMM_NOMAL;
	if(  DevState == COMM_NOMAL )
	{
		AfxSengYK( YkDlgData.wBusNoDest ,YkDlgData.wAddrDest ,YkDlgData.wPntNoDest,
				YkDlgData.wBusNoSrc,YkDlgData.wAddrSrc,YkDlgData.wPntNoSrc,
				YkDlgData.byAction,YkDlgData.wVal);
		time(&AfxGetGlobal()->YkBeginTime);
		YkRtnLabel->setText(UTF8("预置中！"));
		//printf(" YkActionType \n");
	}
	else
		//		QMessageBox::warning(this , UTF8("警告") , UTF8("装置状态不通!!!"),QMessageBox::Yes );			//在ePut800中存在但是因为ePut800_R中槽函数被直接调用而不是通过原来的机制所以这个对话框会提示跨线程
		qDebug() << __FILE__ << __LINE__ << "装置状态不通!";
}/*}}}*/

void CYkDlg::slots_YkExecBtn_clicked()
{/*{{{*/
	YkDlgData.byAction = YK_EXEC;
	WORD DevState = COMM_ABNORMAL;
	AfxGetDevCommState(YkDlgData.wBusNoDest,YkDlgData.wAddrDest,DevState);

//	printf(" %s %d wBusNoDest = %d wAddrDest = %d wPntNoDest = %d wBusNoSrc = %d wAddrSrc = %d wPntNoSrc = %d YK_PREPARE = %d wVal = %d\n ", __func__, __LINE__, YkDlgData.wBusNoDest, YkDlgData.wAddrDest, YkDlgData.wPntNoDest, YkDlgData.wBusNoSrc, YkDlgData.wAddrSrc, YkDlgData.wPntNoSrc, YK_PREPARE, YkDlgData.wVal);
//	DevState = COMM_NOMAL;
	if(  DevState == COMM_NOMAL )
	{
		AfxSengYK( YkDlgData.wBusNoDest,YkDlgData.wAddrDest,YkDlgData.wPntNoDest,
				YkDlgData.wBusNoSrc,YkDlgData.wAddrSrc,YkDlgData.wPntNoSrc,
				YkDlgData.byAction ,YkDlgData.wVal);
	//	printf("%s %d YkDlgData.wPntNoDest = %d\n", __func__, __LINE__, YkDlgData.wPntNoDest);
		time(&AfxGetGlobal()->YkBeginTime);
		YkRtnLabel->setText(UTF8("执行中！"));
	}
	else
		//		QMessageBox::warning(this , UTF8("警告") , UTF8("装置状态不通!!!"),QMessageBox::Yes );			//在ePut800中存在但是因为ePut800_R中槽函数被直接调用而不是通过原来的机制所以这个对话框会提示跨线程
		qDebug() << __FILE__ << __LINE__ << "装置状态不通!";
}/*}}}*/

void CYkDlg::slots_YkCancelBtn_clicked()
{/*{{{*/
	YkDlgData.byAction = YK_CANCEL;
	WORD DevState = COMM_ABNORMAL;
	AfxGetDevCommState(YkDlgData.wBusNoDest ,YkDlgData.wAddrDest,DevState);
	if(  DevState == COMM_NOMAL )
	{
		AfxSengYK( YkDlgData.wBusNoDest ,YkDlgData.wAddrDest ,YkDlgData.wPntNoDest,
				YkDlgData.wBusNoSrc,YkDlgData.wAddrSrc,YkDlgData.wPntNoSrc,
				YkDlgData.byAction,YkDlgData.wVal);
		time(&AfxGetGlobal()->YkBeginTime);
		YkRtnLabel->setText(UTF8("取消中！"));
	}
	else
		QMessageBox::warning(this , UTF8("警告") , UTF8("装置状态不通!!!"),QMessageBox::Yes );
}/*}}}*/

void CYkDlg::slot_YkRtnInRealThread(unsigned short wBusNoDest, unsigned short wAddrDest, unsigned short nPntDest, unsigned short wBusNoSrc, unsigned short wAddrSrc, unsigned short nPntSrc, unsigned char byAction, unsigned short wVal)
{/*{{{*/
	if( (YkDlgData.wBusNoDest == wBusNoSrc) && (YkDlgData.wAddrDest == wAddrSrc)
			&& (YkDlgData.wPntNoDest == nPntSrc) )
	{   /*printf(" YkDlgData.wVal=%d wVal=%d\n ",YkDlgData.wVal,wVal);*/
		if( ( YkDlgData.byAction == YK_PREPARE) && (byAction == YK_PREPARE_RTN) )
		{
			if( YkDlgData.wVal == wVal )
			{   /*printf("预置成功！\n");*/
				YkRtnLabel->setText(UTF8("预置成功！"));
				YkPrepareBtn->setDisabled(TRUE);
				YkExecBtn->setDisabled(FALSE);
				YkCancelBtn->setDisabled(FALSE);
			}
			else
				YkRtnLabel->setText(UTF8("预置失败！"));
		}
		else if( (YkDlgData.byAction == YK_EXEC) && (byAction == YK_EXEC_RTN) )
		{
			if( YkDlgData.wVal == wVal )
			{/*printf("执行成功！\n");*/
				YkRtnLabel->setText(UTF8("执行成功！"));
				YkPrepareBtn->setDisabled(TRUE);
				YkExecBtn->setDisabled(TRUE);
				YkCancelBtn->setDisabled(TRUE);
			}
			else
				YkRtnLabel->setText(UTF8("执行失败！"));
		}
		else if( (YkDlgData.byAction == YK_CANCEL) && (byAction == YK_CANCEL_RTN) )
		{
			if( YkDlgData.wVal == wVal )
			{/*printf("取消成功！\n");*/
				YkRtnLabel->setText(UTF8("取消成功！"));
				YkPrepareBtn->setDisabled(FALSE);
				YkExecBtn->setDisabled(TRUE);
				YkCancelBtn->setDisabled(TRUE);
			}
			else
				YkRtnLabel->setText(UTF8("取消失败！"));
		}
		AfxGetGlobal()->YkBeginTime = 0;
	}
}/*}}}*/

void CYkDlg::slot_YkTimeOut()
{/*{{{*/
	YkRtnLabel->setText(UTF8("操作超时！"));
}/*}}}*/

void CYkDlg::tShowParameter( BYTE TwoOrThree , YKST_MSG tmpYk )
{/*{{{*/
	memcpy( &YkDlgData , &tmpYk , sizeof(YKST_MSG) );						//YkDlgData:只有在这里才被赋值!
	YkRtnLabel->setText(UTF8("尚无返回信息！"));
	YkPrepareBtn->setDisabled(FALSE);										//预置按钮可用，执行和取消不可用!
	YkExecBtn->setDisabled(TRUE);
	YkCancelBtn->setDisabled(TRUE);
	/*
	   if( TwoOrThree == CElement::TWOPOS )
	   {
	   YkPrepareBtn->setDisabled(FALSE);
	   YkExecBtn->setDisabled(TRUE);
	   YkCancelBtn->setDisabled(TRUE);
	   YkEarthPrepareBtn->setDisabled(TRUE);
	   YkEarthExecBtn->setDisabled(TRUE);
	   YkEarthCancelBtn->setDisabled(TRUE);
	   }
	   else if( TwoOrThree == CElement::THREEPOS )
	   {
	   YkPrepareBtn->setDisabled(TRUE);
	   YkExecBtn->setDisabled(TRUE);
	   YkCancelBtn->setDisabled(TRUE);
	   YkEarthPrepareBtn->setDisabled(FALSE);
	   YkEarthExecBtn->setDisabled(TRUE);
	   YkEarthCancelBtn->setDisabled(TRUE);
	   }*/
	show();
}/*}}}*/

//start			06-12
void CYkDlg::tShowParameter( BYTE TwoOrThree , YKST_MSG tmpYk, QString *flag)
{/*{{{*/
	memcpy( &YkDlgData , &tmpYk , sizeof(YKST_MSG) );						//YkDlgData:只有在这里才被赋值!
	YkRtnLabel->setText(UTF8("尚无返回信息！"));
	YkPrepareBtn->setDisabled(FALSE);
	YkExecBtn->setDisabled(TRUE);
	YkCancelBtn->setDisabled(TRUE);
	/*
	   if( TwoOrThree == CElement::TWOPOS )
	   {
	   YkPrepareBtn->setDisabled(FALSE);
	   YkExecBtn->setDisabled(TRUE);
	   YkCancelBtn->setDisabled(TRUE);
	   YkEarthPrepareBtn->setDisabled(TRUE);
	   YkEarthExecBtn->setDisabled(TRUE);
	   YkEarthCancelBtn->setDisabled(TRUE);
	   }
	   else if( TwoOrThree == CElement::THREEPOS )
	   {
	   YkPrepareBtn->setDisabled(TRUE);
	   YkExecBtn->setDisabled(TRUE);
	   YkCancelBtn->setDisabled(TRUE);
	   YkEarthPrepareBtn->setDisabled(FALSE);
	   YkEarthExecBtn->setDisabled(TRUE);
	   YkEarthCancelBtn->setDisabled(TRUE);
	   }*/
	//	show();
}/*}}}*/
//end

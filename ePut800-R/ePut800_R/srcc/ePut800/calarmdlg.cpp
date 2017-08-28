#include "calarmdlg.h"
#include "cglobal.h"
#include "./Pages/cpage.h"
#include "./Element/ceelment.h"
CAlarmDlg::CAlarmDlg(QWidget *parent) :
    QDialog(parent)
{
    m_YxAlarmTab = new QTableWidget();
    m_YxAlarmTab->setWindowFlags((m_YxAlarmTab->windowFlags()|Qt::WindowStaysOnTopHint| Qt::WindowCloseButtonHint) &~Qt::WindowMinMaxButtonsHint);
    m_YxAlarmTab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //m_YxAlarmTab->setFixedSize(600,300);
    //m_YxAlarmTab->move(100,150);
    m_YxAlarmTab->setColumnCount(3);
    m_YxAlarmTab->setRowCount(0);
    m_YxAlarmTab->setColumnWidth(0,200);
    m_YxAlarmTab->setColumnWidth(1,150);
    m_YxAlarmTab->setColumnWidth(2,150);
    QStringList Yc_header;
    Yc_header<<UTF8("名称")<<UTF8("状态")<<UTF8("时间");
    m_YxAlarmTab->setHorizontalHeaderLabels(Yc_header);

    YkLabel = new QLabel(UTF8("遥信告警"));
    YkLabel->setAlignment( Qt::AlignCenter );


    QGridLayout *YkLabelgirdLayout = new QGridLayout;
    YkLabelgirdLayout->addWidget(YkLabel,0,0,1,1);
    YkLabelgirdLayout->addWidget(m_YxAlarmTab,0,0,5,1);

    QVBoxLayout * dlgLayout = new QVBoxLayout;
    dlgLayout->setMargin(20);
    dlgLayout->setSpacing(20);
    dlgLayout->addLayout(YkLabelgirdLayout);
    setLayout(dlgLayout);
    //hide();

    setFixedSize(600,300);
    move(100,150);
    setWindowFlags(Qt::WindowCloseButtonHint| Qt::Window );

    connect(AfxGetGlobal() , SIGNAL(Signal_YxAlarm(char*,char)),this , SLOT(slots_AddYxAlarmItem(char*,char)));
}


void CAlarmDlg::slots_AddYxAlarmItem(char *chName, char byValue)
{
    char BusAddrPnt[30] = "";

    m_YxAlarmTab->insertRow( 0 );
    //printf("chName=%s\n",chName);
    m_YxAlarmTab->setItem(0,0,new QTableWidgetItem(chName));
    memset(BusAddrPnt,0,sizeof(BusAddrPnt));
    sprintf(BusAddrPnt,"%u",byValue);
    m_YxAlarmTab->setItem(0,1,new QTableWidgetItem(BusAddrPnt));
    m_YxAlarmTab->setItem(0,2,new QTableWidgetItem(AfxGetSysTimeStr()));

    if( m_YxAlarmTab->rowCount() > 100 )
    {
        m_YxAlarmTab->removeRow(100);
    }
    show();
}

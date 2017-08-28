#ifndef CYKDLG_H
#define CYKDLG_H
#include <QtGui/QtGui>
#include <QDialog>
#include "structure.h"
class CYkDlg : public QDialog
{
    Q_OBJECT
public:
    explicit CYkDlg(QWidget *parent = 0);
    //virtual void show();
    void tShowParameter( BYTE TwoOrThree , YKST_MSG tmpYk );
	void tShowParameter(BYTE, YKST_MSG, QString *);
    YKST_MSG YkDlgData;

    QLabel * YkRtnLabel;
    QLabel * YkLabel;
    QPushButton * YkPrepareBtn;
    QPushButton * YkExecBtn;
    QPushButton * YkCancelBtn;

    QPushButton * YkEarthPrepareBtn;
    QPushButton * YkEarthExecBtn;
    QPushButton * YkEarthCancelBtn;


public slots:
     void slots_YkPrepareBtn_clicked();
     void slots_YkExecBtn_clicked();
     void slots_YkCancelBtn_clicked();

     void slot_YkRtnInRealThread(unsigned short wBusNoDest, unsigned short wAddrDest, unsigned short nPntDest, unsigned short wBusNoSrc, unsigned short wAddrSrc, unsigned short nPntSrc, unsigned char byAction, unsigned short wVal);
     void slot_YkTimeOut();

};

#endif // CYKDLG_H

#ifndef CALARMDLG_H
#define CALARMDLG_H
#include <QtGui/QtGui>
#include <QDialog>
#include "structure.h"
class CAlarmDlg : public QDialog
{
    Q_OBJECT
public:
    explicit CAlarmDlg(QWidget *parent = 0);
    //virtual void show();

    YKST_MSG YkDlgData;
    QTableWidget * m_YxAlarmTab;
    QLabel * YkLabel;

public slots:
    void slots_AddYxAlarmItem( char* chName, char byValue );
};

#endif // CALARMDLG_H

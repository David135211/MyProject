#ifndef CLOGINDLG_H
#define CLOGINDLG_H
#include <QtGui/QtGui>
#include <QDialog>
#include "./Keyboard/keyboard.h"
#include "eput800lineedit.h"
class CLoginDlg : public QDialog
{
    Q_OBJECT
public:
    explicit CLoginDlg(QWidget *parent = 0);
    virtual void show();

private:

    Eput800LineEdit * usrLineEdit;
    Eput800LineEdit * pwdLineEdit;
    BYTE LoginDldState;

signals:
     void signal_1();

private slots:
     virtual void accept();
     virtual void reject();
     void LineEditClickSlot(QLineEdit * tmpQlineEdit);
};

#endif // CLOGINDLG_H

#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <QtGui/QtGui>
#include <QDialog>
#include <QMouseEvent>
#include  <QImage>
#include  <QKeyEvent>
#include "../Typedef.h"
#include "cresetpwddlg.h"

class Keyboard : public QDialog
{
    Q_OBJECT
public:
    explicit Keyboard(QWidget *parent = 0);

    enum{SYSPARAMKEYBOARD,LOGINDLGKEYBOARD};

    void paintEvent( QPaintEvent * pEvent );
    virtual void mousePressEvent(QMouseEvent *pEvent );
    virtual void mouseReleaseEvent(QMouseEvent *pEvent );
    virtual void mouseMoveEvent(QMouseEvent *pEvent );
    virtual void closeEvent(QCloseEvent *pEvent);
    void KeyBoardShow(QLineEdit * tmpQlineEdit , BYTE UsingZone = SYSPARAMKEYBOARD);
    void KeyBoardhide(QLineEdit * tmpQlineEdit , BYTE UsingZone = SYSPARAMKEYBOARD);

private:
    QRect rect[13];
    BYTE SelectRect ;
    BYTE MouseAction;
    enum{ PressMouse=0x01,ReleaseMouse};
    QLineEdit * KeyLineEdit;
    int PressX;
    int PressY;
    int PressGlobalX;
    int PressGlobalY;
    char RealSuperPwd[16];
    char InputSuperPwd[16];
    BYTE RealSuperPwdLenth;
    BYTE InputSuperPwdLenth;
    BOOL MoveState;

    cresetpwddlg * ResetPwdDlg;

    void IsResetPwd( BYTE SuperByte );


signals:
    void show_signal();
    void hide_signal();

public slots:
    //void KeyBoardShow(QLineEdit * tmpQlineEdit);

};

#endif // KEYBOARD_H

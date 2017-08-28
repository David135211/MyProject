#ifndef CRESETPWDDLG_H
#define CRESETPWDDLG_H

#include <QDialog>

class cresetpwddlg : public QDialog
{
    Q_OBJECT
public:
    explicit cresetpwddlg(QWidget *parent = 0);

    void paintEvent( QPaintEvent * pEvent );
    virtual void mousePressEvent(QMouseEvent *pEvent );
    virtual void mouseMoveEvent(QMouseEvent *pEvent );

private:
    char m_strPic[64] ; //图片路径
    QRect YesRect;
    QRect NoRect;
    int PressX;
    int PressY;
    int PressGlobalX;
    int PressGlobalY;
signals:

public slots:

};

#endif // CRESETPWDDLG_H

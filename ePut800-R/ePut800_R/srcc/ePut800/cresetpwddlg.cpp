#include "cresetpwddlg.h"
#include "cglobal.h"
#include <QPainter>
cresetpwddlg::cresetpwddlg(QWidget *parent) :
    QDialog(parent)
{
    //setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
    memset( m_strPic , 0 , sizeof(m_strPic) );
    memcpy( m_strPic , "/usr/share/ePut800/Pics/重置密码.jpg" , sizeof("/usr/share/ePut800/Pics/重置密码.jpg"));
    //memcpy( m_strPic , "/usr/share/ePut800/Pics/ResetPWD.jpg" , sizeof("/usr/share/ePut800/Pics/ResetPWD.jpg"));

    setFixedSize(67*7,31*7);    //67:31
    move((800-width())/2,(600-height())/2);

    YesRect.setRect( (80*width())/469 , (151*height())/217 , (85*width())/469 , (41*height())/217 );
    NoRect.setRect( (305*width())/469 , (151*height())/217 , (85*width())/469 , (41*height())/217 );
}

void cresetpwddlg::paintEvent( QPaintEvent * pEvent )
{
    QPainter painter(this);
    QPixmap backGround( width() , height() );
    backGround.load(m_strPic);
    painter.drawPixmap(0,0, width(),height(),UTF8(m_strPic));
}

void cresetpwddlg::mousePressEvent(QMouseEvent *pEvent )
{
    if(pEvent->button() == Qt::LeftButton)
    {
        PressX = pEvent->x();
        PressY = pEvent->y();
        PressGlobalX = pEvent->globalX();
        PressGlobalY = pEvent->globalY();
        if( YesRect.contains(pEvent->pos()) )
        {
            AfxResetPwd();
            QDialog::accept();
        }
        else if( NoRect.contains(pEvent->pos()) )
        {
            QDialog::reject();
        }
    }
}

void cresetpwddlg::mouseMoveEvent(QMouseEvent *pEvent)
{
    if(pEvent->buttons() & Qt::LeftButton)//(pEvent->button() == Qt::LeftButton)
    {
        if( abs(PressGlobalX - pEvent->globalX()) > 10 || abs(PressGlobalY - pEvent->globalY()) > 10 )
        {
            move( pEvent->globalX() - PressX ,  pEvent->globalY() - PressY );
        }
    }
}


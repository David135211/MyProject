#include "clogindlg.h"
#include "cglobal.h"
#include "./Pages/cpage.h"

CLoginDlg::CLoginDlg(QWidget *parent) :
    QDialog(parent)
{
    CGlobal * tmpGlobal = AfxGetGlobal();

     QLabel * usrLabel = new QLabel(UTF8("用户名："));
     QLabel * pwdLabel = new QLabel(UTF8("密码："));
     usrLineEdit = new Eput800LineEdit;
     pwdLineEdit = new Eput800LineEdit;
     pwdLineEdit->setEchoMode(Eput800LineEdit::Password);

     usrLineEdit->setText(tmpGlobal->CurrentUsrPwd.Usr);
     usrLineEdit->setEnabled(false);

     QGridLayout *girdLayout = new QGridLayout;
     girdLayout->addWidget(usrLabel,0,0,1,1);
     girdLayout->addWidget(pwdLabel,1,0,1,1);
     girdLayout->addWidget(usrLineEdit,0,1,1,3);
     girdLayout->addWidget(pwdLineEdit,1,1,1,3);

     QPushButton * okBtn = new QPushButton(UTF8("确定"));
     QPushButton * cancelBtn = new QPushButton(UTF8( "取消" ) );
     QHBoxLayout * btnLayout = new QHBoxLayout;
     btnLayout->setSpacing(60);
     btnLayout->addWidget(okBtn);
     btnLayout->addWidget(cancelBtn);

     QVBoxLayout * dlgLayout = new QVBoxLayout;
     dlgLayout->setMargin(40);
     dlgLayout->addLayout(girdLayout);
     dlgLayout->addLayout(btnLayout);
     setLayout(dlgLayout);

     connect(okBtn , SIGNAL(clicked()) , this , SLOT(accept()));
     connect(cancelBtn , SIGNAL(clicked()) , this , SLOT(reject()));


    // Widget * tmpWidget = AfxGetWidget();
    // Numkeyboard = new Keyboard(tmpWidget);
     connect(pwdLineEdit , SIGNAL(clicked(QLineEdit * )) , this , SLOT(LineEditClickSlot(QLineEdit * )));

     setWindowTitle(UTF8("请登录"));
     setFixedSize(400,300);
     setWindowFlags(Qt::WindowCloseButtonHint| Qt::Window );
}

void CLoginDlg::accept()
{
    CGlobal * tmpGlobal = AfxGetGlobal();
    if(usrLineEdit->text().trimmed() == UTF8(tmpGlobal->CurrentUsrPwd.Usr)
        && pwdLineEdit->text().trimmed() == UTF8(tmpGlobal->CurrentUsrPwd.Pwd) )
    {
        pwdLineEdit->clear();
        if( AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_LOGIN_DLG )
            AfxGetWidget()->Numkeyboard->hide();
        AfxSetLoginState(LoginDldState);
        QDialog::accept();
    }
    else
    {
        QMessageBox::warning(this , UTF8("警告") , UTF8("用户或密码错误!!!") ,
                             QObject::tr("确定"));
        pwdLineEdit->setFocus();
    }
}

void CLoginDlg::reject()
{
   pwdLineEdit->clear();
   if( AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_LOGIN_DLG )
       AfxGetWidget()->Numkeyboard->hide();
   QDialog::reject();
}

void CLoginDlg::show()
{
    switch( AfxGetLoginState() )
    {
    case CPage::LOGIN:
        LoginDldState = CPage::LOGOUT;
        setWindowTitle(UTF8("退出登录"));
        break;
    case CPage::LOGOUT:
        LoginDldState = CPage::LOGIN;
        setWindowTitle(UTF8("请登录"));
        break;
    }
    move(200, 150);
    QDialog::show();

}

void CLoginDlg::LineEditClickSlot(QLineEdit * tmpQlineEdit)
{
    switch( AfxGetWidget()->Numkeyboard->isVisible() )
    {
    case TRUE:
        if( AfxGetWidget()->NumkeyboardFocusWindow == Widget::BOARD_LOGIN_DLG )
            AfxGetWidget()->Numkeyboard->KeyBoardhide(tmpQlineEdit,Keyboard::LOGINDLGKEYBOARD);
        else
        {
            AfxGetWidget()->NumkeyboardFocusWindow = Widget::BOARD_LOGIN_DLG;
            AfxGetWidget()->Numkeyboard->KeyBoardShow(tmpQlineEdit,Keyboard::LOGINDLGKEYBOARD);
        }
    break;
    case FALSE:
        AfxGetWidget()->NumkeyboardFocusWindow = Widget::BOARD_LOGIN_DLG;
        AfxGetWidget()->Numkeyboard->KeyBoardShow(tmpQlineEdit,Keyboard::LOGINDLGKEYBOARD);
    break;
    }
}

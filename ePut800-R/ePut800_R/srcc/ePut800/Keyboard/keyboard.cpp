#include "keyboard.h"
#include "cglobal.h"
#include <QPainter>
#include <string.h>
#include <stdio.h>
Keyboard::Keyboard(QWidget *parent) :
    QDialog(parent)
{
    MoveState = FALSE;
//    Widget * tmpWidget = AfxGetWidget();
    ResetPwdDlg = new cresetpwddlg();
    SelectRect = 0xff;
    MouseAction = ReleaseMouse;
    RealSuperPwdLenth = 6;
    InputSuperPwdLenth = 0;
    memset( InputSuperPwd , 0 ,sizeof(InputSuperPwd) );
    memset( RealSuperPwd , 0 ,sizeof(RealSuperPwd) );
    RealSuperPwd[ 0 ] = 10;
    RealSuperPwd[ 1 ] = 10;
    RealSuperPwd[ 2 ] = 10;
    RealSuperPwd[ 3 ] = 11;
    RealSuperPwd[ 4 ] = 11;
    RealSuperPwd[ 5 ] = 11;

    //setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    //setWindowFlags(Qt::FramelessWindowHint );
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint | Qt::Window);

    setFixedSize(210,350);
    move(590,250);
    //move(700,500);
    hide();

    BYTE vbox=0,hbox=0;
    for( vbox=0;vbox<5;vbox++ )
    {
        if( vbox < 4 )
        {
            for( hbox=0;hbox<3;hbox++ )
            {
                rect[hbox+3*vbox].setRect(width()/3*hbox,height()/5*vbox,width()/3,height()/5);
            }
        }
        if( vbox == 4 )
        {
            rect[3*vbox].setRect(0,height()/5*vbox,width(),height()/5);
        }
    }
}

void Keyboard::paintEvent( QPaintEvent * pEvent )
{
    QPainter painter(this);
    BYTE i = 0;
    char tempbuf[ 10 ] = "";
/*
    if( SelectRect != 0xff )
    {
        switch( MouseAction )
        {
        case PressMouse:
            painter.setBrush(QColor( 135,150,235,255 ));
            break;
        case ReleaseMouse:
            painter.setBrush(QColor( 135,206,250,255 ));
            break;
        }
        painter.drawRect(rect[SelectRect]);

        memset(tempbuf,0,sizeof(tempbuf));
        switch( SelectRect )
        {
        case 9:
            sprintf( tempbuf , "%d" , SelectRect-9 );
        break;
        case 10:
            sprintf( tempbuf , "%s" , "." );
        break;
        case 11:
            sprintf( tempbuf , "%s" , "Back" );
        break;
        case 12:
            sprintf( tempbuf , "%s" , "Clear" );
        break;
        default:
            sprintf( tempbuf , "%d" , SelectRect+1 );
        break;
        }
        painter.drawText(rect[SelectRect],Qt::AlignCenter,tempbuf);

        if( MouseAction == ReleaseMouse )
            SelectRect  = 0xff;
        return;
    }
*/
    painter.setBrush(QColor( 135,206,250,255 ));
    painter.drawRect(0,0,width(),height());

    for( i=0;i<13;i++ )
    {
        memset(tempbuf,0,sizeof(tempbuf));
        switch( i )
        {
        case 9:
            sprintf( tempbuf , "%d" , i-9 );
        break;
        case 10:
            sprintf( tempbuf , "%s" , "." );
        break;
        case 11:
            sprintf( tempbuf , "%s" , "Back" );
        break;
        case 12:
            sprintf( tempbuf , "%s" , "Clear" );
        break;
        default:
            sprintf( tempbuf , "%d" , i+1 );
        break;
        }
        painter.drawRect(rect[i]);
        painter.drawText(rect[i],Qt::AlignCenter,tempbuf);
    }

}

void Keyboard::mousePressEvent(QMouseEvent *pEvent )
{
    if(pEvent->button() == Qt::LeftButton)
    {
        BYTE i = 0;
        for( i=0;i<13;i++ )
        {
            PressX = pEvent->x();
            PressY = pEvent->y();
            PressGlobalX = pEvent->globalX();
            PressGlobalY = pEvent->globalY();
            if( rect[ i ].contains(pEvent->pos()) )
            {
                SelectRect = i;
                MouseAction = PressMouse;
                //update(rect[ i ]);
                update();
            }
        }
    }
}

void Keyboard::mouseReleaseEvent(QMouseEvent *pEvent )
{
    if(pEvent->button() == Qt::LeftButton)
    {
        MoveState = FALSE;
        MouseAction = ReleaseMouse;
        //update(rect[ SelectRect ]);
        update();
        if( abs(PressGlobalX - pEvent->globalX()) < 100 && abs(PressGlobalY - pEvent->globalY()) < 100 )
        {
            QString InputStr = KeyLineEdit->text();
            QString tmpstr;
            switch( SelectRect )
            {
            case 0:
                InputStr += '1';
            break;
            case 1:
                InputStr += '2';
            break;
            case 2:
                InputStr += '3';
            break;
            case 3:
                InputStr += '4';
            break;
            case 4:
                InputStr += '5';
            break;
            case 5:
                InputStr += '6';
            break;
            case 6:
                InputStr += '7';
            break;
            case 7:
                InputStr += '8';
            break;
            case 8:
                InputStr += '9';
            break;
            case 9:
                InputStr += '0';
            break;
            case 10:
                InputStr += '.';
            break;
            case 11:
                InputStr.remove( InputStr.length()-1,1 );
            break;
            case 12:
                InputStr.clear();
            break;
            default:

            break;
            }
            IsResetPwd( SelectRect );
            KeyLineEdit->setText(InputStr);
        }
    }
}

void Keyboard::mouseMoveEvent(QMouseEvent *pEvent )
{
    if(pEvent->buttons() & Qt::LeftButton)//(pEvent->button() == Qt::LeftButton)
    {
        if( MoveState == FALSE )
        {
            if( abs(PressGlobalX - pEvent->globalX()) > 100 || abs(PressGlobalY - pEvent->globalY()) > 100 )
            {
                MoveState = TRUE;
                move( pEvent->globalX() - PressX ,  pEvent->globalY() - PressY );
                SelectRect = 0xff;
                update();
                //repaint();
            }
        }
        else
        {
            move( pEvent->globalX() - PressX ,  pEvent->globalY() - PressY );
            SelectRect = 0xff;
            update();
        }
    }
}

void Keyboard::closeEvent(QCloseEvent *pEvent)
{
    KeyBoardhide(KeyLineEdit);
}

void Keyboard::KeyBoardShow(QLineEdit *tmpQlineEdit, BYTE UsingZone)
{
    move(590,250);
    QDialog::show();

    tmpQlineEdit->setFocus();
    KeyLineEdit = tmpQlineEdit;
    if( UsingZone == SYSPARAMKEYBOARD )
    {
        emit show_signal();
    }
}


void Keyboard::KeyBoardhide(QLineEdit *tmpQlineEdit, BYTE UsingZone)
{
    QDialog::hide();

    tmpQlineEdit->clearFocus();
    KeyLineEdit = NULL;
    if( UsingZone == SYSPARAMKEYBOARD )
        emit hide_signal();
}

void Keyboard::IsResetPwd( BYTE SuperByte )
{
    switch( SuperByte )
    {
    case 10:
        InputSuperPwd[InputSuperPwdLenth++] = SuperByte;
    break;
    case 11:
        InputSuperPwd[InputSuperPwdLenth++] = SuperByte;
    break;
    default:
        memset( InputSuperPwd , 0 , sizeof( InputSuperPwd ) );
        InputSuperPwdLenth = 0;
    break;
    }
    if( InputSuperPwdLenth <= RealSuperPwdLenth )
    {
        if( 0 == memcmp( RealSuperPwd , InputSuperPwd , InputSuperPwdLenth))
        {
            if( InputSuperPwdLenth == RealSuperPwdLenth  )
            {
                ResetPwdDlg->show();
                memset( InputSuperPwd , 0 , sizeof( InputSuperPwd ) );
                InputSuperPwdLenth = 0;
            }
        }
        else
        {
            memset( InputSuperPwd , 0 , sizeof( InputSuperPwd ) );
            InputSuperPwdLenth = 0;
        }
    }
    else if( InputSuperPwdLenth > RealSuperPwdLenth )
    {
        memset( InputSuperPwd , 0 , sizeof( InputSuperPwd ) );
        InputSuperPwdLenth = 0;
    }
}


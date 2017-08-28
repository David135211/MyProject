#include "eput800lineedit.h"

Eput800LineEdit::Eput800LineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}

void Eput800LineEdit::mousePressEvent(QMouseEvent *pEvent )
{
    if(pEvent->button() == Qt::LeftButton)
    {
        emit clicked(this);
    }
    QLineEdit::mousePressEvent(pEvent);
}

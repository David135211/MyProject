#ifndef EPUT800LINEEDIT_H
#define EPUT800LINEEDIT_H

#include <QLineEdit>
#include <QMouseEvent>

class Eput800LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit Eput800LineEdit(QWidget *parent = 0);
    virtual void mousePressEvent(QMouseEvent *pEvent );

signals:
    void clicked(QLineEdit * tmpQlineEdit);

public slots:

};

#endif // EPUT800LINEEDIT_H

/****************************************************************************
** Meta object code from reading C++ file 'eput800lineedit.h'
**
** Created: Tue Aug 8 19:45:43 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "eput800lineedit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'eput800lineedit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Eput800LineEdit[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      30,   17,   16,   16, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Eput800LineEdit[] = {
    "Eput800LineEdit\0\0tmpQlineEdit\0"
    "clicked(QLineEdit*)\0"
};

const QMetaObject Eput800LineEdit::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_Eput800LineEdit,
      qt_meta_data_Eput800LineEdit, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Eput800LineEdit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Eput800LineEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Eput800LineEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Eput800LineEdit))
        return static_cast<void*>(const_cast< Eput800LineEdit*>(this));
    return QLineEdit::qt_metacast(_clname);
}

int Eput800LineEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: clicked((*reinterpret_cast< QLineEdit*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Eput800LineEdit::clicked(QLineEdit * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE

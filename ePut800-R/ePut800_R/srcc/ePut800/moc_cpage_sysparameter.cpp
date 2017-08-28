/****************************************************************************
** Meta object code from reading C++ file 'cpage_sysparameter.h'
**
** Created: Tue Aug 8 19:46:03 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "Pages/cpage_sysparameter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cpage_sysparameter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CPage_SysParameter[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      25,   20,   19,   19, 0x0a,
      62,   54,   19,   19, 0x0a,
      94,   19,   19,   19, 0x0a,
     109,   19,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CPage_SysParameter[] = {
    "CPage_SysParameter\0\0text\0"
    "TextChangeFocusRect(QString)\0byState\0"
    "LoginStateChange_SysParam(BYTE)\0"
    "NumBoardHide()\0NumBoardShow()\0"
};

const QMetaObject CPage_SysParameter::staticMetaObject = {
    { &CPage::staticMetaObject, qt_meta_stringdata_CPage_SysParameter,
      qt_meta_data_CPage_SysParameter, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CPage_SysParameter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CPage_SysParameter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CPage_SysParameter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CPage_SysParameter))
        return static_cast<void*>(const_cast< CPage_SysParameter*>(this));
    return CPage::qt_metacast(_clname);
}

int CPage_SysParameter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: TextChangeFocusRect((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: LoginStateChange_SysParam((*reinterpret_cast< BYTE(*)>(_a[1]))); break;
        case 2: NumBoardHide(); break;
        case 3: NumBoardShow(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

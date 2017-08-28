/****************************************************************************
** Meta object code from reading C++ file 'cpage_diagnostic.h'
**
** Created: Tue Aug 8 19:46:02 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "Pages/cpage_diagnostic.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cpage_diagnostic.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CPage_Diagnostic[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   18,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      50,   18,   17,   17, 0x08,
      83,   77,   17,   17, 0x08,
     106,   77,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CPage_Diagnostic[] = {
    "CPage_Diagnostic\0\0str\0signal_MessageShow(QString)\0"
    "slots_MessageShow(QString)\0index\0"
    "slots_BusNoChange(int)\0slots_DevAddrChange(int)\0"
};

const QMetaObject CPage_Diagnostic::staticMetaObject = {
    { &CPage::staticMetaObject, qt_meta_stringdata_CPage_Diagnostic,
      qt_meta_data_CPage_Diagnostic, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CPage_Diagnostic::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CPage_Diagnostic::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CPage_Diagnostic::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CPage_Diagnostic))
        return static_cast<void*>(const_cast< CPage_Diagnostic*>(this));
    return CPage::qt_metacast(_clname);
}

int CPage_Diagnostic::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: signal_MessageShow((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: slots_MessageShow((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: slots_BusNoChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: slots_DevAddrChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void CPage_Diagnostic::signal_MessageShow(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'cglobal.h'
**
** Created: Tue Aug 8 19:45:39 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "cglobal.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cglobal.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CGlobal[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,    9,    8,    8, 0x05,
     112,   41,    8,    8, 0x05,
     257,    8,    8,    8, 0x05,
     294,  279,    8,    8, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_CGlobal[] = {
    "CGlobal\0\0byState\0LoginStateChanged(BYTE)\0"
    "wBusNoDest,wAddrDest,nPntDest,wBusNoSrc,wAddrSrc,nPntSrc,byAction,wVal\0"
    "Signal_YkRtnInRealThread(unsigned short,unsigned short,unsigned short,"
    "unsigned short,unsigned short,unsigned short,unsigned char,unsigned sh"
    "ort)\0"
    "Signal_YkRtnTimeOut()\0chName,byValue\0"
    "Signal_YxAlarm(char*,char)\0"
};

const QMetaObject CGlobal::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CGlobal,
      qt_meta_data_CGlobal, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CGlobal::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CGlobal::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CGlobal::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CGlobal))
        return static_cast<void*>(const_cast< CGlobal*>(this));
    return QObject::qt_metacast(_clname);
}

int CGlobal::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: LoginStateChanged((*reinterpret_cast< BYTE(*)>(_a[1]))); break;
        case 1: Signal_YkRtnInRealThread((*reinterpret_cast< unsigned short(*)>(_a[1])),(*reinterpret_cast< unsigned short(*)>(_a[2])),(*reinterpret_cast< unsigned short(*)>(_a[3])),(*reinterpret_cast< unsigned short(*)>(_a[4])),(*reinterpret_cast< unsigned short(*)>(_a[5])),(*reinterpret_cast< unsigned short(*)>(_a[6])),(*reinterpret_cast< unsigned char(*)>(_a[7])),(*reinterpret_cast< unsigned short(*)>(_a[8]))); break;
        case 2: Signal_YkRtnTimeOut(); break;
        case 3: Signal_YxAlarm((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< char(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void CGlobal::LoginStateChanged(BYTE _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CGlobal::Signal_YkRtnInRealThread(unsigned short _t1, unsigned short _t2, unsigned short _t3, unsigned short _t4, unsigned short _t5, unsigned short _t6, unsigned char _t7, unsigned short _t8)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CGlobal::Signal_YkRtnTimeOut()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void CGlobal::Signal_YxAlarm(char * _t1, char _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE

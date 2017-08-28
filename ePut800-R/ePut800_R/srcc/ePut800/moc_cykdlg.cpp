/****************************************************************************
** Meta object code from reading C++ file 'cykdlg.h'
**
** Created: Tue Aug 8 19:45:42 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "cykdlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cykdlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CYkDlg[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x0a,
      37,    7,    7,    7, 0x0a,
      63,    7,    7,    7, 0x0a,
     162,   91,    7,    7, 0x0a,
     305,    7,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CYkDlg[] = {
    "CYkDlg\0\0slots_YkPrepareBtn_clicked()\0"
    "slots_YkExecBtn_clicked()\0"
    "slots_YkCancelBtn_clicked()\0"
    "wBusNoDest,wAddrDest,nPntDest,wBusNoSrc,wAddrSrc,nPntSrc,byAction,wVal\0"
    "slot_YkRtnInRealThread(unsigned short,unsigned short,unsigned short,un"
    "signed short,unsigned short,unsigned short,unsigned char,unsigned shor"
    "t)\0"
    "slot_YkTimeOut()\0"
};

const QMetaObject CYkDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CYkDlg,
      qt_meta_data_CYkDlg, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CYkDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CYkDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CYkDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CYkDlg))
        return static_cast<void*>(const_cast< CYkDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int CYkDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slots_YkPrepareBtn_clicked(); break;
        case 1: slots_YkExecBtn_clicked(); break;
        case 2: slots_YkCancelBtn_clicked(); break;
        case 3: slot_YkRtnInRealThread((*reinterpret_cast< unsigned short(*)>(_a[1])),(*reinterpret_cast< unsigned short(*)>(_a[2])),(*reinterpret_cast< unsigned short(*)>(_a[3])),(*reinterpret_cast< unsigned short(*)>(_a[4])),(*reinterpret_cast< unsigned short(*)>(_a[5])),(*reinterpret_cast< unsigned short(*)>(_a[6])),(*reinterpret_cast< unsigned char(*)>(_a[7])),(*reinterpret_cast< unsigned short(*)>(_a[8]))); break;
        case 4: slot_YkTimeOut(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'cpage_dataquery.h'
**
** Created: Tue Aug 8 19:46:01 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "Pages/cpage_dataquery.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cpage_dataquery.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CPage_DataQuery[] = {

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
      23,   17,   16,   16, 0x08,
      60,   51,   16,   16, 0x08,
      88,   51,   16,   16, 0x08,
     114,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CPage_DataQuery[] = {
    "CPage_DataQuery\0\0index\0"
    "TreeItemChange(QModelIndex)\0datetime\0"
    "BeginTimeChanged(QDateTime)\0"
    "EndTimeChanged(QDateTime)\0"
    "SlotQuerySoeByTime()\0"
};

const QMetaObject CPage_DataQuery::staticMetaObject = {
    { &CPage::staticMetaObject, qt_meta_stringdata_CPage_DataQuery,
      qt_meta_data_CPage_DataQuery, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CPage_DataQuery::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CPage_DataQuery::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CPage_DataQuery::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CPage_DataQuery))
        return static_cast<void*>(const_cast< CPage_DataQuery*>(this));
    return CPage::qt_metacast(_clname);
}

int CPage_DataQuery::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: TreeItemChange((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: BeginTimeChanged((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 2: EndTimeChanged((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 3: SlotQuerySoeByTime(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

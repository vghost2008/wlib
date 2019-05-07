/****************************************************************************
** Meta object code from reading C++ file 'wguilogwriter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/wguilogwriter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wguilogwriter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_WGUILogWriter_t {
    QByteArrayData data[4];
    char stringdata0[37];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WGUILogWriter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WGUILogWriter_t qt_meta_stringdata_WGUILogWriter = {
    {
QT_MOC_LITERAL(0, 0, 13), // "WGUILogWriter"
QT_MOC_LITERAL(1, 14, 7), // "explain"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 13) // "sigLogMessage"

    },
    "WGUILogWriter\0explain\0\0sigLogMessage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WGUILogWriter[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,
       3,    0,   25,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void WGUILogWriter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WGUILogWriter *_t = static_cast<WGUILogWriter *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->explain(); break;
        case 1: _t->sigLogMessage(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (WGUILogWriter::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WGUILogWriter::explain)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (WGUILogWriter::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WGUILogWriter::sigLogMessage)) {
                *result = 1;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject WGUILogWriter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_WGUILogWriter.data,
      qt_meta_data_WGUILogWriter,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *WGUILogWriter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WGUILogWriter::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_WGUILogWriter.stringdata0))
        return static_cast<void*>(const_cast< WGUILogWriter*>(this));
    if (!strcmp(_clname, "WAbstractLogWriter"))
        return static_cast< WAbstractLogWriter*>(const_cast< WGUILogWriter*>(this));
    if (!strcmp(_clname, "WSingleton<WGUILogWriter>"))
        return static_cast< WSingleton<WGUILogWriter>*>(const_cast< WGUILogWriter*>(this));
    return QObject::qt_metacast(_clname);
}

int WGUILogWriter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void WGUILogWriter::explain()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void WGUILogWriter::sigLogMessage()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE

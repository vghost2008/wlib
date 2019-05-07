/****************************************************************************
** Meta object code from reading C++ file 'guibutton.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/wgui/guibutton.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guibutton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_WAbstractButton_t {
    QByteArrayData data[6];
    char stringdata0[47];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WAbstractButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WAbstractButton_t qt_meta_stringdata_WAbstractButton = {
    {
QT_MOC_LITERAL(0, 0, 15), // "WAbstractButton"
QT_MOC_LITERAL(1, 16, 7), // "explain"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 10), // "sigClicked"
QT_MOC_LITERAL(4, 36, 2), // "id"
QT_MOC_LITERAL(5, 39, 7) // "checked"

    },
    "WAbstractButton\0explain\0\0sigClicked\0"
    "id\0checked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WAbstractButton[] = {

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
       3,    2,   25,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, QMetaType::Bool,    4,    5,

       0        // eod
};

void WAbstractButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WAbstractButton *_t = static_cast<WAbstractButton *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->explain(); break;
        case 1: _t->sigClicked((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (WAbstractButton::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WAbstractButton::explain)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (WAbstractButton::*_t)(unsigned  , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WAbstractButton::sigClicked)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject WAbstractButton::staticMetaObject = {
    { &QAbstractButton::staticMetaObject, qt_meta_stringdata_WAbstractButton.data,
      qt_meta_data_WAbstractButton,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *WAbstractButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WAbstractButton::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_WAbstractButton.stringdata0))
        return static_cast<void*>(const_cast< WAbstractButton*>(this));
    return QAbstractButton::qt_metacast(_clname);
}

int WAbstractButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractButton::qt_metacall(_c, _id, _a);
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
void WAbstractButton::explain()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void WAbstractButton::sigClicked(unsigned  _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_WSwitch_t {
    QByteArrayData data[6];
    char stringdata0[41];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WSwitch_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WSwitch_t qt_meta_stringdata_WSwitch = {
    {
QT_MOC_LITERAL(0, 0, 7), // "WSwitch"
QT_MOC_LITERAL(1, 8, 7), // "explain"
QT_MOC_LITERAL(2, 16, 0), // ""
QT_MOC_LITERAL(3, 17, 10), // "sigClicked"
QT_MOC_LITERAL(4, 28, 2), // "id"
QT_MOC_LITERAL(5, 31, 9) // "ischecked"

    },
    "WSwitch\0explain\0\0sigClicked\0id\0ischecked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WSwitch[] = {

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
       3,    2,   25,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, QMetaType::Bool,    4,    5,

       0        // eod
};

void WSwitch::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WSwitch *_t = static_cast<WSwitch *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->explain(); break;
        case 1: _t->sigClicked((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (WSwitch::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WSwitch::explain)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (WSwitch::*_t)(unsigned  , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WSwitch::sigClicked)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject WSwitch::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WSwitch.data,
      qt_meta_data_WSwitch,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *WSwitch::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WSwitch::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_WSwitch.stringdata0))
        return static_cast<void*>(const_cast< WSwitch*>(this));
    return QWidget::qt_metacast(_clname);
}

int WSwitch::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void WSwitch::explain()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void WSwitch::sigClicked(unsigned  _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE

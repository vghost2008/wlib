/****************************************************************************
** Meta object code from reading C++ file 'wribbonbar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/wgui/wribbonbar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wribbonbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_WRibbonBar_t {
    QByteArrayData data[6];
    char stringdata0[50];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WRibbonBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WRibbonBar_t qt_meta_stringdata_WRibbonBar = {
    {
QT_MOC_LITERAL(0, 0, 10), // "WRibbonBar"
QT_MOC_LITERAL(1, 11, 7), // "explain"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 16), // "sigButtonClicked"
QT_MOC_LITERAL(4, 37, 2), // "id"
QT_MOC_LITERAL(5, 40, 9) // "ischecked"

    },
    "WRibbonBar\0explain\0\0sigButtonClicked\0"
    "id\0ischecked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WRibbonBar[] = {

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

void WRibbonBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WRibbonBar *_t = static_cast<WRibbonBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->explain(); break;
        case 1: _t->sigButtonClicked((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (WRibbonBar::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WRibbonBar::explain)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (WRibbonBar::*_t)(unsigned  , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WRibbonBar::sigButtonClicked)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject WRibbonBar::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WRibbonBar.data,
      qt_meta_data_WRibbonBar,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *WRibbonBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WRibbonBar::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_WRibbonBar.stringdata0))
        return static_cast<void*>(const_cast< WRibbonBar*>(this));
    return QWidget::qt_metacast(_clname);
}

int WRibbonBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void WRibbonBar::explain()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void WRibbonBar::sigButtonClicked(unsigned  _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_WRibbonWidget_t {
    QByteArrayData data[1];
    char stringdata0[14];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WRibbonWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WRibbonWidget_t qt_meta_stringdata_WRibbonWidget = {
    {
QT_MOC_LITERAL(0, 0, 13) // "WRibbonWidget"

    },
    "WRibbonWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WRibbonWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void WRibbonWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject WRibbonWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WRibbonWidget.data,
      qt_meta_data_WRibbonWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *WRibbonWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WRibbonWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_WRibbonWidget.stringdata0))
        return static_cast<void*>(const_cast< WRibbonWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int WRibbonWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_WTabBar_t {
    QByteArrayData data[3];
    char stringdata0[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WTabBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WTabBar_t qt_meta_stringdata_WTabBar = {
    {
QT_MOC_LITERAL(0, 0, 7), // "WTabBar"
QT_MOC_LITERAL(1, 8, 16), // "sigDoubleClicked"
QT_MOC_LITERAL(2, 25, 0) // ""

    },
    "WTabBar\0sigDoubleClicked\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WTabBar[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void WTabBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WTabBar *_t = static_cast<WTabBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sigDoubleClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (WTabBar::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WTabBar::sigDoubleClicked)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject WTabBar::staticMetaObject = {
    { &QTabBar::staticMetaObject, qt_meta_stringdata_WTabBar.data,
      qt_meta_data_WTabBar,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *WTabBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WTabBar::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_WTabBar.stringdata0))
        return static_cast<void*>(const_cast< WTabBar*>(this));
    return QTabBar::qt_metacast(_clname);
}

int WTabBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void WTabBar::sigDoubleClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE

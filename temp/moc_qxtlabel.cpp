/****************************************************************************
** Meta object code from reading C++ file 'qxtlabel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/qxt/qxtlabel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qxtlabel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QxtLabel_t {
    QByteArrayData data[12];
    char stringdata0[118];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QxtLabel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QxtLabel_t qt_meta_stringdata_QxtLabel = {
    {
QT_MOC_LITERAL(0, 0, 8), // "QxtLabel"
QT_MOC_LITERAL(1, 9, 7), // "explain"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 7), // "clicked"
QT_MOC_LITERAL(4, 26, 11), // "textChanged"
QT_MOC_LITERAL(5, 38, 4), // "text"
QT_MOC_LITERAL(6, 43, 9), // "alignment"
QT_MOC_LITERAL(7, 53, 13), // "Qt::Alignment"
QT_MOC_LITERAL(8, 67, 9), // "elideMode"
QT_MOC_LITERAL(9, 77, 17), // "Qt::TextElideMode"
QT_MOC_LITERAL(10, 95, 8), // "rotation"
QT_MOC_LITERAL(11, 104, 13) // "Qxt::Rotation"

    },
    "QxtLabel\0explain\0\0clicked\0textChanged\0"
    "text\0alignment\0Qt::Alignment\0elideMode\0"
    "Qt::TextElideMode\0rotation\0Qxt::Rotation"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QxtLabel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       4,   34, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,
       3,    0,   30,    2, 0x06 /* Public */,
       4,    1,   31,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,

 // properties: name, type, flags
       5, QMetaType::QString, 0x00495103,
       6, 0x80000000 | 7, 0x0009510b,
       8, 0x80000000 | 9, 0x0009510b,
      10, 0x80000000 | 11, 0x0009510b,

 // properties: notify_signal_id
       2,
       0,
       0,
       0,

       0        // eod
};

void QxtLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QxtLabel *_t = static_cast<QxtLabel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->explain(); break;
        case 1: _t->clicked(); break;
        case 2: _t->textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QxtLabel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QxtLabel::explain)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (QxtLabel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QxtLabel::clicked)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (QxtLabel::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QxtLabel::textChanged)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        QxtLabel *_t = static_cast<QxtLabel *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->text(); break;
        case 1: *reinterpret_cast< Qt::Alignment*>(_v) = _t->alignment(); break;
        case 2: *reinterpret_cast< Qt::TextElideMode*>(_v) = _t->elideMode(); break;
        case 3: *reinterpret_cast< Qxt::Rotation*>(_v) = _t->rotation(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        QxtLabel *_t = static_cast<QxtLabel *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setText(*reinterpret_cast< QString*>(_v)); break;
        case 1: _t->setAlignment(*reinterpret_cast< Qt::Alignment*>(_v)); break;
        case 2: _t->setElideMode(*reinterpret_cast< Qt::TextElideMode*>(_v)); break;
        case 3: _t->setRotation(*reinterpret_cast< Qxt::Rotation*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject QxtLabel::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_QxtLabel.data,
      qt_meta_data_QxtLabel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QxtLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QxtLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QxtLabel.stringdata0))
        return static_cast<void*>(const_cast< QxtLabel*>(this));
    return QFrame::qt_metacast(_clname);
}

int QxtLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QxtLabel::explain()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void QxtLabel::clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void QxtLabel::textChanged(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE

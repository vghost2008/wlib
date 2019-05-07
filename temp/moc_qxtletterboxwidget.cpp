/****************************************************************************
** Meta object code from reading C++ file 'qxtletterboxwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/qxt/qxtletterboxwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qxtletterboxwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QxtLetterBoxWidget_t {
    QByteArrayData data[6];
    char stringdata0[68];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QxtLetterBoxWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QxtLetterBoxWidget_t qt_meta_stringdata_QxtLetterBoxWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "QxtLetterBoxWidget"
QT_MOC_LITERAL(1, 19, 12), // "resizeWidget"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 15), // "backgroundColor"
QT_MOC_LITERAL(4, 49, 6), // "margin"
QT_MOC_LITERAL(5, 56, 11) // "resizeDelay"

    },
    "QxtLetterBoxWidget\0resizeWidget\0\0"
    "backgroundColor\0margin\0resizeDelay"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QxtLetterBoxWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       3,   20, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,

 // properties: name, type, flags
       3, QMetaType::QColor, 0x00095107,
       4, QMetaType::Int, 0x00095103,
       5, QMetaType::UInt, 0x00095103,

       0        // eod
};

void QxtLetterBoxWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QxtLetterBoxWidget *_t = static_cast<QxtLetterBoxWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->resizeWidget(); break;
        default: ;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        QxtLetterBoxWidget *_t = static_cast<QxtLetterBoxWidget *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = _t->backgroundColor(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->margin(); break;
        case 2: *reinterpret_cast< uint*>(_v) = _t->resizeDelay(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        QxtLetterBoxWidget *_t = static_cast<QxtLetterBoxWidget *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setBackgroundColor(*reinterpret_cast< QColor*>(_v)); break;
        case 1: _t->setMargin(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setResizeDelay(*reinterpret_cast< uint*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
        QxtLetterBoxWidget *_t = static_cast<QxtLetterBoxWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->clearBackgroundColor(); break;
        default: break;
        }
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_a);
}

const QMetaObject QxtLetterBoxWidget::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_QxtLetterBoxWidget.data,
      qt_meta_data_QxtLetterBoxWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QxtLetterBoxWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QxtLetterBoxWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QxtLetterBoxWidget.stringdata0))
        return static_cast<void*>(const_cast< QxtLetterBoxWidget*>(this));
    return QFrame::qt_metacast(_clname);
}

int QxtLetterBoxWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE

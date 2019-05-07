/****************************************************************************
** Meta object code from reading C++ file 'qxtlineedit.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/qxt/qxtlineedit.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qxtlineedit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QxtLineEdit_t {
    QByteArrayData data[26];
    char stringdata0[324];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QxtLineEdit_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QxtLineEdit_t qt_meta_stringdata_QxtLineEdit = {
    {
QT_MOC_LITERAL(0, 0, 11), // "QxtLineEdit"
QT_MOC_LITERAL(1, 12, 13), // "buttonClicked"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 13), // "setSampleText"
QT_MOC_LITERAL(4, 41, 4), // "text"
QT_MOC_LITERAL(5, 46, 16), // "_qxt_textChanged"
QT_MOC_LITERAL(6, 63, 15), // "buttonAutoRaise"
QT_MOC_LITERAL(7, 79, 10), // "buttonIcon"
QT_MOC_LITERAL(8, 90, 14), // "buttonPosition"
QT_MOC_LITERAL(9, 105, 14), // "ButtonPosition"
QT_MOC_LITERAL(10, 120, 15), // "resetButtonMode"
QT_MOC_LITERAL(11, 136, 15), // "ResetButtonMode"
QT_MOC_LITERAL(12, 152, 10), // "sampleText"
QT_MOC_LITERAL(13, 163, 8), // "NoButton"
QT_MOC_LITERAL(14, 172, 12), // "PositionLeft"
QT_MOC_LITERAL(15, 185, 12), // "PositionAuto"
QT_MOC_LITERAL(16, 198, 13), // "PositionOuter"
QT_MOC_LITERAL(17, 212, 10), // "InnerRight"
QT_MOC_LITERAL(18, 223, 9), // "InnerLeft"
QT_MOC_LITERAL(19, 233, 9), // "InnerAuto"
QT_MOC_LITERAL(20, 243, 10), // "OuterRight"
QT_MOC_LITERAL(21, 254, 9), // "OuterLeft"
QT_MOC_LITERAL(22, 264, 9), // "OuterAuto"
QT_MOC_LITERAL(23, 274, 15), // "HideResetButton"
QT_MOC_LITERAL(24, 290, 17), // "ShowResetNotEmpty"
QT_MOC_LITERAL(25, 308, 15) // "ShowResetAlways"

    },
    "QxtLineEdit\0buttonClicked\0\0setSampleText\0"
    "text\0_qxt_textChanged\0buttonAutoRaise\0"
    "buttonIcon\0buttonPosition\0ButtonPosition\0"
    "resetButtonMode\0ResetButtonMode\0"
    "sampleText\0NoButton\0PositionLeft\0"
    "PositionAuto\0PositionOuter\0InnerRight\0"
    "InnerLeft\0InnerAuto\0OuterRight\0OuterLeft\0"
    "OuterAuto\0HideResetButton\0ShowResetNotEmpty\0"
    "ShowResetAlways"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QxtLineEdit[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       5,   36, // properties
       2,   51, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   30,    2, 0x0a /* Public */,
       5,    1,   33,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    4,

 // properties: name, type, flags
       6, QMetaType::Bool, 0x00095103,
       7, QMetaType::QIcon, 0x00095103,
       8, 0x80000000 | 9, 0x0009510b,
      10, 0x80000000 | 11, 0x0009510b,
      12, QMetaType::QString, 0x00095103,

 // enums: name, flags, count, data
       9, 0x0,   10,   59,
      11, 0x0,    3,   79,

 // enum data: key, value
      13, uint(QxtLineEdit::NoButton),
      14, uint(QxtLineEdit::PositionLeft),
      15, uint(QxtLineEdit::PositionAuto),
      16, uint(QxtLineEdit::PositionOuter),
      17, uint(QxtLineEdit::InnerRight),
      18, uint(QxtLineEdit::InnerLeft),
      19, uint(QxtLineEdit::InnerAuto),
      20, uint(QxtLineEdit::OuterRight),
      21, uint(QxtLineEdit::OuterLeft),
      22, uint(QxtLineEdit::OuterAuto),
      23, uint(QxtLineEdit::HideResetButton),
      24, uint(QxtLineEdit::ShowResetNotEmpty),
      25, uint(QxtLineEdit::ShowResetAlways),

       0        // eod
};

void QxtLineEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QxtLineEdit *_t = static_cast<QxtLineEdit *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->buttonClicked(); break;
        case 1: _t->setSampleText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->_qxt_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QxtLineEdit::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QxtLineEdit::buttonClicked)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        QxtLineEdit *_t = static_cast<QxtLineEdit *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->buttonAutoRaise(); break;
        case 1: *reinterpret_cast< QIcon*>(_v) = _t->buttonIcon(); break;
        case 2: *reinterpret_cast< ButtonPosition*>(_v) = _t->buttonPosition(); break;
        case 3: *reinterpret_cast< ResetButtonMode*>(_v) = _t->resetButtonMode(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->sampleText(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        QxtLineEdit *_t = static_cast<QxtLineEdit *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setButtonAutoRaise(*reinterpret_cast< bool*>(_v)); break;
        case 1: _t->setButtonIcon(*reinterpret_cast< QIcon*>(_v)); break;
        case 2: _t->setButtonPosition(*reinterpret_cast< ButtonPosition*>(_v)); break;
        case 3: _t->setResetButtonMode(*reinterpret_cast< ResetButtonMode*>(_v)); break;
        case 4: _t->setSampleText(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject QxtLineEdit::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_QxtLineEdit.data,
      qt_meta_data_QxtLineEdit,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QxtLineEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QxtLineEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QxtLineEdit.stringdata0))
        return static_cast<void*>(const_cast< QxtLineEdit*>(this));
    return QLineEdit::qt_metacast(_clname);
}

int QxtLineEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
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
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QxtLineEdit::buttonClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE

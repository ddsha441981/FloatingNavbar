/****************************************************************************
** Meta object code from reading C++ file 'PipeServer.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../PipeServer.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PipeServer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10PipeServerE_t {};
} // unnamed namespace

template <> constexpr inline auto PipeServer::qt_create_metaobjectdata<qt_meta_tag_ZN10PipeServerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PipeServer",
        "commandReceived",
        "",
        "command",
        "responseReady",
        "response",
        "commandWithWorker",
        "PipeWorker*",
        "worker"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'commandReceived'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'responseReady'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'commandWithWorker'
        QtMocHelpers::SignalData<void(const QString &, PipeWorker *)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 7, 8 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PipeServer, qt_meta_tag_ZN10PipeServerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PipeServer::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10PipeServerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10PipeServerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10PipeServerE_t>.metaTypes,
    nullptr
} };

void PipeServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PipeServer *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->commandReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->responseReady((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->commandWithWorker((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<PipeWorker*>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< PipeWorker* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (PipeServer::*)(const QString & )>(_a, &PipeServer::commandReceived, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (PipeServer::*)(const QString & )>(_a, &PipeServer::responseReady, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (PipeServer::*)(const QString & , PipeWorker * )>(_a, &PipeServer::commandWithWorker, 2))
            return;
    }
}

const QMetaObject *PipeServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PipeServer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10PipeServerE_t>.strings))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int PipeServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void PipeServer::commandReceived(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void PipeServer::responseReady(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void PipeServer::commandWithWorker(const QString & _t1, PipeWorker * _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}
QT_WARNING_POP

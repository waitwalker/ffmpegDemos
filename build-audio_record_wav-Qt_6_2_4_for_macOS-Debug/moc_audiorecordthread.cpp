/****************************************************************************
** Meta object code from reading C++ file 'audiorecordthread.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../audio_record_wav/audiorecordthread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'audiorecordthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AudioRecordThread_t {
    const uint offsetsAndSize[8];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_AudioRecordThread_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_AudioRecordThread_t qt_meta_stringdata_AudioRecordThread = {
    {
QT_MOC_LITERAL(0, 17), // "AudioRecordThread"
QT_MOC_LITERAL(18, 11), // "timeChanged"
QT_MOC_LITERAL(30, 0), // ""
QT_MOC_LITERAL(31, 2) // "ms"

    },
    "AudioRecordThread\0timeChanged\0\0ms"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AudioRecordThread[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   20,    2, 0x06,    1 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::ULongLong,    3,

       0        // eod
};

void AudioRecordThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AudioRecordThread *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->timeChanged((*reinterpret_cast< qulonglong(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AudioRecordThread::*)(unsigned long long );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AudioRecordThread::timeChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject AudioRecordThread::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_AudioRecordThread.offsetsAndSize,
    qt_meta_data_AudioRecordThread,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_AudioRecordThread_t
, QtPrivate::TypeAndForceComplete<AudioRecordThread, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<unsigned long long, std::false_type>



>,
    nullptr
} };


const QMetaObject *AudioRecordThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AudioRecordThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AudioRecordThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int AudioRecordThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void AudioRecordThread::timeChanged(unsigned long long _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
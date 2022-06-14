#include "audiothread.h"
#include <QDebug>
extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
}

AudioThread::AudioThread(QObject *parent)
    : QThread{parent}
{
    connect(this, &AudioThread::finished,
            this, &AudioThread::deleteLater);
}

AudioThread::~AudioThread() {
    // 断开所有连接
    disconnect();
    // 内存回收之前，正常结束线程
    requestInterruption();
    // 线程安全退出
    quit();
    wait();

    qDebug()<<this<<"析构（内存被回收）";
}

void AudioThread::run() {

}

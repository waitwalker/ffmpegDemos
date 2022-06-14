#include "audiothread.h"
#include <QDebug>
#include <QObject>
extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
}
// \斜线表示后面东西都是你的

#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret,errbuf,sizeof(errbuf));
void freep(void **ptr) {
    free(*ptr);
    *ptr = nullptr;
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
    // 文件已经准备好了
    const char *inFilename = "4800_f32le.pcm";
    const char *outFilename = "44100_s16le.pcm";

    // 输入的一些参数
    AVSampleFormat inSampleFmt = AV_SAMPLE_FMT_FLT;
    int inSampleRate = 48000;
    // 声道布局
    int inChLayout = AV_CH_LAYOUT_MONO;// 单声道

    AVSampleFormat outSampleFmt = AV_SAMPLE_FMT_S16;
    int outSampleRate = 44100;
    // 声道布局
    int outChLayout = AV_CH_LAYOUT_STEREO;// 双声道，立体声

    // 创建重采样上下文
    SwrContext *ctx = swr_alloc_set_opts(nullptr,
                       // 输出参数
                       outChLayout, outSampleFmt, outSampleRate,
                       // 输入参数
                       inChLayout, inSampleFmt, inSampleRate,
                       0, nullptr);
    if (!ctx) {
        // 只要是C语言 alloc创建的堆空间，需要手动释放
        qDebug()<<"swr_alloc_set_opts error";
        return;
    }

    // 初始化重采样上下文
    int ret = swr_init(ctx);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"swr_init error:"<< errbuf;
        swr_free(&ctx);
        return;
    }
    // 创建输入缓冲区
    // 指向缓冲区的指针
    uint8_t **inData = nullptr;
    // 缓冲区的大小 最终会显示缓冲区多大
    int inLineSize = 0;
    // 声道数量
    int inChs = av_get_channel_layout_nb_channels(inChLayout);
    // 缓冲区样本数量
    int inSample = 1024;
    av_samples_alloc_array_and_samples(
                &inData,
                &inLineSize,
                inChs,
                inSample,
                inSampleFmt,
                1
                );

    // 释放资源
    // 释放重采样上下文
    swr_free(&ctx);
}

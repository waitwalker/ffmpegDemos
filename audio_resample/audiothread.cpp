#include "audiothread.h"
#include <QDebug>
#include <QObject>
#include <QFile>
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
    // 文件名
    const char *inFilename = "4800_f32le.pcm";
    QFile inFile(inFilename);
    const char *outFilename = "44100_s16le.pcm";
    QFile outFile(outFilename);

    // 输入的一些参数
    AVSampleFormat inSampleFmt = AV_SAMPLE_FMT_FLT;
    int inSampleRate = 48000;
    // 声道布局
    int inChLayout = AV_CH_LAYOUT_MONO;// 单声道

    AVSampleFormat outSampleFmt = AV_SAMPLE_FMT_S16;
    int outSampleRate = 44100;
    // 声道布局
    int outChLayout = AV_CH_LAYOUT_STEREO;// 双声道，立体声

    // 创建输入缓冲区
    // 指向缓冲区的指针
    uint8_t **inData = nullptr;
    // 缓冲区的大小 最终会显示缓冲区多大
    int inLineSize = 0;
    // 声道数量
    int inChs = av_get_channel_layout_nb_channels(inChLayout);
    // 一个样本的大小
    int inBytesPerSample = inChs * av_get_bytes_per_sample(inSampleFmt);
    // 缓冲区样本数量
    int inSamples = 1024;

    // 创建输出缓冲区
    // 指向缓冲区的指针
    uint8_t **outData = nullptr;
    // 缓冲区的大小 最终会显示缓冲区多大
    int outLineSize = 0;
    // 声道数量
    int outChs = av_get_channel_layout_nb_channels(outChLayout);
    // 缓冲区样本数量
    int outSamples = 1024;
    // 返回值
    int ret;
    // 读取文件的大小
    int len = 0;

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
        goto end;
        return;
    }

    // 初始化重采样上下文
    ret = swr_init(ctx);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"swr_init error:"<< errbuf;
        goto end;
        return;
    }

    // 创建输入缓冲区
    ret = av_samples_alloc_array_and_samples(
                &inData,// inData传进去后，希望调用完这个函数后能指向一块空间，也就是inData
                &inLineSize,// 指向的缓冲区有多大
                inChs,
                inSamples,
                inSampleFmt,
                1);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"av_samples_alloc_array_and_samples error:"<<errbuf;
        goto end;
        return;
    }

    // 创建输出缓冲区
    ret = av_samples_alloc_array_and_samples(
                &outData,
                &outLineSize,
                outChs,
                outSamples,
                outSampleFmt,
                1);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"av_samples_alloc_array_and_samples error:"<<errbuf;
        // 在许多编程语言中goto 语句是禁止的， 为了方便这里
        goto end;
        return;
    }

    // 打开文件
    if(!inFile.open(QFile::ReadOnly)) {
        qDebug()<<"in file open error:"<<inFilename;
        goto end;
    }

    if(!outFile.open(QFile::WriteOnly)) {
        qDebug()<<"out file open error:"<<outFilename;
        goto end;
    }

    // 读取文件数据
    while ((len = inFile.read((char *)inData[0], inLineSize))) {
        // 读取的样本数量
        inSamples = len / inBytesPerSample;
        // 重采样(返回值是转换后的样本数据)
        ret = swr_convert(ctx,
                    outData,
                    outSamples,
                    (const uint8_t **)inData,
                    inSamples
                    );
    }

end:
    // 释放资源
    // 关闭文件
    inFile.close();
    outFile.close();

    // 释放输入缓冲区
    av_freep(&inData);

    // 释放输出缓冲区
    av_freep(&outData);

    // 释放重采样上下文
    swr_free(&ctx);
}

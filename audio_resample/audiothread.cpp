#include "audiothread.h"
#include <QDebug>
#include <QObject>
#include <QFile>
#include "ffmpegs.h"
extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
}
// \斜线表示后面东西都是你的



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
    // 这里就可以多重转换

    // 文件已经准备好了
    ResampleAudioSpec res1;
    res1.filename = "/Users/walkerwait/Desktop/4800_f32le.pcm";
    // 输入的一些参数
    res1.sampleFmt = AV_SAMPLE_FMT_FLT;
    res1.sampleRate = 48000;
    // 声道布局
    res1.chLayout = AV_CH_LAYOUT_MONO;// 单声道

    ResampleAudioSpec res2;
    res2.filename = "/Users/walkerwait/Desktop/44100_s16le.pcm";
    res2.sampleFmt = AV_SAMPLE_FMT_S16;
    res2.sampleRate = 44100;
    // 声道布局
    res2.chLayout = AV_CH_LAYOUT_STEREO;// 双声道，立体声


    ResampleAudioSpec res3;
    res3.filename = "/Users/walkerwait/Desktop/48000_s32le_1.pcm";
    res3.sampleFmt = AV_SAMPLE_FMT_S32;
    res3.sampleRate = 48000;
    // 声道布局
    res3.chLayout = AV_CH_LAYOUT_STEREO;// 双声道，立体声

    FFmpegs::resampleAuido(res2,
                           res3);

//    // 文件名
//    const char *inFilename = "/Users/walkerwait/Desktop/4800_f32le.pcm";
//    // 输入的一些参数
//    AVSampleFormat inSampleFmt = AV_SAMPLE_FMT_FLT;
//    int inSampleRate = 48000;
//    // 声道布局
//    int inChLayout = AV_CH_LAYOUT_MONO;// 单声道

//    const char *outFilename = "/Users/walkerwait/Desktop/44100_s16le.pcm";
//    AVSampleFormat outSampleFmt = AV_SAMPLE_FMT_S16;
//    int outSampleRate = 44100;
//    // 声道布局
//    int outChLayout = AV_CH_LAYOUT_STEREO;// 双声道，立体声
//    FFmpegs::resampleAuido(inFilename,
//                           inSampleRate,
//                           inSampleFmt,
//                           inChLayout,
//                           outFilename,
//                           outSampleRate,
//                           outSampleFmt,
//                           outChLayout);

}

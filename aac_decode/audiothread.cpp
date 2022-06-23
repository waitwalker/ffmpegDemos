#include "audiothread.h"
#include <QDebug>
#include "ffmpegs.h"

AudioThread::AudioThread(QObject *parent)
    : QThread{parent}
{
    connect(this,&AudioThread::finished,
            this,&AudioThread::deleteLater);
}

AudioThread::~AudioThread(){
    // 断开所有连接
    disconnect();
    // 内存回收之前，正常结束线程
    requestInterruption();
    // 安全退出
    quit();
    wait();
    qDebug()<<this<<"析构（内存被回收）";
}

void AudioThread::run() {
//    AudioEncodeSpec in;
//    in.filename = "/Users/walkerwait/Desktop/44100_s16le.pcm";
//    in.sampleRate = 44100;
//    in.sampleFmt = AV_SAMPLE_FMT_S16;
//    in.chLayout = AV_CH_LAYOUT_STEREO;
//    FFmpegs::aacEncode(in,"/Users/walkerwait/Desktop/out7.aac");

    AudioDecodeSpec out;
    out.filename = "/Users/walkerwait/Desktop/out8.pcm";
    FFmpegs::aacDecode("/Users/walkerwait/Desktop/out7.aac",out);

    qDebug()<<"采样率："<<out.sampleRate;
    qDebug()<<"采样格式："<<av_get_sample_fmt_name(out.sampleFmt);
    qDebug()<<"声道叔："<<av_get_channel_layout_nb_channels(out.chLayout);
}

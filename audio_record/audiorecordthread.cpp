#include "audiorecordthread.h"
#include <QDebug>
#include <QFile>
#include <QDateTime>

AudioRecordThread::AudioRecordThread(QObject *parent)
    : QThread{parent}
{

}

AudioRecordThread::~AudioRecordThread() {

}

void showSpec(AVFormatContext *ctx){
    // 获取输入流
    AVStream *stream = ctx->streams[0];

    // 获取音频参数
    AVCodecParameters *params = stream->codecpar;

    // 声道数
    qDebug()<<"声道数："<<params->channels;

    // 采样率
    qDebug()<<"采样率："<<params->sample_rate;

    // 采样格式
    qDebug()<<"采样格式："<<params->format;

    // 每一个样本的一个声道占用多少个字节
    qDebug()<<"每一个样本的一个声道占用字节:"<<av_get_bytes_per_sample((AVSampleFormat)params->format);
}


void AudioRecordThread::run() {

    // 2.获取输入格式对象
    const AVInputFormat *fmt = av_find_input_format(FMT_NAME);
    if(!fmt) {
        qDebug()<<"获取输入格式对象错误："<<FMT_NAME;
        return;
    }

    // 3.打开设备
    AVFormatContext *ctx = nullptr;
    AVDictionary *options;
    int ret = avformat_open_input(&ctx, DEVICE_NAME, fmt, &options);
    if (ret < 0) {
        char errbuf[1024] = {0};
        av_strerror(ret, errbuf, sizeof(errbuf));
        qDebug()<<"打开设备识失败："<<errbuf;
        return;
    }

    // 打印录音设备的参数
    showSpec(ctx);

    // 4.采集数据
    QString fileName = FILE_PATH;
    fileName += QDateTime::currentDateTime().toString("MM_dd_HH_mm_ss");
    fileName += ".pcm";
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)){
        qDebug()<<"打开文件失败："<<fileName;
        // 关闭设备
        avformat_close_input(&ctx);
        return;
    }

    // 数据包
    AVPacket *pkt = av_packet_alloc();
    while (!isInterruptionRequested()) {
        // 不断采集数据
        ret = av_read_frame(ctx, pkt);
        if (ret == 0) {
            file.write((const char *)pkt->data, pkt->size);
        } else if (ret == AVERROR(EAGAIN)) {
            continue;
        } else {
            char err[1024];
            av_strerror(ret, err, sizeof(err));
            qDebug()<<"av_read_frame error"<<err;
            break;
        }
        av_packet_unref(pkt);
    }

    // 5.释放资源
    // 关闭文件
    file.close();

    // 释放资源
    av_packet_free(&pkt);

    // 关闭设备
    avformat_close_input(&ctx);

    qDebug()<<this<<"正常结束---";

}

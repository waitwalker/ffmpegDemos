#include "audiorecordthread.h"
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include "ffmpegs.h"
extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
}

AudioRecordThread::AudioRecordThread(QObject *parent)
    : QThread{parent}
{
    connect(this, &AudioRecordThread::finished, this, &AudioRecordThread::deleteLater);
}

AudioRecordThread::~AudioRecordThread() {
    // 断开所有连接
    disconnect();
    // 内存回收之前，正常结束线程
    requestInterruption();
    // 安全退出
    quit();
    wait();
    qDebug()<<this<<"析构（内存被回收了）";
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

    // 采样格式 16 * 2
    qDebug()<<"采样格式："<<params->format;

    // 每一个样本的一个声道占用多少个字节
    qDebug()<<"每一个样本的一个声道占用字节:"<<av_get_bytes_per_sample((AVSampleFormat)params->format);

    // 编码格式（可以看出采样格式）
    qDebug()<<"编码格式（可以看出采样格式）"<<params->codec_id;

    // 每一个样本的声道占多少位
    qDebug()<<"每一个样本的声道占多少位"<<av_get_bits_per_sample(params->codec_id);

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
    fileName += ".wav";

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)){
        qDebug()<<"打开文件失败："<<fileName;
        // 关闭设备
        avformat_close_input(&ctx);
        return;
    }


    // 获取输入流
    AVStream *stream = ctx->streams[0];

    // 获取音频参数
    AVCodecParameters *params = stream->codecpar;

    // 写入WAV文件头
    WAVHeader header;
    header.sampleRate = params->sample_rate;
    // 2
    header.bitsPerSample = av_get_bits_per_sample(params->codec_id);
    header.numChannels = params->channels;
    if (params->codec_id >= AV_CODEC_ID_PCM_F32BE) {
        header.audioFormat = AUDIO_FORMAT_FLOAT;
    }

    header.blockAlign = header.bitsPerSample * header.numChannels >> 3;
    header.byteRate = header.sampleRate * header.blockAlign;
//    header.dataChunkDataSize = 0;
    file.write((char *)&header, sizeof(WAVHeader));
    // 数据包
    AVPacket *pkt = av_packet_alloc();

    while (!isInterruptionRequested()) {
        // 不断采集数据
        ret = av_read_frame(ctx, pkt);
        if (ret == 0) {

            file.write((const char *)pkt->data, pkt->size);
            // 计算录音时长
            header.dataChunkDataSize += pkt->size;
            unsigned long long ms = 1000.0 * header.dataChunkDataSize / header.byteRate;
            qDebug()<<"录音时长："<<ms;
            emit timeChanged(ms);
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

    // 写入dataChunkDataSize
    // 计算dataChunkDataSize
    // 计算偏移量
    // header.dataChunkDataSize = file.size() - sizeof(WAVHeader);// 整个文件的大小 - WAV文件头的大小
    file.seek(sizeof(WAVHeader) - sizeof(header.dataChunkDataSize));
    file.write((char *)&header.dataChunkDataSize, sizeof(header.dataChunkDataSize));

    // 写入fiffChunkDataSize
    header.riffChunkDataSize = file.size()
            - sizeof (header.riffChunkId)
            - sizeof (header.riffChunkDataSize);
    file.seek(sizeof(header.riffChunkId));
    file.write((char *)&header.riffChunkDataSize, sizeof(header.riffChunkDataSize));

    // 5.释放资源
    // 关闭文件
    file.close();

    // 释放资源
    av_packet_free(&pkt);

    // 关闭设备
    avformat_close_input(&ctx);

    qDebug()<<this<<"正常结束---";

}

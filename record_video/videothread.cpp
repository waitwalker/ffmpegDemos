#include "videothread.h"
#include <QDebug>
#include <QFile>
extern "C" {
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
}

#ifdef Q_OS_WIN
// 格式名称
#define FMT_NAME "dshow"
// 设备名称
#define DEVICE_NAME “video=Integrated Camera”
// YUV文件名
#define FILENAME "F:/out.yuv"

#else
// 格式名称
#define FMT_NAME "avfoundation"
// 设备名称
#define DEVICE_NAME "0"
// YUV文件名
#define FILENAME "/Users/walkerwait/Desktop/工作/Demos/ffmpegDemos/resource/xo.yuv"
#endif

#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret,errbuf,sizeof(errbuf));

VideoThread::VideoThread(QObject *parent)
    : QThread{parent}
{
    connect(this,&VideoThread::finished,
            this,&VideoThread::deleteLater);
}

VideoThread::~VideoThread() {
    // 断开所有连接
    disconnect();
    // 内存回收之前，正常结束线程
    requestInterruption();
    // 安全退出
    quit();
    wait();
    qDebug()<<this<<"析构（内存被回收）";
}

void VideoThread::run() {
    qDebug()<<this<<"开始执行";
    // 获取输入格式对象
    const AVInputFormat *fmt = av_find_input_format(FMT_NAME);
    if (!fmt) {
        qDebug()<<"av_find_input_format error";
        return;
    }

    // 格式上下文 可以利用上下文操作设备
    AVFormatContext *ctx = nullptr;

    // 设备参数
    AVDictionary *options = nullptr;
    av_dict_set(&options,"video_size","640x480", 0);
    av_dict_set(&options,"pixel_format","yuyv422", 0);
    av_dict_set(&options,"framerate","30", 0);

    // 打开设备
    int ret = avformat_open_input(&ctx,DEVICE_NAME,fmt,&options);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"avformat_open_input error:"<<errbuf;
        return;
    }

    //文件
    QFile file(FILENAME);

    // 打开文件
    if (!file.open(QFile::WriteOnly)) {
        qDebug()<<"file open error"<<FILENAME;
        // 关闭设备
        avformat_close_input(&ctx);
        return;
    }

    // 计算一帧的大小
    AVCodecParameters *params = ctx->streams[0]->codecpar;
    AVPixelFormat pixFmt = (AVPixelFormat)params->format;

    int imageSize = av_image_get_buffer_size(
                pixFmt,
                params->width,
                params->height,
                1);

    // 数据包
    AVPacket *pkt = av_packet_alloc();
    while (!isInterruptionRequested()) {
        // 不断采集数据
        ret = av_read_frame(ctx,pkt);

        // 读取成功
        if (ret == 0) {
            // 将数据写入文件
            file.write((const char *)pkt->data, imageSize);

            // 释放资源
            av_packet_unref(pkt);
        } else if (ret == AVERROR(EAGAIN)) {
            continue;
        } else {
            ERROR_BUF(ret);
            qDebug()<<"av_read_frame error"<<errbuf;
            break;
        }
    }

    // 释放资源
    av_packet_free(&pkt);

    // 关闭文件
    file.close();

    // 关闭设备
    avformat_close_input(&ctx);

    qDebug()<< this << "正常结束---------";
}

#include "ffmpegs.h"
#include <QFile>
#include <QDebug>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret,errbuf,sizeof(errbuf));

FFmpegs::FFmpegs()
{

}

// 检查像素格式
static int check_sample_fmt(const AVCodec *codec,
                            enum AVPixelFormat pix_fmt){
    const enum AVPixelFormat *p = codec->pix_fmts;
    while (*p != AV_PIX_FMT_NONE) {
        if (*p == pix_fmt)
            return 1;
        p++;
    }
    return 0;
}

static int encode(AVCodecContext *ctx,
                  AVFrame *frame,
                  AVPacket *pkt,
                  QFile &outFile){
    // 发送数据到编码器
    int ret = avcodec_send_frame(ctx, frame);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"avcodec_send_frame error"<<errbuf;
        return ret;
    }
    // 发送到编码器数据成功
    // 不断从编码器中取出编码后的数据
    while (true) {
        ret = avcodec_receive_packet(ctx,pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            // 遇到这两种错误继续读取数据到frame，然后送到编码器
            return 0;
        } else if (ret < 0) {
            return ret;
        }
        // 成功从编码器拿到编码后的数据
        // 将编码后的数据写入文件
        outFile.write((char *)pkt->data,pkt->size);

        // 释放pkt内部的资源
        av_packet_unref(pkt);
    }

    return ret;
}

void FFmpegs::h264Encode(VideoEncodeSpec &in,
                        const char *outFilename) {
    // 文件
    QFile inFile(in.filename);
    QFile outFile(outFilename);

    // 一帧图片的大小
    int imgSize = av_image_get_buffer_size(in.pix_fmt, in.width, in.height, 1);

    // 返回结果
    int ret = 0;



    // 编码器
    const AVCodec *codec = nullptr;

    // 编码上下文
    AVCodecContext *ctx = nullptr;

    // 一般编解码的时候需要以下两个对象
    // 存放编码前的数据（yuv），一般从inFile读到的数据先放到frame中去，先进行编码，编码后的数据放到Packet中去
    AVFrame *frame = nullptr;

    // 存放编码后的数据（h264），再把packet中的数据写入到outFile中去
    AVPacket *pkt = nullptr;

    // 获取编码器
    codec = avcodec_find_encoder_by_name("libx264");
    if (!codec) {
        qDebug()<<"encoder not found："<<"libx264";
        return;
    }

    // 检测输入数据的采样格式是否支持，如果不支持就需要重采样
    if (!check_sample_fmt(codec, in.pix_fmt)) {
        qDebug()<<"unsupported sample format"<<in.pix_fmt
               << av_get_pix_fmt_name(in.pix_fmt);

        return;
    }

    qDebug()<<"codec name"<<codec->name;

    // 创建编码上下文
    ctx = avcodec_alloc_context3(codec);
    if (!ctx) {
        qDebug()<<"avcodec_alloc_context3 failed";
        return;
    }

    // 设置yuv参数
    ctx->width = in.width;
    ctx->height = in.height;
    ctx->pix_fmt = in.pix_fmt;
    // 设置帧率 1s显示的帧数
    ctx->time_base = {1, in.fps};

    // 打开编码器
    ret = avcodec_open2(ctx,codec,nullptr);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"avcodec_open2 error"<<errbuf;
        goto end;
    }

    // 创建AVFrame
    frame = av_frame_alloc();
    if (!frame) {
        qDebug()<<"av_frame_alloc error";
        goto end;
    }

    // frame 缓冲区中的样本帧数量（由ctx->frame_size决定）
    frame->width = ctx->width;
    frame->height = ctx->height;
    frame->format = ctx->pix_fmt;
    // 设置帧的序号
    frame->pts = 0;

    // 利用width、height、format创建缓冲区
    ret = av_image_alloc(frame->data, frame->linesize, in.width, in.height, in.pix_fmt, 1);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"av_frame_get_buffer error"<<errbuf;
        goto end;
    }
//    ret = av_frame_get_buffer(frame,0);
//    if (ret < 0) {
//        ERROR_BUF(ret);
//        qDebug()<<"av_frame_get_buffer error"<<errbuf;
//        goto end;
//    }

    // 创建AVPacket
    pkt = av_packet_alloc();
    if (!pkt) {
        qDebug()<<"av_packet_alloc error";
        goto end;
    }

    // 打开文件
    if (!inFile.open(QFile::ReadOnly)) {
        qDebug()<<"file open error"<<in.filename;
        goto end;
    }

    if (!outFile.open(QFile::WriteOnly)) {
        qDebug()<<"file open error"<<outFilename;
        goto end;
    }


    // 读取数据到frame中，linesize读取到的大小，也是frame最大的容纳大小，通过比特率，声道，采样格式算出来
    // 尝试填满frame缓冲区，但是最后一次有可能填不满缓冲区
    while ((ret = inFile.read((char *) frame->data[0],
                     imgSize)) > 0) {

        if (encode(ctx, frame, pkt, outFile) < 0) {
            goto end;
        }
        // 设置帧的序号
        frame->pts ++;
    }

    // 刷新缓冲区
    encode(ctx, nullptr, pkt, outFile);

    qDebug()<<"线程正常结束";
end:
    // 关闭文件
    inFile.close();
    outFile.close();

    // 释放资源
    if (frame) {
        av_freep(&frame->data[0]);
        av_frame_free(&frame);
    }
    av_packet_free(&pkt);
    avcodec_free_context(&ctx);
    qDebug()<<"来到end 线程正常结束";
}

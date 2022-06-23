#include "ffmpegs.h"
#include <QFile>
#include <QDebug>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret,errbuf,sizeof(errbuf));

FFmpegs::FFmpegs()
{

}

// 检查采样格式
static int check_sample_fmt(const AVCodec *codec,
                            enum AVSampleFormat sample_fmt){
    const enum AVSampleFormat *p = codec->sample_fmts;
    while (*p != AV_SAMPLE_FMT_NONE) {
        qDebug()<<"当前采样格式："<<av_get_sample_fmt_name(*p);
        if (*p == sample_fmt)
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

void FFmpegs::aacEncode(AudioEncodeSpec &in,
                        const char *outFilename) {
    // 文件
    QFile inFile(in.filename);
    QFile outFile(outFilename);

    // 返回结果
    int ret = 0;

    // 编码器
    const AVCodec *codec = nullptr;

    // 编码上下文
    AVCodecContext *ctx = nullptr;

    // 一般编解码的时候需要以下两个对象
    // 存放编码前的数据（pcm），一般从inFile读到的数据先放到frame中去，先进行编码，编码后的数据放到Packet中去
    AVFrame *frame = nullptr;

    // 存放编码后的数据（aac），再把packet中的数据写入到outFile中去
    AVPacket *pkt = nullptr;

    // 获取编码器
    //codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    codec = avcodec_find_encoder_by_name("libfdk_aac");
    if (!codec) {
        qDebug()<<"encoder not found："<<"libfdk_aac";
        return;
    }

    // libfdk_aac对输入的数据有要求，采样格式必须是16位整数
    // 检测输入数据的采样格式是否支持，如果不支持就需要重采样
    if (!check_sample_fmt(codec, in.sampleFmt)) {
        qDebug()<<"unsupported sample format"<<in.sampleFmt;
        return;
    }

    qDebug()<<"codec name"<<codec->name;

    // 创建编码上下文
    ctx = avcodec_alloc_context3(codec);
    if (!ctx) {
        qDebug()<<"avcodec_alloc_context3 failed";
        return;
    }

    // 设置PCM参数
    ctx->sample_rate = in.sampleRate;
    ctx->sample_fmt = in.sampleFmt;
    ctx->channel_layout = in.chLayout;

    // 比特率
    //ctx->bit_rate = 32000;

    // 规格 也就是编码的等级 有aac aac_he_v2 还有其他的
    ctx->profile = FF_PROFILE_AAC_HE_V2;

    // 打开编码器
    AVDictionary *options = nullptr;
    // 开启vbr（Variable Bit Rate，可变比特率），如果开启vbr，就不用写死比特率了
    av_dict_set(&options,"vbr","1", 0);
    // 命令行里面的采纳数 可以通过option 来设置
    // av_dict_set(&options,"profile:a","aac_he_v2", 0);
    ret = avcodec_open2(ctx,codec,&options);
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
    frame->nb_samples = ctx->frame_size;
    frame->format = ctx->sample_fmt;
    frame->channel_layout = ctx->channel_layout;

    // 利用nb_samples,format,channel_layout创建缓冲区
    ret = av_frame_get_buffer(frame,0);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"av_frame_get_buffer error"<<errbuf;
        goto end;
    }
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
                     frame->linesize[0])) > 0) {
        // 从文件中读取的数据，不足以填满frame缓冲区
        if (ret < frame->linesize[0]) {
            // 每个样本多大
            int bytes = av_get_bytes_per_sample((AVSampleFormat)frame->format);
            // 声道数
            int ch = av_get_channel_layout_nb_channels(frame->channel_layout);
            // 设置真正有效的样本数量
            // 防止编码器编码了一些冗余数据
            frame->nb_samples = ret / (bytes * ch);
        }

        if (encode(ctx, frame, pkt, outFile) < 0) {
            goto end;
        }
    }

    // 刷新缓冲区
    encode(ctx, nullptr, pkt, outFile);

    qDebug()<<"线程正常结束";
end:
    // 关闭文件
    inFile.close();
    outFile.close();

    // 释放资源
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&ctx);
    qDebug()<<"来到end 线程正常结束";
}


void FFmpegs::aacDecode(const char *inFilename,
                        AudioDecodeSpec &out) {
    // 返回结果
    int ret = 0;

    // 文件
    QFile inFile(inFilename);
    QFile outFile(out.filename);

    // 编解码器
    const AVCodec *codec = nullptr;

    // 编解码上下文
    AVCodecContext *ctx = nullptr;

    // 解析器上下文
    AVCodecParserContext *parserCtx = nullptr;

    // 存放解码前的数据（aac）
    AVPacket *pkt = nullptr;

    // 存放解码后的数据（pcm）
    AVFrame *frame = nullptr;

    // 获取编码器
    codec = avcodec_find_decoder_by_name("libfdk_aac");
    if (!codec) {
        qDebug()<<"avcodec_find_decoder_by_name error:"<<"libfdk_aac";
        return;
    }

    // 初始化解析器上下文
    parserCtx = av_parser_init(codec->id);
    if (!parserCtx) {
        qDebug()<<"av_parser_init error";
        return;
    }

    // 创建编解码器上下文
    ctx = avcodec_alloc_context3(codec);
    if (!ctx) {
        qDebug()<<"avcodec_alloc_context3 error";
        goto end;
    }

end:
    // 关闭文件
    inFile.close();
    outFile.close();
    // 释放资源
    av_packet_free(&pkt);
    av_frame_free(&frame);
    av_parser_close(parserCtx);
    avcodec_free_context(&ctx);



}

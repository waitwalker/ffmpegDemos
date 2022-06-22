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
    qDebug()<<"线程正常结束";
}

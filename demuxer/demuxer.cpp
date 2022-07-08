#include "demuxer.h"
#include <QDebug>

#define ERROR_BUF \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));

#define END(func) \
    if (ret < 0) { \
        ERROR_BUF; \
        qDebug() << #func << "error" << errbuf; \
        goto end; \
    }

#define RET(func) \
    if (ret < 0) { \
        ERROR_BUF; \
        qDebug() << #func << "error" << errbuf; \
        return ret; \
    }

Demuxer::Demuxer(QObject *parent)
    : QObject{parent}
{

}

// 初始化视频信息
int Demuxer::initVideoInfo() {
    int ret = initDecoder(&_vDecodeCtx, &_vStreamIndex, AVMEDIA_TYPE_VIDEO);
    RET(initDecoder);

    // 打开文件
    _aOutFile.setFileName(_aOut->filename);
    if (!_aOutFile.open(QFile::WriteOnly)) {
        qDebug()<<"file open error"<<_aOut->filename;
        return -1;
    }

    // 保存视频参数
    _vOut->pxiFmt = _vDecodeCtx->pix_fmt;
    _vOut->width = _vDecodeCtx->width;
    _vOut->height = _vDecodeCtx->height;

    AVRational framerate = av_guess_frame_rate(_fmtCtx, _fmtCtx->streams[_aStreamIndex], nullptr);
    _vOut->fps = framerate.num / framerate.den;

    // 创建用于存放一帧解码图片的缓冲区
    ret = av_image_alloc(_imgBuf, _imgLineSize, _vOut->width, _vOut->height, _vOut->pxiFmt, 1);
    RET(av_image_alloc);
    _imgSize = ret;
    return 0;
}

// 初始化音频信息
int Demuxer::initAudioInfo() {
    // 初始化解码器
    int ret = initDecoder(&_aDecodeCtx, &_aStreamIndex, AVMEDIA_TYPE_AUDIO);
    RET(initDecoder);

    // 打开文件
    _vOutFile.setFileName(_vOut->filename);
    if (!_vOutFile.open(QFile::WriteOnly)) {
        qDebug()<<"file open error"<<_vOut->filename;
        return -1;
    }

    // 保存音频参数
    _aOut->sampleRate = _aDecodeCtx->sample_rate;
    _aOut->sampleFmt = _aDecodeCtx->sample_fmt;
    _aOut->chLayout = _aDecodeCtx->channel_layout;
    // 音频样本帧的大小
    _sampleFrameSize = av_get_bytes_per_sample(_aOut->sampleFmt) * _aDecodeCtx->channels;
    return 0;
}

// 初始化解码器
int Demuxer::initDecoder(AVCodecContext **decodeCtx,
                         int *streamIndex,
                         AVMediaType type) {
    // 根据type寻找最合适的流信息 可能会存在多个流
    // 返回值是流索引
    int ret = av_find_best_stream(_fmtCtx, type, -1, -1, nullptr, 0);
    RET(av_find_best_stream);

    // 检验流 找到流就找到数据了
    *streamIndex = ret;
    AVStream *stream = _fmtCtx->streams[*streamIndex];
    if (!stream) {
        qDebug()<<"stream is empty";
        return -1;
    }
    const AVCodec *decoder = nullptr;
    if (stream->codecpar->codec_id == AV_CODEC_ID_AAC) {
        decoder = avcodec_find_decoder_by_name("libfdk_aac");
    } else {
        decoder = avcodec_find_decoder((stream)->codecpar->codec_id);
    }

    // 为当前流找到合适的解码器
//    const AVCodec *decoder = avcodec_find_decoder((stream)->codecpar->codec_id);
    // 如果解码器不存在
    if (!decoder) {
        qDebug()<<"avcodec_find_decoder not found";
        return -1;
    }

    // 初始化解码上下文 打开解码器
    *decodeCtx = avcodec_alloc_context3(decoder);
    if (!decodeCtx) {
        qDebug()<<"avcodec_alloc_context3 error";
        return -1;
    }

    // 流信息中拷贝参数到解码上下文 format rate等参数
    ret = avcodec_parameters_to_context(*decodeCtx, (stream)->codecpar);
    RET(avcodec_parameters_to_context);

    // 打开解码器
    ret = avcodec_open2(*decodeCtx, decoder, nullptr);
    RET(avcodec_open2);
    return 0;
}

void Demuxer::demux(const char *inFilename,
                    AudioDecodeSpec &aOut,
                    VideoDecodeSpec &vOut){
    // 保留参数
    _aOut = &aOut;
    _vOut = &vOut;

    // 返回结果
    int ret = 0;

    AVPacket *pkt = nullptr;


    // 解封装上下文
    ret = avformat_open_input(&_fmtCtx, inFilename, nullptr, nullptr);
    END(avformat_open_input);

    // 检索流信息
    ret = avformat_find_stream_info(_fmtCtx, nullptr);
    END(avformat_find_stream_info);

    // 打印流信息到控制台 调试
    av_dump_format(_fmtCtx, 0, inFilename, 0);

    fflush(stderr);

    // 初始化视频信息 为做音视频解码做准备
    ret = initVideoInfo();
    if (ret < 0) {
        goto end;
    }

    // 初始化音频信息
    ret = initAudioInfo();
    if (ret < 0) {
        goto end;
    }

    // 初始化frame
    _frame = av_frame_alloc();
    if (!_frame) {
        qDebug()<<"av_frame_alloc error";
        goto end;
    }

    // 初始化packet
    pkt = av_packet_alloc();
    pkt->data = nullptr;
    pkt->size = 0;

    // 从输入文件中读取数据
    while (av_read_frame(_fmtCtx, pkt) == 0) {
        if (pkt->stream_index == _aStreamIndex) {
            // 读到数据 读取到是音频数据 下面开始解码
            ret = decode(_aDecodeCtx, pkt, &Demuxer::writeAudioFrame);

        } else if (pkt->stream_index == _vStreamIndex) {
            // 读取到视频数据 下面开始解码
            ret = decode(_vDecodeCtx, pkt, &Demuxer::writeVideoFrame);
        }
        // 清理一下
        av_packet_unref(pkt);
        if (ret < 0) {
            goto end;
        }
    }

    // 刷新缓冲区
    decode(_aDecodeCtx, nullptr, &Demuxer::writeAudioFrame);
    decode(_vDecodeCtx, nullptr, &Demuxer::writeVideoFrame);

    end:
    av_frame_free(&_frame);
    _aOutFile.close();
    _vOutFile.close();
    avcodec_free_context(&_aDecodeCtx);
    avcodec_free_context(&_vDecodeCtx);
    avformat_close_input(&_fmtCtx);
    av_packet_free(&pkt);
    av_freep(&_imgBuf[0]);
}

// 解码数据
int Demuxer::decode(AVCodecContext *decodeCtx, AVPacket *pkt, void (Demuxer::*func)()){
    // 发送压缩数据到解码器
    int ret = avcodec_send_packet(decodeCtx, pkt);
    RET(avcodec_send_packet);

    while (true) {
        // 获取解码后的数据
        ret = avcodec_receive_frame(decodeCtx, _frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
        }
        RET(avcodec_receive_frame);

        // 写入文件的代码 这个是对象方法
        (this->*func)();

        // 将frame数据写入文件
//        if (decodeCtx->codec->type == AVMEDIA_TYPE_VIDEO) {
//            writeVideoFrame();
//        } else {
//            writeAudioFrame();
//        }
    }
}

void Demuxer::writeVideoFrame() {
    // 拷贝frame数据到imgBuf缓冲区
    av_image_copy(_imgBuf, _imgLineSize,
                  (const uint8_t**)(_frame->data),
                  _frame->linesize,
                  _vOut->pxiFmt, _vOut->width, _vOut->height);
    // 将缓冲区的文件写入文件
    _vOutFile.write((char *)_imgBuf[0], _imgSize);

//    // 写入Y平面
//    _vOutFile.write((char *)_frame->data[0],
//            _frame->linesize[0] * _vOut->height);
//    // 写入U平面
//    _vOutFile.write((char *)_frame->data[1],
//            _frame->linesize[1] * _vOut->height >> 1);
//    // 写入V平面
//    _vOutFile.write((char *)_frame->data[2],
//            _frame->linesize[2] * _vOut->height >> 1);
}

void Demuxer::writeAudioFrame() {
    // libfdk_aac解码出来的PCM数据格式：s16 不是planar
    // aac 解码器解码出来的PCM数据格式是ftlp 是planar
    if (av_sample_fmt_is_planar(_aOut->sampleFmt)) {
        // 是planar


    } else {
        // 非planar
        // linesize 如果是视频 是每个平面一行的大小
        // 如果是音频是每个声道平面的大小
        int v1 = _frame->linesize[0];
        int v2 = _frame->nb_samples * av_get_bytes_per_sample(_aOut->sampleFmt);
        if (v1 != v2) {
            qDebug()<<"frame->linesize"<<_frame->linesize[0];
            qDebug()<<"av_get_bytes_per_sample"<<_frame->nb_samples * av_get_bytes_per_sample(_aOut->sampleFmt);
        }

        //_aOutFile.write((char *)_frame->data[0], _frame->linesize[0]);
        _aOutFile.write((char *)_frame->data[0], _frame->nb_samples * _sampleFrameSize);
    }
}














#include "videoplayer.h"
#include <thread>
#include <QDebug>
#include <QThread>

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

#pragma mark - 构造 析构 方法
VideoPlayer::VideoPlayer(QObject *parent)
    : QObject{parent}
{

}

VideoPlayer::~VideoPlayer() {

}

#pragma mark - 公有方法
VideoPlayer::State VideoPlayer::getState() {
    return _state;
}

void VideoPlayer::play() {
    if (_state == VideoPlayer::Playing) return;
    // 能来到后面说明状态可能是：暂停、停止、正常结束

    // 读取解析文件

    // 解封装、解码、播放、音视频同步

    // 多线程

    // 创建子线程播放

    // 解码后的格式不一定是我们播放器想要的 重采样 格式转换
    // 打开子线程 读取文件 Lamba表达式
    std::thread([this](){
        readFile();
    }).detach();

    setState(VideoPlayer::Playing);
}

void VideoPlayer::pause() {
    if (_state != VideoPlayer::Playing) return;

    // 能来到后面说明状态可能是：正在播放
//    stopTimer();
    setState(VideoPlayer::Paused);
}

void VideoPlayer::stop() {
    if (_state == VideoPlayer::Stopped) return;

    setState(VideoPlayer::Stopped);
}

void VideoPlayer::setFilename(const char *filename) {
    _filename = filename;
}

#pragma mark - 私有方法
void VideoPlayer::setState(State state) {
    if (state == _state) return;
    _state = state;
    emit stateChanged(this);
}

void VideoPlayer::readFile() {
    qDebug()<<"current thread"<<QThread::currentThread();
}


// 初始化视频信息
int VideoPlayer::initVideoInfo() {
    int ret = initDecoder(&_vDecodeCtx, &_vStream, AVMEDIA_TYPE_VIDEO);
    RET(initDecoder);
    return 0;
}

// 初始化音频信息
int VideoPlayer::initAudioInfo() {
    // 初始化解码器
    int ret = initDecoder(&_aDecodeCtx, &_aStream, AVMEDIA_TYPE_AUDIO);
    RET(initDecoder);
    return 0;
}

// 初始化解码器
int VideoPlayer::initDecoder(AVCodecContext **decodeCtx,
                         AVStream **stream,
                         AVMediaType type) {
    // 根据type寻找最合适的流信息 可能会存在多个流
    // 返回值是流索引
    int ret = av_find_best_stream(_fmtCtx, type, -1, -1, nullptr, 0);
    RET(av_find_best_stream);

    // 检验流 找到流就找到数据了
    int streamIndex = ret;
    *stream = _fmtCtx->streams[streamIndex];
    if (!stream) {
        qDebug()<<"stream is empty";
        return -1;
    }

    // 为当前流找到合适的解码器
    const AVCodec *decoder = avcodec_find_decoder((*stream)->codecpar->codec_id);
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
    ret = avcodec_parameters_to_context(*decodeCtx, (*stream)->codecpar);
    RET(avcodec_parameters_to_context);

    // 打开解码器
    ret = avcodec_open2(*decodeCtx, decoder, nullptr);
    RET(avcodec_open2);
    return 0;
}






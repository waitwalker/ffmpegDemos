#include "videoplayer.h"
#include <thread>
#include <QDebug>
#include <QThread>

#define AUDIO_MAX_PKT_SIZE 1000
#define VIDEO_MAX_PKT_SIZE 500


#pragma mark - 构造 析构 方法
VideoPlayer::VideoPlayer(QObject *parent)
    : QObject{parent}
{
    // 只需要初始化一次
    if(SDL_Init(SDL_INIT_AUDIO)) {
        qDebug()<<"初始化SDL失败SDL_Init error "<<SDL_GetError();
        emit playFailed(this);
        return;
    }
}

VideoPlayer::~VideoPlayer() {
    // 退出SDL
    SDL_Quit();
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
    if (_state == Stopped) {
        std::thread([this](){
            this->readFile();
        }).detach();
    }
//    this->readFile();
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

    // 释放资源
    free();
}

void VideoPlayer::setFilename(QString &filename) {
    const char *name = filename.toUtf8().data();
    memcpy(_filename, name, strlen(name) + 1);
    qDebug()<<"读取到的文件名称："<<_filename;
}

int64_t VideoPlayer::getDuration() {
    qDebug()<<"_fmtCtx:"<<_fmtCtx;
    return _fmtCtx ? _fmtCtx->duration : 0;
}

void VideoPlayer::setVolume(int volume) {
    _volume = volume;
}

int VideoPlayer::getVolume() {
    return _volume;
}

void VideoPlayer::setMute(bool mute) {
    _mute = mute;
}

bool VideoPlayer::isMute() {
    return _mute;
}

#pragma mark - 私有方法
void VideoPlayer::setState(State state) {
    if (state == _state) return;
    _state = state;
    emit stateChanged(this);
}

void VideoPlayer::readFile() {
    qDebug()<<"current thread"<<QThread::currentThread();

    // 返回结果
    int ret = 0;

    qDebug()<<"读取到的文件信息："<<_filename;

    // 解封装上下文
    ret = avformat_open_input(&_fmtCtx, "/Users/walkerwait/Desktop/1_cut.mp4", nullptr, nullptr);
    qDebug()<<"当前返回值是："<<ret;
    qDebug()<<"当前返回值是_fmtCtx："<<_fmtCtx;
    END(avformat_open_input);

    // 检索流信息
    ret = avformat_find_stream_info(_fmtCtx, nullptr);
    END(avformat_find_stream_info);

    // 打印流信息到控制台 调试
    av_dump_format(_fmtCtx, 0, _filename, 0);

    fflush(stderr);

    // 初始化音频信息 && 初始化视频信息
    bool hasAudio = initAudioInfo() >= 0;
    bool hasVideo = initVideoInfo() >= 0;
    if (!hasAudio && !hasVideo) {
        fataError();
        return;
    }

    // 到这里基本初始化完成 发送一个信号
    emit initFinished(this);

    // 从输入文件中读取数据
    while (_state != Stopped) {
        // 大文件情况需要考虑

        AVPacket pkt;
        // 不能无限制的调用这个 调用一次就读一次
        if (_vPktList.size() >= VIDEO_MAX_PKT_SIZE || _aPktList.size() >= AUDIO_MAX_PKT_SIZE) {
            SDL_Delay(10);
            continue;
        }
        qDebug()<<"当前pkt大小"<<_vPktList.size()<<_aPktList.size();
        ret = av_read_frame(_fmtCtx, &pkt);
        if (ret == 0) {
            if (pkt.stream_index == _aStream->index) {
                // 读到数据 读取到是音频数据 下面开始解码
                addAduioPkt(pkt);
            } else if (pkt.stream_index == _vStream->index) {
                // 读取到视频数据 下面开始解码
                addVideoPkt(pkt);
            } else {
                // 如果不是音频、视频流直接释放
                av_packet_unref(&pkt);
            }
        } else if(ret == AVERROR_EOF){
            // 读到了文件尾部
            break;
        } else {
            // 这次读取失败，跳过去
            ERROR_BUF;
            qDebug()<<"av_read_frame error"<<errbuf;
            continue;
        }
    }
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

// 释放资源
void VideoPlayer::free() {
    avformat_close_input(&_fmtCtx);
    freeAudio();
    freeVideo();
}

void VideoPlayer::fataError() {
    setState(Stopped);
    emit playFailed(this);
    free();
}






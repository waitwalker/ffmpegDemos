#include "videoplayer.h"
#include <QDebug>


// 初始化音频信息
int VideoPlayer::initAudioInfo() {
    // 初始化解码器
    int ret = initDecoder(&_aDecodeCtx, &_aStream, AVMEDIA_TYPE_AUDIO);
    RET(initDecoder);

    // 初始化重采样上下文
    ret = initSwr();
    RET(initSwr);

    // 初始化SDL
    ret = initSDL();
    RET(initSDL);
    return 0;
}

// 初始化重采样上下文
int VideoPlayer::initSwr() {
    // 重采样输入参数
    _aSwrInSpec.sampleFmt = _aDecodeCtx->sample_fmt;
    _aSwrInSpec.sampleRate = _aDecodeCtx->sample_rate;
    _aSwrInSpec.chLayout = _aDecodeCtx->channel_layout;
    _aSwrInSpec.chs = _aDecodeCtx->channels;

    // 重采样之后的输出参数
    _aSwrOutSpec.sampleFmt = AV_SAMPLE_FMT_S16;
    _aSwrOutSpec.sampleRate = 44100;
    _aSwrOutSpec.chLayout = AV_CH_LAYOUT_STEREO;// 双声道 立体声
    _aSwrOutSpec.chs = av_get_channel_layout_nb_channels(_aSwrOutSpec.chLayout);
    _aSwrOutSpec.bytesPerSampleFrame = _aSwrOutSpec.chs * av_get_bytes_per_sample((AVSampleFormat)_aSwrOutSpec.sampleFmt);

    // 创建重采样上下文
    _aSwrCtx = swr_alloc_set_opts(nullptr,
                                 _aSwrOutSpec.chLayout,
                                  _aSwrOutSpec.sampleFmt,
                                  _aSwrOutSpec.sampleRate,
                                  _aSwrInSpec.chLayout,
                                  _aSwrInSpec.sampleFmt,
                                  _aSwrInSpec.sampleRate,
                                  0, nullptr);
    if (!_aSwrCtx) {
        qDebug()<<"初始化_aSwrCtx失败";
        return -1;
    }

    // 初始化重采样上下文
    int ret = swr_init(_aSwrCtx);
    RET(swr_init);

    // 初始化输入frame
    _aSwrInFrame = av_frame_alloc();
    if (!_aSwrInFrame) {
        qDebug()<<"av_frame_alloc in error";
        return -1;
    }

    // 初始化输出frame
    _aSwrOutFrame = av_frame_alloc();
    if (!_aSwrOutFrame) {
        qDebug()<<"av_frame_alloc out error";
        return -1;
    }

    qDebug()<<"前data[0]"<<_aSwrOutFrame->data[0];

    // 初始化重采样输出frame的data【0】空间
    ret = av_samples_alloc(_aSwrOutFrame->data,
                     _aSwrOutFrame->linesize,
                     _aSwrOutSpec.chs,
                     4096, _aSwrOutSpec.sampleFmt, 1);
    RET(av_samples_alloc);
    qDebug()<<"后data[0]"<<_aSwrOutFrame->data[0];

    return 0;
}


// 初始化SDL
int VideoPlayer::initSDL() {
    SDL_AudioSpec spec;
    spec.freq = _aSwrOutSpec.sampleRate;
    // 采样格式 s16le
    spec.format = AUDIO_S16LSB;
    spec.channels = _aSwrOutSpec.chs;
    spec.samples = 512;
    spec.callback = sdlAudioCallBackFunc;
    spec.userdata = this;

    // 打开音频设备
    if (SDL_OpenAudio(&spec, nullptr)) {
        qDebug()<<"打开音频设备失败SDL_OpenAudio"<<SDL_GetError();
        return -1;
    }

    // 开始播放
    SDL_PauseAudio(0);
    return 0;
}

// SDL 填充缓冲区的回调函数
void VideoPlayer::sdlAudioCallBackFunc(void *userdata, Uint8 * stream,
                      int len) {
    VideoPlayer *player = (VideoPlayer *)userdata;
    player->sdlAudioCallBack(stream, len);
}

void VideoPlayer::sdlAudioCallBack(Uint8 *stream, int len) {
    // len ： SDL音频缓冲区剩余的大小（还未填充的大小）
    while (len > 0) {
        int dataSize = decodeAudio();
        qDebug()<<"每次解码的大小："<<dataSize;
        if (dataSize <= 0) {

        } else {

        }
//        // 将一个pkt包解析后的pcm数据填充到SDL的音频缓冲区
//        SDL_MixAudio(stream, src, srcLen, SDL_MIX_MAXVOLUME);
//        // 移动偏移量
//        len -= srcLen;
//        stream += srcLen;
    }
}

int VideoPlayer::decodeAudio() {
    _aMutex->lock();
//    while (_aPktList->empty()) {
//        _aMutex->wait();
//    }

    if (_aPktList->empty()) {
        _aMutex->unlock();
        return 0;
    }

    // 取出头部的数据包
    AVPacket pkt = _aPktList->front();
    // 从头部删除
    _aPktList->pop_front();
    // 解锁
    _aMutex->unlock();

    // 发送数据到解码器
    int ret = avcodec_send_packet(_aDecodeCtx, &pkt);
    // 释放pkt
    av_packet_unref(&pkt);
    RET(avcodec_send_packet);

    ret = avcodec_receive_frame(_aDecodeCtx, _aSwrInFrame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return 0;
    } else RET(avcodec_receive_frame);

    // 由于解码出来的PCM，有可能与SDL要求的格式不一样，需要重采样
    // 重采样样本数
    int outSamples = av_rescale_rnd(_aSwrOutSpec.sampleRate,
                                    _aSwrOutFrame->nb_samples,
                                    _aSwrInSpec.sampleRate,
                                    AV_ROUND_UP);

    // 开始重采样
    ret = swr_convert(_aSwrCtx,
                      _aSwrOutFrame->data,outSamples,
                      (const  uint8_t **)_aSwrInFrame->data,_aSwrInFrame->nb_samples);
    RET(swr_convert);
    return ret * _aSwrOutSpec.bytesPerSampleFrame;
}


void VideoPlayer::addAduioPkt(AVPacket &pkt) {
    _aMutex->lock();
    _aPktList->push_back(pkt);
    _aMutex->signal();
    _aMutex->unlock();
}

void VideoPlayer::clearAudioPktList() {
    _aMutex->lock();
    for(AVPacket &pkt : *_aPktList) {
        av_packet_unref(&pkt);
    }
    _aPktList->clear();
    _aMutex->unlock();
}



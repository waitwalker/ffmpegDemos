#include "videoplayer.h"
#include <QDebug>





// 初始化SDL
int VideoPlayer::initSDL() {
    SDL_AudioSpec spec;
    spec.freq = 44100;
    // 采样格式 s16le
    spec.format = AUDIO_S16LSB;
    spec.channels = 2;
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
        qDebug()<<dataSize;
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

    ret = avcodec_receive_frame(_aDecodeCtx, _aframe);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return 0;
    } else RET(avcodec_receive_frame);

    qDebug()<<"解码后的数据"<<_aframe->sample_rate;
    qDebug()<<"解码后的数据"<<_aframe->channels;
    qDebug()<<"解码后的数据"<<_aframe->sample_rate;
    // 由于解码出来的PCM，有可能与SDL要求的格式不一样，需要重采样
    return _aframe->nb_samples * _aframe->channels * av_get_bytes_per_sample((AVSampleFormat)_aframe->format);
}


// 初始化音频信息
int VideoPlayer::initAudioInfo() {
    // 初始化解码器
    int ret = initDecoder(&_aDecodeCtx, &_aStream, AVMEDIA_TYPE_AUDIO);
    RET(initDecoder);

    // 初始化frame
    _aframe = av_frame_alloc();
    if (!_aframe) {
        qDebug()<<"av_frame_alloc error";
        return -1;
    }

    // 初始化SDL
    ret = initSDL();
    RET(initSDL);
    return 0;
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



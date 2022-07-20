#include "videoplayer.h"
#include <QDebug>

// 初始化视频信息
int VideoPlayer::initVideoInfo() {
    int ret = initDecoder(&_vDecodeCtx, &_vStream, AVMEDIA_TYPE_VIDEO);
    RET(initDecoder);
    return 0;
}

void VideoPlayer::addVideoPkt(AVPacket &pkt) {
    _vMutex->lock();
    _vPktList->push_back(pkt);
    _vMutex->signal();
    _vMutex->unlock();
}

void VideoPlayer::clearVideoPktList() {
    _vMutex->lock();
    for(AVPacket &pkt : *_vPktList) {
        av_packet_unref(&pkt);
    }
    _vPktList->clear();
    _vMutex->unlock();
}

void VideoPlayer::freeVideo() {

}

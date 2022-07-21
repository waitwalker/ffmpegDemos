#include "videoplayer.h"
#include <QDebug>
#include <thread>

// 初始化视频信息
int VideoPlayer::initVideoInfo() {
    int ret = initDecoder(&_vDecodeCtx, &_vStream, AVMEDIA_TYPE_VIDEO);
    RET(initDecoder);
    // 开启新的线程去解码视频数据
    std::thread([this](){
        decodeVideo();
    }).detach();

    return 0;
}

void VideoPlayer::addVideoPkt(AVPacket &pkt) {
    _vMutex.lock();
    _vPktList.push_back(pkt);
    _vMutex.signal();
    _vMutex.unlock();
}

void VideoPlayer::clearVideoPktList() {
    _vMutex.lock();
    for(AVPacket &pkt : _vPktList) {
        av_packet_unref(&pkt);
    }
    _vPktList.clear();
    _vMutex.unlock();
}

// 释放视频相关
void VideoPlayer::freeVideo() {

}

// 解码视频
void VideoPlayer::decodeVideo() {
    // 不断的从vPktList中取出对象
    while (true) {
        _vMutex.lock();
        if (_vPktList.empty()) {
            _vMutex.unlock();
            continue;
        }

        // 从集合中取出最前面的视频包数据  这相当于拷贝构造
        AVPacket pkt = _vPktList.front();
        _vPktList.pop_front();
        _vMutex.unlock();

        // 发送压缩数据到解码器
        int ret = avcodec_send_packet(_vDecodeCtx, &pkt);
        // 释放pkt
        av_packet_unref(&pkt);

        CONTINUE(avcodec_send_packet);

        while (true) {
            ret = avcodec_receive_frame(_vDecodeCtx, _aSwrInFrame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                continue;
            } else CONTINUE(avcodec_receive_frame);

            // 像素格式的转换
        }

    }
}

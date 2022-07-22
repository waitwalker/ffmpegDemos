#include "videoplayer.h"
#include <QDebug>
#include <thread>
extern "C" {
#include <libavutil/imgutils.h>
}

// 初始化视频信息
int VideoPlayer::initVideoInfo() {
    int ret = initDecoder(&_vDecodeCtx, &_vStream, AVMEDIA_TYPE_VIDEO);
    RET(initDecoder);

    // 初始化视频像素格式转换
    ret = initSws();
    RET(initSws);

    // 开启新的线程去解码视频数据
    std::thread([this](){
        decodeVideo();
    }).detach();

    return 0;
}

int VideoPlayer::initSws() {
    // 像素格式转换输出格式

    _vSwsOutSpec.width = _vDecodeCtx->width;
    _vSwsOutSpec.height = _vDecodeCtx->height;
    _vSwsOutSpec.pixfmt = AV_PIX_FMT_RGB24;
    //初始化视频格式转换上下文
    _vSwsCtx = sws_getContext(_vDecodeCtx->width,
                              _vDecodeCtx->height,
                              _vDecodeCtx->pix_fmt,
                              _vSwsOutSpec.width,
                              _vSwsOutSpec.height,
                              _vSwsOutSpec.pixfmt,
                              SWS_BILINEAR, nullptr, nullptr, nullptr);

    // 初始化输入frame
    _vSwsInFrame = av_frame_alloc();
    if (!_vSwsInFrame) {
        qDebug()<<"av_frame_alloc in error";
        return -1;
    }

    // 初始化输出frame  不用创建data[0]指向的空间，解码的时候内部会创建
    _vSwsOutFrame = av_frame_alloc();
    if (!_vSwsOutFrame) {
        qDebug()<<"av_frame_alloc out error";
        return -1;
    }

    // _vSwsOutFrame的data[0]指向的内存空间
    // 为了初始化data【0】，视频解码出来的一定是一帧数据，
    // 意味着data【0】指向的数据能放下一帧就可以了
    // 用以下方法创建一帧数据大小
    int ret = av_image_alloc(_vSwsOutFrame->data,
                             _vSwsOutFrame->linesize,
                             _vSwsOutSpec.width,
                             _vSwsOutSpec.height,
                             _vSwsOutSpec.pixfmt, 1);
    RET(av_image_alloc);

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
            ret = avcodec_receive_frame(_vDecodeCtx, _vSwsInFrame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            } else BREAK(avcodec_receive_frame);

            // 解码后的yuv数据
            qDebug()<<_vSwsInFrame->data;

            // 假停顿
            SDL_Delay(33);

            // 像素格式的转换
            // _vSwsInFrame=>_vSwsOutFrame
            sws_scale(_vSwsCtx,
                      _vSwsInFrame->data, _vSwsInFrame->linesize,
                      0, _vDecodeCtx->height,
                      _vSwsOutFrame->data, _vSwsOutFrame->linesize);
            // 像素格式转换后的数据
            qDebug()<<_vSwsOutFrame->data[0];

            // 发出信号
            emit frameDecoded(this, _vSwsOutFrame->data[0],
                    _vSwsOutSpec);
        }

    }
}

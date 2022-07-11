#include "videoplayer.h"

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

    // 解封装、解码、播放、音视频同步

    // 多线程

    // 创建子线程播放

    // 解码后的格式不一定是我们播放器想要的 重采样 格式转换

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






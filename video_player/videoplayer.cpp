#include "videoplayer.h"

VideoPlayer::VideoPlayer(QObject *parent)
    : QObject{parent}
{

}

VideoPlayer::~VideoPlayer() {

}


void VideoPlayer::setState(State state) {
    if (state == _state) return;
    _state = state;
    emit stateChanged();
}

VideoPlayer::State VideoPlayer::getState() {
    return _state;
}

void VideoPlayer::play() {
    if (_state == VideoPlayer::Playing) return;
    // 能来到后面说明状态可能是：暂停、停止、正常结束
    //_timerId = startTimer(1000/_yuv.fps);
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

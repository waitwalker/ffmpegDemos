#include "yuvplayer.h"

YuvPlayer::YuvPlayer(QWidget *parent)
    : QWidget{parent}
{

}

void YuvPlayer::play() {
    if (_timerId) {
        killTimer(_timerId);
    }
}

void YuvPlayer::pause() {

}

void YuvPlayer::stop() {

}

bool YuvPlayer::isPlaying() {

}

void YuvPlayer::setYuv(Yuv &yuv) {
    _yuv = yuv;
}

void YuvPlayer::timerEvent(QTimerEvent *event){

}

// 析构
YuvPlayer::~YuvPlayer() {
    _file.close();
    SDL_DestroyTexture(_texture);
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
}

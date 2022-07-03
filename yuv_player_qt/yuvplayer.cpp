#include "yuvplayer.h"
#include <QPainter>
extern "C" {
#include <libavutil/imgutils.h>
}


YuvPlayer::State YuvPlayer::getState() {
    return _state;
}

YuvPlayer::YuvPlayer(QWidget *parent)
    : QWidget{parent}
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background: black");
}

void YuvPlayer::play() {
    _timerId = startTimer(1000/_yuv.fps);
    _state = YuvPlayer::Playing;
}

void YuvPlayer::pause() {
    if (_timerId) {
        killTimer(_timerId);
    }
    _state = YuvPlayer::Paused;
}

void YuvPlayer::stop() {
    if (_timerId) {
        killTimer(_timerId);
    }
    _state = YuvPlayer::Stopped;
}

bool YuvPlayer::isPlaying() {
    return _state == YuvPlayer::Playing;
}

// 当组件想重绘的时候，就会调用这个函数
// 想要绘制什么内容，在这个函数中实现
void YuvPlayer::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    // 将图片绘制到当前组件上
//    painter.drawImage(data, width, height, QImage::Format);
}

void YuvPlayer::setYuv(Yuv &yuv) {
    _yuv = yuv;

    // 打开文件
    _file.setFileName(_yuv.filename);
    if (!_file.open(QFile::ReadOnly)) {
        qDebug()<<"file open error" <<_yuv.filename;
    }
}

void YuvPlayer::timerEvent(QTimerEvent *event){
    int imgSize = av_image_get_buffer_size(_yuv.pixelFormat,
                                           _yuv.width, _yuv.height, 1);
    char data[imgSize];
    if (_file.read(data, imgSize) > 0) {

    } else {
        // 文件数据已经读取完毕
        killTimer(_timerId);
    }
}

// 析构
YuvPlayer::~YuvPlayer() {
    _file.close();
}

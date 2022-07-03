#include "yuvplayer.h"
#include <QPainter>
#include "xcd.h"
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
    if (!_currentImage) return;
    QPainter painter(this);
    // 将图片绘制到当前组件上
//    painter.drawImage(QPoint(0,0),*_currentImage);
    painter.drawImage(QRect(0,0,width(), height()),*_currentImage);
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
        RawVideoFrame in = {
            data,
            _yuv.width,_yuv.height, _yuv.pixelFormat
        };

        RawVideoFrame out = {
            nullptr,
            _yuv.width, _yuv.height, AV_PIX_FMT_RGB24
        };
        XCD::convertRawFrame(in, out);
        freeCurrentImage();
        _currentImage = new QImage((uchar *)out.pixels, out.width, out.height, QImage::Format_RGB888);
        // 刷新
        update();
    } else {
        // 文件数据已经读取完毕
        killTimer(_timerId);
    }
}

void YuvPlayer::freeCurrentImage() {
    if(!_currentImage) return;
    free(_currentImage->bits());
    delete _currentImage;
    _currentImage = nullptr;
}

// 析构
YuvPlayer::~YuvPlayer() {
    _file.close();
    freeCurrentImage();
}

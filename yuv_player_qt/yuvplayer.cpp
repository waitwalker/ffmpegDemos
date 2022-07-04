#include "yuvplayer.h"
#include <QPainter>
#include "xcd.h"
extern "C" {
#include <libavutil/imgutils.h>
}

void YuvPlayer::setState(State state) {
    if (state == _state) return;
    if (state == YuvPlayer::Stopped || state == YuvPlayer::Finished) {

        // 让文件读取指针回到文件首部
        _file.seek(0);

    }
    _state = state;
    emit stateChanged();
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
    if (_state == YuvPlayer::Playing) return;
    // 能来到后面说明状态可能是：暂停、停止、正常结束
    _timerId = startTimer(1000/_yuv.fps);
    setState(YuvPlayer::Playing);
}

void YuvPlayer::pause() {
    if (_state != YuvPlayer::Playing) return;

    // 能来到后面说明状态可能是：正在播放
    stopTimer();
    setState(YuvPlayer::Paused);
}

void YuvPlayer::stop() {
    if (_state == YuvPlayer::Stopped) return;

    // 能来到后面说明状态可能是：正在播放、暂停、正常完毕
    stopTimer();
    // 释放图片
    freeCurrentImage();
    // 刷新
    update();
    setState(YuvPlayer::Stopped);
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
    painter.drawImage(_dstRect, *_currentImage);
}

void YuvPlayer::setYuv(Yuv &yuv) {
    _yuv = yuv;

    // 组件尺寸
    int w = width();
    int h = height();

    // 计算Rect
    int dx = 0;
    int dy = 0;
    int dw = _yuv.width;
    int dh = _yuv.height;
    // 计算目标尺寸
    if (dw > w || dh > h) { // 缩放
        if (dw * h > w * dw) {//视频的宽高比 > 播放器的宽高比
            dh = w * dh / dw;
            dw = w;
        } else {
            dw = h * dw / dh;
            dh = h;
        }
    }

    // 居中
    dx = (w - dw) >> 1;
    dy = (h - dh) >> 1;

    _dstRect = QRect(dx, dy, dw, dh);

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

        // 刷新 调用后会重绘
        update();
    } else {
        // 文件数据已经读取完毕
        // 停止定时器
        stopTimer();
        // 正常播放完毕
        setState(YuvPlayer::Finished);
    }
}

void YuvPlayer::stopTimer() {
    if (_timerId == 0) return;
    killTimer(_timerId);
    _timerId = 0;
}

// 释放当前帧数据
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

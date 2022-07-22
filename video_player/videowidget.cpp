#include "videowidget.h"
#include <QDebug>
#include <QPainter>

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget{parent}
{
    qDebug()<<"VideoWidget初始化了";
}

VideoWidget::~VideoWidget() {
    if (_image) {
        delete _image;
        _image = nullptr;
    }
}


void VideoWidget::paintEvent(QPaintEvent *event) {
    if (!_image) return;
    // 将图片绘制到屏幕上
    QPainter(this).drawImage(QRect(0,0, width(), height()), *_image);
}


void VideoWidget::onFrameDecoded(VideoPlayer *player,
                                 uint8_t *data,
                                 VideoPlayer::VideoSwsSpec spec) {
    // 释放之前的图片
    if (_image) {
        delete _image;
        _image = nullptr;
    }

    if (data != nullptr) {
        _image = new QImage((uchar *)data,
                            spec.width, spec.height,
                            QImage::Format_RGB888);
    }
    update();
}




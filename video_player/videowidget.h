#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include "videoplayer.h"

class VideoWidget : public QWidget
{
    Q_OBJECT
public:

    // 构造函数
    explicit VideoWidget(QWidget *parent = nullptr);
    // 析构函数
    ~VideoWidget();

private:
    QImage *_image = nullptr;
    void paintEvent(QPaintEvent *event) override;

signals:

public slots:
    void onFrameDecoded(VideoPlayer *player,
                        uint8_t *data,
                        VideoPlayer::VideoSwsSpec spec);


};

#endif // VIDEOWIDGET_H

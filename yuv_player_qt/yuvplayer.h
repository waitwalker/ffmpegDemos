#ifndef YUVPLAYER_H
#define YUVPLAYER_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QFile>

extern "C" {
#include <libavutil/avutil.h>
}

#define FILENAME ""

typedef struct {
    const char *filename;
    int width;
    int height;
    AVPixelFormat pixelFormat;// 像素格式
    int fps;// 帧率
} Yuv;

class YuvPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit YuvPlayer(QWidget *parent = nullptr);
    ~YuvPlayer();
    // 播放器状态
    typedef enum {
        Playing,
        Paused,
        Stopped = 0,
        Finished
    } State;

    void play();
    void pause();
    void stop();
    bool isPlaying();

    void setYuv(Yuv &yuv);

    State getState();

private:
    QWidget *_widget;
    QFile *_file = nullptr;
    // 一帧图片大小
    int _imageSize;
    int _timerId = 0;
    Yuv _yuv;
    State _state = Stopped;

    QRect _dstRect;

    QImage *_currentImage = nullptr;
    void timerEvent(QTimerEvent *event);

    void paintEvent(QPaintEvent *event);

    // 释放图片
    void freeCurrentImage();

    // 停止定时器
    void stopTimer();

    // 关闭文件
    void closeFile();

    // 设置状态（改变状态）
    void setState(State state);

signals:
    // 状态改变的时候发送信号
    void stateChanged();

};

#endif // YUVPLAYER_H

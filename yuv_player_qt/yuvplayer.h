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
    QFile _file;
    int _timerId = 0;
    Yuv _yuv;
    State _state = Stopped;

    QRect _dstRect;

    QImage *_currentImage = nullptr;
    void timerEvent(QTimerEvent *event);

    void paintEvent(QPaintEvent *event);

    void freeCurrentImage();

signals:

};

#endif // YUVPLAYER_H

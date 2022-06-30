#ifndef YUVPLAYER_H
#define YUVPLAYER_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <SDL2/SDL.h>
#include <QFile>

extern "C" {
#include <libavutil/avutil.h>
}

#define FILENAME ""

#define RET(judge, func) \
    if (judge) {\
        qDebug()<<#func << "error"<<SDL_GetError(); \
        return; \
    }


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
    SDL_Window *_window = nullptr;
    SDL_Renderer *_renderer = nullptr;
    SDL_Texture *_texture = nullptr;
    QFile _file;
    int _timerId = 0;
    Yuv _yuv;
    State _state = Stopped;
    void timerEvent(QTimerEvent *event);

signals:

};

#endif // YUVPLAYER_H

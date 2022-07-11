#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>

class VideoPlayer : public QObject
{
    Q_OBJECT

public:

    typedef enum {
        Stopped = 0,
        Playing,
        Paused,
        Failed
    } State;
    // 构造函数
    explicit VideoPlayer(QObject *parent = nullptr);
    // 析构函数
    ~VideoPlayer();

    // 播放
    void play();
    // 暂停
    void pause();
    // 停止
    void stop();
    // 是否正在播放
    bool isPlaying();
    // 获取播放状态
    State getState();


    //void setYuv(Yuv &yuv);

private:
    // 当前播放状态
    State _state = Stopped;
    // 设置状态（改变状态）
    void setState(State state);
signals:
    // 状态改变的时候发送信号
    void stateChanged();

signals:

};

#endif // VIDEOPLAYER_H

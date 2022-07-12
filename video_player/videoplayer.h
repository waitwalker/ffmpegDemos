#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
extern "C" {
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}

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

    // 设置文件
    void setFilename(const char *filename);

    // 获取总时长
    int64_t getDuration();

private:
    // 文件名称
    const char *_filename;
    // 当前播放状态
    State _state = Stopped;
    // 设置状态（改变状态）
    void setState(State state);

    // 读取文件数据
    void readFile();

    // 解封装上下文
    AVFormatContext *_fmtCtx = nullptr;
    // 音频解码上下文
    AVCodecContext *_aDecodeCtx = nullptr;
    // 视频解码上下文
    AVCodecContext *_vDecodeCtx = nullptr;
    // 音频解码器
    AVCodec *aDecodec = nullptr;
    // 视频解码器
    AVCodec *vDecodec = nullptr;
    // 音频和视频流
    AVStream *_aStream = nullptr, *_vStream = nullptr;

    // 存放解码后的数据
    AVFrame *_aframe = nullptr, *_vframe = nullptr;

    // 初始化音频信息
    int initAudioInfo();
    // 初始化视频信息
    int initVideoInfo();
    // 初始化解码器和解码上下文
    int initDecoder(AVCodecContext **decodeCtx,
                    AVStream **stream,
                    AVMediaType type);


signals:
    // 状态改变的时候发送信号
    void stateChanged(VideoPlayer *player);
    void initFinished(VideoPlayer *player);





















signals:

};

#endif // VIDEOPLAYER_H

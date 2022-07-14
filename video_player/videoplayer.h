#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <list>
#include "condmutex.h"
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

    // 获取总时长 1s=1000毫秒=10^6微秒
    int64_t getDuration();

private:

    /*********** 音频相关 ***********/
    // 视频解码上下文
    AVCodecContext *_vDecodeCtx = nullptr;
    // 音频流
    AVStream *_aStream = nullptr;
    // 音频解码器
    AVCodec *aDecodec = nullptr;
    // 存放解码后的数据
    AVFrame *_aframe = nullptr;
    // 存放音频包的列表
    std::list<AVPacket> *_aPktList = nullptr;
    // 添加数据包到音频列表中
    void addAduioPkt(AVPacket &pkt);
    // 音频包列表的锁 用来加锁_aPktList
    CondMutex *_aMutex = nullptr;
    // 清空音频包列表
    void clearAudioPktList();

    /*********** 视频相关 ***********/
    // 音频解码上下文
    AVCodecContext *_aDecodeCtx = nullptr;
    // 视频流
    AVStream *_vStream = nullptr;
    // 视频解码器
    AVCodec *vDecodec = nullptr;
    // 存放解码后的数据
    AVFrame  *_vframe = nullptr;
    // 存放视频包的列表
    std::list<AVPacket> *_vPktList = nullptr;
    // 添加数据包到视频列表中
    void addVideoPkt(AVPacket &pkt);
    // 视频包列表的锁 用来加锁_vPktList
    CondMutex *_vMutex = nullptr;
    // 清空视频包列表
    void clearVideoPktList();


    /*********** 其他公用相关 ***********/
    // 解封装上下文
    AVFormatContext *_fmtCtx = nullptr;
    // 文件名称
    const char *_filename;
    // 当前播放状态
    State _state = Stopped;
    // 设置状态（改变状态）
    void setState(State state);
    // 读取文件数据
    void readFile();
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
    void playFailed(VideoPlayer *player);





















signals:

};

#endif // VIDEOPLAYER_H

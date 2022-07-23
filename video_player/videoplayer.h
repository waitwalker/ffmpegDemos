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
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#define ERROR_BUF \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));

#define CODE(func, code) \
    if (ret < 0) { \
        ERROR_BUF; \
        qDebug() << #func << "error" << errbuf; \
        code; \
    }

#define END(func) CODE(func, fataError(); return;)
#define RET(func) CODE(func, return ret;)
#define CONTINUE(func) CODE(func, continue;)
#define BREAK(func) CODE(func, break;)



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

    // 视频中一帧数据格式
    typedef struct{
        int width;
        int height;
        AVPixelFormat pixfmt;
    } VideoSwsSpec;

    // 音量
    typedef enum{
        Min = 0,
        Max = 100
    } Volume;


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
    void setFilename(QString &filename);

    // 获取总时长 1s=1000毫秒=10^6微秒
    int getDuration();

    // 当前播放时间 单位是秒
    int getCurrent();


    // 释放资源
    void free();
    void freeAudio();
    void freeVideo();

    // 设置音量
    void setVolume(int volume);
    // 设置静音
    void setMute(bool mute);
    // 是否静音
    bool isMute();
    // 获取音量
    int getVolume();
private:

    /*********** 音频相关 ***********/
    typedef struct{
        int sampleRate;
        AVSampleFormat sampleFmt;
        int chLayout;
        int chs;
        int bytesPerSampleFrame;
    } AudioSwrSpec;
    // 视频解码上下文
    AVCodecContext *_vDecodeCtx = nullptr;
    // 音频流
    AVStream *_aStream = nullptr;
    // 音频解码器
    AVCodec *aDecodec = nullptr;
    // 存放音频包的列表
    std::list<AVPacket> _aPktList;
    // 音频包列表的锁 用来加锁_aPktList
    CondMutex _aMutex;
    // 音频重采样上下文
    SwrContext *_aSwrCtx = nullptr;
    // 音频重采样输入输出参数
    AudioSwrSpec _aSwrInSpec;
    AudioSwrSpec _aSwrOutSpec;
    // 音频重采样输入输出存放解码后的数据
    AVFrame *_aSwrInFrame = nullptr;
    AVFrame *_aSwrOutFrame = nullptr;
    int _aSwrOutIdx = 0;
    // 重采样输出PCM的大小
    int _aSwrOutSize = 0;
    // 音量
    int _volume = Max;

    // 是否静音
    bool _mute = false;

    // 音频时钟 当前音频包对应的时间值
    double _aClock = 0.0;

    // 添加数据包到音频列表中
    void addAduioPkt(AVPacket &pkt);
    // 清空音频包列表
    void clearAudioPktList();

    // 初始化音频重采样上下文
    int initSwr();

    // 初始化SDL
    int initSDL();
    // SDL 填充缓冲区的回调函数
    static void sdlAudioCallBackFunc(void *userdata, Uint8 * stream,
                          int len);
    // SDL 填充缓冲区的回调函数
    void sdlAudioCallBack(Uint8 * stream,
                          int len);

    // 音频解码
    int decodeAudio();


    /*********** 视频相关 ***********/
    // 音频解码上下文
    AVCodecContext *_aDecodeCtx = nullptr;
    // 视频流
    AVStream *_vStream = nullptr;
    // 视频解码器
    AVCodec *vDecodec = nullptr;
    // 视频像素格式转换输入输出存放解码后的数据
    AVFrame *_vSwsInFrame = nullptr;
    AVFrame *_vSwsOutFrame = nullptr;

    // 存放视频包的列表
    std::list<AVPacket> _vPktList;
    // 添加数据包到视频列表中
    void addVideoPkt(AVPacket &pkt);
    // 视频包列表的锁 用来加锁_vPktList
    CondMutex _vMutex;

    SwsContext *_vSwsCtx = nullptr;
    // 视频格式转换输出frame的参数
    VideoSwsSpec _vSwsOutSpec;

    // 视频时钟 当前视频包对应的时间值
    double _vClock = 0.0;
    // 清空视频包列表
    void clearVideoPktList();

    // 解码视频
    void decodeVideo();

    // 初始化视频格式转换上下文
    int initSws();




    /*********** 其他公用相关 ***********/
    // 解封装上下文
    AVFormatContext *_fmtCtx = nullptr;
    // 文件名称
    char _filename[512];
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
    // 发生严重错误
    void fataError();



signals:
    // 状态改变的时候发送信号
    void stateChanged(VideoPlayer *player);
    void initFinished(VideoPlayer *player);
    void playFailed(VideoPlayer *player);
    void frameDecoded(VideoPlayer *player,
                      uint8_t *data, VideoSwsSpec spec);
    void timeChange(VideoPlayer *player);




















signals:

};

#endif // VIDEOPLAYER_H

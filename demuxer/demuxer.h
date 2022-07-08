#ifndef DEMUXER_H
#define DEMUXER_H

#include <QObject>
#include <QFile>
extern "C" {
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}



typedef struct {
    const char *filename;
    int width;
    int height;
    AVPixelFormat pxiFmt;
    int fps;
} VideoDecodeSpec;

typedef struct {
    const char *filename;
    int sampleRate;
    AVSampleFormat sampleFmt;
    int chLayout;
} AudioDecodeSpec;

class Demuxer : public QObject
{
    Q_OBJECT
public:
    explicit Demuxer(QObject *parent = nullptr);

    void demux(const char *inFilename,
               AudioDecodeSpec &aOut,
               VideoDecodeSpec &vOut);
private:
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
//    AVStream *_aStream = nullptr, *_vStream = nullptr;

    // 流索引
    int _aStreamIndex = 0, _vStreamIndex = 0;

    // 文件
    QFile _aOutFile;
    QFile _vOutFile;

    // 函数参数
    AudioDecodeSpec *_aOut;
    VideoDecodeSpec *_vOut;
    // 存放解码后的数据
    AVFrame *_frame = nullptr;

    // 存放一帧解码图片的缓冲区
    uint8_t *_imgBuf[4] = {nullptr};
    int _imgLineSize[4] = {0};
    int _imgSize = 0;

    int initAudioInfo();
    int initVideoInfo();
    int initDecoder(AVCodecContext **decodeCtx,
                    int *streamIndex,
                    AVMediaType type);
    int decode(AVCodecContext *decodeCtx,
               AVPacket *pkt,
               void (Demuxer::*func)());

    void writeVideoFrame();
    void writeAudioFrame();

signals:

};

#endif // DEMUXER_H

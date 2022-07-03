#ifndef XCD_H
#define XCD_H

#include <QObject>
extern "C" {
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#define ERR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof(errbuf));

typedef struct {
    const char * filename;
    int width;
    int height;
    AVPixelFormat pixelFormat;
} RawVideoFile ;


typedef struct {
    // 一帧的像素数据
    char * pixels;
    int width;
    int height;
    AVPixelFormat pixelFormat;
} RawVideoFrame ;


class XCD : public QObject
{
    Q_OBJECT
public:
    explicit XCD(QObject *parent = nullptr);
    static void convertRawVideo(RawVideoFile &in,
                                RawVideoFile &out);

    static void convertRawFrame(RawVideoFrame &in,
                                RawVideoFrame &out);

signals:

};

#endif // XCD_H

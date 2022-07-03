#ifndef XCD_H
#define XCD_H

#include <QObject>
extern "C" {
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

typedef struct {
    const char * filename;
    int width;
    int height;
    AVPixelFormat pixelFormat;
} RawVideoFile ;


class XCD : public QObject
{
    Q_OBJECT
public:
    explicit XCD(QObject *parent = nullptr);
    static void convertRawVideo(RawVideoFile &in,
                                RawVideoFile &out);

signals:

};

#endif // XCD_H

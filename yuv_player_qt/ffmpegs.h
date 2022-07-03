#ifndef FFMPEGS_H
#define FFMPEGS_H

extern "C" {
#include <libavutil/avutil.h>
}

typedef struct {
    const char * filename;
    int width;
    int height;
    AVPixelFormat pixelFormat;
} RawVideoFile ;


class FFmpegs
{
public:
    FFmpegs();

    static void convertRawVideo(RawVideoFile &in,
                                RawVideoFile &out);
};

#endif // FFMPEGS_H

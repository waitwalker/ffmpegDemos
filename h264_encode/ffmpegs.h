#ifndef FFMPEGS_H
#define FFMPEGS_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

typedef struct {
    const char *filename;
    int width;
    int height;
    AVPixelFormat pix_fmt;
    int fps;
} VideoEncodeSpec;

class FFmpegs
{
public:
    FFmpegs();
    static void h264Encode(VideoEncodeSpec &in,
           const char *outFilename);
};

#endif // FFMPEGS_H

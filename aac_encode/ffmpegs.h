#ifndef FFMPEGS_H
#define FFMPEGS_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

typedef struct {
    const char *filename;
    int sampleRate;
    AVSampleFormat sampleFmt;
    uint64_t chLayout;
} AudioEncodeSpec;

class FFmpegs
{
public:
    FFmpegs();
    static void aacEncode(AudioEncodeSpec &in,
           const char *outFilename);
};

#endif // FFMPEGS_H

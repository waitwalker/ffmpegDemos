#ifndef FFMPEGS_H
#define FFMPEGS_H
extern "C" {
#include <libavformat/avformat.h>
}

class FFmpegs
{
public:
    FFmpegs();
    // 音频重采样工具方法封装
    static void resampleAuido(const char *inFilename,
                              int inSampleRate,
                              AVSampleFormat inSampleFmt,
                              int inChLayout,
                              const char *outFilename,
                              int outSampleRate,
                              AVSampleFormat outSampleFmt,
                              int outChLayout
                              );
};

#endif // FFMPEGS_H

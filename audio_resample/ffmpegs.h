#ifndef FFMPEGS_H
#define FFMPEGS_H
extern "C" {
#include <libavformat/avformat.h>
}

// 重采样相关参数
typedef struct {
    // 文件路径
    const char *filename;
    // 采样格式
    AVSampleFormat sampleFmt;
    // 采样率
    int sampleRate;
    // 声道布局
    int  chLayout;
} ResampleAudioSpec;

class FFmpegs
{
public:
    FFmpegs();
    // 音频重采样工具方法封装 两个方法一样
    static void resampleAuido(ResampleAudioSpec &in,
                              ResampleAudioSpec &out);

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

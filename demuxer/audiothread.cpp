#include "audiothread.h"
#include <QDebug>
//#include "ffmpegs.h"
#include "demuxer.h"
extern "C" {
#include <libavutil/imgutils.h>
}

AudioThread::AudioThread(QObject *parent)
    : QThread{parent}
{
    connect(this,&AudioThread::finished,
            this,&AudioThread::deleteLater);
}

AudioThread::~AudioThread(){
    // 断开所有连接
    disconnect();
    // 内存回收之前，正常结束线程
    requestInterruption();
    // 安全退出
    quit();
    wait();
    qDebug()<<this<<"析构（内存被回收）";
}

void AudioThread::run() {
//    AudioEncodeSpec in;
//    in.filename = "/Users/walkerwait/Desktop/44100_s16le.pcm";
//    in.sampleRate = 44100;
//    in.sampleFmt = AV_SAMPLE_FMT_S16;
//    in.chLayout = AV_CH_LAYOUT_STEREO;
//    FFmpegs::aacEncode(in,"/Users/walkerwait/Desktop/out7.aac");

    AudioDecodeSpec aOut;
    aOut.filename = "/Users/walkerwait/Desktop/out_audio.pcm";

    VideoDecodeSpec vOut;
    vOut.filename = "/Users/walkerwait/Desktop/out_video.yuv";
    Demuxer().demux("/Users/walkerwait/Desktop/工作/Demos/ffmpegDemos/resource/1_cut.mp4", aOut, vOut);

    qDebug()<<aOut.sampleRate
           <<av_get_channel_layout_nb_channels(aOut.chLayout)
          <<av_get_sample_fmt_name(aOut.sampleFmt);
    qDebug()<<"宽度："<<vOut.width;
    qDebug()<<"高度："<<vOut.height;
    qDebug()<<"编码格式："<<av_get_pix_fmt_name(vOut.pxiFmt);

//    FFmpegs::h264Decode("/Users/walkerwait/Desktop/out.h264",out);


}

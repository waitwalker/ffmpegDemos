#include "audiothread.h"
#include <QDebug>
#include "ffmpegs.h"

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
    VideoEncodeSpec in;
    in.filename = "/Users/walkerwait/Desktop/工作/Demos/ffmpegDemos/resource/1.yuv";
    in.width = 640;
    in.height = 360;
    in.fps = 30;
    in.pix_fmt = AV_PIX_FMT_YUV420P;
    FFmpegs::h264Encode(in,"/Users/walkerwait/Desktop/out.h264");
}

#include "playthread.h"
#include <QDebug>
#include <SDL2/SDL.h>
#include <QDebug>
#include <QFile>

#define FILE_NAME "/Users/walkerwait/Desktop/in.wav"


typedef struct {
    char *data = nullptr;
    int len = 0;
    int pulllen = 0;
} AudioBuffer;

PlayThread::PlayThread(QObject *parent)
    : QThread{parent}
{
    connect(this, &PlayThread::finished, this, &PlayThread::deleteLater);
}


PlayThread::~PlayThread() {
    disconnect();
    requestInterruption();
    quit();
    wait();
    qDebug()<<this<<"析构（释放内存）";
}

// 等待音频设备回调（会回调很多次）在子线程，默认会创建一个线程
void pull_audio_data(void *userData,
                     // 需要往stream中填充pcm数据
                     Uint8 *stream,
                     // 希望填充的大小（等于缓冲区样本数量*每个样本的大小 samples * channels * bit / 8 = 4096）
                     int len
                     ){
    qDebug()<<"pull_audio_data"<<len;

    // 清空stream
    SDL_memset(stream, 0, len);

    AudioBuffer *buffer = (AudioBuffer *)userData;

    // 文件还没准备好
    if(buffer->len <=0 ) {
        return;
    }

    // 取len,bufferLen大的最小值
    buffer->pulllen = (len > buffer->len) ? buffer->len : len;

    // 填充数据 到 stream去
    SDL_MixAudio(stream, (Uint8 *)buffer->data, buffer->pulllen, SDL_MIX_MAXVOLUME);
    buffer->pulllen += buffer->pulllen;
    buffer->len -= buffer->pulllen;

}

void PlayThread::run(){
    SDL_version version;
    SDL_VERSION(&version);
    qDebug()<<"当前版本："<<version.major<<version.minor<<version.patch;

    // 初始化Audio子系统
    int ret = SDL_Init(SDL_INIT_AUDIO);
    if (ret < 0) {
        qDebug()<<"SDL_Init error"<<SDL_GetError();
        return;
    }

    // 加载wav文件
    SDL_AudioSpec spec;
    // 指向pcm数据
    Uint8 *data = nullptr;
    // pcm数据的长度
    Uint32 len;
    SDL_LoadWAV(FILE_NAME, &spec, &data, &len);
    if (!SDL_LoadWAV(FILE_NAME, &spec, &data, &len)) {
        SDL_Quit();
        qDebug()<<"SDL_LoadWAV error"<<SDL_GetError();
        return;
    }

    qDebug()<<spec.freq<<spec.channels;
    qDebug()<<*data;
    qDebug()<<len;

    if (SDL_OpenAudio(&spec,nullptr) < 0) {
        qDebug()<<"SDL_OpenAudio error"<<SDL_GetError();
        SDL_Quit();
        return;
    }

    // 打开文件
    QFile file(FILE_NAME);
    if (!file.open(QFile::ReadOnly)) {
        qDebug()<<"file open error"<<FILE_NAME;
        SDL_CloseAudio();
        SDL_Quit();
        return;
    }

    // SDL播放视频有两种方式：
    // PUSH（推）程序主动推送数据给音频设备
    // PULL（拉）音频设备主动向程序拉取数据

    // 开始播放 (0是取消暂停)
    SDL_PauseAudio(0);
//    // 存放从文件中读取的数据 是一个数组
//    char data[BUFFER_SIZE];
//    while (!isInterruptionRequested()) {
//        // 只要从文件中读取的音频数据，还没有填充完毕就跳过
//        if (buffer.len > 0) continue;
//        // 真实大小，虽然传的是BUFFER_SIZE但是真实读出来的不是这个大小
//        buffer.len = file.read(data, BUFFER_SIZE);
//        // 文件数据已经读取完毕
//        if (buffer.len <= 0 ) {
//            // 剩余的样本数量
//            int samples = buffer.pulllen / BYTES_PER_SAMPLE;
//            int ms = samples * 1000 / SAMPLE_RATE;
//            SDL_Delay(ms);
//            qDebug()<<ms;
//            break;
//        }
//        // 读取到了文件数据
//        buffer.data = data;
//        // 采样率（每秒采样的次数）
//        // freq
//        // 每个样本的大小
//        // size
//        // 字节率 = freq * size
//        // 2000/字节率 = 时间
//    }

    // 关闭文件
    file.close();

    // 关闭设备
    SDL_CloseAudio();

    // 清除所有的子系统
    SDL_Quit();
}

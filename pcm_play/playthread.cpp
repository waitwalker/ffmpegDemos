#include "playthread.h"
#include <QDebug>
#include <SDL2/SDL.h>
#include <QDebug>
#include <QFile>

#define SMAPLE_RATE 44100
#define SMAPLE_SIZE 16
#define CHANNELS 2
#define FILE_NAME "/Users/walkerwait/Desktop/06_07_11_35_05.pcm"
#define BUFFER_SIZE 4096

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

int bufferLen;
char *bufferdata;

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

    // 文件还没准备好
    if(bufferLen <=0 ) {
        return;
    }

    // 取len,bufferLen大的最小值
    len = (len > bufferLen) ? bufferLen : len;

    // 填充数据
    SDL_MixAudio(stream, (Uint8 *)bufferdata, len, SDL_MIX_MAXVOLUME);
    bufferdata += len;
    bufferdata -= len;

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

    // 打开设备
    SDL_AudioSpec spec;
    // 采样率
    spec.freq = SMAPLE_RATE;
    // 声道
    spec.channels = CHANNELS;
    spec.format = AUDIO_S16MSB;
    spec.callback = pull_audio_data;
    // 音频缓冲区样本的数量（这个值必须是2的幂）
    spec.samples = 1024;

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
    // 存放从文件中读取的数据
    char data[BUFFER_SIZE];
    while (!isInterruptionRequested()) {
        // 真实大小，虽然传的是BUFFER_SIZE但是真实读出来的不是这个大小
        bufferLen = file.read(data, BUFFER_SIZE);
        if (bufferLen <= 0 ) {
            break;
        }
        // 读取到了文件数据
        bufferdata = data;

        while (bufferLen > 0) {
            SDL_Delay(1);
        }
    }

    // 关闭文件
    file.close();

    // 关闭设备
    SDL_CloseAudio();

    // 清除所有的子系统
    SDL_Quit();
}
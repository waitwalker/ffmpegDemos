#include "mainwindow.h"
#include <QApplication>
#include "ffmpegs.h"

extern "C" {
#include <libavdevice/avdevice.h>
}

int main(int argc, char *argv[])
{
    // 1.注册设备
    avdevice_register_all();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    WAVHeader header;
    header.riffChunkDataSize = 5556;
    header.sampleRate = 44100;
    header.bitsPerSample = 16;
    header.numChannels = 2;
    header.blockAlign = header.bitsPerSample * header.numChannels >> 3;
    header.byteRate = header.bitsPerSample * header.blockAlign;
    header.dataChunkDataSize = 1413120;
    FFmpegs::pcm2wav(header,"/Users/walkerwait/Desktop/06_07_14_20_04.pcm","/Users/walkerwait/Desktop/out.wav");
    return a.exec();
}

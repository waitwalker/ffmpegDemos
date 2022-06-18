#include "ffmpegs.h"
#include <QFile>
#include <QDebug>

FFmpegs::FFmpegs()
{

}

void FFmpegs::pcm2wav(WAVHeader &header,
                      const char *pcmFileName,
                      const char *wavFileName) {
    header.blockAlign = header.bitsPerSample * header.numChannels >> 3;
    header.byteRate = header.bitsPerSample * header.blockAlign;
    // 打开pcm文件
    QFile pcmFile(pcmFileName);

    // 打开文件
    if (!pcmFile.open(QFile::ReadOnly)) {
        qDebug() << "pcm文件打开失败："<<pcmFileName;
        return;
    }
    header.dataChunkDataSize = pcmFile.size();
    header.dataChunkDataSize = pcmFile.size();
    header.riffChunkDataSize = header.dataChunkDataSize
                               + sizeof (WAVHeader)
                               - sizeof (header.riffChunkId)
                               - sizeof (header.riffChunkDataSize);

    // 打开wav文件
    QFile wavFile(wavFileName);
    if (!wavFile.open(QFile::WriteOnly)) {
        qDebug()<<"wav文件打开失败："<<wavFileName;
        // 这里打开wav文件路径失败，但是打开pcm文件已经成功了，需要把pcm文件关闭
        pcmFile.close();
        return;
    }

    // 文件中写入头部数据
    wavFile.write((const char *)&header, sizeof(WAVHeader));

    // 写入pcm数据
    // 读取pcm文件的到缓冲区大小
    char buf[1024];
    // 读取到的真正大小
    int size;
    while ((size = pcmFile.read(buf, sizeof(buf))) > 0){
        wavFile.write(buf, size);
    }

    // 关闭文件
    pcmFile.close();
    wavFile.close();
}

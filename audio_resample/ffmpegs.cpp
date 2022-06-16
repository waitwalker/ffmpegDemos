#include "ffmpegs.h"
#include <QDebug>
#include <QObject>
#include <QFile>
extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
}

#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret,errbuf,sizeof(errbuf));

FFmpegs::FFmpegs()
{

}

void FFmpegs::resampleAuido(ResampleAudioSpec &in,
                            ResampleAudioSpec &out) {
    // 文件已经准备好了
    // 文件名
    QFile inFile(in.filename);
    QFile outFile(out.filename);

    // 输入的一些参数双声道，立体声

    // 创建输入缓冲区
    // 指向缓冲区的指针
    uint8_t **inData = nullptr;
    // 缓冲区的大小 最终会显示缓冲区多大
    int inLineSize = 0;
    // 声道数量
    int inChs = av_get_channel_layout_nb_channels(in.chLayout);
    // 一个样本的大小
    int inBytesPerSample = inChs * av_get_bytes_per_sample(in.sampleFmt);
    // 缓冲区样本数量
    int inSamples = 1024;

    // 创建输出缓冲区
    // 指向缓冲区的指针
    uint8_t **outData = nullptr;
    // 缓冲区的大小 最终会显示缓冲区多大
    int outLineSize = 0;
    // 声道数量
    int outChs = av_get_channel_layout_nb_channels(out.chLayout);
    // 一个样本的大小
    int outBytesPerSample = outChs * av_get_bytes_per_sample(out.sampleFmt);
    // 缓冲区样本数量 根据输入缓冲区计算出来 为了保持单位时间处理样本的数量，好比之前8分钟，处理完不能让人家播放10分钟
    int outSamples = av_rescale_rnd(out.sampleRate, inSamples, in.sampleRate, AV_ROUND_UP);//向上取整
    // 返回值
    int ret;
    // 读取文件的大小
    int len = 0;

    // 创建重采样上下文
    SwrContext *ctx = swr_alloc_set_opts(nullptr,
                       // 输出参数
                       out.chLayout, out.sampleFmt, out.sampleRate,
                       // 输入参数
                       in.chLayout, in.sampleFmt, in.sampleRate,
                       0, nullptr);
    if (!ctx) {
        // 只要是C语言 alloc创建的堆空间，需要手动释放
        qDebug()<<"swr_alloc_set_opts error";
        goto end;
        return;
    }

    // 初始化重采样上下文
    ret = swr_init(ctx);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"swr_init error:"<< errbuf;
        goto end;
        return;
    }

    // 创建输入缓冲区
    ret = av_samples_alloc_array_and_samples(
                &inData,// inData传进去后，希望调用完这个函数后能指向一块空间，也就是inData
                &inLineSize,// 指向的缓冲区有多大
                inChs,
                inSamples,
                in.sampleFmt,
                1);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"av_samples_alloc_array_and_samples error:"<<errbuf;
        goto end;
        return;
    }

    // 创建输出缓冲区
    ret = av_samples_alloc_array_and_samples(
                &outData,
                &outLineSize,
                outChs,
                outSamples,
                out.sampleFmt,
                1);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"av_samples_alloc_array_and_samples error:"<<errbuf;
        // 在许多编程语言中goto 语句是禁止的， 为了方便这里
        goto end;
        return;
    }

    // 打开文件
    if(!inFile.open(QFile::ReadOnly)) {
        qDebug()<<"in file open error:"<<in.filename;
        goto end;
    }

    if(!outFile.open(QFile::WriteOnly)) {
        qDebug()<<"out file open error:"<<out.filename;
        goto end;
    }

    // 读取文件数据
    while ((len = inFile.read((char *)inData[0], inLineSize)) > 0) {
        // 读取的样本数量 读的大小除以每个样本的大小
        inSamples = len / inBytesPerSample;
        // 重采样(返回值是转换后的样本数据)
        ret = swr_convert(ctx,
                    outData,
                    outSamples,
                    (const uint8_t **)inData,
                    inSamples
                    );
        if(ret < 0) {
            ERROR_BUF(ret);
            qDebug()<<"swr_convert error";
            goto end;
        }
        // 将转换后的数据写入到输出文件中
        outFile.write((char *)outData[0], ret * outBytesPerSample);
    }

    // 检测一下缓冲区是否还有残留的样本
    while ((ret = swr_convert(ctx,
                              outData,
                              outSamples,
                              nullptr,
                              0)) > 0) {
        outFile.write((char *)outData[0], ret * outBytesPerSample);
    }

end:
    // 释放资源
    // 关闭文件
    inFile.close();
    outFile.close();

    // 释放输入缓冲区
    av_freep(&inData);

    // 释放输出缓冲区
    av_freep(&outData);

    // 释放重采样上下文
    swr_free(&ctx);

}

void FFmpegs::resampleAuido(const char *inFilename,
                              int inSampleRate,
                              AVSampleFormat inSampleFmt,
                              int inChLayout,
                              const char *outFilename,
                              int outSampleRate,
                              AVSampleFormat outSampleFmt,
                              int outChLayout
                   ) {
    // 文件已经准备好了
    // 文件名
    QFile inFile(inFilename);
    QFile outFile(outFilename);

    // 输入的一些参数双声道，立体声

    // 创建输入缓冲区
    // 指向缓冲区的指针
    uint8_t **inData = nullptr;
    // 缓冲区的大小 最终会显示缓冲区多大
    int inLineSize = 0;
    // 声道数量
    int inChs = av_get_channel_layout_nb_channels(inChLayout);
    // 一个样本的大小
    int inBytesPerSample = inChs * av_get_bytes_per_sample(inSampleFmt);
    // 缓冲区样本数量
    int inSamples = 1024;

    // 创建输出缓冲区
    // 指向缓冲区的指针
    uint8_t **outData = nullptr;
    // 缓冲区的大小 最终会显示缓冲区多大
    int outLineSize = 0;
    // 声道数量
    int outChs = av_get_channel_layout_nb_channels(outChLayout);
    // 一个样本的大小
    int outBytesPerSample = outChs * av_get_bytes_per_sample(outSampleFmt);
    // 缓冲区样本数量 根据输入缓冲区计算出来 为了保持单位时间处理样本的数量，好比之前8分钟，处理完不能让人家播放10分钟
    int outSamples = av_rescale_rnd(outSampleRate, inSamples, inSampleRate, AV_ROUND_UP);//向上取整
    // 返回值
    int ret;
    // 读取文件的大小
    int len = 0;

    // 创建重采样上下文
    SwrContext *ctx = swr_alloc_set_opts(nullptr,
                       // 输出参数
                       outChLayout, outSampleFmt, outSampleRate,
                       // 输入参数
                       inChLayout, inSampleFmt, inSampleRate,
                       0, nullptr);
    if (!ctx) {
        // 只要是C语言 alloc创建的堆空间，需要手动释放
        qDebug()<<"swr_alloc_set_opts error";
        goto end;
        return;
    }

    // 初始化重采样上下文
    ret = swr_init(ctx);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"swr_init error:"<< errbuf;
        goto end;
        return;
    }

    // 创建输入缓冲区
    ret = av_samples_alloc_array_and_samples(
                &inData,// inData传进去后，希望调用完这个函数后能指向一块空间，也就是inData
                &inLineSize,// 指向的缓冲区有多大
                inChs,
                inSamples,
                inSampleFmt,
                1);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"av_samples_alloc_array_and_samples error:"<<errbuf;
        goto end;
        return;
    }

    // 创建输出缓冲区
    ret = av_samples_alloc_array_and_samples(
                &outData,
                &outLineSize,
                outChs,
                outSamples,
                outSampleFmt,
                1);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug()<<"av_samples_alloc_array_and_samples error:"<<errbuf;
        // 在许多编程语言中goto 语句是禁止的， 为了方便这里
        goto end;
        return;
    }

    // 打开文件
    if(!inFile.open(QFile::ReadOnly)) {
        qDebug()<<"in file open error:"<<inFilename;
        goto end;
    }

    if(!outFile.open(QFile::WriteOnly)) {
        qDebug()<<"out file open error:"<<outFilename;
        goto end;
    }

    // 读取文件数据
    while ((len = inFile.read((char *)inData[0], inLineSize)) > 0) {
        // 读取的样本数量 读的大小除以每个样本的大小
        inSamples = len / inBytesPerSample;
        // 重采样(返回值是转换后的样本数据)
        ret = swr_convert(ctx,
                    outData,
                    outSamples,
                    (const uint8_t **)inData,
                    inSamples
                    );
        if(ret < 0) {
            ERROR_BUF(ret);
            qDebug()<<"swr_convert error";
            goto end;
        }
        // 将转换后的数据写入到输出文件中
        outFile.write((char *)outData[0], ret * outBytesPerSample);
    }

    // 检测一下缓冲区是否还有残留的样本
    while ((ret = swr_convert(ctx,
                              outData,
                              outSamples,
                              nullptr,
                              0)) > 0) {
        outFile.write((char *)outData[0], ret * outBytesPerSample);
    }

end:
    // 释放资源
    // 关闭文件
    inFile.close();
    outFile.close();

    // 释放输入缓冲区
    av_freep(&inData);

    // 释放输出缓冲区
    av_freep(&outData);

    // 释放重采样上下文
    swr_free(&ctx);

}

#include "xcd.h"
#include <QDebug>
#include <QFile>



XCD::XCD(QObject *parent)
    : QObject{parent}
{

}

void XCD::convertRawVideo(RawVideoFile &in,
                              RawVideoFile &out) {
    // 上下文
    SwsContext *ctx = nullptr;

    // 输入 输出缓冲区
    uint8_t *inData[4];
    uint8_t *outData[4];
    // 每一个平面的大小
    int inStrides[4];
    int outStrides[4];
    // 有多少个像素，Y就有多少字节
    // 每一帧图片大小
    int inFrameSize;
    int outFrameSize;
    // 返回结果
    int ret = 0;
    // 进行到了哪一帧
    int frameIdx = 0;

    QFile inFile(in.filename);
    QFile outFile(out.filename);

    // 创建上下文
    ctx = sws_getContext(in.width, in.height, in.pixelFormat,
                         out.width, out.height, out.pixelFormat,
                         SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!ctx) {
        qDebug()<<"sws_getContext error";
        goto end;
    }

    // 创建输入缓冲区
    ret = av_image_alloc(inData, inStrides, in.width, in.height, in.pixelFormat, 1);
    if (ret < 0) {
        ERR_BUF(ret);
        qDebug()<< "av_image_alloc error:"<<errbuf;
        goto end;
    }

    // 创建输出缓冲区
    ret = av_image_alloc(outData, outStrides, out.width, out.height, out.pixelFormat, 1);
    if (ret < 0) {
        ERR_BUF(ret);
        qDebug()<< "av_image_alloc error:"<<errbuf;
        goto end;
    }

    if (!inFile.open(QFile::ReadOnly)) {
        qDebug()<<"inFile.open error";
        goto end;
    }

    if (!outFile.open(QFile::WriteOnly)) {
        qDebug()<<"outFile.open error";
        goto end;
    }

    // 计算每一帧图片大小
    inFrameSize = av_image_get_buffer_size(in.pixelFormat, in.width, in.height, 1);
    outFrameSize = av_image_get_buffer_size(out.pixelFormat, out.width, out.height, 1);

    // 进行每一帧的转换
    while (inFile.read((char *)inData[0], inFrameSize) == inFrameSize) {
        // 转换
        sws_scale(ctx, inData, inStrides, 0, in.height,  outData, outStrides);

        // 写到输出文件里面去
        outFile.write((char *)outData[0], outFrameSize);

        qDebug()<<"转换完第"<<frameIdx++<<"帧";
    }

end:
    inFile.close();
    outFile.close();
    av_freep(&inData[0]);
    av_freep(&outData[0]);
    sws_freeContext(ctx);

}

void XCD::convertRawFrame(RawVideoFrame &in,
                              RawVideoFrame &out) {
    // 上下文
    SwsContext *ctx = nullptr;

    // 输入 输出缓冲区
    uint8_t *inData[4];
    uint8_t *outData[4];
    // 每一个平面的大小
    int inStrides[4];
    int outStrides[4];
    // 有多少个像素，Y就有多少字节
    // 每一帧图片大小
    int inFrameSize;
    int outFrameSize;
    // 返回结果
    int ret = 0;

    // 创建上下文
    ctx = sws_getContext(in.width, in.height, in.pixelFormat,
                         out.width, out.height, out.pixelFormat,
                         SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!ctx) {
        qDebug()<<"sws_getContext error";
        goto end;
    }

    // 创建输入缓冲区
    ret = av_image_alloc(inData, inStrides, in.width, in.height, in.pixelFormat, 1);
    if (ret < 0) {
        ERR_BUF(ret);
        qDebug()<< "av_image_alloc error:"<<errbuf;
        goto end;
    }

    // 创建输出缓冲区
    ret = av_image_alloc(outData, outStrides, out.width, out.height, out.pixelFormat, 1);
    if (ret < 0) {
        ERR_BUF(ret);
        qDebug()<< "av_image_alloc error:"<<errbuf;
        goto end;
    }
    // 计算每一帧图片大小
    inFrameSize = av_image_get_buffer_size(in.pixelFormat, in.width, in.height, 1);
    outFrameSize = av_image_get_buffer_size(out.pixelFormat, out.width, out.height, 1);

    // 拷贝输入数据
    memcpy(inData[0], in.pixels, inFrameSize);

    // 转换
    sws_scale(ctx, inData, inStrides, 0, in.height,  outData, outStrides);

    // 写到输出文件里面去
    out.pixels = (char *)malloc(outFrameSize);
    memcpy(out.pixels, outData[0], outFrameSize);

end:

    av_freep(&inData[0]);
    av_freep(&outData[0]);
    sws_freeContext(ctx);

}

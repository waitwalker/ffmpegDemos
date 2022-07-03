#include "xcd.h"
#include <QDebug>

XCD::XCD(QObject *parent)
    : QObject{parent}
{

}

void XCD::convertRawVideo(RawVideoFile &in,
                              RawVideoFile &out) {
    SwsContext *ctx = nullptr;

    // 创建上下文
    ctx = sws_getContext(in.width, in.height, in.pixelFormat,
                         out.width, out.height, out.pixelFormat,
                         SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!ctx) {
        qDebug()<<"sws_getContext error";
        goto end;
    }

    // 转换
    sws_scale();

end:
    sws_freeContext(ctx);

}

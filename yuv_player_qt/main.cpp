#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include "xcd.h"

int main(int argc, char *argv[])
{
//    RawVideoFile in = {
//        "/Users/walkerwait/Desktop/out.yuv",
//        1920,1076,AV_PIX_FMT_YUV420P
//    };
//    RawVideoFile out = {
//        "/Users/walkerwait/Desktop/out1.yuv",
//        1920,1076,AV_PIX_FMT_RGB24
//    };

//    XCD::convertRawVideo(in, out);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

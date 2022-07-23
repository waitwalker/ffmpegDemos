#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

/**
音视频同步方案：
1.视频同步到音频
音频频率已经定好了，最好的做法

2.音频同步到视频
以视频为标准，音频参考视频那边时间
*/

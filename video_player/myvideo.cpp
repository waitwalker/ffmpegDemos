#include "myvideo.h"
#include <QDebug>

MyVideo::MyVideo(QWidget *parent)
    : QWidget{parent}
{
    qDebug()<<"MyVideo初始化了";
}

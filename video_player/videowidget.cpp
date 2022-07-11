#include "videowidget.h"
#include <QDebug>

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget{parent}
{
    qDebug()<<"VideoWidget初始化了";
}

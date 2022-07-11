#include "videoslider.h"
#include <QMouseEvent>

VideoSlider::VideoSlider(QWidget *parent)
    : QSlider{parent}
{

}

void VideoSlider:: mousePressEvent(QMouseEvent *ev) {
    int currentValue = minimum() + (ev->pos().x() * 1.0 / width()) * (maximum() - minimum());
    setValue(currentValue);

    QSlider::mousePressEvent(ev);
}

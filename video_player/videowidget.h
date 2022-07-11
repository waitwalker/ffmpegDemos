#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>

class VideoWidget : public QWidget
{
    Q_OBJECT
public:

    // 构造函数
    explicit VideoWidget(QWidget *parent = nullptr);
    // 析构函数
    ~VideoWidget();

private:

signals:

};

#endif // VIDEOWIDGET_H

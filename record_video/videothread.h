#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QObject>
#include <QThread>

class VideoThread : public QThread
{
    Q_OBJECT
private:
    void run() override;
public:
    explicit VideoThread(QObject *parent = nullptr);
    ~VideoThread();

signals:

};

#endif // VIDEOTHREAD_H

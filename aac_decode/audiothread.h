#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <QObject>
#include <QThread>

class AudioThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioThread(QObject *parent = nullptr);
    ~AudioThread();
private:
    void run() override;
signals:

};

#endif // AUDIOTHREAD_H

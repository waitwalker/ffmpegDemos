#ifndef SHOWTHREAD_H
#define SHOWTHREAD_H

#include <QObject>
#include <QThread>

class ShowThread : public QThread
{
    Q_OBJECT
private:
    void run() override;
public:
    explicit ShowThread(QObject *parent = nullptr);
    ~ShowThread();

signals:

};

#endif // SHOWTHREAD_H

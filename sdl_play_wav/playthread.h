#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QObject>
#include <QThread>

class PlayThread : public QThread
{
    Q_OBJECT
public:
    explicit PlayThread(QObject *parent = nullptr);
    ~PlayThread();
protected:
    void run() override;

signals:

};

#endif // PLAYTHREAD_H

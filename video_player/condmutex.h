#ifndef CONDMUTEX_H
#define CONDMUTEX_H

#include <SDL2/SDL.h>
#include <QObject>

class CondMutex : public QObject
{
    Q_OBJECT
public:
    explicit CondMutex(QObject *parent = nullptr);
    ~CondMutex();

    void lock();
    void unlock();

    void signal();
    void wait();
    void broadcast();


signals:
private:
    // 互斥锁
    SDL_mutex *_mutex = nullptr;

    // 条件变量
    SDL_cond *_cond = nullptr;

};

#endif // CONDMUTEX_H

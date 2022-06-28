#ifndef SHOWTHREAD_H
#define SHOWTHREAD_H

#include <QObject>
#include <QThread>
#include <SDL2/SDL.h>

class ShowThread : public QThread
{
    Q_OBJECT
private:
    void run() override;
    SDL_Texture *createTexture(SDL_Renderer *render);

    void showClick(SDL_Event &event, SDL_Renderer *renderer, SDL_Texture *texture);
public:
    explicit ShowThread(QObject *parent = nullptr);
    ~ShowThread();

signals:

};

#endif // SHOWTHREAD_H

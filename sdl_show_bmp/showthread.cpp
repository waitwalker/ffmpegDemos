#include "showthread.h"
#include <QDebug>
// 头文件已经帮忙处理 extern了
#include <SDL2/SDL.h>
extern "C" {

}

#define END(judge, func) \
    if (!judge) { \
        qDebug()<<#func<< "error:"<<SDL_GetError(); \
        goto end; \
    }

ShowThread::ShowThread(QObject *parent)
    : QThread{parent}
{
    connect(this, &ShowThread::finished,
            this, &ShowThread::deleteLater);
}

ShowThread::~ShowThread() {
    disconnect();
    requestInterruption();

    quit();
    wait();
    qDebug()<<"正常析构了";
}

void ShowThread::run(){
    // 像素数据
    SDL_Surface *surface = nullptr;
    // 窗口
    SDL_Window *window = nullptr;

    // 初始化子系统
    END(SDL_Init(SDL_INIT_VIDEO),SDL_Init);
//    if (SDL_Init(SDL_INIT_VIDEO)) {
//        qDebug()<<"SDL_Init error:"<<SDL_GetError();
//        goto end;
//    }

    // 加载bmp文件
    surface =  SDL_LoadBMP("");

    END(!surface, SDL_LoadBMP);
//    if (!surface) {
//        qDebug()<<"SDL_LoadBMP error:"<<SDL_GetError();
//        goto end;
//    }

    // 创建一个窗口
     window = SDL_CreateWindow("SDL显示图片",
                     // x
                     SDL_WINDOWPOS_UNDEFINED,
                     // y
                     SDL_WINDOWPOS_UNDEFINED,
                     // w
                     surface->w,
                     // h
                     surface->h,
                     SDL_WINDOW_SHOWN
                     );
    END(!window,SDL_CreateWindow);
//    if (!window) {
//        qDebug()<<"SDL_CreateWindow error:"<<SDL_GetError();
//        goto end;
//    }

    while (!isInterruptionRequested()) {

    }
end:
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

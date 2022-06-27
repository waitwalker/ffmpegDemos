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
    qDebug()<<"开始执行run方法";
    // 像素数据
    SDL_Surface *surface = nullptr;
    // 窗口
    SDL_Window *window = nullptr;

    // 渲染上下文
    SDL_Renderer *renderer = nullptr;

    // 创建纹理
    SDL_Texture *texture = nullptr;

    // 矩形框
    SDL_Rect srcRect = {0,0,512,512};
    SDL_Rect dstRect = {0,0,512,512};

    // 初始化子系统
    END(SDL_Init(SDL_INIT_VIDEO),SDL_Init);
//    if (SDL_Init(SDL_INIT_VIDEO)) {
//        qDebug()<<"SDL_Init error:"<<SDL_GetError();
//        goto end;
//    }

    // 加载bmp文件
    qDebug()<<"走到这里1";
    surface =  SDL_LoadBMP("/Users/walkerwait/Desktop/工作/Demos/ffmpegDemos/resource/in.bmp");
    qDebug()<<"走到这里2";
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

    // 创建渲染上下文
    renderer = SDL_CreateRenderer(window,-1,
                                  SDL_RENDERER_ACCELERATED |
                                  SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, 0);
        END(!renderer, SDL_CreateRenderer);
    }

    // 创建纹理
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    END(!texture, SDL_CreateTextureFromSurface);

    // 设置绘制颜色（画笔颜色）
    SDL_SetRenderDrawColor(renderer,
                           0,0,0,SDL_ALPHA_OPAQUE);

    // 用绘制颜色（画笔颜色）清除渲染目标
    SDL_RenderClear(renderer);

    // 拷贝纹理数据到渲染目标（默认是window）
    END(SDL_RenderCopy(renderer, texture, &srcRect, &dstRect), SDL_RenderCopy);

    // 更新所有的操作到屏幕上
    SDL_RenderPresent(renderer);

    SDL_Delay(2000);

end:
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_Quit();
}

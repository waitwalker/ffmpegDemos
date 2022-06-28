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

SDL_Texture * ShowThread::createTexture(SDL_Renderer *renderer) {
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32,
                      SDL_TEXTUREACCESS_TARGET,
                      50,50);
    if (!texture) return nullptr;

    // 设置纹理为渲染目标
    if (SDL_SetRenderTarget(renderer, texture)) return nullptr;

    // 设置颜色
    if (SDL_SetRenderDrawColor(renderer,0, 0, 0, 255)) return nullptr;;

    // 清空
    if (SDL_RenderClear(renderer)) return nullptr;;

    // 设置颜色
    if (SDL_SetRenderDrawColor(renderer,255, 255,0, SDL_ALPHA_OPAQUE)) return nullptr;;

    // 画空心的矩形框 应该是填满整个纹理
    SDL_Rect rect = {0,0,50,50};
    if (SDL_RenderDrawRect(renderer, &rect)) return nullptr;

    // 画线
    if (SDL_RenderDrawLine(renderer, 0,0,50,50)) return nullptr;;
    if (SDL_RenderDrawLine(renderer, 50,0,0,50)) return nullptr;

    return texture;
}

ShowThread::~ShowThread() {
    disconnect();
    requestInterruption();

    quit();
    wait();
    qDebug()<<"正常析构了";
}

void ShowThread::showClick(SDL_Event &event,
                           SDL_Renderer *renderer,
                           SDL_Texture *texture) {
    SDL_MouseButtonEvent btn = event.button;

    int w = 0;
    int h = 0;
    // 查询纹理宽高
    if (SDL_QueryTexture(texture, nullptr, nullptr, &w, &h)) return;
    // 点击在中间位置
    int x = btn.x - (w >> 1);
    int y = btn.y - (h >> 1);
    SDL_Rect rect = {x,y,w,h};

    // 清除操作
    if (SDL_RenderClear(renderer)) return;

    // 赋值纹理到渲染目标
    SDL_RenderCopy(renderer, texture, nullptr, &rect);

    // 更新渲染操作到屏幕上
    SDL_RenderPresent(renderer);
}

void ShowThread::run(){
    qDebug()<<"开始执行run方法";

    // 窗口
    SDL_Window *window = nullptr;

    // 渲染上下文
    SDL_Renderer *renderer = nullptr;

    // 创建纹理
    SDL_Texture *texture = nullptr;

    SDL_Rect dstRect = {100,100,50,50};

    // 初始化子系统
    END(SDL_Init(SDL_INIT_VIDEO),SDL_Init);
//    if (SDL_Init(SDL_INIT_VIDEO)) {
//        qDebug()<<"SDL_Init error:"<<SDL_GetError();
//        goto end;
//    }

    // 创建一个窗口
     window = SDL_CreateWindow("SDL修改渲染目标",
                     // x
                     SDL_WINDOWPOS_UNDEFINED,
                     // y
                     SDL_WINDOWPOS_UNDEFINED,
                     // w
                     1000,
                     // h
                     1000,
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
    texture = createTexture(renderer);
    END(!texture, SDL_CreateTextureFromSurface);

    // 设置渲染目标为window
    END(SDL_SetRenderTarget(renderer, nullptr), SDL_SetRenderTarget);

    // 设置绘制颜色（画笔颜色）
    END(SDL_SetRenderDrawColor(renderer,
                                   0,0,0,SDL_ALPHA_OPAQUE),SDL_SetRenderDrawColor);

    // 用绘制颜色（画笔颜色）清除渲染目标
    END(SDL_RenderClear(renderer),SDL_RenderClear);

    // 拷贝文件到渲染目标
    END(SDL_RenderCopy(renderer, texture, nullptr, &dstRect), SDL_RenderCopy);

    // 更新所有的操作到屏幕上
    SDL_RenderPresent(renderer);

    // 等待退出事件 监听各种事件
    while (!isInterruptionRequested()) {
        SDL_Event event;
        SDL_WaitEvent(&event);
        qDebug()<<"事件"<<event.type;
        switch (event.type) {
            case SDL_QUIT:
                goto end;

        case SDL_MOUSEBUTTONUP:
            showClick(event, renderer, texture);
            break;

        }
    }

end:

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_Quit();
}

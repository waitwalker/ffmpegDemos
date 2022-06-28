#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "showthread.h"
#include <SDL2/SDL.h>
#include <QFile>

#define END(judge, func) \
    if (judge) { \
        qDebug() << #func << "error" << SDL_GetError(); \
        goto end; \
    }

#define FILENAME "/Users/walkerwait/Desktop/工作/Demos/ffmpegDemos/resource/image.yuv"
#define PIXEL_FORMAT SDL_PIXELFORMAT_IYUV
#define IMG_W 512
#define IMG_H 512


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_showButton_clicked()
{
    // 窗口
    SDL_Window *window = nullptr;

    // 渲染上下文
    SDL_Renderer *renderer = nullptr;

    // 纹理（直接跟特定驱动程序相关的像素数据）
    SDL_Texture *texture = nullptr;

    QFile file(FILENAME);

    // 初始化子系统
    END(SDL_Init(SDL_INIT_VIDEO), SDL_Init);

    // 创建窗口
    window = SDL_CreateWindow(
                 // 标题
                 "SDL显示YUV图片",
                 // x
                 SDL_WINDOWPOS_UNDEFINED,
                 // y
                 SDL_WINDOWPOS_UNDEFINED,
                 // w
                 IMG_W,
                 // h
                 IMG_H,
                 SDL_WINDOW_SHOWN
             );
    END(!window, SDL_CreateWindow);

    // 创建渲染上下文
    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED |
                                  SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, 0);
        END(!renderer, SDL_CreateRenderer);
    }

    // 创建纹理
    texture = SDL_CreateTexture(renderer,
                                PIXEL_FORMAT,
                                SDL_TEXTUREACCESS_STREAMING,
                                IMG_W, IMG_H);
    END(!texture, SDL_CreateTextureFromSurface);

    // 读取yuv文件数据 读取一张图片上所有的YUV数据
    if (!file.open(QFile::ReadOnly)) {
        qDebug()<<"file open error"<<FILENAME;
        goto end;
    }

    // 将YUV的像素数据填充到texture上去
    END(SDL_UpdateTexture(texture, nullptr, file.readAll().data(), IMG_W), SDL_UpdateTexture);

    // 画一个红色的矩形框
    END(SDL_SetRenderDrawColor(renderer,
                               255, 0, 0, SDL_ALPHA_OPAQUE),
        SDL_SetRenderDrawColor);

    // 设置绘制颜色（画笔颜色）
    END(SDL_SetRenderDrawColor(renderer,
                               0, 0, 0, SDL_ALPHA_OPAQUE),
        SDL_SetRenderDrawColor);

    // 用绘制颜色（画笔颜色）清除渲染目标
    END(SDL_RenderClear(renderer),
        SDL_RenderClear);

    // 拷贝纹理数据到渲染目标（默认是window）
    END(SDL_RenderCopy(renderer, texture, nullptr, nullptr),
        SDL_RenderCopy);

    // 更新所有的渲染操作到屏幕上
    SDL_RenderPresent(renderer);

    SDL_Delay(12000);

end:
    file.close();
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


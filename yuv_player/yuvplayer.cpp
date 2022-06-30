#include "yuvplayer.h"
extern "C" {
#include <libavutil/imgutils.h>
}

static const std::map<AVPixelFormat, SDL_PixelFormatEnum> PIXEL_FORMAT_MAP = {
    {AV_PIX_FMT_YUV420P, SDL_PIXELFORMAT_IYUV},
    {AV_PIX_FMT_YUYV422, SDL_PIXELFORMAT_YUY2},
    {AV_PIX_FMT_NONE, SDL_PIXELFORMAT_UNKNOWN},
};

YuvPlayer::State YuvPlayer::getState() {
    return _state;
}

YuvPlayer::YuvPlayer(QWidget *parent)
    : QWidget{parent}
{
    // 创建窗口
    _window = SDL_CreateWindowFrom((void *) winId());
    RET(!_window, SDL_CreateWindowFrom);

    // 创建渲染上下文
    _renderer = SDL_CreateRenderer(_window, -1,
                                   SDL_RENDERER_ACCELERATED |
                                   SDL_RENDERER_PRESENTVSYNC);
    if (!_renderer) {
        _renderer = SDL_CreateRenderer(_window, -1, 0);
        RET(!_renderer, SDL_CreateRenderer);
    }
}

void YuvPlayer::play() {
    _timerId = startTimer(1000/_yuv.fps);
    _state = YuvPlayer::Playing;
}

void YuvPlayer::pause() {
    if (_timerId) {
        killTimer(_timerId);
    }
    _state = YuvPlayer::Paused;
}

void YuvPlayer::stop() {
    if (_timerId) {
        killTimer(_timerId);
    }
    _state = YuvPlayer::Stopped;
}

bool YuvPlayer::isPlaying() {
    return _state == YuvPlayer::Playing;
}

void YuvPlayer::setYuv(Yuv &yuv) {
    _yuv = yuv;
    // 创建纹理
    _texture = SDL_CreateTexture(_renderer,
                                 PIXEL_FORMAT_MAP.find(_yuv.pixelFormat)->second,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 _yuv.width, _yuv.height);
    RET(!_texture, SDL_CreateTexture);

    // 打开文件
    _file.setFileName(_yuv.filename);
    if (!_file.open(QFile::ReadOnly)) {
        qDebug()<<"file open error" <<_yuv.filename;
    }
}

void YuvPlayer::timerEvent(QTimerEvent *event){
    int imgSize = av_image_get_buffer_size(_yuv.pixelFormat,
                                           _yuv.width, _yuv.height, 1);
    char data[imgSize];
    if (_file.read(data, imgSize) > 0) {
        // 将YUV的像素数据填充到texture上
        RET(SDL_UpdateTexture(_texture, nullptr, data, _yuv.width), SDL_UpdateTexture);

        // 设置绘制颜色（画笔颜色）
        RET(SDL_SetRenderDrawColor(_renderer,0,0,0,SDL_ALPHA_OPAQUE), SDL_SetRenderDrawColor);

        // 用绘制颜色（画笔颜色）清除渲染目标
        RET(SDL_RenderCopy(_renderer, _texture, nullptr, nullptr), SDL_RenderCopy);

        // 更新所有的渲染操作到屏幕上
        SDL_RenderPresent(_renderer);
    } else {
        // 文件数据已经读取完毕
        killTimer(_timerId);
    }
}

// 析构
YuvPlayer::~YuvPlayer() {
    _file.close();
    SDL_DestroyTexture(_texture);
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
}

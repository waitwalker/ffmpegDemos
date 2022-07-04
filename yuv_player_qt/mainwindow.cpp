#include "mainwindow.h"
#include "ui_mainwindow.h"

static int yuvIndex = 0;
static const Yuv yuvs[] = {
    {
            "/Users/walkerwait/Desktop/工作/Demos/ffmpegDemos/resource/1.yuv",
            640,360,
            AV_PIX_FMT_YUV420P,
            30
    },
    {
            "/Users/walkerwait/Desktop/工作/Demos/ffmpegDemos/resource/2.yuv",
            960,400,
            AV_PIX_FMT_YUV420P,
            30
    },
    {
            "/Users/walkerwait/Desktop/工作/Demos/ffmpegDemos/resource/3.yuv",
            854,480,
            AV_PIX_FMT_YUV420P,
            30
    }
};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建播放器
    _player = new YuvPlayer(this);

    // 监听播放器
    connect(_player, &YuvPlayer::stateChanged,
            this, &MainWindow::onPlayerStateChanged);

    // 设置frame
    int w = 400;
    int h = 400;
    int x = (width() - w) >> 1;
    int y = (height() - w) >> 1;

    _player->setGeometry(x, y, w, h);

    // 设置YUV
    Yuv yuv = yuvs[0];
    _player->setYuv(yuv);
}

// 监听到播放器状态改变
void MainWindow::onPlayerStateChanged() {
    qDebug()<<"播放器当前状态："<<_player->getState();
    if (_player->getState() == YuvPlayer::Playing) {
        ui->playButton->setText("暂停");
    } else {
        // 不是处于播放状态 按钮置为播放
        ui->playButton->setText("播放");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_playButton_clicked()
{
    if (_player->isPlaying()) {
        _player->pause();
    } else {
        _player->play();
    }
}


void MainWindow::on_stopButton_clicked()
{
    _player->stop();
}


void MainWindow::on_nextButton_clicked()
{
    int yuvCount = sizeof (yuvs) / sizeof(Yuv);
    yuvIndex = ++yuvIndex % yuvCount;
    Yuv yuv = yuvs[yuvIndex];
    _player->stop();
    _player->setYuv(yuv);
    _player->play();
}


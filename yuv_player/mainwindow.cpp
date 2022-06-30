#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建播放器
    _player = new YuvPlayer(this);

    // 设置frame
    int w = 600;
    int h = 600;
    int x = (width() - w) >> 1;
    int y = (height() - w) >> 1;

    _player->setGeometry(x, y, w, h);

    // 设置YUV
    Yuv yuv = {
        "/Users/walkerwait/Desktop/out.yuv",
        1920,1076,
        AV_PIX_FMT_YUV420P,
        30
    };
    _player->setYuv(yuv);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_playButton_clicked()
{
    if (_player->isPlaying()) {
        _player->pause();
        ui->playButton->setText("播放");
    } else {
        _player->play();
        ui->playButton->setText("暂停");
    }
}


void MainWindow::on_stopButton_clicked()
{
    _player->stop();
}


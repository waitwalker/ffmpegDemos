#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "playthread.h"
#include <SDL2/SDL.h>


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

void showVersion() {

}

void MainWindow::on_playButton_clicked()
{
    if (playThread) {
        playThread->requestInterruption();
        playThread = nullptr;
        ui->playButton->setText("开始播放");
    } else {
        PlayThread *playThread = new PlayThread(this);
        playThread->start();
        // 监听线程结束
        connect(playThread, &PlayThread::finished,[this](){
            // 线程已经结束
            this->playThread = nullptr;
            ui->playButton->setText("开始播放");
        });
        ui->playButton->setText("停止播放");
    }
}


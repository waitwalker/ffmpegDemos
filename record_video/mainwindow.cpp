#include "mainwindow.h"
#include "ui_mainwindow.h"

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


void MainWindow::on_recordButton_clicked()
{
    if (!_videoThread) { // 点击了“开始录音”
        // 开启线程
        _videoThread = new VideoThread(this);
        _videoThread->start();

        connect(_videoThread, &VideoThread::finished,
        [this]() { // 线程结束
            _videoThread = nullptr;
            ui->recordButton->setText("开始录视频");
        });

        // 设置按钮文字
        ui->recordButton->setText("结束录视频");
    } else { // 点击了“结束录音”
        // 结束线程
//        _audioThread->setStop(true);
        _videoThread->requestInterruption();
        _videoThread = nullptr;

        // 设置按钮文字
        ui->recordButton->setText("开始录视频");
    }
}


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
    if (!_audioThread) {
        _audioThread = new AudioRecordThread(this);
        _audioThread->start();
        connect(_audioThread, &AudioRecordThread::finished,[this](){
           _audioThread = nullptr;
           ui->recordButton->setText("开始录音");
        });
        ui->recordButton->setText("停止录音");
    } else {
        _audioThread->requestInterruption();
        _audioThread = nullptr;
        ui->recordButton->setText("开始录音");
    }
}


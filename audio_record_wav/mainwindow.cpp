#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(_audioThread, &AudioRecordThread::timeChanged, [this](unsigned long long ms){
        QTime time(0,0,0,0);
        QString text = time.addMSecs(ms).toString("mm:ss.z");
        ui->timeLabel->setText(text);
    });
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


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>
#include <QDebug>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    onTimeChanged(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTimeChanged(unsigned long long ms) {
    QTime time(0,0,0,0);
    QString text = time.addMSecs(ms).toString("mm:ss.z");
    qDebug()<<"时间："<<text;
    ui->timeLabel->setText(text.left(7));
}


void MainWindow::on_recordButton_clicked()
{
    if (!_audioThread) {
        _audioThread = new AudioRecordThread(this);
        _audioThread->start();
        connect(_audioThread, &AudioRecordThread::timeChanged, this, &MainWindow::onTimeChanged);
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


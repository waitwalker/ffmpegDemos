#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

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

// 播放暂停按钮
void MainWindow::on_playBtn_clicked()
{

}

// 停止播放按钮
void MainWindow::on_stopBtn_clicked()
{
    int count = ui->playWidget->count();
    int idx = ui->playWidget->currentIndex();
    ui->playWidget->setCurrentIndex(++idx % count);
}

// 静音按钮
void MainWindow::on_silenceBtn_clicked()
{

}

// 打开文件按钮
void MainWindow::on_openFileBtn_clicked()
{
    QString filename = QFileDialog::getOpenFileName(nullptr,
                                 "/",
                                 "",
                                 "");
    qDebug()<<"文件名称："<<filename;
}


void MainWindow::on_currentSlider_valueChanged(int value)
{
    qDebug()<<"播放进度"<<value;
}


void MainWindow::on_volumeSlider_valueChanged(int value)
{
    qDebug()<<"音量"<<value;
}


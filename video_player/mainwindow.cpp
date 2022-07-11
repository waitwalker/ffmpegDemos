#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->playWidget->setCurrentWidget(ui->openFilePage);
    ui->playBtn->setEnabled(false);
    ui->stopBtn->setEnabled(false);
    ui->currentSlider->setEnabled(false);
    ui->volumeSlider->setEnabled(false);
    ui->durationLabel->setText("00:00:00");
    ui->volumeSlider->setValue(ui->volumeSlider->maximum());
    ui->volumeLabel->setText("100");

    // 创建播放器
    _player = new VideoPlayer();

    // 监听信号
    connect(_player, &VideoPlayer::stateChanged,
            this, &MainWindow::onPlayerStateChanged);
}

void MainWindow::onPlayerStateChanged(VideoPlayer *player) {
    VideoPlayer::State state = player->getState();
    if (state == VideoPlayer::Playing) {
        ui->playBtn->setText("暂停");
    } else {
        ui->playBtn->setText("播放");
    }

    if (state == VideoPlayer::Stopped) {
        ui->playBtn->setEnabled(false);
        ui->stopBtn->setEnabled(false);
        ui->currentSlider->setEnabled(false);
        ui->volumeSlider->setEnabled(false);
        ui->durationLabel->setText("00:00:00");
        ui->volumeSlider->setValue(ui->volumeSlider->maximum());
        ui->volumeLabel->setText("100");

        // 显示打开文件页面
        ui->playWidget->setCurrentWidget(ui->openFilePage);
    } else {
        ui->playBtn->setEnabled(true);
        ui->stopBtn->setEnabled(true);
        ui->currentSlider->setEnabled(true);
        ui->volumeSlider->setEnabled(true);

        // 显示播放视频页面
        ui->playWidget->setCurrentWidget(ui->videoPage);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 播放暂停按钮
void MainWindow::on_playBtn_clicked()
{
    VideoPlayer::State state = _player->getState();
    if (state == VideoPlayer::Playing) {
        _player->pause();
    } else {
        _player->play();
    }
}

// 停止播放按钮
void MainWindow::on_stopBtn_clicked()
{
//    int count = ui->playWidget->count();
//    int idx = ui->playWidget->currentIndex();
//    ui->playWidget->setCurrentIndex(++idx % count);
    _player->stop();
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
    if (filename.isEmpty()) return;
    _player->setFilename(filename.toUtf8().data());
    _player->play();
}


void MainWindow::on_currentSlider_valueChanged(int value)
{
    qDebug()<<"播放进度"<<value;

}


void MainWindow::on_volumeSlider_valueChanged(int value)
{
    qDebug()<<"音量"<<value;
    ui->volumeLabel->setText(QString("%1").arg(value));
}


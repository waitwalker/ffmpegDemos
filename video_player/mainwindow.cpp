#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

// #pragma mark - 构造 析构函数
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 注册信号的类型，保证能发出信号
    qRegisterMetaType<VideoPlayer::VideoSwsSpec>("VideoSwsSpec");

    // 设置音量滑块的范围
    ui->volumeSlider->setRange(VideoPlayer::Volume::Min,
                               VideoPlayer::Volume::Max);
    ui->volumeSlider->setValue(ui->volumeSlider->maximum());

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
    // 播放状态改变
    connect(_player, &VideoPlayer::stateChanged,
            this, &MainWindow::onPlayerStateChanged);
    // 播放状态改变
    connect(_player, &VideoPlayer::stateChanged,
            ui->videoWidget, &VideoWidget::onPlayerStateChanged);

    // 初始化完成
    connect(_player, &VideoPlayer::initFinished,
            this, &MainWindow::onPlayerInitFinished);

    // 播放器失败
    connect(_player, &VideoPlayer::playFailed,
            this, &MainWindow::onPlayerPlayFailed);
    // 播放器失败
    connect(_player, &VideoPlayer::timeChange,
            this, &MainWindow::onPlayerTimeChanged);

    // 视频像素格式转换成功
    connect(_player, &VideoPlayer::frameDecoded,
            ui->videoWidget, &VideoWidget::onFrameDecoded);
}


MainWindow::~MainWindow()
{
    delete ui;
    delete _player;
}

#pragma mark - 槽函数
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
        ui->currentSlider->setValue(0);
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

void MainWindow::onPlayerInitFinished(VideoPlayer *player) {
    qDebug()<<"总时长： "<<player->getDuration();

    // 设置slider范围
    int64_t microsSeconds= player->getDuration();
    ui->currentSlider->setRange(0, microsSeconds);
    // 设置时长
    ui->currentLabel->setText(getDurationText(microsSeconds));
}

void MainWindow::onPlayerTimeChanged(VideoPlayer *player) {
    ui->currentSlider->setValue(player->getCurrent());
}

void MainWindow::onPlayerPlayFailed(VideoPlayer *player) {
    QMessageBox::critical(nullptr, "提示","错误了");
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
    if (_player->isMute()) {
        _player->setMute(false);
        ui->silenceBtn->setText("静音");
    } else {
        _player->setMute(true);
        ui->silenceBtn->setText("开音");
    }

}

// 打开文件按钮
void MainWindow::on_openFileBtn_clicked()
{
//    QString file_full, file_name,filePath,file_path,file_suffix;
//        QFileInfo fileinfo;
//        filePath = QCoreApplication::applicationDirPath();
//        file_full = QFileDialog::getOpenFileName(nullptr, tr("打开视频文件"),
//                                                 QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first(),
//                                                 "");
    QString filename = QFileDialog::getOpenFileName(nullptr,
                                 "/",
                                 "",
                                 "");
    qDebug()<<"文件名称："<<filename;
    if (filename.isEmpty()) return;
    qDebug()<<"C语言文件名："<<filename.toUtf8().data();
    _player->setFilename(filename);
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
    _player->setVolume(value);
}

#pragma mark - 私有方法
QString MainWindow::getDurationText(int value) {
    int64_t seconds = value;
    int h = seconds / 3600;
    int m = (seconds % 3600) / 60;
    int s = seconds % 60;

    qDebug()<<h<<m<<s;

    QString hStr = QString("0%1").arg(h).right(2);
    QString mStr = QString("0%1").arg(m).right(2);
    QString sStr = QString("0%1").arg(s).right(2);
    QString str = QString("%1:%2:%3").arg(hStr).arg(mStr).arg(sStr);
    return str;
}


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "videoplayer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_playBtn_clicked();

    void on_stopBtn_clicked();

    void on_silenceBtn_clicked();

    void on_openFileBtn_clicked();

    void on_currentSlider_valueChanged(int value);

    void on_volumeSlider_valueChanged(int value);

    void onPlayerStateChanged(VideoPlayer *player);
    void onPlayerInitFinished(VideoPlayer *player);
    void onPlayerPlayFailed(VideoPlayer *player);
    void onPlayerTimeChanged(VideoPlayer *player);












private:
    Ui::MainWindow *ui;
    VideoPlayer *_player;
    QString getDurationText(int value);
};
#endif // MAINWINDOW_H

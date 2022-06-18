#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "audiorecordthread.h"

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
    void on_recordButton_clicked();
    void onTimeChanged(unsigned long long ms);

private:
    Ui::MainWindow *ui;
    AudioRecordThread *_audioThread = nullptr;
};
#endif // MAINWINDOW_H

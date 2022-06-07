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
    PlayThread *playThread = new PlayThread(this);
    playThread->start();
}


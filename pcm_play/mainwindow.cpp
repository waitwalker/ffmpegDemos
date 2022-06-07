#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <SDL2/SDL.h>
#include <QDebug>
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


void MainWindow::on_playButton_clicked()
{
    SDL_version version;
    SDL_VERSION(&version);
    qDebug()<<"当前版本："<<version.major<<version.minor<<version.patch;
}


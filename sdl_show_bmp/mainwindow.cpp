#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "showthread.h"

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


void MainWindow::on_showButton_clicked()
{
    qDebug()<<"走到这里 线程start";
    ShowThread *show = new ShowThread(this);
    show->start();
}


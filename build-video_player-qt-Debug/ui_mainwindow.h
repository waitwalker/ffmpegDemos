/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.2.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "videoslider.h"
#include "videowidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QStackedWidget *playWidget;
    QWidget *openFilePage;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *openFileBtn;
    QWidget *videoPage;
    QGridLayout *gridLayout_2;
    VideoWidget *videoWidget;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout;
    QPushButton *playBtn;
    QPushButton *stopBtn;
    QHBoxLayout *horizontalLayout_2;
    VideoSlider *currentSlider;
    QLabel *currentLabel;
    QLabel *g;
    QLabel *durationLabel;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *silenceBtn;
    VideoSlider *volumeSlider;
    QLabel *volumeLabel;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        playWidget = new QStackedWidget(centralwidget);
        playWidget->setObjectName(QString::fromUtf8("playWidget"));
        openFilePage = new QWidget();
        openFilePage->setObjectName(QString::fromUtf8("openFilePage"));
        horizontalLayout_5 = new QHBoxLayout(openFilePage);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        openFileBtn = new QPushButton(openFilePage);
        openFileBtn->setObjectName(QString::fromUtf8("openFileBtn"));
        openFileBtn->setMaximumSize(QSize(80, 44));

        horizontalLayout_5->addWidget(openFileBtn);

        playWidget->addWidget(openFilePage);
        videoPage = new QWidget();
        videoPage->setObjectName(QString::fromUtf8("videoPage"));
        gridLayout_2 = new QGridLayout(videoPage);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        videoWidget = new VideoWidget(videoPage);
        videoWidget->setObjectName(QString::fromUtf8("videoWidget"));

        gridLayout_2->addWidget(videoWidget, 0, 0, 1, 1);

        playWidget->addWidget(videoPage);

        gridLayout->addWidget(playWidget, 0, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        playBtn = new QPushButton(centralwidget);
        playBtn->setObjectName(QString::fromUtf8("playBtn"));
        playBtn->setMaximumSize(QSize(60, 16777215));

        horizontalLayout->addWidget(playBtn);

        stopBtn = new QPushButton(centralwidget);
        stopBtn->setObjectName(QString::fromUtf8("stopBtn"));
        stopBtn->setMaximumSize(QSize(60, 16777215));

        horizontalLayout->addWidget(stopBtn);


        horizontalLayout_4->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        currentSlider = new VideoSlider(centralwidget);
        currentSlider->setObjectName(QString::fromUtf8("currentSlider"));
        currentSlider->setMinimumSize(QSize(240, 0));
        currentSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(currentSlider);

        currentLabel = new QLabel(centralwidget);
        currentLabel->setObjectName(QString::fromUtf8("currentLabel"));
        currentLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(currentLabel);

        g = new QLabel(centralwidget);
        g->setObjectName(QString::fromUtf8("g"));
        g->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(g);

        durationLabel = new QLabel(centralwidget);
        durationLabel->setObjectName(QString::fromUtf8("durationLabel"));
        durationLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(durationLabel);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        silenceBtn = new QPushButton(centralwidget);
        silenceBtn->setObjectName(QString::fromUtf8("silenceBtn"));
        silenceBtn->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_3->addWidget(silenceBtn);

        volumeSlider = new VideoSlider(centralwidget);
        volumeSlider->setObjectName(QString::fromUtf8("volumeSlider"));
        volumeSlider->setMinimumSize(QSize(100, 0));
        volumeSlider->setMaximumSize(QSize(100, 16777215));
        volumeSlider->setMaximum(100);
        volumeSlider->setValue(100);
        volumeSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_3->addWidget(volumeSlider);

        volumeLabel = new QLabel(centralwidget);
        volumeLabel->setObjectName(QString::fromUtf8("volumeLabel"));
        volumeLabel->setMaximumSize(QSize(30, 16777215));
        volumeLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(volumeLabel);


        horizontalLayout_2->addLayout(horizontalLayout_3);


        horizontalLayout_4->addLayout(horizontalLayout_2);


        gridLayout->addLayout(horizontalLayout_4, 1, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 24));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        playWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        openFileBtn->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\346\226\207\344\273\266", nullptr));
        playBtn->setText(QCoreApplication::translate("MainWindow", "\346\222\255\346\224\276", nullptr));
        stopBtn->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242", nullptr));
        currentLabel->setText(QCoreApplication::translate("MainWindow", "00:00:00", nullptr));
        g->setText(QCoreApplication::translate("MainWindow", "/", nullptr));
        durationLabel->setText(QCoreApplication::translate("MainWindow", "00:00:00", nullptr));
        silenceBtn->setText(QCoreApplication::translate("MainWindow", "\351\235\231\351\237\263", nullptr));
        volumeLabel->setText(QCoreApplication::translate("MainWindow", "100", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

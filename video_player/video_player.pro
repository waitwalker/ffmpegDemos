QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    condmutex.cpp \
    main.cpp \
    mainwindow.cpp \
    videoplayer.cpp \
    videoplayer_audio.cpp \
    videoplayer_video.cpp \
    videoslider.cpp \
    videowidget.cpp

HEADERS += \
    condmutex.h \
    mainwindow.h \
    videoplayer.h \
    videoslider.h \
    videowidget.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
    FFMPEG_HOME = F:/Dev/msys64/usr/local/ffmpeg
    SDL_HOME = F:/Dev/msys64/usr/local/sdl
}

macx {
    FFMPEG_HOME = /usr/local/ffmpeg
    SDL_HOME = /opt/homebrew/Cellar/sdl2/2.0.20
    QMAKE_INFO_PLIST = mac/Info.plist
}

INCLUDEPATH += $${SDL_HOME}/include
LIBS += -L$${SDL_HOME}/lib \
        -lSDL2

INCLUDEPATH += $${FFMPEG_HOME}/include

LIBS += -L$${FFMPEG_HOME}/lib \
        -lavdevice \
        -lswresample \
        -lavformat \
        -lavcodec \
        -lavutil \
        -lswscale

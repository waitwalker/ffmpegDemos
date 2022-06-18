#ifndef AUDIORECORDTHREAD_H
#define AUDIORECORDTHREAD_H

#include <QObject>
#include <QThread>
extern "C" {
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#ifdef Q_OS_WIN
#define FMT_NAME "dshow"
#define DEVICE_NAME "audio=麦克风阵列 (Realtek(R) Audio)"
#define FILE_NAME "F:/out.pcm"
#else
#define FMT_NAME "avfoundation"
#define DEVICE_NAME ":0"
#define FILE_PATH "/Users/walkerwait/Desktop/"
#endif
class AudioRecordThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioRecordThread(QObject *parent = nullptr);
    ~AudioRecordThread();
protected:
    void run() override;

signals:

};

#endif // AUDIORECORDTHREAD_H

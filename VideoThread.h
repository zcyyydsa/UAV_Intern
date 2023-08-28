#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include "include.h"

extern QImage img;

extern "C"
{
    #include <ffmpegsdk/include/libavcodec/avcodec.h>
    #include <ffmpegsdk/include/libavformat/avformat.h>
    #include <ffmpegsdk/include/libswscale/swscale.h>
    #include <ffmpegsdk/include/libavdevice/avdevice.h>
    #include <ffmpegsdk/include/libavformat/version.h>
    #include <ffmpegsdk/include/libavutil/time.h>
    #include <ffmpegsdk/include/libavutil/mathematics.h>
}

class videothread : public QThread
{
    Q_OBJECT
public:
    void startPlay(QString url);
private:
    void run();
signals:
    void VideoData(); //每获取到一帧图像 就发送此信号
private:
    QString m_strFileName;
    AVFrame *pFrame; //存储一帧解码后像素数据
    AVFrame *pFrameRGB;
};


#endif // VIDEOTHREAD_H

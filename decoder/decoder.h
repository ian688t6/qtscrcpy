#ifndef DECODER_H
#define DECODER_H
#include <QThread>
#include <QPointer>
#include <QMutex>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}
class Frames;
class DeviceSocket;
class Decoder : public QThread
{
    Q_OBJECT
public:
    Decoder();
    virtual ~Decoder();
public:
    static bool init();
    static bool deinit();

    void setFrame(Frames *frames);
    void setDeviceSocket(DeviceSocket *socket);
    qint32 recvData(quint8 *buf, qint32 bufSize);
    bool startDecoder();
    void stopDecoder();

signals:
    void onNewFrame();
    void onDecodeStop();

protected:
    void run();
    void pushFrame();

private:
    QPointer<DeviceSocket> m_deviceSocket;
    bool m_quit = false;
    Frames *m_frames;
};

#endif // DECODER_H

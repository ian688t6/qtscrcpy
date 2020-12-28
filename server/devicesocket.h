#ifndef DEVICESOCKET_H
#define DEVICESOCKET_H

#include <QTcpSocket>
#include <QWaitCondition>
#include <QMutex>

class DeviceSocket : public QTcpSocket
{
    Q_OBJECT
public:
    DeviceSocket(QObject *parent = nullptr);
    ~DeviceSocket();

    quint32 subThreadRecvData(quint8* buf, quint32 bufSize);

protected:
    bool event(QEvent *event);

protected:
    void onReadyRead();
    void quitNotify();
private:
    QMutex m_mutex;
    QWaitCondition m_recvDataCond;

    bool m_recvData = false;
    bool m_quit = false;

    quint8 *m_buffer = Q_NULLPTR;
    quint32 m_bufferSize = 0;
    quint32 m_dataSize = 0;
};

#endif // DEVICESOCKET_H

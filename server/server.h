#ifndef SERVER_H
#define SERVER_H
#include <QObject>
#include <QSize>
#include "tcpserver.h"
#include "devicesocket.h"
#include "adbprocess.h"

class Server : public QObject
{
    Q_OBJECT
public:
    enum ServerStatus{
        ServerStatusNone,
        ServerStatusPush,
        ServerStatusEnableReverse,
        ServerStatusExec,
        ServerStatusRunning
    };
    Server(QObject *parent=nullptr);
    bool start(const QString& serial, const quint16 localPort, const quint16 maxSize, const quint32 bitRate);
    void stop();
    bool installServer();
    bool removeServer();
    bool enableTunnelReverse();
    bool disableTunnelReverse();
    bool execute();
    QString getServerPath();
    bool readInfo(QString& deviceName, QSize& size);
    DeviceSocket *getDeviceSocket();

signals:
    void serverStartResult(bool success);
    void serverConnected(bool success, const QString& deviceName, const QSize& size);

private slots:
    void onServerProcessResult(AdbProcess::AdbRetCode code);
    void onServerStatusResult(AdbProcess::AdbRetCode code);
private:
    QString m_serial = "";
    quint16 m_maxSize;
    quint32 m_bitRate;
    quint16 m_localPort;
    QString m_crop = "";
    ServerStatus m_serverState;
    QString m_serverPath = "";
    AdbProcess m_process;
    AdbProcess m_serverProcess;
    bool m_serverInstalled = false;
    bool m_reverseCreated = false;
    bool m_tunnelForward = false;
    bool m_sendFrameMeta = false;
    TcpServer m_serverSocket;
    DeviceSocket *m_deviceSocket = Q_NULLPTR;

private:
    bool serverStartByStep(void);
};

#endif // SERVER_H

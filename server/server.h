#ifndef SERVER_H
#define SERVER_H
#include <QObject>
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
    bool start(const QString& serial, const quint16 localPort, const quint16 maxSize, const quint16 bitRate);
    bool installServer();
    bool removeServer();
    bool enableTunnelReverse();
    bool disableTunnelReverse();
    bool execute();
    QString getServerPath();

signals:
    void serverStartResult(bool success);

private slots:
    void onServerProcessResult(AdbProcess::AdbRetCode code);

private:
    QString m_serial = "";
    quint16 m_maxSize;
    quint16 m_bitRate;
    quint16 m_localPort;
    ServerStatus m_serverState;
    QString m_serverPath = "";
    AdbProcess m_process;
    AdbProcess m_serverProcess;
    bool m_serverInstalled = false;
    bool m_reverseCreated = false;
private:
    bool serverStartByStep(void);
};

#endif // SERVER_H

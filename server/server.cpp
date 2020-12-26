#include <QFileInfo>
#include <QCoreApplication>

#include "server.h"

#define DEVICE_SERVER_PATH "/data/local/tmp/scrcpy-server.jar"
#define SOCKET_NAME "scrcpy"

Server::Server(QObject *parent)
    :QObject(parent)
{
    connect(&m_process, &AdbProcess::adbProcessResult, this, &Server::onServerProcessResult);
    connect(&m_serverProcess, &AdbProcess::adbProcessResult, this, &Server::onServerProcessResult);
}

bool Server::start(const QString &serial, const quint16 localPort, const quint16 maxSize, const quint16 bitRate)
{
    m_serial = serial;
    m_localPort = localPort;
    m_maxSize = maxSize;
    m_bitRate = bitRate;
    m_serverState = ServerStatusNone;
    return serverStartByStep();
}

bool Server::installServer()
{
    m_process.push(m_serial, getServerPath(), DEVICE_SERVER_PATH);
    return true;
}

bool Server::removeServer()
{
    AdbProcess *adb = new AdbProcess();
    if (!adb) {
        return false;
    }
    connect(adb, &AdbProcess::adbProcessResult, this, [this](AdbProcess::AdbRetCode retcode){
        if (AdbProcess::AdbRetStartSucc != retcode) {
            sender()->deleteLater();
        }
    });
    adb->remove(m_serial, DEVICE_SERVER_PATH);
    return true;
}

bool Server::enableTunnelReverse()
{
    m_process.reverse(m_serial, SOCKET_NAME, m_localPort);
    return true;
}

bool Server::disableTunnelReverse()
{
    AdbProcess *adb = new AdbProcess();
    if (!adb) {
        return false;
    }
    connect(adb, &AdbProcess::adbProcessResult, this, [this](AdbProcess::AdbRetCode retcode){
        if (AdbProcess::AdbRetStartSucc != retcode) {
            sender()->deleteLater();
        }
    });
    adb->removeReverse(m_serial, SOCKET_NAME);
    return true;
}

bool Server::execute()
{
    QStringList args;
    args << "shell";
    args << QString("CLASSPATH=%1").arg(DEVICE_SERVER_PATH);
    args << "/";
    args << "com.genymobile.scrcpy.Server";
    args << QString::number(m_maxSize);
    args << QString::number(m_bitRate);
    args << "false";
    args << "";
    m_serverProcess.execute(m_serial, args);
    return true;
}

QString Server::getServerPath()
{
    if (m_serverPath.isEmpty()) {
        m_serverPath = QString::fromLocal8Bit(qgetenv("QTSCRCPY_SERVER_PATH"));
        QFileInfo fileInfo(m_serverPath);
        if ((m_serverPath.isEmpty()) || (!fileInfo.isFile())) {
            m_serverPath = QCoreApplication::applicationDirPath() + "/scrcpy-server";
        }
    }
    return m_serverPath;
}

void Server::onServerProcessResult(AdbProcess::AdbRetCode code)
{
    if (sender() == &m_serverProcess) {
        if (ServerStatusExec == m_serverState) {
            if (AdbProcess::AdbRetStartSucc == code) {
                m_serverState = ServerStatusRunning;
                emit serverStartResult(true);
            } else if (AdbProcess::AdbRetStartFail == code) {
                qCritical("adb shell start server failed");
                m_serverState = ServerStatusNone;
                /* Todo: disable reverse */
                if (m_reverseCreated) {
                    disableTunnelReverse();
                }
                /* Todo: remove server */
                removeServer();
                emit serverStartResult(false);
            }
        }
        return;
    }
    if (ServerStatusNone == m_serverState) {
        return;
    }

    switch (m_serverState) {
    case ServerStatusPush:
        if (AdbProcess::AdbRetExecSucc == code) {
            m_serverState = ServerStatusEnableReverse;
            m_serverInstalled = true;
            serverStartByStep();
        } else if (AdbProcess::AdbRetStartSucc != code) {
            qCritical("adb push failed");
            m_serverState = ServerStatusNone;
            emit serverStartResult(false);
        }
        break;

    case ServerStatusEnableReverse:
        if (AdbProcess::AdbRetExecSucc == code) {
            m_reverseCreated = true;
            m_serverState = ServerStatusExec;
            serverStartByStep();
        } else if (AdbProcess::AdbRetStartSucc != code) {
            qCritical("adb enable reverse failed");
            m_serverState = ServerStatusNone;
            /* Todo: remove server */
            if (m_serverInstalled) {
                removeServer();
                m_serverInstalled = false;
            }
            emit serverStartResult(false);
        }
        break;
    default:
        break;
    }
}

bool Server::serverStartByStep()
{
    bool ret = false;

    if (ServerStatusNone == m_serverState) {
        return ret;
    }

    switch (m_serverState) {
    case ServerStatusPush:
        ret = installServer();
        break;
    case ServerStatusEnableReverse:
        ret = enableTunnelReverse();
        break;

    case ServerStatusExec:
        ret = execute();
        break;
    default:
        break;
    }

    return ret;
}

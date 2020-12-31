#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QSize>
#include <QTimerEvent>
#include <QCoreApplication>
#include <QFileInfo>
#include "server.h"

#define DEVICE_SERVER_PATH "/data/local/tmp/scrcpy-server.jar"
#define SOCKET_NAME "qtscrcpy"
#define DEVICE_INFO_FIELD_LENGTH    (64)

Server::Server(QObject *parent)
    :QObject(parent)
{
    connect(&m_process, &AdbProcess::adbProcessResult, this, &Server::onServerStatusResult);
    connect(&m_serverProcess, &AdbProcess::adbProcessResult, this, &Server::onServerProcessResult);
    connect(&m_serverSocket, &QTcpServer::newConnection, this, [this](){
        QString deviceName;
        QSize size;
        m_deviceSocket = dynamic_cast<DeviceSocket *>(m_serverSocket.nextPendingConnection());
        if (m_deviceSocket && m_deviceSocket->isValid() && readInfo(deviceName, size)) {
            m_serverSocket.close();
            m_tunnelEnabled = false;
            disableTunnelReverse();
            removeServer();
            emit serverConnected(true, deviceName, size);
        } else {
            stop();
            emit serverConnected(false, deviceName, size);
        }
    });
}

bool Server::start(const QString &serial, const quint16 localPort, const quint16 maxSize, const quint32 bitRate)
{
    m_serial = serial;
    m_localPort = localPort;
    m_maxSize = maxSize;
    m_bitRate = bitRate;
    m_serverState = ServerStatusPush;
    return serverStartByStep();
}

bool Server::connectTo()
{
    if (ServerStatusRunning != m_serverState) {
        qWarning("server not run");
        return false;
    }

    if (!m_tunnelForward && !m_deviceSocket) {
        startAcceptTimeoutTimer();
        return true;
    }

    // device server need time to start
    QTimer::singleShot(600, this, [this](){
        QString deviceName;
        QSize deviceSize;
        bool success = false;

        m_deviceSocket = new DeviceSocket();

        // wait for devices server start
        m_deviceSocket->connectToHost(QHostAddress::LocalHost, m_localPort);
        if (!m_deviceSocket->waitForConnected(1000)) {
            stop();
            qWarning("connect to server failed");
            emit serverConnected(false, "", QSize());
            return false;
        }
        if (QTcpSocket::ConnectedState == m_deviceSocket->state()) {
            // connect will success even if devices offline, recv data is real connect success
            // because connect is to pc adb server
            m_deviceSocket->waitForReadyRead(1000);
            // devices will send 1 byte first on tunnel forward mode
            QByteArray data = m_deviceSocket->read(1);
            if (!data.isEmpty() && readInfo(deviceName, deviceSize)) {
                success = true;
            } else {
                qWarning("connect to server read device info failed");
                success = false;
            }
        } else {
            qWarning("connect to server failed");
            m_deviceSocket->deleteLater();
            success = false;
        }

        if (success) {
            // we don't need the adb tunnel anymore
            disableTunnelForward();
            m_tunnelEnabled = false;
        } else {
            stop();
        }
        emit serverConnected(success, deviceName, deviceSize);
    });

    return true;
}

void Server::stop()
{
    if (m_deviceSocket) {
        qDebug() << "device socket close ----";
        m_deviceSocket->close();
        m_deviceSocket->deleteLater();
    }

    m_serverProcess.kill();
    if (m_tunnelEnabled) {
        if (m_tunnelForward) {
            disableTunnelForward();
        } else {
            disableTunnelReverse();
        }
        m_tunnelForward = false;
        m_tunnelEnabled = false;
    }
}

bool Server::installServer()
{
    m_process.push(m_serial, getServerPath(), DEVICE_SERVER_PATH);
    return true;
}

bool Server::removeServer()
{
//    AdbProcess *adb = new AdbProcess();
//    if (!adb) {
//        return false;
//    }
//    connect(adb, &AdbProcess::adbProcessResult, this, [this](AdbProcess::AdbRetCode retcode){
//        if (AdbProcess::AdbRetStartSucc != retcode) {
//            sender()->deleteLater();
//        }
//    });
//    adb->remove(m_serial, DEVICE_SERVER_PATH);
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

bool Server::enableTunnelForward()
{
    if (m_process.isRunning()) {
        m_process.kill();
    }
    m_process.forward(m_serial, m_localPort, SOCKET_NAME);
    return true;
}

bool Server::disableTunnelForward()
{
    AdbProcess* adb = new AdbProcess();
    if (!adb) {
        return false;
    }
    connect(adb, &AdbProcess::adbProcessResult, this, [this](AdbProcess::AdbRetCode processResult){
        if (AdbProcess::AdbRetStartSucc != processResult) {
            sender()->deleteLater();
        }
    });
    adb->forwardRemove(m_serial, m_localPort);
    return true;
}

bool Server::execute()
{
    if (m_serverProcess.isRunning()) {
        m_serverProcess.kill();
    }
    QStringList args;
    args << "shell";
    args << QString("CLASSPATH=%1").arg(DEVICE_SERVER_PATH);
    args << "app_process";
    args << "/"; // unused;
    args << "com.genymobile.scrcpy.Server";
    args << QString::number(m_maxSize);
    args << QString::number(m_bitRate);
    args << (m_tunnelForward ? "true" : "false");
    if (m_crop.isEmpty()) {
        args << "-";
    } else {
        args << m_crop;
    }
    args << (m_sendFrameMeta ? "true" : "false");
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

bool Server::readInfo(QString &deviceName, QSize &size)
{
    unsigned char buf[DEVICE_INFO_FIELD_LENGTH + 4];
    if (m_deviceSocket->bytesAvailable() <= (DEVICE_INFO_FIELD_LENGTH + 4)) {
        m_deviceSocket->waitForReadyRead(300);
    }
    quint64 len = m_deviceSocket->read((char *)buf, sizeof(buf));
    if (len < (DEVICE_INFO_FIELD_LENGTH + 4)) {
        qInfo("Could not retrieve device infomation");
        return false;
    }
    buf[DEVICE_INFO_FIELD_LENGTH - 1] = '\0';
    deviceName = (char *)buf;
    size.setWidth((buf[DEVICE_INFO_FIELD_LENGTH] << 8) | (buf[DEVICE_INFO_FIELD_LENGTH + 1]));
    size.setHeight((buf[DEVICE_INFO_FIELD_LENGTH + 2] << 8) | (buf[DEVICE_INFO_FIELD_LENGTH + 3]));
    return true;
}

DeviceSocket *Server::getDeviceSocket()
{
    return m_deviceSocket;
}

void Server::onServerProcessResult(AdbProcess::AdbRetCode code)
{
    if (ServerStatusExec == m_serverState) {
        if (AdbProcess::AdbRetStartSucc == code) {
            m_serverState = ServerStatusRunning;
            emit serverStartResult(true);
        } else if (AdbProcess::AdbRetStartFail == code) {
            qCritical("adb shell start server failed");
            m_serverState = ServerStatusNone;
            /* Todo: disable reverse */
            if (m_tunnelEnabled) {
                disableTunnelReverse();
                m_tunnelEnabled = false;
            }
            /* Todo: remove server */
            removeServer();
            emit serverStartResult(false);
        }
    }
}

void Server::onServerStatusResult(AdbProcess::AdbRetCode code)
{
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
            m_tunnelEnabled = true;
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
        m_serverSocket.setMaxPendingConnections(1);
        if (!m_serverSocket.listen(QHostAddress::LocalHost, m_localPort)) {
            qCritical(QString("Could not listen on port %1").arg(m_localPort).toStdString().c_str());
            m_serverState = ServerStatusNone;
            disableTunnelReverse();
            removeServer();
            emit serverStartResult(false);
            return false;
        }
        ret = execute();
        break;
    default:
        break;
    }
    if (!ret) {
        emit serverStartResult(false);
    }
    return ret;
}

void Server::startAcceptTimeoutTimer()
{
    stopAcceptTimeoutTimer();
    m_acceptTimeoutTimer = startTimer(1000);
}

void Server::stopAcceptTimeoutTimer()
{
    if (m_acceptTimeoutTimer) {
        killTimer(m_acceptTimeoutTimer);
        m_acceptTimeoutTimer = 0;
    }
}

void Server::timerEvent(QTimerEvent *event)
{
    if (event && m_acceptTimeoutTimer == event->timerId()) {
        stopAcceptTimeoutTimer();
        emit serverConnected(false, "", QSize());
    }
}

#include <QDebug>
#include "adbprocess.h"

AdbProcess::AdbProcess(QObject *parent)
    : QProcess(parent)
{
    initSignal();
}

void AdbProcess::initSignal()
{
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
       this, [this](int exitCode, QProcess::ExitStatus exitStatus){
       qDebug() << exitCode << exitStatus;
       if ((QProcess::NormalExit == exitStatus) && (0 == exitCode)) {
           emit adbProcessResult(AdbRetExecSucc);
       } else {
           emit adbProcessResult(AdbRetExecFail);
       }
    });

    connect(this, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        qDebug() << error;
        if (QProcess::FailedToStart == error) {
            emit adbProcessResult(AdbRetStartSucc);
        } else {
            emit adbProcessResult(AdbRetStartFail);
        }
    });

    connect(this, &QProcess::readyReadStandardError, this, [this]() {
        m_errorOutput = QString::fromLocal8Bit(readAllStandardError()).trimmed();
        qDebug() << m_errorOutput;
    });

    connect(this, &QProcess::readyReadStandardOutput, this, [this]() {
        m_standardOutput = QString::fromLocal8Bit(readAllStandardOutput()).trimmed();
        qDebug() << m_standardOutput;
    });

    connect(this, &QProcess::started, this, [this]() {
        qDebug() << "started";
    });
}

void AdbProcess::execute(const QString &serial, const QStringList &args)
{
    QStringList adbArgs;
    if (!serial.isEmpty()) {
        adbArgs << "-s" << serial;
    }
    adbArgs << args;
    qDebug() << "adb" << adbArgs.join(" ");
    start("adb", adbArgs);
}

void AdbProcess::push(const QString &serial, const QString &local, const QString &remote)
{
    QStringList adbArgs;
    adbArgs << "push";
    adbArgs << local;
    adbArgs << remote;
    execute(serial, adbArgs);
}

void AdbProcess::remove(const QString &serial, const QString &remote)
{
    QStringList adbArgs;
    adbArgs << "shell";
    adbArgs << "rm";
    adbArgs << remote;
    execute(serial, adbArgs);
}

void AdbProcess::reverse(const QString &serial, const QString &deviceSocketName, const quint16 localPort)
{
    QStringList adbArgs;
    adbArgs << "reverse";
    adbArgs << QString("localabstract:%1").arg(deviceSocketName);
    adbArgs << QString("tcp:%1").arg(localPort);
    execute(serial, adbArgs);
}

void AdbProcess::removeReverse(const QString &serial, const QString &deviceSocketName)
{
    QStringList adbArgs;
    adbArgs << "reverse";
    adbArgs << "--remove";
    adbArgs << QString("localabstract:%1").arg(deviceSocketName);
    execute(serial, adbArgs);
}

QStringList AdbProcess::getDevicesSerialFromStdout()
{
    QStringList serials;
    // "List of devices attached\nATOMCN0000001350\tdevice"
    QStringList devicesInfoList = m_standardOutput.split(QRegExp("\r\n|\n"), Qt::SkipEmptyParts);
    for (QString deviceInfo : devicesInfoList) {
        QStringList infos = deviceInfo.split(QRegExp("\t"), Qt::SkipEmptyParts);
        if ((2 == infos.count()) && (0 == infos[1].compare("device"))) {
            serials << infos[0];
        }
    }

    return serials;
}

QString AdbProcess::getDeviceIpFromStdout()
{
    QString ipaddr = "";
    QString strIpExp = "inet [\\d.]*";
    QRegExp ipRegExp(strIpExp, Qt::CaseInsensitive);
    if (ipRegExp.indexIn(m_standardOutput) != -1) {
        ipaddr = ipRegExp.cap(0);
        ipaddr = ipaddr.right(ipaddr.size() - 5);
    }
    return ipaddr;
}

QString AdbProcess::getStdOut()
{
    return m_standardOutput;
}

QString AdbProcess::getErrOut()
{
    return m_errorOutput;
}

bool AdbProcess::isRunning()
{
    if (QProcess::NotRunning == state()) {
        return false;
    } else {
        return true;
    }
}

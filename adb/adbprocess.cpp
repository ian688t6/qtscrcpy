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
        qDebug() << readAllStandardError();
    });

    connect(this, &QProcess::readyReadStandardOutput, this, [this]() {
        qDebug() << readAllStandardOutput();
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

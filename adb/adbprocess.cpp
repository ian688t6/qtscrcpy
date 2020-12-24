#include <QDebug>
#include "adbprocess.h"

AdbProcess::AdbProcess(QObject *parent)
    : QProcess(parent)
{
    InitSignal();
}

void AdbProcess::InitSignal()
{
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
       this, [this](int exitCode, QProcess::ExitStatus exitStatus){
       qDebug() << exitCode << exitStatus;
    });

    connect(this, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        qDebug() << error;
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

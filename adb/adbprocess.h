#ifndef ADBPROCESS_H
#define ADBPROCESS_H

#include <QProcess>

class AdbProcess : public QProcess
{
public:
    AdbProcess(QObject *parent = nullptr);

private:
    void InitSignal();
};

#endif // ADBPROCESS_H

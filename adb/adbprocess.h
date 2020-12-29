#ifndef ADBPROCESS_H
#define ADBPROCESS_H

#include <QProcess>

class AdbProcess : public QProcess
{
    Q_OBJECT
public:
    enum AdbRetCode {
        AdbRetStartSucc,  // adb command start success
        AdbRetStartFail,    // adb command start fail
        AdbRetExecSucc,    // adb command execute success
        AdbRetExecFail,     // adb command execute fail
        AdbRetNotFound,      // adb command not found
    };

    AdbProcess(QObject *parent = nullptr);
    void execute(const QString& serial, const QStringList& args);
    void push(const QString& serial, const QString& local, const QString& remote);
    void remove(const QString& serial, const QString& remote);
    void reverse(const QString& serial, const QString& deviceSocketName, const quint16 localPort);
    void removeReverse(const QString& serial, const QString& deviceSocketName);
    QStringList getDevicesSerialFromStdout();
    QString getDeviceIpFromStdout();
    QString getStdOut();
    QString getErrOut();
    bool isRunning();
signals:
    void adbProcessResult(AdbRetCode result);
private:
    void initSignal();
    QString m_standardOutput;
    QString m_errorOutput;

};

#endif // ADBPROCESS_H

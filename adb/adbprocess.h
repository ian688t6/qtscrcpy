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

signals:
    void adbProcessResult(AdbRetCode result);
private:
    void initSignal();

};

#endif // ADBPROCESS_H

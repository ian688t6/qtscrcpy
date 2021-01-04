#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QPointer>

class DeviceSocket;
class ControlEvent;
class Controller : public QObject
{
    Q_OBJECT
public:
    Controller(QObject *parent=nullptr);
    void setDeviceSocket(DeviceSocket *socket);
    void postControlEvent(ControlEvent *controlEvent);
protected:
    bool event(QEvent *event);

private:
    bool sendControl(const QByteArray& buffer);

private:
    QPointer<DeviceSocket> m_deviceSocket;
};

#endif // CONTROLLER_H

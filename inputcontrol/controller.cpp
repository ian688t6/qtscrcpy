#include <QCoreApplication>
#include "controller.h"
#include "controlevent.h"
#include "devicesocket.h"

Controller::Controller(QObject *parent)
    : QObject(parent)
{

}

void Controller::setDeviceSocket(DeviceSocket *socket)
{
    m_deviceSocket = socket;
}

void Controller::postControlEvent(ControlEvent *controlEvent)
{
    if (controlEvent) {
        QCoreApplication::postEvent(this, controlEvent);
    }
}

bool Controller::event(QEvent *event)
{
    if (event && (event->type() == ControlEvent::Control)) {
        ControlEvent *controlEvent = dynamic_cast<ControlEvent *>(event);

        if (controlEvent) {
            sendControl(controlEvent->serializeData());
        }
        return true;
    }
    return QObject::event(event);
}

bool Controller::sendControl(const QByteArray &buffer)
{
    if (buffer.isEmpty()) {
        return false;
    }
    qint32 len = 0;
    if (m_deviceSocket) {
        len = m_deviceSocket->write(buffer.data(), buffer.length());
    }
    return (len == buffer.length()) ? true : false;
}

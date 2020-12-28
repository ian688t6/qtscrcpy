#include "tcpserver.h"
#include "devicesocket.h"

TcpServer::TcpServer(QObject *parent)
    : QTcpServer(parent)
{

}

void TcpServer::incomingConnection(qintptr handle)
{
    DeviceSocket *deviceSocket = new DeviceSocket();
    deviceSocket->setSocketDescriptor(handle);
    this->addPendingConnection(deviceSocket);
}

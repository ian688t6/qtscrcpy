#include <QDebug>
//#include "adbprocess.h"

#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(&m_server, &Server::serverStartResult, this, [this](bool success){
        qDebug() << "server started" << success;
    });
    connect(&m_server, &Server::serverConnected, this, [this](bool success, const QString& deviceName, const QSize& size){
        qDebug() << "server connected" << success << deviceName << size;
    });
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_testBtn_clicked()
{
//    QStringList arguments;
////    arguments << "devices";
//    arguments << "shell";
//    arguments << "ip";
//    arguments << "-f";
//    arguments << "inet";
//    arguments << "addr";
//    arguments << "show";
//    arguments << "wlan0";
//    AdbProcess *myProcess = new AdbProcess(this);
//    connect(myProcess, &AdbProcess::adbProcessResult, this, [this, myProcess](AdbProcess::AdbRetCode retcode){
//        if (AdbProcess::AdbRetExecSucc == retcode) {
////            qDebug() << myProcess->getDevicesSerialFromStdout().join("*");
//            qDebug() << myProcess->getDeviceIpFromStdout();
//        }
//    });
//    myProcess->execute("", arguments);
//    myProcess->push("", "/home/jyin/Documents/vimrc", "/storage/sdcard0/Download");
//    myProcess->remove("", "/storage/sdcard0/Download/vimrc");
//    myProcess->removeReverse("", "scrcpy");
    qDebug() << "start Btn clicked";
    m_server.start("", 27183, 1080, 8000000);
}

void Dialog::on_stopBtn_clicked()
{
    m_server.stop();
}

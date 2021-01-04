#include <QDebug>
#include "controlevent.h"

#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(&m_server, &Server::serverStartResult, this, [this](bool success){
        if (success) {
            m_server.connectTo();
        } else {
            close();
        }
    });
    connect(&m_server, &Server::serverConnected, this, [this](bool success, const QString& deviceName, const QSize& size){
        qDebug() << "server connected" << success << deviceName << size;
        if (success) {
            m_decoder.setDeviceSocket(m_server.getDeviceSocket());
            m_decoder.startDecoder();
        }
    });

    m_frames.init();
    m_decoder.setFrame(&m_frames);
    connect(&m_decoder, &Decoder::onNewFrame, this, [this](){
        m_frames.lock();
        const AVFrame *frame = m_frames.consumeRenderedFrame();
        m_videoWidget->setFrameSize(QSize(frame->width, frame->height));
        m_videoWidget->updateTextures(frame->data[0], frame->data[1], frame->data[2], frame->linesize[0], frame->linesize[1], frame->linesize[2]);
        m_frames.unLock();
    });

    m_videoWidget = new QYUVOpenGLWidget(Q_NULLPTR);
    m_videoWidget->resize(240, 432);
}

Dialog::~Dialog()
{
    m_frames.deinit();
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

    m_server.start("", 27183, 720, 8000000);
    if (m_videoWidget) {
        m_videoWidget->show();
    }
}

void Dialog::on_stopBtn_clicked()
{
    m_server.stop();
}

void Dialog::on_testInputBtn_clicked()
{
    QRect pos;
    pos.setLeft(100);
    pos.setTop(100);
    pos.setWidth(m_videoWidget->frameSize().width());
    pos.setHeight(m_videoWidget->frameSize().height());
    ControlEvent mouseEvent(ControlEvent::ControlEventMouse);
    mouseEvent.setMouseEventData(AMOTION_EVENT_ACTION_DOWN, AMOTION_EVENT_BUTTON_PRIMARY, pos);
    m_server.getDeviceSocket()->write(mouseEvent.serializeData());
}

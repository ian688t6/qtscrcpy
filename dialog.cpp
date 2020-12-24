#include <QDebug>
#include "adbprocess.h"

#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_testBtn_clicked()
{
    QStringList arguments;
    arguments << "devices";

    AdbProcess *myProcess = new AdbProcess(this);
    connect(myProcess, &AdbProcess::adbProcessResult, this, [this](AdbProcess::AdbRetCode retcode){
        qDebug() << ">>>>>>>" << retcode;
    });
    myProcess->execute("", arguments);
}

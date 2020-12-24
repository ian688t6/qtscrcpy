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
    QString program = "adb";
//    QStringList arguments;
//    arguments << "-style" << "fusion";

    AdbProcess *myProcess = new AdbProcess(this);
    myProcess->start(program, nullptr);
}

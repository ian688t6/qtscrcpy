#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "server.h"
#include "frames.h"
#include "decoder.h"
#include "qyuvopenglwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_testBtn_clicked();

    void on_stopBtn_clicked();

private:
    Ui::Dialog *ui;
    Server m_server;
    Frames m_frames;
    Decoder m_decoder;
    QYUVOpenGLWidget *m_videoWidget;
};
#endif // DIALOG_H

#include "dialog.h"
#include "decoder.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qputenv("QTSCRCPY_SERVER_PATH", "../qtscrcpy/addons/scrcpy-server");
    Decoder::init();
    QApplication a(argc, argv);
    Dialog w;
    w.show();
    int ret = a.exec();
    Decoder::deinit();
    return ret;
}

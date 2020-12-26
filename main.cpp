#include "dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    qputenv("QTSCRCPY_SERVER_PATH", "../qtscrcpy/addons/scrcpy-server");
    QApplication a(argc, argv);
    Dialog w;
    w.show();
    return a.exec();
}

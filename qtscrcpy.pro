QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    dialog.cpp

HEADERS += \
    dialog.h

FORMS += \
    dialog.ui

# Sub module
include($$PWD/adb/adb.pri)
include($$PWD/server/server.pri)
include($$PWD/decoder/decoder.pri)
include($$PWD/render/render.pri)
include($$PWD/android/android.pri)
include($$PWD/inputcontrol/inputcontrol.pri)
include($$PWD/commen/commen.pri)

# Include sub module path
INCLUDEPATH += \
    $$PWD/adb \
    $$PWD/server \
    $$PWD/decoder \
    $$PWD/commen \
    $$PWD/render \
    $$PWD/android \
    $$PWD/inputcontrol \
    $$PWD/addons/ffmpeg/include

# dependence lib
LIBS += \
        -L$$PWD/addons/ffmpeg/lib -lavformat \
        -L$$PWD/addons/ffmpeg/lib -lavcodec \
        -L$$PWD/addons/ffmpeg/lib -lavutil \
        -L$$PWD/addons/ffmpeg/lib -lswscale

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

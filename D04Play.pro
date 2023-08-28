QT       += core gui
QT       += network
QT       += webchannel
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH +=$$PWD/ffmpegsdk/include


LIBS += $$PWD/ffmpegsdk/lib/avcodec.lib\
        $$PWD/ffmpegsdk/lib/avdevice.lib\
        $$PWD/ffmpegsdk/lib/avfilter.lib\
        $$PWD/ffmpegsdk/lib/avformat.lib\
        $$PWD/ffmpegsdk/lib/avutil.lib\
        $$PWD/ffmpegsdk/lib/postproc.lib\
        $$PWD/ffmpegsdk/lib/swresample.lib\
        $$PWD/ffmpegsdk/lib/swscale.lib\

SOURCES += \
    SendThread.cpp \
    VideoThread.cpp \
    battery.cpp \
    flystates.cpp \
    hg_protocol.cpp \
    main.cpp \
    mainwindow.cpp \
    paintarea.cpp \
    udphandle.cpp \
    udpstates.cpp

HEADERS += \
    SendThread.h \
    VideoThread.h \
    battery.h \
    flystates.h \
    hg_protocol.h \
    hg_protocol_types.h \
    include.h \
    mainwindow.h \
    paintarea.h \
    udphandle.h \
    udpstates.h

FORMS += \
    flystates.ui \
    mainwindow.ui \
    paintarea.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qss.qrc

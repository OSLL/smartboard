#-------------------------------------------------
#
# Project created by QtCreator 2011-03-13T21:07:44
#
#-------------------------------------------------

TEMPLATE = app

VERSION = 0.2.0

QT       += core gui network xml svg

CONFIG       += qt link_pkgconfig
#CONFIG       -= app_bundle
PKGCONFIG     =  libwhiteboard dbus-glib-1

#TARGET = smartboard
#target.path=/usr/local/bin

# for desctop
INCLUDEPATH  += /usr/local/include/whiteboard/qt-core

# for meego
#INCLUDEPATH  += /usr/lib/madde/linux-i686/sysroots/meego-core-ia32-madde-sysroot-1.1-fs/usr/include/whiteboard/qt-core
#INCLUDEPATH  += /usr/lib/madde/linux-i686/sysroots/meego-core-ia32-madde-sysroot-1.1-fs/usr/include/whiteboard

# for maemo
#INCLUDEPATH  += /home/host/NokiaQtSDK/Maemo/4.6.2/sysroots/fremantle-arm-sysroot-20.2010.36-2-slim/usr/include/whiteboard/qt-core
#INCLUDEPATH  += /home/host/NokiaQtSDK/Maemo/4.6.2/sysroots/fremantle-arm-sysroot-20.2010.36-2-slim/usr/include/whiteboard

# for desctop
LIBS         += -L/usr/local/lib \
                -lq_whiteboard_node
# for meego
#LIBS         += -L/usr/lib/madde/linux-i686/sysroots/meego-core-ia32-madde-sysroot-1.1-fs/usr/lib \
#                -lq_whiteboard_node
# for maemo
#LIBS         += -L/home/host/NokiaQtSDK/Maemo/4.6.2/sysroots/fremantle-arm-sysroot-20.2010.36-2-slim/usr/lib \
#                -lq_whiteboard_node

SOURCES += main.cpp \
           smartboard.cpp \
           kphandlers.cpp \
           finddialog.cpp \
           viewboard.cpp \
           sticker.cpp

HEADERS  += smartboard.h \
            kphandlers.h \
            finddialog.h \
            viewboard.h \
            sticker.h

FORMS    += smartboard.ui

RESOURCES += smartboard.qrc

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target
}
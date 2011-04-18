#-------------------------------------------------
#
# Project created by QtCreator 2011-03-13T21:07:44
#
#-------------------------------------------------

TEMPLATE = app
QT       += core gui network xml svg

CONFIG       += qt link_pkgconfig
#CONFIG       -= app_bundle
PKGCONFIG     =  libwhiteboard dbus-glib-1

#TARGET = smartboard
#target.path=/usr/local/bin

INCLUDEPATH  += /usr/local/include/whiteboard/qt-core

LIBS         += -L/usr/local/lib \
                -lq_whiteboard_node

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

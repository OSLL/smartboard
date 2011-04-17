#-------------------------------------------------
#
# Project created by QtCreator 2011-03-13T21:07:44
#
#-------------------------------------------------

TEMPLATE = app
QT       += core gui network xml

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
    finddialog.cpp

HEADERS  += smartboard.h \
            kphandlers.h \
    finddialog.h

FORMS    += smartboard.ui

RESOURCES += smartboard.qrc

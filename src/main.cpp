// main.cpp

#include <QtGui/QApplication>
#include <glib-object.h>
#include "smartboard.h"
#include "q_whiteboard_node.h"
#include "q_sib_discovery.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSibDiscovery *dis;
    SmartBoard *sb;

    g_type_init();
    dbus_g_thread_init();

    // connection to SIB
    dis = new QSibDiscovery(&app);
    sb = new SmartBoard;

    QObject::connect(dis, SIGNAL(sibList(const QList<QSibInfo *>&)),
                     sb,    SLOT(sibList(const QList<QSibInfo *>&)));

    QObject::connect(dis, SIGNAL(sibInserted(const QSibInfo *)),
                     sb,    SLOT(sibInserted(const QSibInfo *)));

    QObject::connect(dis, SIGNAL(sibRemoved(QString)),
                     sb,    SLOT(sibRemoved(QString)));

    dis->getSibs();

    //sb->setMaximumSize(400, 600);
    sb->show();

    return app.exec();
}

#ifndef KPHANDLERS_H
#define KPHANDLERS_H

#include <QObject>

#include "q_whiteboard_node.h"
#include "templatequery.h"

class KPHandlers : public QObject
{
    Q_OBJECT

public:
    explicit KPHandlers(QObject *parent = 0);

private:

signals:

public slots:

};

#endif // KPHANDLERS_H

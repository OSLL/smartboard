// viewboard.cpp
// SmartBoard view, that show all available announcements in graphic view

#include "viewboard.h"

ViewBoard::ViewBoard(QObject *parent) :
    QGraphicsScene(parent)
{

}

ViewBoard::~ViewBoard()
{
    // remove all scene items
    QList<QGraphicsItem*> list = this->items();
    foreach(QGraphicsItem *item, list)
        this->removeItem(item);
}

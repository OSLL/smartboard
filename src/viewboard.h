// viewboard.h
//

#ifndef VIEWBOARD_H
#define VIEWBOARD_H

#include <QObject>
#include <QGraphicsScene>


class ViewBoard : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit ViewBoard(QObject *parent = 0);
    ~ViewBoard();

signals:

public slots:

};

#endif // VIEWBOARD_H

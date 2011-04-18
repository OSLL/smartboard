// sticker.h

#ifndef STICKER_H
#define STICKER_H

#include <QGraphicsSvgItem>
#include <QGraphicsScene>
#include <QSvgRenderer>
#include <QString>

class Sticker : public QGraphicsSvgItem
{
public:
    Sticker();
    Sticker(const QString& image);
    ~Sticker();

    QSvgRenderer *m_renderer;

private:
    void setNewItemImage(QGraphicsSvgItem *item, QString image);
};

#endif // STICKER_H

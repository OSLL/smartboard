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

    void setNewItemImage(QGraphicsSvgItem *item, const QString &image);
    void setStickerTheme(QGraphicsSvgItem *item, const QString &theme);

private slots:
    void stickerFullInfo();
};

#endif // STICKER_H

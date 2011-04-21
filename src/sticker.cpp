// sticker.h
// announcement scene sticker

#include <QGraphicsSvgItem>
#include "sticker.h"

Sticker::Sticker() : m_renderer(0)
{
}

Sticker::Sticker(const QString& image) : m_renderer(new QSvgRenderer(image))
{
    this->setSharedRenderer(m_renderer);
    setFlag(ItemIsMovable,true);
    //setCacheMode(DeviceCoordinateCache);
    //connect(this, SIGNAL(), this, SLOT(stickerFullInfo()));
}

Sticker::~Sticker()
{
    if (m_renderer != 0)
    {
        delete m_renderer;
    }
}

// set new image to sticker
void Sticker::setNewItemImage(QGraphicsSvgItem *item, const QString &image)
{
    m_renderer->load(image);
    item->setSharedRenderer(m_renderer);
}

// set theme text to sticker
void Sticker::setStickerTheme(QGraphicsSvgItem *item, const QString &theme)
{
    QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem(item);
    text->setPos(-20, 25);
    text->setPen(QPen(QColor("green")));
    text->setText(theme);

}

//
void Sticker::stickerFullInfo()
{

}

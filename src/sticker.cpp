// sticker.h
// announcement scene sticker

#include "sticker.h"

Sticker::Sticker() : m_renderer(0)
{
}

Sticker::Sticker(const QString& image) : m_renderer(new QSvgRenderer(image))
{
    this->setSharedRenderer(m_renderer);
}

Sticker::~Sticker()
{
    if (m_renderer != 0)
    {
        delete m_renderer;
    }
}

// set new image to sticker
void Sticker::setNewItemImage(QGraphicsSvgItem *item, QString image)
{
    m_renderer->load(image);
    item->setSharedRenderer(m_renderer);
}

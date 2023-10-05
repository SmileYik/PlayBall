#include "circular.h"
#include <qbrush.h>
#include <qfont.h>
#include <qnamespace.h>
#include <qpainter.h>

Circular::Circular(RgbColor _color)
    : color(_color.r, _color.g, _color.b)
{

}

void Circular::paint(const IEntity *entity, QPainter *paint)
{
    const Point p = entity->getLocation();
    paint->setBrush(QBrush(color, Qt::BrushStyle::SolidPattern));
    paint->setPen(color);
    paint->drawEllipse(p.x, p.y, entity->getWidth(), entity->getHeight());
}

const RgbColor Circular::getColor() const
{
    return {(unsigned char) color.red(), (unsigned char) color.green(), (unsigned char) color.blue()};
}
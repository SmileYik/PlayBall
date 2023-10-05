#include "foodshape.h"

// DEFINE_GET_SET_FUNCTION(QColor, Color, AFoodShape, color, , const, , , )
DEFINE_GET_SET_FUNCTION(float, Rotate, AFoodShape, rotate, , const, , , )

const RgbColor AFoodShape::getColor() const
{
    return {(unsigned char) color.red(), (unsigned char) color.green(), (unsigned char) color.blue()};
}

int FoodShapeTriangle::id() const {return 1;}

void FoodShapeTriangle::paint(const IEntity *entity, QPainter *paint)
{
    Point p = entity->getCenterLocation();
    double radius = entity->getWidth() / 2.0;

    QPoint left(p.x - radius * std::sin(PI / 3), p.y + radius * std::cos(PI / 3));
    QPoint right(p.x + radius * std::sin(PI / 3), p.y + radius * std::cos(PI / 3));
    QPoint top(p.x, p.y - radius);

    QPoint points[3] = {
        left, top, right
    };
    // paint->rotate(rotate);
    paint->setBrush(QBrush(color));
    paint->drawPolygon(points, 3, Qt::FillRule::WindingFill);
}
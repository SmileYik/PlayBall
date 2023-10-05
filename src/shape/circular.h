#ifndef CIRCULAR_H
#define CIRCULAR_H

#include "../interface/ishape.h"
#include "../util/rgbcolor.h"

class Circular : public IShape
{
public:
    Circular(RgbColor _color);
    ~Circular() = default;
    void paint(const IEntity *entity, QPainter *paint) override;
    const RgbColor getColor() const override;
private:
    QColor color;
};

#endif // CIRCULAR_H
#ifndef ISHAPE_H
#define ISHAPE_H

#include <QPainter>
#include "ientity.h"
#include "../util/rgbcolor.h"

class IShape
{
public:
    virtual ~IShape() = default;;
    virtual const RgbColor getColor() const = 0;
    virtual void paint(const IEntity *entity, QPainter *paint) = 0;
};

#endif // ISHAPE_H
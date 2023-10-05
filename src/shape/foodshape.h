#ifndef FOODSHAPE_H
#define FOODSHAPE_H

#include "../interface/ishape.h"
#include "../util/rgbcolor.h"
#include "../util/easygetset.h"
#include <cmath>
#include <qbrush.h>
#include <qcolor.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpoint.h>

#define  PI 3.1415926

class AFoodShape : public IShape
{
public:
    AFoodShape(RgbColor _color, float _rotate) : color(_color.r, _color.g, _color.b), rotate(_rotate) {}
    virtual ~AFoodShape() = default;
    virtual int id() const = 0;
    const RgbColor getColor() const override;
    // DECLARE_GET_SET_FUNCTION(QColor, Color, , const);
    DECLARE_GET_SET_FUNCTION(float, Rotate, , const);
protected:
    QColor color;
    float rotate = 0;
};

class FoodShapeTriangle : public AFoodShape
{
public:
    FoodShapeTriangle(RgbColor _color, float _rotate) : AFoodShape(_color, _rotate) {}
    ~FoodShapeTriangle() = default;
    int id() const override;
    void paint(const IEntity *entity, QPainter *paint) override;
};

#endif
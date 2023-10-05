#ifndef POINT_H
#define POINT_H

#include <cmath>
#include "../basictype.h"

class Point
{
public:
    double x;
    double y;
    uint64 distancePowed(const Point &p)
    {
        return std::pow(p.x - x, 2) + std::pow(p.y - y, 2);
    }
};

#endif
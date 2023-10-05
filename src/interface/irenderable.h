#ifndef IRENDERABLE_H
#define IRENDERABLE_H

#include <QPainter>

class IRenderable
{
public:
    virtual ~IRenderable() = default;;
    virtual void render(QPainter* painter) = 0;
};

#endif // IRENDERABLE_H
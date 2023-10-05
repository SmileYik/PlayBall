#ifndef IENTITY_H
#define IENTITY_H

#include "../util/point.h"
#include "iupdatable.h"
#include "irenderable.h"

class IEntity : public IUpdatable, public IRenderable
{
public:
    virtual ~IEntity() = default;
    virtual int getEntityType() const = 0;
    virtual int getEntityId() const = 0;
    virtual const Point getLocation() const = 0;
    virtual void setLocation(const Point point) = 0;
    virtual const Point getCenterLocation() const = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual void remove() = 0;
    virtual bool isRemoved() const = 0;
};

#endif // IENTITY_H
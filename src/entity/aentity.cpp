#include "aentity.h"

AEntity::AEntity(int _id, int _width, int _height, IShape *_shape, Point _centerLocation, Game::IGame *_game)
    : entityId(_id),
      width(_width),
      height(_height),
      location({_centerLocation.x - _width / 2., _centerLocation.y - _height / 2.}),
      removed(false),
      shape(_shape),
      game(_game)
{

}

AEntity::~AEntity()
{
    if (shape != nullptr)
    {
        delete shape;
        shape = nullptr;
    }
}

int AEntity::getEntityId() const
{
    return entityId;
}

int AEntity::getHeight() const
{
    return height;
}

int AEntity::getWidth() const
{
    return width;
}

void AEntity::remove()
{
    removed = true;
}

bool AEntity::isRemoved() const
{
    return removed;
}

const Point AEntity::getLocation() const
{
    return location;
}

void AEntity::setLocation(const Point point) 
{
    this->location = Point(point);
}

const Point AEntity::getCenterLocation() const
{
    Point center;
    center.x = location.x + (width / 2);
    center.y = location.y + (height / 2);
    return center;
}

void AEntity::render(QPainter *painter)
{
    if (shape == nullptr)
    {
        return;
    }
    painter->save();
    shape->paint(this, painter);
    painter->restore();
}

const IShape* AEntity::getShape() const
{
    return shape;
}
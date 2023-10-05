#include "entityfood.h"
#include "aentity.h"

EntityFood::EntityFood(int _id, int _width, int _height, IShape *_shape, double _food, Point _location)
    : AEntity(_id, _width, _height, _shape, _location),
    food(_food)
{

}

EntityFood::~EntityFood()
{
    if (shape != nullptr)
    {
        delete shape;
        shape = nullptr;
    }
}

int EntityFood::getEntityType() const
{
    return ENTITY_TYPE_ENTITY_FOOD;
}

DEFINE_GET_SET_FUNCTION(double, Food, EntityFood, food, , const, , , )
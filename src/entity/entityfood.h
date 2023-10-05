#ifndef ENTITYFOOD_H
#define ENTITYFOOD_H

#include "aentity.h"
#include "../util/easygetset.h"

#define ENTITY_TYPE_ENTITY_FOOD 2

class EntityFood : public AEntity
{
public:
    EntityFood(int _id, int _width, int _height, IShape *_shape, double _food, Point _location = {0, 0});
    ~EntityFood();
    void update(double interval) override {};
    int getEntityType() const override;
    DECLARE_GET_SET_FUNCTION(double, Food, , const);
private:
    double food;
};
#endif // ENTITYFOOD_H
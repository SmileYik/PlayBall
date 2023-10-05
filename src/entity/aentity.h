#ifndef AENTITY_H
#define AENTITY_H

#include "../interface/ientity.h"
#include "../interface/ishape.h"
#include "../interface/igame.h"

class AEntity : public IEntity
{
public:
    AEntity(int _id, int _width, int _height, IShape *_shape, Point _centerLocation = {0, 0}, Game::IGame *_game = nullptr);
    virtual ~AEntity();
    const Point getLocation() const override;
    void setLocation(const Point point) override;
    void remove() override;
    bool isRemoved() const override;
    int getWidth() const override;
    int getHeight() const override;
    int getEntityId() const override;
    const IShape* getShape() const;
    const Point getCenterLocation() const override;
    void render(QPainter *painter) override;
    void update(double interval) override = 0;
protected:
    Point location; // 绘画开始的点
    int entityId;
    double width;
    double height;
    int removed;
    IShape *shape = nullptr;
    Game::IGame *game = nullptr;
};

#endif
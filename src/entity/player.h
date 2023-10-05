#ifndef PLAYER_H
#define PLAYER_H

#include "aentity.h"
#include "../interface/icontrollable.h"
#include "../util/easygetset.h"

#define ENTITY_TYPE_PLAYER 1

class Player : public IControllable, public AEntity
{
public:
    Player(std::string _name, int _id, double _radius, IShape *_shape, Point _location = {0, 0}, Game::IGame *_game = nullptr);
    ~Player();
    void moveHorizon(double xAxis) override;
    void moveVertical(double yAxis) override;
    
    void update(double interval) override;
    void render(QPainter* painter) override;

    int getEntityType() const override;

    const std::string getName() const;

    void growth(double food);
    double totalAte();

    DECLARE_GET_SET_FUNCTION(double, XAxis, , const);
    DECLARE_GET_SET_FUNCTION(double, YAxis, , const);

    DECLARE_GET_SET_FUNCTION(float, XSpeed, , const);
    DECLARE_GET_SET_FUNCTION(float, YSpeed, , const);
    DECLARE_GET_SET_FUNCTION(double, Radius, , const);
    DECLARE_GET_SET_FUNCTION(double, CurrentGrowth, , const);

private:
    

private:
    std::string name;
    double xAxis = 0;
    double yAxis = 0;
    double speed = 50;
    float xSpeed = 0;
    float ySpeed = 0;
    float speedAttenuation = 3;
    float speedAttenuation2 = 0.8;
    double direction;

    double initRadius;
    double radius;
    double nextGrowth;
    double currentGrowth = 0;
};

#endif
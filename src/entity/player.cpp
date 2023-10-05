#include "player.h"
#include "aentity.h"
#include <algorithm>
#include <qcolor.h>
#include <qfont.h>
#include <qmatrix.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qvector.h>
#include <string>
#include <cmath>
#include <QString>

#define PI acos(-1)

Player::Player(std::string _name, int _id, double _radius, IShape *_shape, Point _location, Game::IGame *_game) 
    : AEntity(_id, 2 * _radius, 2 * _radius, _shape, _location, _game), 
      name(_name),
      initRadius(_radius),
      radius(_radius),
      nextGrowth(2 * PI * _radius)
{

}

Player::~Player()
{
    if (shape != nullptr)
    {
        delete shape;
        shape = nullptr;
    }
}

int Player::getEntityType() const 
{
    return ENTITY_TYPE_PLAYER;
}

void Player::moveHorizon(double xAxis)
{
    this->xAxis += xAxis;
    if (this->xAxis > 1)
    {
        this->xAxis = 1;
    } 
    else if (this->xAxis < -1)
    {
        this->xAxis = -1;
    }
}

void Player::moveVertical(double yAxis)
{
    this->yAxis += yAxis;
    if (this->yAxis > 1)
    {
        this->yAxis = 1;
    } 
    else if (this->yAxis < -1)
    {
        this->yAxis = -1;
    }
}

const std::string Player::getName() const
{
    return name;
}

double Player::totalAte()
{
    double l = 0;
    for (int i = initRadius; i <= radius; ++i)
    {
        l += 4 * PI * radius;
    }
    return l + currentGrowth;
}

void Player::update(double interval)
{
    double xySpeedAttenuation1 = interval * speedAttenuation;
    double xySpeedAttenuation2 = interval * speedAttenuation2;
    bool xSpeedNegtive = false;
    bool ySpeedNegtive = false;
    if (xSpeed != 0)
    {
        if (xSpeed > 0)
        {
            xSpeed = std::max(0.0, xSpeed - xySpeedAttenuation2);
        }
        else
        {
            xSpeedNegtive = true;
            xSpeed = std::min(0.0, xSpeed + xySpeedAttenuation2);
        }
    }

    if (ySpeed != 0)
    {
        if (ySpeed > 0)
        {
            ySpeed = std::max(0.0, ySpeed - xySpeedAttenuation2);
        }
        else
        {
            ySpeedNegtive = true;
            ySpeed = std::min(0.0, ySpeed + xySpeedAttenuation2);
        }
    }
    
    xSpeed += xAxis * xySpeedAttenuation1;
    ySpeed += yAxis * xySpeedAttenuation1;

    if (xSpeed > 1) 
    {
        xSpeed = 1;
    } 
    else if (xSpeed < -1) 
    {
        xSpeed = -1;
    }

    if (ySpeed > 1)
    {
        ySpeed = 1;
    }
    else if (ySpeed < -1)
    {
        ySpeed = -1;
    }
    // double xySpeedSum = std::abs(xSpeed) + std::abs(ySpeed);
    double xySpeedSum = std::sqrt(xSpeed * xSpeed + ySpeed * ySpeed);
    if (xySpeedSum != 0) {
        location.x += interval * xSpeed * speed / xySpeedSum;
        location.y += interval * ySpeed * speed / xySpeedSum;
    }

    direction = -std::atan(ySpeed / xSpeed);
    if (ySpeed <= 0 && xSpeed <= 0)
    {
        direction = std::acos(xSpeed / xySpeedSum);
    }
    else if (ySpeed >= 0 && xSpeed <= 0)
    {
        direction = PI + std::asin(ySpeed / xySpeedSum);
    }
    else if (ySpeed >= 0 && xSpeed >= 0)
    {
        direction = PI + std::acos(-xSpeed / xySpeedSum);
    }
    // DEBUG_MSG("ANGLE = %f", direction / PI * 180);

    // DEBUG_MSG("direction: %f, by: (%f / %f) = %f", direction, ySpeed, xSpeed, ySpeed / xSpeed);

    xAxis = yAxis = 0;
}

void Player::render(QPainter* painter) 
{
    if (shape == nullptr)
    {
        return;
    }
    painter->save();
    shape->paint(this, painter);
    Point p = getCenterLocation();
    double angle = direction / PI * 180;
    int start = angle - 15;
    int end = angle + 15;
    painter->setPen(QPen(painter->brush(), radius / 10));

    painter->drawArc(location.x - 10, location.y - 10, 20 + width, 20 + height, start * 16, (end - start) * 16);
    QFont font = painter->font();
    
    
    if (radius / 3.0 < 8)
    {
        font.setPointSizeF(radius / 3.0);
        painter->setFont(font);
        painter->drawText(location.x - 10, p.y - height, width + 20, height / 2, Qt::AlignCenter, QString::fromStdString(name));
    }
    else
    {
        painter->setBrush(QColor{255, 255, 255});
        painter->setPen(QColor{255, 255, 255});
        font.setPointSizeF(8);
        painter->setFont(font);
        std::string tmp = name;
        if (name.size() > 8)
        {
            // int size = name.size();
            // int count = size / 8;
            // for (int i = 0; i < count; ++i)
            // {
            //     tmp += name.substr(8 * i, 8) + "\n";
            // }
            // if (count % 8 != 0)
            // {
            //     tmp += name.substr(count * 8, count % 8);
            // }
        }
        painter->drawText(p.x - radius - 10, p.y - radius * 0.8, width + 20, height - radius * 0.3, Qt::AlignCenter | Qt::TextExpandTabs, QString::fromStdString(tmp));
    }
    
    painter->restore();
}

DEFINE_GET_SET_FUNCTION(double, XAxis, Player, xAxis, , const, , ,)
DEFINE_GET_SET_FUNCTION(double, YAxis, Player, yAxis, , const, , ,)

DEFINE_GET_SET_FUNCTION(float, XSpeed, Player, xSpeed, , const, , ,)
DEFINE_GET_SET_FUNCTION(float, YSpeed, Player, ySpeed, , const, , ,)

DEFINE_GET_SET_FUNCTION(double, Radius, Player, radius, , const, , , {
    double l = 4 * PI * radius;
    nextGrowth = l;
    width = radius * 2;
    height = radius * 2;
})

DEFINE_GET_SET_FUNCTION(double, CurrentGrowth, Player, currentGrowth, , const, , , {
    if (currentGrowth > nextGrowth)
    {
        currentGrowth -= nextGrowth;
        setRadius(radius + 1);
    }
})

void Player::growth(double food)
{
    currentGrowth += food;
    if (currentGrowth > nextGrowth)
    {
        currentGrowth -= nextGrowth;
        setRadius(radius + 1);
    }
}
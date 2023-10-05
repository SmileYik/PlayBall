#ifndef ICONTROLLABLE_H
#define ICONTROLLABLE_H

class IControllable 
{
public:
    virtual ~IControllable() = default;
    virtual void moveHorizon(double xAxis) = 0;
    virtual void moveVertical(double yAxis) = 0;
};
#endif // ICONTROLLABLE_H
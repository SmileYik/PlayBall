#ifndef IUPDATABLE_H
#define IUPDATABLE_H

class IUpdatable 
{
public:
    virtual ~IUpdatable() = default;;
    virtual void update(double interval) = 0;
};

#endif // IUPDATABLE_H
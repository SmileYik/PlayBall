#ifndef IGAME_H
#define IGAME_H

#include "../network/packet/ipacket.h"
#include "ientity.h"
#include "irenderable.h"
#include "iupdatable.h"
#include <vector>

class Player;

namespace Game {

class IGame;

static IGame* game = nullptr;

class IGame : public IUpdatable, public IRenderable
{
public:
    virtual ~IGame() = default;
    virtual void handlePacket(const Network::IPacket* packet) = 0;
    virtual IEntity* getEntityById(int entityId) = 0;
    virtual Player* getPlayer(std::string name) = 0;
    virtual std::vector<IEntity*> getEntitiesInRadius(Point p, double radius) = 0;
    virtual const std::vector<Player*> players() = 0;
    virtual const std::map<int, IEntity*> entities() = 0;
};
}

#endif // IGAME_H
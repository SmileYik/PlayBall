#ifndef SIMPLEGAME_H
#define SIMPLEGAME_H

#include "entity/player.h"
#include "interface/ientity.h"
#include "interface/igame.h"
#include "network/packet/ipacket.h"
#include "network/server.h"
#include <map>
#include <mutex>
#include <qpainter.h>

namespace Game {

#define PACKET_HANDLE_CASE(ID, PACKET, VAR) \
    case ID: handle##PACKET((Network::PACKET*) VAR); break

#define PACKET_HANDLE_DECLARE(PACKET, VAR) \
    void handle##PACKET(const Network::PACKET *VAR)

#define PACKET_HANDLE_DEFINE(CLASS, PACKET, VAR) \
    void CLASS::handle##PACKET(const Network::PACKET *VAR)

#define SEND_PACKET_TO_SERVER(VAR) \
    if (server != nullptr) server->send(VAR)

#define SERVER_SIDE(CODE) \
    if (server != nullptr) {CODE}

#define CLIENT_SIDE(CODE) \
    if (server == nullptr) {CODE}

class SimpleGame : public IGame
{

public:
    SimpleGame(Network::IServer *_server = nullptr);
    ~SimpleGame();
    void handlePacket(const Network::IPacket* packet) override;
    IEntity* getEntityById(int entityId) override;
    Player* getPlayer(std::string name) override;
    std::vector<IEntity*> getEntitiesInRadius(Point p, double radius) override;
    void update(double interval) override;
    void render(QPainter *painter) override;
    const std::vector<Player*> players() override;
    const std::map<int, IEntity*> entities() override;
protected:
    int nextEntityId();
    void sendPacket(Network::IPacket *packet);
    void playerEatEntity(Player *player, IEntity *entity);

    PACKET_HANDLE_DECLARE(PacketPlayerQuit, packet);
    PACKET_HANDLE_DECLARE(PacketPlayerJoin, packet);
    PACKET_HANDLE_DECLARE(PacketPlayerMove, packet);
    PACKET_HANDLE_DECLARE(PacketPlayerInfo, packet);
    PACKET_HANDLE_DECLARE(PacketPlayerLocation, packet);
    PACKET_HANDLE_DECLARE(PacketFoodSpawn, packet);
    PACKET_HANDLE_DECLARE(PacketFoodAte, packet);

private:
    Network::IServer *server;
    std::map<std::string, Player*> playerMap;
    std::map<int, IEntity*> entitiesIdMap;
    std::mutex entityLock;
    int entityId = 0;
};


}


#endif // SIMPLEGAME_H
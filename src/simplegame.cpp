#include "simplegame.h"
#include "entity/entityfood.h"
#include "entity/player.h"
#include "interface/ientity.h"
#include "network/packet/ipacket.h"
#include "network/server.h"
#include "shape/circular.h"
#include "shape/foodshape.h"
#include "util/debug.h"
#include "util/easygetset.h"
#include "util/point.h"
#include "util/rgbcolor.h"
#include <cmath>
#include <cstdlib>
#include <mutex>
#include <qpainter.h>
#include <vector>

namespace Game {

SimpleGame::SimpleGame(Network::IServer *_server)
    : server(_server)
{
    
}

SimpleGame::~SimpleGame()
{

}

void SimpleGame::update(double interval)
{
    std::lock_guard<std::mutex> lock(entityLock);

    SERVER_SIDE({
        int entitySize = entitiesIdMap.size();
        while (entitySize < 100)
        {
            Point location;
            do {
                location = Point({(double) (rand() % 400 + 50), (double) (rand() % 400 + 50)});
            } while (false); // locationToEntities.count(location.x) && locationToEntities[location.x].count(location.y));
            int size = rand() % 5 + 5;
            RgbColor color({(unsigned char) (rand() % 255), (unsigned char) (rand() % 255), (unsigned char) (rand() % 255)});
            FoodShapeTriangle *shape = new FoodShapeTriangle(color, rand() % 360);
            EntityFood *entity = new EntityFood(nextEntityId(), size, size, shape, rand() % 350, location);
            entitiesIdMap[entity->getEntityId()] = entity;
            Network::PacketFoodSpawn *packet = new Network::PacketFoodSpawn();
            packet->food = entity->getFood();
            packet->color = entity->getShape()->getColor();
            packet->location = entity->getLocation();
            packet->rotate = shape->getRotate();
            packet->size = entity->getWidth();
            packet->entityId = entity->getEntityId();
            server->send(packet);
            ++entitySize;
        }
    })


    FOREACH_ITERATOR(playerMap, entry, {
        entry.second->update(interval);
    })

    SERVER_SIDE({
        FOREACH_ITERATOR(playerMap, entry, {
            Player *player = entry.second;
            std::vector<IEntity*> entities = getEntitiesInRadius(player->getCenterLocation(), player->getRadius());
            
            FOREACH_ITERATOR(entities, entity, {
                if (entity->getEntityId() == player->getEntityId()) continue;
                if (entity->getEntityType() == ENTITY_TYPE_PLAYER)
                {
                    Player *other = (Player *) entity;
                    if (std::abs(player->getRadius() - other->getRadius()) > 5)
                    {
                        if (player->getRadius() > other->getRadius())
                        {
                            playerEatEntity(player, entity);
                        } 
                        else
                        {
                            playerEatEntity(other, player);
                        }
                    }
                } else if (entity->getEntityType() == ENTITY_TYPE_ENTITY_FOOD)
                {
                    playerEatEntity(player, entity);
                }
            })
        })
    })



    // remove
    std::list<int> removedIds;
    std::list<std::string> removePlayerNames;
    FOREACH_ITERATOR(entitiesIdMap, entry, {
        if (entry.second->isRemoved())
        {
            removedIds.push_back(entry.first);
            if (entry.second->getEntityType() == ENTITY_TYPE_PLAYER)
            {
                removePlayerNames.push_back(((Player*) entry.second)->getName());
            }
        }
    })

    FOREACH_ITERATOR(removePlayerNames, name, {
        playerMap.erase(name);
    })

    FOREACH_ITERATOR(removedIds, id, {
        delete entitiesIdMap[id];
        entitiesIdMap.erase(id);
    })
}

void SimpleGame::render(QPainter *painter)
{
    std::lock_guard<std::mutex> lock(entityLock);
    FOREACH_ITERATOR(entitiesIdMap, entry, {
        entry.second->render(painter);
    })
    FOREACH_ITERATOR(playerMap, entry, {
        entry.second->render(painter);
    })
}

void SimpleGame::handlePacket(const Network::IPacket* packet) 
{
    switch (packet->id()) {
        PACKET_HANDLE_CASE(0, PacketPlayerQuit, packet);
        PACKET_HANDLE_CASE(1, PacketPlayerJoin, packet);
        PACKET_HANDLE_CASE(2, PacketPlayerMove, packet);
        PACKET_HANDLE_CASE(3, PacketPlayerInfo, packet);
        PACKET_HANDLE_CASE(4, PacketPlayerLocation, packet);
        PACKET_HANDLE_CASE(5, PacketFoodSpawn, packet);
        PACKET_HANDLE_CASE(6, PacketFoodAte, packet);
    }
    delete packet;
}

IEntity* SimpleGame::getEntityById(int entityId) 
{
    std::lock_guard<std::mutex> lock(entityLock);
    return entitiesIdMap.count(entityId) ? entitiesIdMap[entityId] : nullptr;
}

Player* SimpleGame::getPlayer(std::string name) 
{
    std::lock_guard<std::mutex> lock(entityLock);
    return playerMap.count(name) ? playerMap[name] : nullptr;
}

inline
std::vector<IEntity*> SimpleGame::getEntitiesInRadius(Point p, double radius) 
{
    int minX = p.x - radius;
    int maxX = p.x + radius;
    int minY = p.y - radius;
    int maxY = p.y + radius;
    std::vector<IEntity*> entities;
    int powOfRadius = radius * radius;

    // for (int x = minX; x <= maxX; ++x)
    // {
    //     int powOfX = std::pow(x - p.x, 2);
    //     if (locationToEntities.count(x))
    //     {
    //         std::map<int, std::list<IEntity*>> *yEntities = &(locationToEntities[x]);
    //         for (int y = minY; y <= maxY; ++y)
    //         {
    //             if (yEntities->count(y) && powOfX + std::pow(y - p.y, 2) <= powOfRadius)
    //             {
    //                 FOREACH_ITERATOR(locationToEntities[x][y], entity, {
    //                     // if (entity->isRemoved()) continue;
    //                     entities.push_back(entity);
    //                 })
    //                 locationToEntities[x][y].clear();
    //             }
    //         }
    //     }
    // }

    FOREACH_ITERATOR(entitiesIdMap, entry, {
        if (p.distancePowed(entry.second->getCenterLocation()) <= powOfRadius)
        {
            entities.push_back(entry.second);
        }
    })

    FOREACH_ITERATOR(playerMap, entry, {
        Player *player = entry.second;
        if (p.distancePowed(player->getCenterLocation()) <= powOfRadius)
        {
            entities.push_back(player);
        }
    })

    return entities;
}

int SimpleGame::nextEntityId()
{
    while (entityId == 0 || entitiesIdMap.count(entityId))
    {
        ++entityId;
    }
    return entityId++;
}

void SimpleGame::sendPacket(Network::IPacket *packet)
{
    if (server != nullptr)
    {
        server->send(packet);
    }
}

const std::vector<Player*> SimpleGame::players()
{
    std::lock_guard<std::mutex> lock(entityLock);
    std::vector<Player*> entities;
    FOREACH_ITERATOR(playerMap, entry, {
        Player *player = entry.second;
        entities.push_back(player);
    })
    return entities;
}

const std::map<int, IEntity*> SimpleGame::entities()
{
    std::lock_guard<std::mutex> lock(entityLock);
    return std::map<int, IEntity*>(entitiesIdMap);
}

inline
void SimpleGame::playerEatEntity(Player *player, IEntity *entity)
{
    SERVER_SIDE({
        Network::PacketFoodAte *packet = new Network::PacketFoodAte();
        packet->entityId = entity->getEntityId();
        packet->ateBy = player->getEntityId();
        SEND_PACKET_TO_SERVER(packet);
    });

    if (entity->getEntityType() == ENTITY_TYPE_PLAYER)
    {
        DEBUG_INFO("start eat player");
        Player *other = (Player *) entity;
        player->growth(other->totalAte());
        other->remove();
        DEBUG_INFO("ate over");
    }
    else 
    {
        DEBUG_INFO("start eat food");
        EntityFood *food = (EntityFood*) entity;
        player->growth(food->getFood());
        food->remove();
        DEBUG_INFO("ate over");
    }
}

PACKET_HANDLE_DEFINE(SimpleGame, PacketPlayerQuit, packet)
{
    DEBUG_INFO("Handling PacketPlayerQuit start.");
    std::lock_guard<std::mutex> lock(entityLock);
    if (playerMap.count(packet->playerName))
    {
        Player *player = playerMap[packet->playerName];
        DEBUG_MSG("Player %s quit game.", packet->playerName.c_str());

        playerMap.erase(player->getName());
        entitiesIdMap.erase(player->getEntityId());
        delete player;
    }
    DEBUG_INFO("Handling PacketPlayerQuit over.");
}

PACKET_HANDLE_DEFINE(SimpleGame, PacketPlayerJoin, packet)
{
    DEBUG_INFO("Handling PacketPlayerJoin start.");
    std::lock_guard<std::mutex> lock(entityLock);
    int id = packet->entityId;
    if (id == 0)
    {
        id = nextEntityId();
    }
    Player *player = new Player(
        packet->playerName, id, 10, new Circular(packet->mainColor), {250, 250}, this
    );
    playerMap[player->getName()] = player;
    entitiesIdMap[id] = player;
    SEND_PACKET_TO_SERVER(new Network::PacketPlayerJoin(
        id, packet->playerName, packet->mainColor
    ));
    DEBUG_INFO("Handling PacketPlayerJoin over.");
}

PACKET_HANDLE_DEFINE(SimpleGame, PacketPlayerMove, packet)
{
    DEBUG_INFO("Handling PacketPlayerMove start.");
    std::lock_guard<std::mutex> lock(entityLock);
    if (!entitiesIdMap.count(packet->entityId))
    {
        DEBUG_MSG("[WARN] [PacketPlayerMove] Not find entity id: %d", packet->entityId);
        return;
    }
    Player *p = (Player *) entitiesIdMap[packet->entityId];
    p->moveHorizon(packet->xAxis);
    p->moveVertical(packet->yAxis);
    SEND_PACKET_TO_SERVER(new Network::PacketPlayerMove(
        packet->entityId, packet->xAxis, packet->yAxis
    ));
    DEBUG_INFO("Handling PacketPlayerMove over.");
}

PACKET_HANDLE_DEFINE(SimpleGame, PacketPlayerInfo, packet)
{
    DEBUG_INFO("Handling PacketPlayerInfo start.");
    std::lock_guard<std::mutex> lock(entityLock);
    if (!entitiesIdMap.count(packet->entityId))
    {
        DEBUG_MSG("[WARN] [PacketPlayerInfo] Not find entity id: %d", packet->entityId);
        return;
    }
    Player *p = (Player *) entitiesIdMap[packet->entityId];
    p->setXSpeed(packet->xSpeed);
    p->setYSpeed(packet->ySpeed);
    p->setRadius(packet->radius);
    p->setCurrentGrowth(packet->currentGrowth);
    p->setLocation(packet->location);
    DEBUG_INFO("Handling PacketPlayerInfo over.");
}

PACKET_HANDLE_DEFINE(SimpleGame, PacketPlayerLocation, packet)
{
    DEBUG_INFO("Handling PacketPlayerLocation start.");
    std::lock_guard<std::mutex> lock(entityLock);
    if (!entitiesIdMap.count(packet->entityId))
    {
        DEBUG_MSG("[WARN] [PacketPlayerLocation] Not find entity id: %d", packet->entityId);
        return;
    }
    Player *p = (Player *) entitiesIdMap[packet->entityId];
    p->setXSpeed(packet->xSpeed);
    p->setYSpeed(packet->ySpeed);
    p->setLocation(packet->location);
    DEBUG_INFO("Handling PacketPlayerLocation over.");
}

PACKET_HANDLE_DEFINE(SimpleGame, PacketFoodSpawn, packet)
{
    DEBUG_INFO("Handling PacketFoodSpawn start.");
    std::lock_guard<std::mutex> lock(entityLock);
    FoodShapeTriangle *shape = new FoodShapeTriangle(packet->color, packet->rotate);
    EntityFood *entity = new EntityFood(packet->entityId, packet->size, packet->size, shape, packet->food, packet->location);
    entitiesIdMap[entity->getEntityId()] = entity;
    DEBUG_INFO("Handling PacketFoodSpawn over.");
}

PACKET_HANDLE_DEFINE(SimpleGame, PacketFoodAte, packet)
{
    DEBUG_INFO("Handling PacketFoodAte start.");
    std::lock_guard<std::mutex> lock(entityLock);
    if (entitiesIdMap.count(packet->ateBy) && entitiesIdMap.count(packet->entityId))
    {
        playerEatEntity((Player *) entitiesIdMap[packet->ateBy], entitiesIdMap[packet->entityId]);
    }
    DEBUG_INFO("Handling PacketFoodAte over.");
}

}
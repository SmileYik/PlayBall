#ifndef IPACKET_H
#define IPACKET_H

#include <cstdlib>
#include <map>
#include <functional>
#include "../socket/ioreader.h"
#include "../../util/rgbcolor.h"
#include "../socket/ioreader.h"
#include "../../util/point.h"

namespace Network 
{

// #define REGISTER_PACKET(PACKET) \
//     const static PackId register##PACKET = registerPacket([]() { \
//         return new PACKET(); \
//     })

#define JUDGE_AND_NEW_PACKET(ID, PACKET) \
    case ID: DEBUG_MSG("Create a packet: %s(%d)", #PACKET, ID); return new PACKET()

#define DEFINE_PACKET_TYPE_FUNCTION(ID, PACKET, VAR, READ, SEND) \
    PackId PACKET::id() const { return ID; } \
    void PACKET::read(IOReader *VAR) {READ} \
    void PACKET::send(IOReader *VAR) {SEND}

typedef unsigned char PackId;
class IPacket;
class IPacketReceive;

// PackId registerPacket(std::function<IPacket*(void)> constructor);
IPacketReceive* newPacket(PackId id);

class IPacket
{
public:
    virtual ~IPacket() = default;
    virtual PackId id() const = 0;
    virtual void send(IOReader* reader) = 0;
};

class IPacketReceive
{
public:
    virtual void read(IOReader* reader) = 0;
};

class APacket : public IPacket, public IPacketReceive
{
public:
    APacket() = default;
    virtual ~APacket() = default;
};

class AEntityPacket : public APacket
{
public:
    AEntityPacket(int _entityId = 0);
    virtual ~AEntityPacket() = default;
    int entityId;
};

class PacketPlayerQuit : public AEntityPacket
{
public:
    PacketPlayerQuit() = default;
    ~PacketPlayerQuit() = default;
    
    PackId id() const override;
    void read(IOReader* reader) override;
    void send(IOReader* reader) override;

public:
    std::string playerName;
};

class PacketPlayerJoin : public AEntityPacket
{
public:
    PacketPlayerJoin() = default;
    PacketPlayerJoin(int _entityId, std::string _playerName, RgbColor _mainColor);
    ~PacketPlayerJoin() = default;
    
    PackId id() const override;
    void read(IOReader* reader) override;
    void send(IOReader* reader) override;

public:
    std::string playerName;
    RgbColor mainColor;
};

class PacketPlayerMove : public AEntityPacket
{
public:
    PacketPlayerMove() = default;
    PacketPlayerMove(int _entityId, double _xAxis, double _yAxis);
    ~PacketPlayerMove() = default;
    
    PackId id() const override;
    void read(IOReader* reader) override;
    void send(IOReader* reader) override;

public:
    double xAxis;
    double yAxis;
};

class PacketPlayerInfo : public AEntityPacket
{
public:
    PacketPlayerInfo() = default;
    ~PacketPlayerInfo() = default;
    
    PackId id() const override;
    void read(IOReader* reader) override;
    void send(IOReader* reader) override;

public:
    double xAxis;
    double yAxis;
    float xSpeed;
    float ySpeed;
    double radius;
    double currentGrowth;
    Point location;
};

class PacketPlayerLocation : public AEntityPacket
{
public:
    PacketPlayerLocation() = default;
    ~PacketPlayerLocation() = default;
    
    PackId id() const override;
    void read(IOReader* reader) override;
    void send(IOReader* reader) override;

public:
    float xSpeed;
    float ySpeed;
    Point location;
};

class PacketFoodSpawn : public AEntityPacket
{
public:
    PacketFoodSpawn() = default;
    ~PacketFoodSpawn() = default;
    
    PackId id() const override;
    void read(IOReader* reader) override;
    void send(IOReader* reader) override;

public:
    RgbColor color;
    float rotate;
    int size;
    char shape;
    int food;
    Point location;
};

class PacketFoodAte : public AEntityPacket
{
public:
    PacketFoodAte() = default;
    ~PacketFoodAte() = default;
    
    PackId id() const override;
    void read(IOReader* reader) override;
    void send(IOReader* reader) override;

public:
    int ateBy;
};

}

#endif
#include "ipacket.h"
#include "../../util/debug.h"
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Network {

IPacketReceive* newPacket(PackId id)
{
    switch (id) {
        JUDGE_AND_NEW_PACKET(0, PacketPlayerQuit);
        JUDGE_AND_NEW_PACKET(1, PacketPlayerJoin);
        JUDGE_AND_NEW_PACKET(2, PacketPlayerMove);
        JUDGE_AND_NEW_PACKET(3, PacketPlayerInfo);
        JUDGE_AND_NEW_PACKET(4, PacketPlayerLocation);
        JUDGE_AND_NEW_PACKET(5, PacketFoodSpawn);
        JUDGE_AND_NEW_PACKET(6, PacketFoodAte);
    }

    return nullptr;
}

AEntityPacket::AEntityPacket(int _entityId)
    : entityId(_entityId)
{

}

DEFINE_PACKET_TYPE_FUNCTION(0, PacketPlayerQuit, io,
{
    entityId = io->readInt();
    playerName = io->readString();
},
{
    io->write(id());
    io->writeString(playerName.c_str());
})

PacketPlayerJoin::PacketPlayerJoin(int _entityId, std::string _playerName, RgbColor _mainColor)
    : AEntityPacket(_entityId),
      playerName(_playerName),
      mainColor(_mainColor)
{

}

DEFINE_PACKET_TYPE_FUNCTION(1, PacketPlayerJoin, reader,
{
    entityId = reader->readInt();
    char *chars = reader->readString();
    playerName = std::string(chars);
    delete chars;
    mainColor.r = reader->read();
    mainColor.g = reader->read();
    mainColor.b = reader->read();

    DEBUG_MSG("Receive PacketPlayerJoin packet. id = %d, playerName = %s, mainColor = (%d, %d, %d)", entityId, playerName.c_str(), mainColor.r, mainColor.b, mainColor.g);
},
{
    reader->write(id());
    reader->writeInt(entityId);
    reader->writeString(playerName.c_str());
    reader->write(mainColor.r);
    reader->write(mainColor.g);
    reader->write(mainColor.b);

    DEBUG_MSG("Send PacketPlayerJoin packet. id = %d, playerName = %s, mainColor = (%d, %d, %d)", entityId, playerName.c_str(), mainColor.r, mainColor.b, mainColor.g);
})

PacketPlayerMove::PacketPlayerMove(int _entityId, double _xAxis, double _yAxis)
    : AEntityPacket(_entityId),
    xAxis(_xAxis),
    yAxis(_yAxis)
{

}

DEFINE_PACKET_TYPE_FUNCTION(2, PacketPlayerMove, reader, 
{
    entityId = reader->readInt();
    xAxis = reader->readDouble();
    yAxis = reader->readDouble();
    // DEBUG_MSG("Receive PacketPlayerMove packet. id = %d, xAxis = %f, yAxis = %f", entityId, xAxis, yAxis);
},
{
    reader->write(id());
    reader->writeInt(entityId);
    reader->writeDouble(xAxis);
    reader->writeDouble(yAxis);
    // DEBUG_MSG("Send PacketPlayerMove packet. id = %d, xAxis = %f, yAxis = %f", entityId, xAxis, yAxis);
});

DEFINE_PACKET_TYPE_FUNCTION(3, PacketPlayerInfo, io,
{
    entityId = io->readInt();
    xSpeed = io->readFloat();
    ySpeed = io->readFloat();
    radius = io->readDouble();
    currentGrowth = io->readDouble();
    location.x = io->readDouble();
    location.y = io->readDouble();
},
{
    io->write(id());
    io->writeInt(entityId);
    io->writeFloat(xSpeed);
    io->writeFloat(ySpeed);
    io->writeDouble(radius);
    io->writeDouble(currentGrowth);
    io->writeDouble(location.x);
    io->writeDouble(location.y);
})

DEFINE_PACKET_TYPE_FUNCTION(4, PacketPlayerLocation, io,
{
    entityId = io->readInt();
    xSpeed = io->readFloat();
    ySpeed = io->readFloat();
    location.x = io->readFloat();
    location.y = io->readFloat();
},
{
    io->write(id());
    io->writeInt(entityId);
    io->writeFloat(xSpeed);
    io->writeFloat(ySpeed);
    io->writeFloat(location.x);
    io->writeFloat(location.y);
})

DEFINE_PACKET_TYPE_FUNCTION(5, PacketFoodSpawn, io,
{
    entityId = io->readInt();
    location.x = io->readFloat();
    location.y = io->readFloat();
    color.r = io->read();
    color.g = io->read();
    color.b = io->read();
    rotate = io->readFloat();
    shape = io->read();
    food = io->readInt();
    size = io->readInt();
},
{
    io->write(id());
    io->writeInt(entityId);
    io->writeFloat(location.x);
    io->writeFloat(location.y);
    io->write(color.r);
    io->write(color.g);
    io->write(color.b);
    io->writeFloat(rotate);
    io->write(shape);
    io->writeInt(food);
    io->writeInt(size);
})

DEFINE_PACKET_TYPE_FUNCTION(6, PacketFoodAte, io,
{
    entityId = io->readInt();
    ateBy = io->readInt();
},
{
    io->write(id());
    io->writeInt(entityId);
    io->writeInt(ateBy);
})

}
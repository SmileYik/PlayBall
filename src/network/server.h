#ifndef SERVER_H
#define SERVER_H

#include "packet/ipacket.h"
#include "socket/ioreader.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <map>
#include <string>
#include <mutex>
#include <queue>

namespace Network {

class IServer
{
public:
    virtual void start(int port) = 0;
    virtual void stop() = 0;
    virtual void send(IPacket* packet) = 0;
    virtual void send(std::string playerName, IPacket* packet) = 0;
    virtual IPacket* receive() = 0;
};

class SimpleGameServer : public IServer
{
public:
    SimpleGameServer();
    ~SimpleGameServer();

    void start(int port) override;
    void stop() override;
    void send(IPacket* packet) override;
    void send(std::string playerName, IPacket* packet) override;
    IPacket* receive() override;

private:
    void run();

private:
    class PacketPack {
    public:
        IPacket* packet = nullptr;
        bool sendSomeone = false;
        std::string playerName = "";
    };

private:
    QTcpServer *socketServer = nullptr;
    std::list<IOReader*> connectedPlayers;
    std::map<std::string, IOReader*> playerNameConnectionMap;
    std::mutex connectionLock;

    std::queue<IPacket*> packetQueue;
    std::mutex packetQueueLock;

    std::queue<PacketPack> waitToSendQueue;
    std::mutex waitToSendQueueLock;
};

}



#endif // SERVER_H
#ifndef CLIENT_H
#define CLIENT_H

#include "packet/ipacket.h"
#include "socket/ioreader.h"
#include <mutex>
#include <queue>
#include <string>
#include <QTcpSocket>

namespace Network {

class IClient
{
public:
    virtual void start(std::string host, int port) = 0;
    virtual void stop() = 0;
    virtual void send(IPacket *packet) = 0;
    virtual IPacket* receive() = 0;
};

class SimpleGameClient : public IClient
{
public:
    SimpleGameClient();
    ~SimpleGameClient();
    void start(std::string host, int port) override;
    void stop() override;
    void send(IPacket *packet) override;
    IPacket* receive() override;
private:
    void run();

private:
    QTcpSocket *socket = nullptr;
    IOReader *reader = nullptr;

    std::queue<IPacket*> packetQueue;
    std::mutex packetQueueLock;

    std::queue<IPacket*> waitToSendQueue;
    std::mutex waitToSendQueueLock;
};

}




#endif
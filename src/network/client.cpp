#include "client.h"
#include "packet/ipacket.h"
#include "socket/ioreader.h"
#include <mutex>
#include <thread>
#include "../util/debug.h"

namespace Network {

SimpleGameClient::SimpleGameClient()
{

}

SimpleGameClient::~SimpleGameClient()
{
    stop();
    if (reader == nullptr)
    {
        delete socket;
    }
    else 
    {
        delete reader;
    }
}

void SimpleGameClient::start(std::string host, int port) 
{
    socket = new QTcpSocket();
    socket->connectToHost(QString::fromStdString(host), port);
    DEBUG_MSG("[Client] Client start to connect to %s:%d.", host.c_str(), port);
    int waitTimes = 3 * 1000 / 5;
    if (socket->waitForConnected())
    {
        reader = new IOReader(socket);
        DEBUG_MSG("[Client] Client connected to %s:%d.", host.c_str(), port);
        run();
    }
    else
    {
        DEBUG_MSG("[Client] Client can't connected to %s:%d.", host.c_str(), port);
    }
}

void SimpleGameClient::stop() 
{
    socket->disconnectFromHost();
    socket->close();
}

void SimpleGameClient::send(IPacket *packet) 
{
    DEBUG_MSG("[Client] Put a packet to wait queue: %d", packet==nullptr);
    std::lock_guard<std::mutex> lock(waitToSendQueueLock);
    waitToSendQueue.push(packet);
}

IPacket* SimpleGameClient::receive() 
{
    std::lock_guard<std::mutex> lock(packetQueueLock);
    if (packetQueue.empty())
    {
        return nullptr;
    }
    IPacket *packet = packetQueue.front();
    packetQueue.pop();
    return packet;
}

void SimpleGameClient::run()
{
    while (true)
    {
        // receive
        {
            if (reader->isClosed())
            {
                // todo closed
            } else if (reader->bytesAvailable() > 0)
            {
                PackId id = reader->read();
                IPacketReceive *packetReceive = newPacket(id);
                packetReceive->read(reader);
                
                std::lock_guard<std::mutex> lock(packetQueueLock);
                packetQueue.push(static_cast<IPacket*>(static_cast<APacket*>(packetReceive)));
                DEBUG_MSG("[Client] Receive a packet: %d", (static_cast<IPacket*>(static_cast<APacket*>(packetReceive)))->id());
            }
        }

        // send
        {
            std::lock_guard<std::mutex> lock(waitToSendQueueLock);
            while (!waitToSendQueue.empty())
            {
                IPacket *packet = waitToSendQueue.front();
                waitToSendQueue.pop();
                DEBUG_MSG("[Client] Send a packet: %d", packet->id());
                packet->send(reader);
                reader->flush();    
                delete packet;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

}



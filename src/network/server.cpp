#include "server.h"
#include "packet/ipacket.h"
#include "socket/ioreader.h"
#include <chrono>
#include <mutex>
#include <qabstractsocket.h>
#include <qtcpserver.h>
#include <set>
#include <thread>
#include "../util/easygetset.h"
#include "../util/debug.h"

namespace Network {

SimpleGameServer::SimpleGameServer()
{

}

SimpleGameServer::~SimpleGameServer() 
{
    stop();
    FOREACH_ITERATOR(connectedPlayers, reader, 
    {
        IOReader *reader = (*begin);
        delete reader;
    })

    while (!packetQueue.empty())
    {
        delete packetQueue.front();
        packetQueue.pop();
    }

    while (!waitToSendQueue.empty())
    {
        PacketPack pack = waitToSendQueue.front();
        waitToSendQueue.pop();
        delete pack.packet;
    }

    socketServer->close();
    delete socketServer;
}

void SimpleGameServer::start(int port) 
{
    socketServer = new QTcpServer();
    socketServer->listen(QHostAddress::Any, port);
    int waitTimes = 3 * 1000 / 5;
    while (!socketServer->isListening() && waitTimes > 0)
    {
        --waitTimes;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    if (waitTimes == 0)
    {
        DEBUG_ERROR("[SERVER] Failed to start server.");
        return;
    }
    DEBUG_MSG("[SERVER] Server listening %d.", port);
    run();
}

void SimpleGameServer::stop() 
{
    socketServer->close();
}

void SimpleGameServer::send(IPacket* packet) 
{
    std::lock_guard<std::mutex> lock(waitToSendQueueLock);
    waitToSendQueue.push({packet});
}

void SimpleGameServer::send(std::string playerName, IPacket* packet) 
{
    std::lock_guard<std::mutex> lock(waitToSendQueueLock);
    waitToSendQueue.push({packet, true, playerName});
}

IPacket* SimpleGameServer::receive() 
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

void SimpleGameServer::run()
{
    while (socketServer->isListening())
    {
        // connect
        if (socketServer->waitForNewConnection(5))
        {
            DEBUG_INFO("[Server] Incoming a new connection");
            std::lock_guard<std::mutex> lock(connectionLock);
            QTcpSocket *socket = socketServer->nextPendingConnection();
            DEBUG_MSG("[Server] Incoming a new connection: %s", socket->peerAddress().toString().toStdString().c_str());
            connectedPlayers.emplace_back(new IOReader(socket));
            std::unique_lock<std::mutex>();
        }

        // receive
        {
            std::lock_guard<std::mutex> lock(connectionLock);
            std::map<IOReader*, std::list<IOReader*>::iterator> waitToDelete;
            FOREACH_ITERATOR(connectedPlayers, reader, 
            {
                reader = *begin;
                if (reader->socket()->waitForDisconnected(0))
                {
                    DEBUG_INFO("There have a closed reader.");
                    waitToDelete[reader] = begin;
                    // todo delete
                } else if (reader->bytesAvailable() > 0)
                {
                    PackId id = reader->read();
                    IPacketReceive* packetReceive = Network::newPacket(id);
                    packetReceive->read(reader);

                    std::lock_guard<std::mutex> lock(packetQueueLock);
                    IPacket *packet =  static_cast<IPacket*>(static_cast<APacket*>(packetReceive));
                    packetQueue.push(packet);
                    if (packet->id() == 1)
                    {
                        std::string name = ((PacketPlayerJoin *) packet)->playerName;
                        playerNameConnectionMap[name] = reader;
                    }
                    // DEBUG_MSG("[Server] Receive a packet: %d", packet->id());
                }
            })

            if (!waitToDelete.empty())
            {
                std::list<std::string> waitToDeletePlayers;
                FOREACH_ITERATOR(playerNameConnectionMap, entry, {
                    if (waitToDelete.count(entry.second))
                    {
                        DEBUG_MSG("Player %s quit game.", entry.first.c_str());
                        waitToDeletePlayers.push_back(entry.first);
                        connectedPlayers.erase(waitToDelete[entry.second]);
                        waitToDelete.erase(entry.second);
                        entry.second->socket()->close();
                        delete entry.second;
                        DEBUG_MSG("Player %s quited game.", entry.first.c_str());
                    }
                })

                FOREACH_ITERATOR(waitToDeletePlayers, name, {
                    playerNameConnectionMap.erase(name);
                    // auto quit player
                    PacketPlayerQuit *packet = new PacketPlayerQuit();
                    packet->playerName = name;
                    PacketPlayerQuit *packetReceived = new PacketPlayerQuit();
                    packetReceived->playerName = name;
                    send(packet);
                    std::lock_guard<std::mutex> lock(packetQueueLock);
                    packetQueue.push(packetReceived);
                });

                FOREACH_ITERATOR(waitToDelete, entry, {
                    DEBUG_INFO("Start to delete closed client.");
                    connectedPlayers.erase(entry.second);
                    delete entry.first;
                })
            }

        }

        // send
        {
            std::lock_guard<std::mutex> lock(waitToSendQueueLock);
            if (!waitToSendQueue.empty())
            {
                PacketPack pack = waitToSendQueue.front();
                waitToSendQueue.pop();

                if (pack.sendSomeone)
                {
                    std::lock_guard<std::mutex> lock(connectionLock);
                    IOReader *reader = playerNameConnectionMap[pack.playerName];
                    if (reader != nullptr)
                    {
                        // DEBUG_MSG("[Server] Send a packet %d to %s", pack.packet->id(), pack.playerName.c_str());
                        pack.packet->send(reader);
                        reader->flush();
                    }
                }
                else 
                {
                    std::lock_guard<std::mutex> lock(connectionLock);
                    FOREACH_ITERATOR(connectedPlayers, reader, {
                        if (reader != nullptr)
                        {
                            // DEBUG_MSG("[Server] Broadcast a packet %d.", pack.packet->id());
                            pack.packet->send(reader);
                            reader->flush();
                        }
                    })
                }
                delete pack.packet;
            }
        }
        // std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
}
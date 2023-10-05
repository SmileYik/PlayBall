#include "gamewindow.h"

#include <QGridLayout>
#include <cstdlib>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <QPushButton>
#include "network/server.h"
#include "simplegame.h"
#include "entity/entityfood.h"
#include "shape/foodshape.h"


GameWindow::GameWindow()
{
    srand((unsigned int) time(NULL));
    hostEdit = new QTextEdit(this);
    portEdit = new QTextEdit(this);
    playerNameEdit = new QTextEdit(this);
    QPushButton *startServer = new QPushButton(this);
    startServer->setText("Create A Room");
    QPushButton *joinServer = new QPushButton(this);
    joinServer->setText("Join A Game"); 

    hostEdit->setText("127.0.0.1");
    hostEdit->setMaximumHeight(40);
    portEdit->setText("16650");
    portEdit->setMaximumHeight(40);
    playerNameEdit->setText(QString("Player%0").arg(rand() % 1000));
    playerNameEdit->setMaximumHeight(40);

    openGL = new GLWidget(this);
    
    QGridLayout *layout = new QGridLayout;
    
    layout->addWidget(playerNameEdit, 0, 0);
    layout->addWidget(hostEdit, 1, 0);
    layout->addWidget(portEdit, 2, 0);
    QGridLayout *layout2 = new QGridLayout;
    layout2->addWidget(startServer, 3, 0);
    layout2->addWidget(joinServer, 3, 1);
    layout->addLayout(layout2, 3, 0);
    layout->addWidget(openGL, 4, 0);
    setLayout(layout);

    connect(startServer, &QPushButton::clicked, this, [=] {
        serverThread = new std::thread([=]() {
            Network::IServer *server = new Network::SimpleGameServer();
            std::thread a([=](){
                server->start(portEdit->toPlainText().toInt());
            });
            Game::IGame *game = new Game::SimpleGame(server);
            uint32 times = 0;
            uint64 last = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            std::map<int, int> movedPlayer;
            while (true)
            {
                Network::IPacket *packet = nullptr;
                while ((packet = server->receive()) != nullptr)
                {
                    if (packet->id() == 2)
                    {
                        movedPlayer[((Network::AEntityPacket*) packet)->entityId] = 3;
                    } else if (packet->id() == 1)
                    {
                        std::string name = ((Network::PacketPlayerJoin *) packet)->playerName;
                        auto players = game->players();
                        FOREACH_ITERATOR(players, player, {
                            server->send(name, new Network::PacketPlayerJoin(
                                player->getEntityId(), player->getName(), player->getShape()->getColor()
                            ));
                            auto info = new Network::PacketPlayerInfo();
                            info->location = Point(player->getLocation());
                            info->entityId = player->getEntityId();
                            info->radius = player->getRadius();
                            // info->xAxis = player->getXAxis();
                            // info->yAxis = player->getYAxis();
                            info->xSpeed = player->getXSpeed();
                            info->ySpeed = player->getYSpeed();
                            info->currentGrowth = player->getCurrentGrowth();
                            server->send(info);
                        })
                        auto entities = game->entities();
                        FOREACH_ITERATOR(entities, e, {
                            if (e.second->getEntityType() == 2)
                            {
                                EntityFood *entity = (EntityFood *) e.second;
                                Network::PacketFoodSpawn *packet = new Network::PacketFoodSpawn();
                                packet->food = entity->getFood();
                                packet->color = entity->getShape()->getColor();
                                packet->location = entity->getLocation();
                                packet->rotate = ((AFoodShape *) entity->getShape())->getRotate();
                                packet->size = entity->getWidth();
                                packet->entityId = entity->getEntityId();
                                server->send(packet);
                            }
                        })
                    }
                    game->handlePacket(packet);
                }

                uint64 current = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                game->update((double)(current - last) / 1000.0);
                last = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

                if (true)
                {
                    FOREACH_ITERATOR(movedPlayer, entry, {
                        if (entry.second > 0) {
                            Player *p = (Player *) game->getEntityById(entry.first);
                            auto packet = new Network::PacketPlayerLocation();
                            packet->location = Point(p->getLocation());
                            packet->entityId = p->getEntityId();
                            // packet->radius = p->getRadius();
                            // packet->xAxis = p->getXAxis();
                            // packet->yAxis = p->getYAxis();
                            packet->xSpeed = p->getXSpeed();
                            packet->ySpeed = p->getYSpeed();
                            // packet->currentGrowth = p->getCurrentGrowth();
                            server->send(packet);
                            movedPlayer[p->getEntityId()]--;
                        }
                    })
                    

                    // auto list = game->players();
                    // FOREACH_ITERATOR(list, p, {
                    //     auto packet = new Network::PacketPlayerLocation();
                    //     packet->location = Point(p->getLocation());
                    //     packet->entityId = p->getEntityId();
                    //     // packet->radius = p->getRadius();
                    //     // packet->xAxis = p->getXAxis();
                    //     // packet->yAxis = p->getYAxis();
                    //     packet->xSpeed = p->getXSpeed();
                    //     packet->ySpeed = p->getYSpeed();
                    //     // packet->currentGrowth = p->getCurrentGrowth();
                    //     server->send(packet);
                    // })
                }

                ++times;

                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        });
    });

    connect(joinServer, &QPushButton::clicked, this, [=] {
        openGL->start(playerNameEdit->toPlainText().toStdString(), {
            (unsigned char) (rand() % 255),
            (unsigned char) (rand() % 255),
            (unsigned char) (rand() % 255)
        }, hostEdit->toPlainText().toStdString(), portEdit->toPlainText().toInt());
    });
}
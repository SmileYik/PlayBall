#include "glwidget.h"

#include <QPainter>
#include <QTimer>
#include <cstdlib>
#include <qbrush.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qsurfaceformat.h>
#include <qtimer.h>

#include "../simplegame.h"


GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{

    setFixedSize(500, 500);
    setAutoFillBackground(true);
    QSurfaceFormat surfaceformat;
    surfaceformat.setSamples(8);
    setFormat(surfaceformat);
}

void GLWidget::start(std::string _playerName, RgbColor _color, std::string _host, int _port)
{
    playerName = _playerName;
    game = new Game::SimpleGame();
    client = new Network::SimpleGameClient();
    a = new std::thread([=](){
        client->start(_host, _port);
    });
    

    client->send(new Network::PacketPlayerJoin(
        0, playerName, _color
    ));

    

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=] () {
        
        Network::IPacket *packet = nullptr;
        while ((packet = client->receive()) != nullptr)
        {
            game->handlePacket(packet);
        }

        Player *player = game->getPlayer(playerName);
        if (player != nullptr)
        {
            bool send = false;
            double x = 0, y = 0;
            for(auto begin = pressedKeys.cbegin(), end = pressedKeys.end(); begin != end; ++begin)
            {
                switch (*begin)
                {
                    case Qt::Key_W:
                        y = -1;
                        send = true;
                        player->moveVertical(-1);
                        break;
                    case Qt::Key_S:
                        y = 1;
                        send = true;
                        player->moveVertical(1);
                        break;
                    case Qt::Key_A:
                        x = -1;
                        send = true;
                        player->moveHorizon(-1);
                        break;
                    case Qt::Key_D:
                        x = 1;
                        send = true;
                        player->moveHorizon(1);
                        break;
                }
            }
            if (send)
                client->send(new Network::PacketPlayerMove(
                    player->getEntityId(), player->getXAxis(), player->getYAxis()
                ));
        }

        game->update((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - times) / 1000.0);
        times = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        
        ++times;
        update();
    });
    timer->start(20);
    // connect(this, &GLWidget::keyReleaseEvent, this, [=](QKeyEvent *event) {
    //     const Point p = player->getLocation();
    //     player->setLocation(Point({p.x + 1, p.y}));
    // });
}

void GLWidget::paintEvent(QPaintEvent *event)
{
    if (game == nullptr)
    {
        return;
    }
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    // painter.fillRect(event->rect(), QBrush(QColor(64, 32, 64)));
    QString keys = "";
    for(auto begin = pressedKeys.cbegin(), end = pressedKeys.end(); begin != end; ++begin)
        {
            switch (*begin)
            {
                case Qt::Key_W:
                    keys.append(", W");
                    break;
                case Qt::Key_S:
                    keys.append(", S");
                    break;
                case Qt::Key_A:
                    keys.append(", A");
                    break;
                case Qt::Key_D:
                    keys.append(", D");
                    break;
            }
        }
    QFont f = font();
    f.setPointSize(16);
    painter.setFont(f);
    
    painter.fillRect(event->rect(), QBrush({240, 240, 240}));

    game->render(&painter);
    
    painter.drawText(0, 30, "Pressed Key: " + keys.right(keys.size() - 1));

    painter.end();
}

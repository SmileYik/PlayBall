#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include "../interface/igame.h"
#include "../network/client.h"
#include <QKeyEvent>
#include <cstdlib>
#include <set>
#include <string>
#include <thread>

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent);

    void start(std::string _playerName, RgbColor _color, std::string _host, int _port);

    void keyPressEvent2(QKeyEvent *event) {
        pressedKeys.insert(event->key());
    };

    void keyReleaseEvent2(QKeyEvent *event) {
        pressedKeys.erase(event->key());
    };
protected:
    void paintEvent(QPaintEvent *event) override;
    uint64 times = 0;
    Game::IGame *game = nullptr;
    std::thread *a = nullptr;
    Network::IClient *client;
    std::set<int> pressedKeys;
    std::string playerName = QString("player %0").arg(rand()).toStdString();
};

#endif

#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include "widget/glwidget.h"
#include <QTextEdit>
#include <qtextedit.h>
#include <thread>

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    GLWidget *openGL = nullptr;
    QTextEdit *hostEdit = nullptr;
    QTextEdit *portEdit = nullptr;
    QTextEdit *playerNameEdit = nullptr;
    std::thread *serverThread = nullptr;
    
    GameWindow();

    void keyPressEvent(QKeyEvent *event) override {
        openGL->keyPressEvent2(event);
    };

    void keyReleaseEvent(QKeyEvent *event) override {
        openGL->keyReleaseEvent2(event);
    }
};

#endif // GAMEWINDOW_H
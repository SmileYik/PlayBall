#include "gamewindow.h"
#include <QApplication>

#ifdef PLATFORMS_WINDOWS
# include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin)
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GameWindow window;
    window.show();
    return a.exec();
}

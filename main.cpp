#include "mainmenugreeting.h"

#include <aetl.h>

#include <QApplication>

int main(int argc, char *argv[])
{
    LogFile::BeginLogging();

    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("CEECAM");
    QCoreApplication::setApplicationName("CEECAM Toolkit");
    QCoreApplication::setApplicationVersion("test");

    MainMenuGreeting w;
    w.show();
    return a.exec();
}

#include <QCoreApplication>
#include <QtNetwork>
#include "github_updater.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    GitHub_Updater updater("App name");

    if(!updater.startProcessing("latest release api")){
        updater.printError("Couldn't update " + updater.appName());
    }

    return 0;
}

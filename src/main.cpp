#include <QCoreApplication>
#include <QtNetwork>
#include "github_updater.h"
#include <iostream>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    GitHub_Updater updater("App Name");

    if(!updater.startProcessing("link to download data, for example github api latest release")){
        updater.printError("Couldn't update " + updater.appName());
    }

    return 0;
}

#include <QCoreApplication>
#include <QtNetwork>
#include "github_updater.h"
#include <iostream>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    GitHub_Updater updater("Your app name");

    if(!updater.startProcessing("Your link to download data")){
        updater.printError("Couldn't update " + updater.appName());
    }

    return 0;
}

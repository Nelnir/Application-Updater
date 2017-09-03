#include <QCoreApplication>
#include <QtNetwork>
#include "github_updater.h"
#include <iostream>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    GitHub_Updater updater("App name");

    if(!updater.startProcessing("Latest release api")){
        updater.printError("Couldn't update " + updater.appName());
    } else if(updater.UpdatingItself()){
        updater.UpdateItself();
    }

    return 0;
}

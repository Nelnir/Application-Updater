#include <QCoreApplication>
#include <QtNetwork>
#include <iostream>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    QNetworkAccessManager manager;


    std::cerr << "test\n";
    return 0;
}

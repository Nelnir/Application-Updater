#include "github_updater.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QDir>
#include <iostream>
#include "utils.h"

GitHub_Updater::GitHub_Updater(const QString& l_name) : Updater(l_name)
{
    addDefaultNamingConventions();

}

Variant GitHub_Updater::processData(std::unique_ptr<QNetworkReply> & reply)
{
    QJsonObject object = QJsonDocument::fromJson(static_cast<QString>(reply->readAll()).toUtf8()).object();

    QString remoteVersion = object["tag_name"].toString();
    setRemoteVersion(remoteVersion);
    QString versionName = object["name"].toString();
    QString publishedAt = object["published_at"].toString();
    publishedAt.remove(publishedAt.indexOf('T'), publishedAt.length() - publishedAt.indexOf('T'));
    QString bodyText = object["body"].toString();

    processBodyText(bodyText);

    if(remoteVersion == version()){
        return Variant::Same_Version;
    }

    std::cout << "Found a new version: ";   printGreenText(remoteVersion);
    std::cout << "Published at: ";          printGreenText(publishedAt);
    std::cout << "Version name: ";          printGreenText(versionName);
    std::cout << "Info: ";                  printGreenText(bodyText);


    QJsonArray assets = object["assets"].toArray();

    for(int i = 0;i<assets.size();++i)
    {
        QJsonObject asset = assets[i].toObject();
        QString assetName = asset["name"].toString();

        if(processAsset(assetName))
        {
            QStringList files;
            QString path;

            auto & itr = m_operations.find(assetName);
            /// if contains, need to check if this file exist in "path"(second value of pair) folder
            /// otherwise in main folder
            if(itr != m_operations.end())
            {
                if(itr->first == "->"){
                    files = QDir(QString::fromStdString(Utils::getWorkingDirectory()) + itr->second).entryList(QDir::NoDotAndDotDot | QDir::Files);
                    QString tmp = QString::fromStdString(Utils::getWorkingDirectory()) + itr->second;
                    path = tmp + "\\" + assetName;

                    if(!QDir(tmp).exists()) /// if path doesnt exist
                        QDir().mkpath(tmp); /// create it
                }
            }
            else
            {
                files = QDir().entryList(QDir::NoDotAndDotDot | QDir::Files);
                path = QString::fromStdString(Utils::getWorkingDirectory()) + assetName;
            }

            if(files.contains(assetName)) /// remove this file
                removeFile(path); /// delete original file, and replace it with new one

            std::cout << "Downloading file: ";  printYellowText(assetName);

            reply.reset(m_manager.get(QNetworkRequest(QUrl(asset["browser_download_url"].toString()))));
            m_loop.exec(QEventLoop::ExcludeUserInputEvents);

            if(reply->error() != QNetworkReply::NoError) return Variant::Error;

            reply.reset(m_manager.get(QNetworkRequest(reply->header(QNetworkRequest::LocationHeader).toString()))); /// binary file
            m_loop.exec(QEventLoop::ExcludeUserInputEvents);

            if(reply->error() != QNetworkReply::NoError) return Variant::Error;

            QFile file;
            file.setFileName(path);
            file.open(QIODevice::WriteOnly);
            file.write(reply->readAll());
            file.flush();
            file.close();
        }
    }

    return Variant::Success;
}

void GitHub_Updater::processBodyText(QString &l_text)
{
    QStringList list = l_text.split("///");
    l_text = list.first(); /// visible to user

    QString hidden; /// hidden commands like. move file xxx to directory xxxx
    if(list.size() == 2)
        hidden = list.last();

    QTextStream stream(&hidden);

    while(!stream.atEnd())
    {
        QString line = stream.readLine();
        QTextStream lineStream(&line);
        while(!lineStream.atEnd())
        {
            QString file, operation, path;
            lineStream >> file >> operation >> path;

            if(path.isEmpty() || !lineStream.atEnd() || file[0] == '/') // if more than 3 strings, ignore whole line
                continue;

            m_operations.insert(file, qMakePair(operation, path));
        }
    }
}

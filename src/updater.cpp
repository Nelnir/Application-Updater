#include "updater.h"
#include <QCoreApplication>
#include <QFile>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <iostream>
#include <memory>
Updater::Updater(const QString & l_appName) : m_version("???"), m_appName(l_appName)
{
    QStringList arguments = QCoreApplication::arguments();
    if(arguments.size() == 2)
        m_version = arguments.last();

    PrintStartMessage();

    QObject::connect(&m_manager, &QNetworkAccessManager::finished, &m_loop, &QEventLoop::quit);
}

Updater::~Updater() {}

bool Updater::removeFile(const QString &l_name)
{
    QFile::rename(l_name, l_name + "delete");
    if(m_filesToDelete.contains(l_name + "delete")){
        return false;
    }
    m_filesToDelete.push_back(l_name + "delete");
    return true;
}

void Updater::setConsoleTextColor(const QString &l_color)
{
#ifdef WIN32
    if(l_color == "red")
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    else if(l_color == "green")
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    else if(l_color == "default")
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
    else if (l_color == "yellow")
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    else if (l_color == "blue")
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    else if (l_color == "white")
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
#endif // WIN32
}

bool Updater::addDefaultNamingConventions()
{
    if(!m_fileNaming.empty()){
        return false;
    }

    m_fileNaming.emplace(OperatingSystem::Win7_32, "WIN7_32");
    m_fileNaming.emplace(OperatingSystem::Win7_64, "WIN7_64");
    m_fileNaming.emplace(OperatingSystem::Win10_32, "WIN10_32");
    m_fileNaming.emplace(OperatingSystem::Win10_64, "WIN10_64");
    m_fileNaming.emplace(OperatingSystem::WinXP_32, "WINXP_32");
    m_fileNaming.emplace(OperatingSystem::WinXP_64, "WINXP_64");

    return true;
}

void Updater::deleteOldFiles()
{
    if(!m_filesToDelete.empty())
    {
        std::cout << "Deleting old files..." << std::endl;
        QFile file;
        while(!m_filesToDelete.empty()){
            file.setFileName(m_filesToDelete.first());
            if(!file.remove()){
                printError("Error when deleting file: " + file.fileName());
            }
            m_filesToDelete.removeFirst();
        }
    }
}

bool Updater::addNamingConvention(const OperatingSystem &l_sys, const QString &l_name)
{
    return m_fileNaming.emplace(l_sys, l_name).second;
}

void Updater::printBlueText(const QString &l_text)
{
    setConsoleTextColor("blue");
    std::cout << l_text.toStdString() << std::endl;
    setConsoleTextColor("default");
}

void Updater::printError(const QString &l_text)
{
    setConsoleTextColor("red");
    std::cerr << l_text.toStdString() << std::endl;
    setConsoleTextColor("default");
}

void Updater::printGreenText(const QString &l_text)
{
    setConsoleTextColor("green");
    std::cout << l_text.toStdString() << std::endl;
    setConsoleTextColor("default");
}

void Updater::printYellowText(const QString &l_text)
{
    setConsoleTextColor("yellow");
    std::cout << l_text.toStdString() << std::endl;
    setConsoleTextColor("default");
}

void Updater::printWhiteText(const QString &l_text)
{
    setConsoleTextColor("white");
    std::cout << l_text.toStdString() << std::endl;
    setConsoleTextColor("default");
}

void Updater::PrintStartMessage()
{
    setConsoleTextColor("blue");
    std::cout << m_appName.toStdString();
    setConsoleTextColor("default");
    std::cout << " version ";
    setConsoleTextColor("blue");
    std::cout << m_version.toStdString();
    setConsoleTextColor("default");
    std::cout << " updater" << std::endl;
}

bool Updater::startProcessing(const QString & l_link)
{
    if(m_version == "???"){
        printError("Unable to check " + m_appName + " version");
        return false;
    }

    std::cout << "Trying to download data..." << std::endl;

    std::unique_ptr<QNetworkReply> reply(m_manager.get(QNetworkRequest(QUrl(l_link))));
    m_loop.exec(QEventLoop::ExcludeUserInputEvents);

    if(reply->error() == QNetworkReply::NoError){
        Variant variant = processData(reply);
        if(variant == Variant::Success){
            deleteOldFiles();
            PrintSuccessMessage();
            return true;
        }
        else if(variant == Variant::Error){
            removeDownloadedData();
            printError("Error while processing data");
            return false;
        }
        else if(variant == Variant::Same_Version){
            PrintSameVersionMessage();
            return true;
        }
    }
    else{
        std::cerr << "Unable to download data from server" << std::endl;
        return false;
    }

    return false;
}

bool Updater::processAsset(QString &l_name)
{
    OperatingSystem os = operatingSystem();
    if(os == OperatingSystem::Unknown){
        printError("Unsupported operating system or cpu architecture");
        return false;
    }

    if(l_name.contains(m_fileNaming.find(os)->second)){
        l_name.remove(l_name.indexOf("__"), l_name.length() - l_name.indexOf("__") - (l_name.length() - l_name.lastIndexOf('.')));
        return true;
    }
    else{
        for(auto & itr : m_fileNaming)
            if(l_name.contains(itr.second)) /// if doesnt contains any naming_convinitions, return true, otherwise false
                return false;
    }
    return true;
}

OperatingSystem Updater::operatingSystem()
{
    QString architecture = QSysInfo::currentCpuArchitecture();
    if(architecture == "x86_64")
    {
        switch(QSysInfo::windowsVersion())
        {
        case QSysInfo::WV_XP:           return OperatingSystem::WinXP_64;
        case QSysInfo::WV_WINDOWS10:    return OperatingSystem::Win10_64;
        case QSysInfo::WV_WINDOWS7:     return OperatingSystem::Win7_64;
        }
    }
    else if(architecture == "x86")
    {
        switch(QSysInfo::windowsVersion())
        {
        case QSysInfo::WV_XP:           return OperatingSystem::WinXP_32;
        case QSysInfo::WV_WINDOWS10:    return OperatingSystem::Win10_32;
        case QSysInfo::WV_WINDOWS7:     return OperatingSystem::Win7_32;
        }
    }
    else{
        return OperatingSystem::Unknown;
    }
    return OperatingSystem::Unknown;
}

void Updater::removeDownloadedData()
{
    while(!m_filesToDelete.empty()){
        QString first = m_filesToDelete.first();
        QFileInfo info(first.remove(first.length() - 6, 6));
        if(info.exists()) // "delete", if exists, then we know that there is already a new file, so delete it
        {
            if(!QFile::remove(info.filePath())){
                printError("Error when deleting file: " + info.fileName());
            }
        }

        QFile::rename(first, info.filePath()); /// rename "old file"

        m_filesToDelete.removeFirst();
    }
}

void Updater::PrintSuccessMessage()
{
    setConsoleTextColor("white");
    std::cout << "Successfully updated ";
    setConsoleTextColor("blue");
    std::cout << m_appName.toStdString();
    setConsoleTextColor("white");
    std::cout << " to version ";
    printBlueText(m_remoteVersion);
}

void Updater::PrintSameVersionMessage()
{
    std::cout << "Your version(";
    setConsoleTextColor("blue");
    std::cout << version().toStdString();
    setConsoleTextColor("default");
    std::cout << ") is actual" << std::endl;
}

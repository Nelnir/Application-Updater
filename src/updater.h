#ifndef UPDATER_H
#define UPDATER_H

#include <QNetworkAccessManager>
#include <QEventLoop>
#include <unordered_map>
#include "utils.h"

#define WIN_32

#ifdef WIN_32
#include <Windows.h>
#endif // WIN_32

enum class OperatingSystem {Win10_64 = 1, Win10_32, Win7_64, Win7_32, WinXP_64, WinXP_32, Unknown};
enum class Variant {Success, Same_Version, Error};

using FileNames = std::unordered_map<OperatingSystem, QString>;

class Updater
{
protected:
    QNetworkAccessManager m_manager;
    QEventLoop m_loop;
private:
    QString m_version;
    QString m_remoteVersion;
    QString m_appName;

    QStringList m_filesToDelete;

    FileNames m_fileNaming;

#ifdef WIN_32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif // WIN_32
public:
    Updater(const QString & l_appName);
    ~Updater();
    bool startProcessing(const QString& l_link); /// returns true on successful update, otherwise false

    inline QString appName() { return m_appName; }
    inline QString version() { return m_version; }

    void setConsoleTextColor(const QString& l_color);
    void printError(const QString& l_text);
    void printBlueText(const QString& l_text);
    void printGreenText(const QString& l_text);
    void printYellowText(const QString& l_text);
    void printWhiteText(const QString& l_text);
private:
    void deleteOldFiles();
    void PrintStartMessage();
    void PrintSuccessMessage();
    void PrintSameVersionMessage();
    void removeDownloadedData(); /// when in middle of downloading we will experience problems with internet connection
    OperatingSystem operatingSystem();
protected:
    bool addNamingConvention(const OperatingSystem& l_sys, const QString& l_name);
    bool addDefaultNamingConventions();
    inline void setRemoteVersion(const QString& l_ver) { m_remoteVersion = l_ver; }

    virtual Variant processData(std::unique_ptr<QNetworkReply>& l_reply) = 0;

    bool removeFile(const QString & l_name);

    bool processAsset(QString& l_name); /// checks if given asset name is for this OS
};

#endif // UPDATER_H

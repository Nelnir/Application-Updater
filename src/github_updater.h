#ifndef GITHUB_UPDATER_H
#define GITHUB_UPDATER_H

#include "updater.h"
#include <QHash>
                           /// type  path
using Operation = QPair<QString, QString>;
                                  /// file     operation
using Operations = QHash<QString, Operation>;

class GitHub_Updater : public Updater
{
    Operations m_operations;
public:
    GitHub_Updater(const QString& l_name);
protected:
    bool processData(std::unique_ptr<QNetworkReply>& l_reply);
private:
    void processBodyText(QString& l_text);
};

#endif // GITHUB_UPDATER_H

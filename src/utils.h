#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "updater.h"

#ifdef XP_TARGET
#include <QCoreApplication>
#endif

#ifndef XP_TARGET
#include <filesystem>
#endif

namespace Utils {

inline std::string getWorkingDirectory(){
#ifdef XP_TARGET
    return QCoreApplication::applicationDirPath().toStdString() + '\\';
#endif

#ifndef XP_TARGET
   return std::experimental::filesystem::current_path().string() + '\\';
#endif
}

}

#endif // UTILS_H

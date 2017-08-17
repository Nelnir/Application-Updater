#ifndef UTILS_H
#define UTILS_H


#include <string>
#include <experimental/filesystem>

#pragma once
#define WIN_32

namespace Utils {

#ifdef WIN_32

inline std::string getWorkingDirectory(){
    return std::experimental::filesystem::current_path().string() + '\\';
}

#endif // WIN_32

}

#endif // UTILS_H

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <experimental/filesystem>


namespace Utils {

inline std::string getWorkingDirectory(){
    return std::experimental::filesystem::current_path().string() + '\\';

}

}

#endif // UTILS_H

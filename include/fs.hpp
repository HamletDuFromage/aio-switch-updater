#pragma once

#include <switch.h>
#include <set>
#include <json.hpp>

namespace fs
{

    int removeDir(const char* path);
    nlohmann::json parseJsonFile(const char* path);
    void writeJsonToFile(nlohmann::json &data, const char* path);
    bool copyFile(const char *to, const char *from);
    std::string copyFiles(const char* path);
    void createTree(std::string path);
    std::set<std::string> readLineByLine(const char * path);

}

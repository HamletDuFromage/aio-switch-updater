#pragma once

#include "constants.hpp"
#include <switch.h>
#include <set>
#include <string>
#include <vector>

namespace extract {

    static constexpr u32 MaxTitleCount = 64000;

    typedef struct Title {
        std::string id;
        std::string name;
        bool operator ==(const Title&x) const {
            return id == x.id;
        }

        bool operator <(const Title&x) const {
            return id < x.id;
        }
    } Title;

    void extract(const char* filename, const char* workingPath = ROOT_PATH, int overwriteInis = 1);
    void extract(const char * filename, const char* workingPath, const char* toExclude);
    std::vector<std::string> getInstalledTitlesNs();
    std::vector<std::string> excludeTitles(const char* path, std::vector<std::string> listedTitles);
    void writeTitlesToFile(std::set<std::string> titles, const char* path);
    void extractCheats(const char * zipPath, std::vector<std::string> titles, CFW cfw, bool credits = false);
    void extractAllCheats(const char * zipPath, CFW cfw);
    void removeCheats(CFW cfw);
    bool isBID(std::string bid);

}
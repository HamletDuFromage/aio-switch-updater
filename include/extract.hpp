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

    void extract(const std::string& filename, const std::string& workingPath = ROOT_PATH, int overwriteInis = 1);
    void extract(const std::string&  filename, const std::string& workingPath, const std::string& toExclude);
    std::vector<std::string> getInstalledTitlesNs();
    std::vector<std::string> excludeTitles(const std::string& path, const std::vector<std::string>& listedTitles);
    void writeTitlesToFile(std::set<std::string> titles, const std::string& path);
    void extractCheats(const std::string&  zipPath, std::vector<std::string> titles, CFW cfw, bool credits = false);
    void extractAllCheats(const std::string&  zipPath, CFW cfw);
    void removeCheats(CFW cfw);
    bool isBID(std::string bid);

}
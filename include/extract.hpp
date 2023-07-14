#pragma once

#include <switch.h>

#include <functional>
#include <set>
#include <string>
#include <vector>

#include "constants.hpp"

namespace extract {
    static constexpr u32 MaxTitleCount = 64000;
    typedef struct Title
    {
        std::string id;
        std::string name;
        bool operator==(const Title& x) const
        {
            return id == x.id;
        }

        bool operator<(const Title& x) const
        {
            return id < x.id;
        }
    } Title;

    void extract(
        const std::string& filename, const std::string& workingPath = ROOT_PATH, bool preserveInis = false, std::function<void()> func = []() { return; });
    std::vector<std::string> getInstalledTitlesNs();
    std::vector<std::string> excludeTitles(const std::string& path, const std::vector<std::string>& listedTitles);
    void writeTitlesToFile(const std::set<std::string>& titles, const std::string& path);
    void extractCheats(const std::string& archivePath, const std::vector<std::string>& titles, CFW cfw, const std::string& version, bool extractAll = false);
    void extractAllCheats(const std::string& archivePath, CFW cfw, const std::string& version);
    void removeCheats();
    void removeOrphanedCheats();
    bool removeCheatsDirectory(const std::string& entry);
    bool isBID(const std::string& bid);
}  // namespace extract
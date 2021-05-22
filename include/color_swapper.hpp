#pragma once

#include <json.hpp>

namespace JC {

    int setColor(std::vector<int> colors);
    int backupToJSON(nlohmann::json &profiles, const std::string& path);
    std::vector<std::pair<std::string, std::vector<int>>> getProfiles(const std::string& path);
    void changeJCColor(std::vector<int> values);
    nlohmann::json backupProfile();
    void backupJCColor(const std::string& path);

}

namespace PC {

    int setColor(std::vector<int> colors);
    int backupToJSON(nlohmann::json &profiles,const std::string& path);
    std::vector<std::pair<std::string, std::vector<int>>> getProfiles(const std::string& path);
    void changePCColor(std::vector<int> values);
    nlohmann::json backupProfile();
    void backupPCColor(const std::string& path);

}

namespace ColorSwapper {

    int hexToBGR(std::string hex);
    std::string BGRToHex(int v);
    bool isHexaAnd3Bytes(std::string str);

}
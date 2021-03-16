#pragma once

#include "json.hpp"

namespace JC {

    int setColor(std::vector<int> colors);
    int backupToJSON(nlohmann::json &profiles, const char* path);
    std::tuple<std::vector<std::string>, std::vector<std::vector<int>>> getProfiles(const char* path);
    void changeJCColor(std::vector<int> values);
    nlohmann::json backupProfile();
    void backupJCColor(const char* path);

}

namespace PC {

    int setColor(std::vector<int> colors);
    int backupToJSON(nlohmann::json &profiles, const char* path);
    std::tuple<std::vector<std::string>, std::vector<std::vector<int>>> getProfiles(const char* path);
    void changePCColor(std::vector<int> values);
    nlohmann::json backupProfile();
    void backupPCColor(const char* path);

}

namespace ColorSwapper {

    int hexToBGR(std::string hex);
    std::string BGRToHex(int v);
    bool isHexaAnd3Bytes(std::string str);

}
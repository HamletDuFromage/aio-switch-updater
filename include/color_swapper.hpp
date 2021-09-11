#pragma once

#include <json.hpp>

namespace JC {

    int setColor(const std::vector<int>& colors);
    int backupToJSON(nlohmann::json& profiles, const std::string& path);
    std::vector<std::pair<std::string, std::vector<int>>> getProfiles(const std::string& path);
    void changeJCColor(const std::vector<int>& values);
    nlohmann::json backupProfile();
    void backupJCColor(const std::string& path);

}  // namespace JC

namespace PC {

    int setColor(const std::vector<int>& colors);
    int backupToJSON(nlohmann::json& profiles, const std::string& path);
    std::vector<std::pair<std::string, std::vector<int>>> getProfiles(const std::string& path);
    void changePCColor(const std::vector<int>& values);
    nlohmann::json backupProfile();
    void backupPCColor(const std::string& path);

}  // namespace PC

namespace ColorSwapper {

    int hexToBGR(const std::string& hex);
    std::string BGRToHex(int v);
    bool isHexaAnd3Bytes(const std::string& str);

}  // namespace ColorSwapper
#pragma once

#include <switch.h>

#include <borealis.hpp>
#include <json.hpp>
#include <regex>
#include <set>

#include "constants.hpp"

namespace util {

    typedef char NsApplicationName[0x201];
    typedef uint8_t NsApplicationIcon[0x20000];

    typedef struct
    {
        uint64_t tid;
        NsApplicationName name;
        NsApplicationIcon icon;
        brls::ListItem* listItem;
    } app;

    void clearConsole();
    bool isArchive(const std::string& path);
    void downloadArchive(const std::string& url, contentType type);
    void downloadArchive(const std::string& url, contentType type, long& status_code);
    void extractArchive(contentType type, const std::string& version = "");
    std::string formatListItemTitle(const std::string& str, size_t maxScore = 140);
    std::string formatApplicationId(u64 ApplicationId);
    std::vector<std::string> fetchPayloads();
    void shutDown(bool reboot = false);
    void rebootToPayload(const std::string& path);
    void showDialogBoxInfo(const std::string& text);
    int showDialogBoxBlocking(const std::string& text, const std::string& opt);
    int showDialogBoxBlocking(const std::string& text, const std::string& opt1, const std::string& opt2);
    std::string getLatestTag(const std::string& url);
    std::string downloadFileToString(const std::string& url);
    std::string getCheatsVersion();
    void saveToFile(const std::string& text, const std::string& path);
    std::string readFile(const std::string& path);
    std::string getAppPath();
    void restartApp();
    bool isErista();
    void removeSysmodulesFlags(const std::string& directory);
    std::string lowerCase(const std::string& str);
    std::string upperCase(const std::string& str);
    std::string getErrorMessage(long status_code);
    bool isApplet();
    std::string getContentsPath();
    bool getBoolValue(const nlohmann::ordered_json& jsonFile, const std::string& key);
    const nlohmann::ordered_json getValueFromKey(const nlohmann::ordered_json& jsonFile, const std::string& key);
}  // namespace util
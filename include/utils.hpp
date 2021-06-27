#pragma once

#include "constants.hpp"
#include <regex>
#include <set>
#include <switch.h>
#include <borealis.hpp>
#include <json.hpp>

namespace util {

    typedef char NsApplicationName[0x201];
    typedef uint8_t NsApplicationIcon[0x20000];

    typedef struct {
        uint64_t tid;
        NsApplicationName name;
        NsApplicationIcon icon;
        brls::ListItem* listItem;
    } app;

    void clearConsole();
    bool isArchive(const std::string&  path);
    void downloadArchive(std::string url, archiveType type);
    void downloadArchive(std::string url, archiveType type, long& status_code);
    void extractArchive(archiveType type, std::string tag = "0");
    std::string formatListItemTitle(const std::string &str, size_t maxScore = 140);
    std::string formatApplicationId(u64 ApplicationId);
    std::vector<std::string> fetchPayloads();
    void shutDown(bool reboot = false);
    void rebootToPayload(const std::string& path);
    int showDialogBox(std::string text, std::string opt);
    int showDialogBox(std::string text, std::string opt1, std::string opt2);
    std::string getLatestTag(const std::string& url);
    std::string downloadFileToString(const std::string& url);
    void saveVersion(std::string version, const std::string& path);
    std::string readVersion(const std::string& path);
    bool isErista();
    void removeSysmodulesFlags(const std::string&  directory);
    std::string lowerCase(const std::string& str);
    std::string getErrorMessage(long status_code);
    bool isApplet();
}
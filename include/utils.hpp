#pragma once

#include <regex>
#include <set>
#include <switch.h>
#include <borealis.hpp>
#include <json.hpp>
#include "constants.hpp"

namespace util {

typedef char NsApplicationName[0x201];
typedef uint8_t NsApplicationIcon[0x20000];

struct app
{
    uint64_t tid;
    NsApplicationName name;
    NsApplicationIcon icon;
    brls::ListItem* listItem;
};

void createTree(std::string path);
void clearConsole();
bool isArchive(const char * path);
void downloadArchive(std::string url, archiveType type);
void extractArchive(archiveType type, std::string tag = "0");
std::string formatListItemTitle(const std::string &str, size_t maxScore = 140);
std::string formatApplicationId(u64 ApplicationId);
std::set<std::string> readLineByLine(const char * path);
std::vector<std::string> fetchPayloads();
void shut_down(bool reboot = false);
int showDialogBox(std::string text, std::string opt);
int showDialogBox(std::string text, std::string opt1, std::string opt2);
std::string getLatestTag(const char *url);
Result CopyFile(const char src_path[FS_MAX_PATH], const char dest_path[FS_MAX_PATH]);
void saveVersion(std::string version, const char* path);
std::string readVersion(const char* path);
void cp(const char *to, const char *from);
std::string copyFiles(const char* path);
int removeDir(const char* path);
bool isErista();
void removeSysmodulesFlags(const char * directory);
nlohmann::json parseJsonFile(const char* path);
void writeJsonToFile(nlohmann::json &profiles, const char* path);

}
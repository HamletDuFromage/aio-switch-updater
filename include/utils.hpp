#pragma once

#include <regex>
#include <set>
#include <switch.h>
#include "constants.hpp"
#include <borealis.hpp>


typedef char NsApplicationName[0x201];
typedef uint8_t NsApplicationIcon[0x20000];

struct app
{
    uint64_t tid;
    NsApplicationName name;
    NsApplicationIcon icon;
    brls::ListItem* listItem;
};

std::vector<std::string> htmlProcess(std::string s, std::regex rgx);
void createTree(std::string path);
void clearConsole();
bool isArchive(const char * path);
void downloadArchive(std::string url, archiveType type);
void extractArchive(archiveType type, std::string tag = "0");
void progressTest(std::string url, archiveType type);
std::string formatListItemTitle(const std::string str, size_t maxScore = 140);
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
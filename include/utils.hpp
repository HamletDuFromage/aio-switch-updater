#pragma once

#include <regex>
#include <iostream>
#include <set>
#include "download.hpp"
#include "extract.hpp"
#include "constants.hpp"
#include "progress_event.hpp"
#include "json.hpp"
#include "main_frame.hpp"

CFW getCFW();
bool isServiceRunning(const char *serviceName);
std::vector<std::string> htmlProcess(std::string s, std::regex rgx);
void createTree(std::string path);
void clearConsole();
bool isArchive(const char * path);
void downloadArchive(std::string url, archiveType type);
void extractArchive(archiveType type);
void progressTest(std::string url, archiveType type);
std::string formatListItemTitle(const std::string str, size_t maxScore = 140);
std::string formatApplicationId(u64 ApplicationId);
std::set<std::string> readLineByLine(const char * path);
std::vector<std::string> fetchPayloads();
void shut_down(bool reboot = false);
int showDialogBox(std::string text, std::string opt);
int showDialogBox(std::string text, std::string opt1, std::string opt2);
std::string getLatestTag(const char *url);
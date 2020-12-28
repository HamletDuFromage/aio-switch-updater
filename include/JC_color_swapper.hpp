#pragma once

#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <tuple>

#include <switch.h>
#include "constants.hpp"
#include "progress_event.hpp"
#include "utils.hpp"
#include "json.hpp"

int hexToBGR(std::string hex);
std::string BGRToHex(int v);
bool isHexaAnd3Bytes(std::string str);
int setColor(std::vector<int> colors);
int backupToJSON(nlohmann::json &profiles, const char* path);
void writeJSONToFile(nlohmann::json &profiles, const char* path);
std::tuple<std::vector<std::string>, std::vector<std::vector<int>>> getProfiles(const char* path);
void changeJCColor(std::vector<int> values);
nlohmann::json backupProfile();
void backupJCColor(const char* path);
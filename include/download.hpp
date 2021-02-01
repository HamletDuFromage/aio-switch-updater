#pragma once

#define ON              1
#define OFF             0

#include <time.h>
#include <math.h>
#include <curl/curl.h>

#include <string>
#include <regex>

#include <switch.h>

#include "progress_event.hpp"

void downloadFile(const char *url, const char *output, int api);
std::tuple<std::vector<std::string>, std::vector<std::string>> fetchLinks(const char *url);
std::string fetchTitle(const char *url);
std::string downloadPage(const char* url);
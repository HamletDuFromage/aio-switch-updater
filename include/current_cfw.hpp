#pragma once

#include "constants.hpp"
#include <string>

bool isServiceRunning(const char *serviceName);
CFW getCFW();
std::string getAmsInfo();
const extern CFW running_cfw;
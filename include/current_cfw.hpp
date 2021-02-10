#pragma once

#include "constants.hpp"

bool isServiceRunning(const char *serviceName);
CFW getCFW();
static CFW running_cfw = ams;
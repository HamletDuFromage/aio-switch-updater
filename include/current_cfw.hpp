#pragma once

#include "constants.hpp"

bool isServiceRunning(const char *serviceName);
CFW getCFW();
extern CFW running_cfw;
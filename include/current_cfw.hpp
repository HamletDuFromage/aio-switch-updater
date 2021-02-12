#pragma once

#include "constants.hpp"

bool isServiceRunning(const char *serviceName);
CFW getCFW();
const extern CFW running_cfw;
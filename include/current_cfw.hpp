#pragma once

#include "constants.hpp"

bool isServiceRunning(const char *serviceName);
Result smAtmosphereHasService(bool *out, SmServiceName name);
CFW getCFW();
const extern CFW running_cfw;
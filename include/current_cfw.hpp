#pragma once

#include "constants.hpp"
#include <string>

namespace CurrentCfw {

    CFW getCFW();
    std::string getAmsInfo();
    const extern CFW running_cfw;

}
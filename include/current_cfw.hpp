#pragma once

#include <string>

#include "constants.hpp"

namespace CurrentCfw {

    CFW getCFW();
    std::string getAmsInfo();
    extern CFW running_cfw;

}  // namespace CurrentCfw
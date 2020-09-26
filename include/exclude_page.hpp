#pragma once

#include <borealis.hpp>
#include <switch.h>
#include <cstring>
#include "utils.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <vector>
#include <string>

typedef struct app App;

class ExcludePage : public brls::AppletFrame
{
    private:
        brls::List* list;
        brls::Label* label;
        brls::ListItem* save;
        std::vector<App*> apps;
        std::set<std::string> titles;
        //std::vector<brls::ToggleListItem*> items;
        std::tuple<std::vector<brls::ToggleListItem*>, std::vector<std::string>> items;

    public:
        ExcludePage();
};
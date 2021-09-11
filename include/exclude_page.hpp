#pragma once

#include <borealis.hpp>
#include <set>

#include "utils.hpp"

class ExcludePage : public brls::AppletFrame
{
private:
    brls::List* list;
    brls::Label* label;
    brls::ListItem* save;
    std::vector<util::app*> apps;
    std::set<std::string> titles;
    std::set<std::pair<brls::ToggleListItem*, std::string>> items;

public:
    ExcludePage();
};
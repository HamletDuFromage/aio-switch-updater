#pragma once

#include <borealis.hpp>
#include "json.hpp"
#include "constants.hpp"
#include <fstream>  

class HideTabsPage : public brls::AppletFrame
{
    private:
        brls::List* list;
        brls::Label* label;
        brls::ToggleListItem* about;
        brls::ToggleListItem* ams;
        brls::ToggleListItem* cfws;
        brls::ToggleListItem* sigpatches;
        brls::ToggleListItem* fws;
        brls::ToggleListItem* cheats;

    public:
        HideTabsPage();
};
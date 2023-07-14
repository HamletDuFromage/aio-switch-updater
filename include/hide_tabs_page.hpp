#pragma once

#include <borealis.hpp>

class HideTabsPage : public brls::AppletFrame
{
private:
    brls::List* list;
    brls::Label* label;
    brls::ToggleListItem* about;
    brls::ToggleListItem* ams;
    brls::ToggleListItem* cfws;
    brls::ToggleListItem* fws;
    brls::ToggleListItem* cheats;
    brls::ToggleListItem* custom;
    brls::ToggleListItem* outdatedTitles;
    brls::ToggleListItem* jccolor;
    brls::ToggleListItem* pccolor;
    brls::ToggleListItem* downloadpayload;
    brls::ToggleListItem* rebootpayload;
    brls::ToggleListItem* netsettings;
    brls::ToggleListItem* browser;
    brls::ToggleListItem* move;
    brls::ToggleListItem* cleanup;
    brls::ToggleListItem* language;

public:
    HideTabsPage();
};

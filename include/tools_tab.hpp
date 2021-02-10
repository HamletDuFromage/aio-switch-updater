#pragma once

#include <borealis.hpp>
//#include "ntcp.hpp"

class ToolsTab : public brls::List
{
    private:
        brls::ListItem* cheats;
        brls::ListItem* JCcolor;
        brls::ListItem* PCcolor;
        brls::ListItem* updateApp;
        brls::ListItem* rebootPayload;
        brls::ListItem* downloadPayload;
        brls::ListItem* changelog;
        brls::ListItem* language;
        brls::ListItem* cleanUp;
        brls::ListItem* hideTabs;
        brls::ListItem* ntcp;
        brls::ListItem* netSettings;
        brls::ListItem* browser;

        brls::StagedAppletFrame* stagedFrame;

    public:
    ToolsTab(std::string tag);

};
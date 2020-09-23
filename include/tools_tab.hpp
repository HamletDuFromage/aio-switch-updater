#pragma once

#include <borealis.hpp>
#include "confirm_page.hpp"
#include "worker_page.hpp"
#include "app_page.hpp"
#include "payload_page.hpp"
#include "download_payload_page.hpp"
#include "changelog_page.hpp"
#include "JC_page.hpp"
#include "extract.hpp"
#include "utils.hpp"

class ToolsTab : public brls::List
{
    private:
        brls::ListItem* deleteCheats;
        brls::ListItem* viewCheats;
        brls::ListItem* JCcolor;
        brls::ListItem* updateApp;
        brls::ListItem* rebootPayload;
        brls::ListItem* downloadPaysload;
        brls::ListItem* changelog;
        brls::StagedAppletFrame* stagedFrame;

    public:
    ToolsTab();

};
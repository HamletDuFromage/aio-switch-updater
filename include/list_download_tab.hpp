#pragma once

#include <borealis.hpp>
#include "constants.hpp"

class ListDownloadTab : public brls::List
{
    private:
        brls::ListItem* listItem;
        brls::ListItem *cheatslipsItem;
        brls::Label *notFound;
        brls::Label *description;
        brls::Label *cheatSlipLabel;
    public:
        ListDownloadTab(archiveType type);
        ~ListDownloadTab();
};
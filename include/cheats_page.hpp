#pragma once 

#include <borealis.hpp>
#include "app_page.hpp"
#include "exclude_page.hpp"

class CheatsPage : public brls::AppletFrame
{
    private:
        brls::List* list;
        brls::ListItem* view;
        brls::ListItem* deleteCheats;
        brls::ListItem* exclude;
        brls::StagedAppletFrame* stagedFrame;

    public:
        CheatsPage();
};
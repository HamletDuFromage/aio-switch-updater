#pragma once 

#include <borealis.hpp>

class CheatsPage : public brls::AppletFrame
{
    private:
        brls::List* list;
        brls::ListItem* view;
        brls::ListItem* deleteCheats;
        brls::ListItem* exclude;
        brls::ListItem* dlAll;
        brls::StagedAppletFrame* stagedFrame;

    public:
        CheatsPage();
};
#pragma once 

#include <borealis.hpp>
#include "app_page.hpp"
#include "exclude_page.hpp"

class LanguageOptionPage : public brls::AppletFrame
{
    private:
        brls::List* list;
        brls::ListItem* English;
        brls::ListItem* Chinese;

        brls::StagedAppletFrame* stagedFrame;

    public:
        LanguageOptionPage();
};
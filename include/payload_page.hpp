#pragma once

#include <borealis.hpp>

class PayloadPage : public brls::AppletFrame
{
    private:
        brls::Label* label;
        brls::List* list;
        brls::ListItem* listItem;
        brls::ListItem* reboot;
        brls::ListItem* shutDown;

    public:
        PayloadPage();
};
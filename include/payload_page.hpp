#pragma once

#include "utils.hpp"
#include "reboot_payload.h"

class PayloadPage : public brls::AppletFrame
{
    private:
        brls::Label* label;
        brls::List* list;
        std::vector<brls::ListItem*> items;
        brls::ListItem* reboot;
        brls::ListItem* shutDown;

    public:
        PayloadPage();
};
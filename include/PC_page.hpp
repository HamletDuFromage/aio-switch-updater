#pragma once

#include "PC_color_swapper.hpp"
#include "confirm_page.hpp"
#include "worker_page.hpp"

class PCPage : public brls::AppletFrame
{
    private:
        brls::List* list;
        brls::Label* label;
        std::vector<brls::ListItem*> items;
        brls::ListItem* restore;
        brls::ListItem* backup;
    
    public:
        PCPage();
};
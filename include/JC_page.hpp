#pragma once

#include "utils.hpp"
#include "JC_color_swapper.hpp"
#include "constants.hpp"
#include "confirm_page.hpp"
#include "worker_page.hpp"

class JCPage : public brls::AppletFrame
{
    private:
        brls::List* list;
        brls::Label* label;
        std::vector<brls::ListItem*> items;
        brls::ListItem* restore;
        brls::ListItem* backup;
    
    public:
        JCPage();
};
#pragma once

#include <borealis.hpp>

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
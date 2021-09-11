#pragma once

#include <borealis.hpp>

class PCPage : public brls::AppletFrame
{
private:
    brls::List* list;
    brls::Label* label;
    brls::ListItem* listItem;
    brls::ListItem* restore;
    brls::ListItem* backup;

public:
    PCPage();
};
#pragma once

#include <borealis.hpp>

class CheatsPage : public brls::AppletFrame
{
private:
    brls::List* list;
    brls::ListItem* item;
    brls::StagedAppletFrame* stagedFrame;

public:
    CheatsPage();
};
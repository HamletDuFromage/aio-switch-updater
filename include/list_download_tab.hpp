#pragma once

#include <borealis.hpp>

#include "constants.hpp"

class ListDownloadTab : public brls::List
{
private:
    brls::ListItem* listItem;
    brls::ListItem* cheatslipsItem;
    brls::ListItem* gbatempItem;
    brls::Label* notFound;
    brls::Label* description;
    brls::Label* cheatsLabel;
    int size = 0;
    void createCheatSlipItem();
    void creategbatempItem();

public:
    ListDownloadTab(const archiveType type);
    brls::View* getDefaultFocus() override;
};
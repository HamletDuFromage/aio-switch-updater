#pragma once

#include <borealis.hpp>

class ChangelogPage : public brls::AppletFrame
{
    private:
        brls::List* list;
        std::vector<brls::ListItem*> items;

    public:
        ChangelogPage();
};
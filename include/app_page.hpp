#pragma once

#include <borealis.hpp>
#include <algorithm>
#include <set>

typedef struct app App;

enum class appPageType{
    base,
    cheatSlips,
    gbatempCheats
};

class AppPage : public brls::AppletFrame
{
    private:
        brls::List* list;
        brls::Label* label;
        brls::ListItem* download;
        brls::ListItem* listItem;
        std::vector<App*> apps;
        std::set<std::string> titles;

    public:
        AppPage(const appPageType type = appPageType::base);
};
#pragma once

#include <borealis.hpp>
#include <tuple>
#include <set>

typedef struct app App;

class ExcludePage : public brls::AppletFrame
{
    private:
        brls::List* list;
        brls::Label* label;
        brls::ListItem* save;
        std::vector<App*> apps;
        std::set<std::string> titles;
        std::tuple<std::vector<brls::ToggleListItem*>, std::vector<std::string>> items;

    public:
        ExcludePage();
};
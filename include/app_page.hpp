#pragma once

#include <borealis.hpp>
#include <switch.h>
#include "utils.hpp"
#include <filesystem>
#include <fstream>
#include <algorithm>

#include "worker_page.hpp"
#include "confirm_page.hpp"
#include "download_cheats_page.hpp"

typedef struct app App;


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
        AppPage(bool cheatSlips = false);
};
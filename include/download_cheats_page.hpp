#pragma once 

#include <borealis.hpp>
#include <switch.h>
#include "download.hpp"
#include "utils.hpp"
#include "json.hpp"

class DownloadCheatsPage : public brls::AppletFrame
{
    private:
        brls::List* list;
        brls::Label* label;
        brls::ListItem* del;
        brls::ToggleListItem* listItem;
        std::vector<std::pair<brls::ToggleListItem*, int>> toggles;

    public:
        DownloadCheatsPage(uint64_t tid);
        std::string GetBuilID(uint64_t tid);
        std::string GetCheatsTitle(nlohmann::json cheat);
        void WriteCheats(uint64_t tid, std::string bid, std::string cheatContent);
        void DeleteCheats(uint64_t tid, std::string bid);
        void ShowCheatsContent(nlohmann::ordered_json titles);
};
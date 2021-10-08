#pragma once

#include <borealis.hpp>
#include <json.hpp>

#include "constants.hpp"

class ListDownloadTab : public brls::List
{
private:
    brls::ListItem* listItem;
    brls::ListItem* cheatslipsItem;
    brls::ListItem* gbatempItem;
    nlohmann::ordered_json nxlinks;
    std::string currentCheatsVer = "";
    std::string newCheatsVer = "";
    contentType type;
    int size = 0;
    void createList(contentType type);
    void createCheatSlipItem();
    void creategbatempItem();
    void setDescription();
    void displayNotFound();

public:
    ListDownloadTab(const contentType type, const nlohmann::ordered_json& nxlinks = nlohmann::ordered_json::object());
};
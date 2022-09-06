#pragma once

#include <borealis.hpp>
#include <json.hpp>
#include <set>

#include "constants.hpp"

class AmsTab : public brls::List
{
private:
    brls::ListItem* listItem;
    bool erista;
    contentType type;
    nlohmann::ordered_json hekate;
    std::string GetRepoName(const std::string& repo);
    std::set<std::string> GetLastDownloadedModules(const std::string& json_path);
    void CreateStagedFrames(const std::string& text, const std::string& url, bool erista, bool ams = true, bool hekate = false, const std::string& text_hekate = "", const std::string& hekate_url = "");
    void CreateDownloadItems(const nlohmann::ordered_json& cfw_links, bool hekate = true, bool ams = true);
    nlohmann::ordered_json SortDeepseaModules(const nlohmann::ordered_json& modules);
    void ShowCustomDeepseaBuilder(nlohmann::ordered_json& modules);

public:
    AmsTab(const nlohmann::json& nxlinks, const bool erista = true, const bool custom = false);
};

class UnTogglableListItem : public brls::ToggleListItem
{
public:
    UnTogglableListItem(const std::string& label, bool initialValue, std::string description = "", std::string onValue = "On", std::string offValue = "Off") : ToggleListItem(label, initialValue, description, onValue, offValue) {}
    virtual bool onClick() override;
};
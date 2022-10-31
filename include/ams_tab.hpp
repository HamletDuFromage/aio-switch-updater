#pragma once

#include <borealis.hpp>
#include <json.hpp>
#include <set>

#include "constants.hpp"

class AmsTab : public brls::List
{
protected:
    brls::ListItem* listItem;
    bool erista;
    nlohmann::ordered_json nxlinks;
    nlohmann::ordered_json hekate;
    contentType type;
    void CreateStagedFrames(const std::string& text, const std::string& url, bool erista, bool ams = true, bool hekate = false, const std::string& text_hekate = "", const std::string& hekate_url = "");
    bool CreateDownloadItems(const nlohmann::ordered_json& cfw_links, bool hekate = true, bool ams = true);
    void CreateNotFoundLabel();
    virtual void RegisterListItemAction(brls::ListItem* listItem);

public:
    AmsTab(const nlohmann::ordered_json& nxlinks, const bool erista = true);
};

class AmsTab_Regular : public AmsTab
{
private:
    void CreateLists();
    bool CreateDownloadItems(const nlohmann::ordered_json& cfw_links, bool hekate = true, bool ams = true);
    void ShowCustomDeepseaBuilder(nlohmann::ordered_json& modules);
    std::set<std::string> GetLastDownloadedModules(const std::string& json_path);
    nlohmann::ordered_json SortDeepseaModules(const nlohmann::ordered_json& modules);
    std::string GetRepoName(const std::string& repo);

public:
    AmsTab_Regular(const nlohmann::ordered_json& nxlinks, const bool erista = true);
};

class AmsTab_Custom : public AmsTab
{
private:
    nlohmann::ordered_json custom_packs;
    void CreateLists();
    void RegisterListItemAction(brls::ListItem* listItem) override;
    void AddLinkCreator();

public:
    AmsTab_Custom(const nlohmann::ordered_json& nxlinks, const bool erista = true);
};

class UnTogglableListItem : public brls::ToggleListItem
{
public:
    UnTogglableListItem(const std::string& label, bool initialValue, std::string description = "", std::string onValue = "On", std::string offValue = "Off") : ToggleListItem(label, initialValue, description, onValue, offValue) {}
    virtual bool onClick() override;
};
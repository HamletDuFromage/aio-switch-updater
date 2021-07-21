#pragma once

#include <borealis.hpp>
#include <json.hpp>
#include <set>

class AmsTab : public brls::List
{
    private:
        brls::ListItem* listItem;
        brls::Label *description;
        int size = 0;
        bool erista;
        nlohmann::ordered_json cfws = {};
        std::string GetRepoName(const std::string& repo);
        std::set<std::string> GetLastDownloadedModules(const std::string& json_path);
        void CreateStagedFrames(const std::string& text, const std::string& url, const std::string& operation, bool erista, bool hekate = false, const std::string& text_hekate = "", const std::string& hekate_url = "");
        void CreateDownloadItems(const nlohmann::ordered_json& cfw_links, bool hekate = true);
        nlohmann::ordered_json SortDeepseaModules(const nlohmann::ordered_json& modules);
        void ShowCustomDeepseaBuilder(nlohmann::ordered_json& modules);

    public:
        AmsTab(const bool erista = true, const bool hideStandardEntries = false);
        brls::View* getDefaultFocus() override;
};

class UnTogglableListItem : public brls::ToggleListItem
{
    public:
        UnTogglableListItem(const std::string& label, bool initialValue, std::string description = "", std::string onValue = "On", std::string offValue = "Off") : ToggleListItem(label, initialValue, description, onValue, offValue) {}
        virtual bool onClick() override;
};
#pragma once

#include <switch.h>

#include <algorithm>
#include <borealis.hpp>
#include <filesystem>
#include <json.hpp>
#include <set>

static constexpr uint32_t MaxTitleCount = 64000;

enum class appPageType
{
    base,
    cheatSlips,
    gbatempCheats
};

class AppPage : public brls::AppletFrame
{
private:
    brls::ListItem* download;
    std::set<std::string> titles;

protected:
    brls::List* list;
    brls::Label* label;
    brls::ListItem* listItem;
    void CreateDownloadAllButton();
    uint64_t GetCurrentApplicationId();
    u32 InitControlData(NsApplicationControlData** controlData);
    uint32_t GetControlData(u64 tid, NsApplicationControlData* controlData, u64& controlSize, std::string& name);
    virtual void PopulatePage();
    virtual void CreateLabel(){};
    virtual void AddListItem(const std::string& name, uint64_t tid);

public:
    AppPage();
};

class AppPage_Exclude : public AppPage
{
private:
    std::set<std::pair<brls::ToggleListItem*, std::string>> items;
    void PopulatePage() override;
    void CreateLabel() override;

public:
    AppPage_Exclude();
};

class AppPage_CheatSlips : public AppPage
{
private:
    void CreateLabel() override;
    void AddListItem(const std::string& name, uint64_t tid) override;

public:
    AppPage_CheatSlips();
};

class AppPage_Gbatemp : public AppPage
{
private:
    void CreateLabel() override;
    void AddListItem(const std::string& name, uint64_t tid) override;

public:
    AppPage_Gbatemp();
};

class AppPage_DownloadedCheats : public AppPage
{
private:
    std::set<std::string> titles;
    void CreateLabel() override;
    void AddListItem(const std::string& name, uint64_t tid) override;
    void GetExistingCheatsTids();

public:
    AppPage_DownloadedCheats();
};

class AppPage_OutdatedTitles : public AppPage
{
private:
    nlohmann::ordered_json versions;
    void AddListItem(const std::string& name, uint64_t tid) override;

public:
    AppPage_OutdatedTitles();
};
#pragma once

#include <switch.h>

#include <borealis.hpp>
#include <filesystem>
#include <json.hpp>

namespace cheats_util {
    u32 GetVersion(uint64_t title_id);
    void ShowCheatFiles(uint64_t tid, const std::string& name);
    void ShowCheatSheet(u64 tid, const std::string& bid, const std::string& name);
    bool CreateCheatList(const std::filesystem::path& path, brls::List** cheatsList);
    void DeleteCheats(u64 tid, const std::string& bid);
}  // namespace cheats_util

class DownloadCheatsPage : public brls::AppletFrame
{
protected:
    brls::List* list;
    brls::Label* label;
    brls::ListItem* del;
    uint64_t tid = 0;
    std::string bid = "";
    std::string name;
    u32 version = 0;

    DownloadCheatsPage(uint64_t tid, const std::string& name);
    void GetBuildID();
    void GetBuildIDFromDmnt();
    void GetBuildIDFromFile();
    void WriteCheats(const std::string& cheatContent);
    void AddCheatsfileListItem();
    void ShowBidNotFound();
    void ShowCheatsNotFound(const std::string& versionsWithCheats = "");

    typedef struct
    {
        u64 base;
        u64 size;
    } DmntMemoryRegionExtents;

    typedef struct
    {
        u64 process_id;
        u64 title_id;
        DmntMemoryRegionExtents main_nso_extents;
        DmntMemoryRegionExtents heap_extents;
        DmntMemoryRegionExtents alias_extents;
        DmntMemoryRegionExtents address_space_extents;
        u8 main_nso_build_id[0x20];
    } DmntCheatProcessMetadata;
};

class DownloadCheatsPage_CheatSlips : public DownloadCheatsPage
{
private:
    brls::ToggleListItem* listItem;
    std::vector<std::pair<brls::ToggleListItem*, int>> toggles;
    std::string GetCheatsTitle(nlohmann::ordered_json cheat);
    void ShowCheatsContent(nlohmann::ordered_json titles);

public:
    DownloadCheatsPage_CheatSlips(uint64_t tid, const std::string& name);
};

class DownloadCheatsPage_GbaTemp : public DownloadCheatsPage
{
private:
    brls::ListItem* listItem;

public:
    DownloadCheatsPage_GbaTemp(uint64_t tid, const std::string& name);
};
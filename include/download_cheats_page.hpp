#pragma once 

#include <borealis.hpp>
#include <switch.h>
#include <json.hpp>

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
        std::string GetBuilIDFromFile(uint64_t tid);
        std::string GetCheatsTitle(nlohmann::json cheat);
        void WriteCheats(uint64_t tid, std::string bid, std::string cheatContent);
        void DeleteCheats(uint64_t tid, std::string bid);
        void ShowCheatsContent(nlohmann::ordered_json titles);

        typedef struct {
            u64 base;
            u64 size;
        } DmntMemoryRegionExtents;

        typedef struct {
            u64 process_id;
            u64 title_id;
            DmntMemoryRegionExtents main_nso_extents;
            DmntMemoryRegionExtents heap_extents;
            DmntMemoryRegionExtents alias_extents;
            DmntMemoryRegionExtents address_space_extents;
            u8 main_nso_build_id[0x20];
        } DmntCheatProcessMetadata;
};
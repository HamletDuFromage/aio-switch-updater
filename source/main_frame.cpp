#include "main_frame.hpp"
#include "about_tab.hpp"
#include "list_download_tab.hpp"
#include "ams_tab.hpp"
#include "tools_tab.hpp"
#include "utils.hpp"
#include "fs.hpp"
#include <json.hpp>
#include <fstream>

namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::json;

namespace {
    constexpr const char AppTitle[] = APP_TITLE;
    constexpr const char AppVersion[] = APP_VERSION;
}

MainFrame::MainFrame() : TabFrame()
{
    this->setIcon("romfs:/gui_icon.png");
    this->setTitle(AppTitle);

    s64 freeStorage;
    std::string tag = util::getLatestTag(TAGS_INFO);
    this->setFooterText(fmt::format("menus/main/footer_text"_i18n, 
                            (!tag.empty() && tag != AppVersion) ? AppVersion + "menus/main/new_update"_i18n : AppVersion,
                            R_SUCCEEDED(fs::getFreeStorageSD(freeStorage)) ? (float)freeStorage/0x40000000 : -1)
                        );

    json hideStatus = fs::parseJsonFile(HIDE_TABS_JSON);

    bool erista = util::isErista();

    if(hideStatus.find("about") == hideStatus.end() || !hideStatus["about"])
        this->addTab("menus/main/about"_i18n, new AboutTab());
    
    if(hideStatus.find("atmosphere") == hideStatus.end() || !hideStatus["atmosphere"])
        this->addTab("menus/main/update_ams"_i18n, new AmsTab(erista));

    if(hideStatus.find("cfw") == hideStatus.end() || !hideStatus["cfw"])
        this->addTab("menus/main/update_cfw"_i18n, new ListDownloadTab(archiveType::cfw));

    if(hideStatus.find("sigpatches") == hideStatus.end() || !hideStatus["sigpatches"])
        this->addTab("menus/main/update_sigpatches"_i18n, new ListDownloadTab(archiveType::sigpatches));

    if(hideStatus.find("firmwares") == hideStatus.end() || !hideStatus["firmwares"])
        this->addTab("menus/main/download_firmware"_i18n, new ListDownloadTab(archiveType::fw));

    if(hideStatus.find("cheats") == hideStatus.end() || !hideStatus["cheats"])
        this->addTab("menus/main/download_cheats"_i18n, new ListDownloadTab(archiveType::cheats));

    if(hideStatus.find("tools") == hideStatus.end() || !hideStatus["tools"])
        this->addTab("menus/main/tools"_i18n, new ToolsTab(tag, erista));

    this->registerAction("" , brls::Key::B, [this] { return true; });
}

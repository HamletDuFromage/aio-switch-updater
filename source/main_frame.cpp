#include "main_frame.hpp"
#include "about_tab.hpp"
#include "list_download_tab.hpp"
#include "ams_tab.hpp"
#include "tools_tab.hpp"
#include "json.hpp"
#include <fstream>
#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::json;

MainFrame::MainFrame() : TabFrame()
{
    this->setIcon("romfs:/gui_icon.png");
    this->setTitle(std::string(APP_TITLE));

    std::string tag = util::getLatestTag(TAGS_INFO);
    if(!tag.empty() && tag != APP_VERSION)
        this->setFooterText("v" + std::string(APP_VERSION) + "menus/main/new_update"_i18n);
    else
        this->setFooterText("v" + std::string(APP_VERSION));
    
    json hideStatus = util::parseJsonFile(HIDE_TABS_JSON);

    bool erista = util::isErista();

    if(hideStatus.find("about") == hideStatus.end() || !hideStatus["about"])
        this->addTab("menus/main/about"_i18n, new AboutTab());
    
    if(erista && (hideStatus.find("atmosphere") == hideStatus.end() || !hideStatus["atmosphere"]))
        this->addTab("menus/main/update_ams"_i18n, new AmsTab());

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

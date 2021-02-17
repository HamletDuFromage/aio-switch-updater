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

    std::string tag = getLatestTag(TAGS_INFO);
    if(!tag.empty() && tag != APP_VERSION)
        this->setFooterText("v" + std::string(APP_VERSION) + "menus/main_app"_i18n );
    else
        this->setFooterText("v" + std::string(APP_VERSION));
    
    json hideStatus;
    std::ifstream hideFile(HIDE_TABS_JSON);

    std::string fileContent((std::istreambuf_iterator<char>(hideFile) ),
                            (std::istreambuf_iterator<char>()    ));

    if(json::accept(fileContent))   hideStatus = json::parse(fileContent);
    else                            hideStatus = json::object();

    bool erista = isErista();

    if(hideStatus.find("about") == hideStatus.end() || !hideStatus["about"])
        this->addTab("menus/main_about"_i18n, new AboutTab());
    
    if(erista && (hideStatus.find("atmosphere") == hideStatus.end() || !hideStatus["atmosphere"]))
        this->addTab("menus/main_update_ams"_i18n, new AmsTab());

    if(hideStatus.find("cfw") == hideStatus.end() || !hideStatus["cfw"])
        this->addTab("menus/main_update_cfw"_i18n, new ListDownloadTab(cfw));

    if(hideStatus.find("sigpatches") == hideStatus.end() || !hideStatus["sigpatches"])
        this->addTab("menus/main_update_si"_i18n, new ListDownloadTab(sigpatches));

    if(hideStatus.find("firmwares") == hideStatus.end() || !hideStatus["firmwares"])
        this->addTab("menus/main_firmwares"_i18n, new ListDownloadTab(fw));

    if(hideStatus.find("cheats") == hideStatus.end() || !hideStatus["cheats"])
        this->addTab("menus/main_cheats"_i18n, new ListDownloadTab(cheats));

    if(hideStatus.find("tools") == hideStatus.end() || !hideStatus["tools"])
        this->addTab("menus/main_tools"_i18n , new ToolsTab(tag, erista));

    this->registerAction("" , brls::Key::B, [this] { return true; });
}

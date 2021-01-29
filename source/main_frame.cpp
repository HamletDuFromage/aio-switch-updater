#include "main_frame.hpp"
 
namespace i18n = brls::i18n;
using namespace i18n::literals;

MainFrame::MainFrame() : TabFrame()
{
    std::string tag = getLatestTag(TAGS_INFO);
    if(!tag.empty() && tag != APP_VERSION)
        setTitle(std::string(APP_TITLE) + "menus/main_app"_i18n );
    else
        setTitle(std::string(APP_TITLE));
    this->setIcon("romfs:/gui_icon.png");
    this->setFooterText("v" + std::string(APP_VERSION));

    this->addTab("menus/main_about"_i18n , new AboutTab());

    //this->addSeparator();

    this->addTab("menus/main_update_ams"_i18n , new AmsTab());
    this->addTab("menus/main_update_cfw"_i18n , new ListDownloadTab(cfw));
    this->addTab("menus/main_update_si"_i18n , new ListDownloadTab(sigpatches));
    this->addTab("menus/main_firmwares"_i18n , new ListDownloadTab(fw));
    this->addTab("menus/main_cheats"_i18n , new ListDownloadTab(cheats));

    //this->addSeparator();

    this->addTab("menus/main_tools"_i18n , new ToolsTab(tag));

}

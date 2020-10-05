#include "main_frame.hpp"
#include "lang.hpp"
using namespace lang::literals;
MainFrame::MainFrame() : TabFrame()
{
    std::string tag = getLatestTag(TAGS_INFO);
    if(!tag.empty() && tag != APP_VERSION)
        setTitle(std::string(APP_TITLE) + "main_app"_lang);
    else
        setTitle(std::string(APP_TITLE) + "main_v"_lang + std::string(APP_VERSION));
    this->addTab("main_about"_lang, new AboutTab());

    this->addSeparator();

    this->addTab("main_update_cfw"_lang, new ListDownloadTab(cfw));
    this->addTab("main_update_si"_lang, new ListDownloadTab(sigpatches));
    this->addTab("main_firmwares"_lang, new ListDownloadTab(fw));
    this->addTab("main_cheats"_lang, new ListDownloadTab(cheats));


    this->addSeparator();
    this->addTab("main_tools"_lang, new ToolsTab(tag));
}

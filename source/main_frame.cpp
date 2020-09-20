#include "main_frame.hpp"

MainFrame::MainFrame() : TabFrame()
{

    //this->setIcon(new Logo(LogoStyle::HEADER));
    brls::Logger::debug("MainFrame");
    setTitle(std::string(APP_TITLE) + " v" + std::string(APP_VERSION));
    this->addTab("About", new AboutTab());

    this->addSeparator();

    this->addTab("Update CFW", new ListDownloadTab(cfw));
    this->addTab("Update sigpatches", new ListDownloadTab(sigpatches));
    this->addTab("Download firmwares", new ListDownloadTab(fw));
    this->addTab("Download cheats", new ListDownloadTab(cheats));


    this->addSeparator();

    this->addTab("Tools", new ToolsTab());
}

#include "list_download_tab.hpp"

ListDownloadTab::ListDownloadTab(archiveType type) :
    brls::List()
{
    std::tuple<std::vector<std::string>, std::vector<std::string>> links;
    std::string operation = "Getting ";
    std::string firmwareText("\uE016  Here are firmware dumps from \"https://darthsternie.net/switch-firmwares/\". "\
                "Once downloaded, it will be extracted in \"/firmware\". You can then install the update through Daybreak or ChoiDuJour.\n"\
                "\uE016  Current FW: "
    );
    SetSysFirmwareVersion ver;
    if (R_SUCCEEDED(setsysGetFirmwareVersion(&ver))) firmwareText += ver.display_version;
    else firmwareText += "not found";
    this->description = new brls::Label(brls::LabelStyle::DESCRIPTION, "", true);
    switch(type){
        case sigpatches:
            links = fetchLinks(SIGPATCHES_URL);
            operation += "sigpatches";
            this->description->setText(
                "\uE016  Sigpatches allow your Switch to install and run unofficial NSP file. " \
                "Make sure you pick the correct sigpatches for your setup (pure Atmosphere or Hekate+Atmosphere)."
            );
            break;
        case fw:
            links = fetchLinks(FIRMWARE_URL);
            operation += "firmware";
            this->description->setText(firmwareText);
            break;
        case app:
            std::get<0>(links).push_back("Latest version");
            std::get<1>(links).push_back(APP_URL);
            operation += "app";
            break;
        case cfw:
            links = fetchLinks(CFW_URL);
            operation += "CFW";
            this->description->setText(
                "\uE016  Here is a list of the main Switch CFWs. If you want to use Atmosphere + Hekate, download Atmosphere and then Hekate."
            );
            break;
        case cheats:
            std::string cheatsVer = fetchTitle(CHEATS_RELEASE_URL);
            if(cheatsVer != "-1"){
                std::get<0>(links).push_back("Latest (ver " + cheatsVer + ")");
                switch(getCFW()){
                    case sxos:
                        std::get<1>(links).push_back(CHEATS_URL_TITLES);
                        break;
                    case ams:
                        std::get<1>(links).push_back(CHEATS_URL_CONTENTS);
                        break;
                    case rnx:
                        std::get<1>(links).push_back(CHEATS_URL_CONTENTS);
                        break;
                }
            }
            operation += "cheats";
            this->description->setText(
                "\uE016  This will downloaded a daily updated archive of cheat code from gbatemp.net. "\
                "Cheat codes for games you don't have installed won't be extracted to your SD card. "\
                "Title IDs listed in '" + std::string(CHEATS_EXCLUDE) + "' won't recieve cheat updates."
            );
            break;
    }
/*     std::get<0>(links).push_back("Test");
    std::get<1>(links).push_back("https://github.com"); */
    this->addView(description);

    int nbLinks = std::get<0>(links).size();
    if(nbLinks){
        linkItems.reserve(nbLinks);
        for (int i = 0; i<nbLinks; i++){
            std::string url = std::get<1>(links)[i];
            std::string text("Downloading:\n" + std::get<0>(links)[i] + "\n\nFrom:\n" + url);
            linkItems[i] = new brls::ListItem(std::get<0>(links)[i]);
            linkItems[i]->getClickEvent()->subscribe([&, text, url, type, operation](brls::View* view) {
                brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
                stagedFrame->setTitle(operation);
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, text)
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "Downloading...", [url, type](){downloadArchive(url, type);})
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "Extracting...", [type](){extractArchive(type);})
                );
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, "All done!", true)
                );
                brls::Application::pushView(stagedFrame);
            });
            this->addView(linkItems[i]);
        }
    }
    else{
        notFound = new brls::Label(
            brls::LabelStyle::DESCRIPTION,
            "Could not find a download link, make sure the Switch has access to the internet.\n"\
            "If this problem persists, please open an issue on Github",
            true
        );
        notFound->setHorizontalAlign(NVG_ALIGN_CENTER);
        this->addView(notFound);
    }

}

ListDownloadTab::~ListDownloadTab(){
    
}
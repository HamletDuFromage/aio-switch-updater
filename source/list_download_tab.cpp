#include "list_download_tab.hpp"
#include "lang.hpp"
using namespace lang::literals;
ListDownloadTab::ListDownloadTab(archiveType type) :
    brls::List()
{
    std::tuple<std::vector<std::string>, std::vector<std::string>> links;
    std::string operation = "Getting"_lang;
    std::string firmwareText("firmware_text"_lang
    );

    std::string currentCheatsVer = 
                "currentCeatsver"_lang;
                
    this->description = new brls::Label(brls::LabelStyle::DESCRIPTION, "", true);
    switch(type){
        case sigpatches:
            links = fetchLinks(SIGPATCHES_URL);
            operation += "operation_1"_lang;
            this->description->setText(
                "list_sigpatches"_lang
            );
            break;
        case fw:
            links = fetchLinks(FIRMWARE_URL);
            operation += "operation_2"_lang;
            SetSysFirmwareVersion ver;
            if (R_SUCCEEDED(setsysGetFirmwareVersion(&ver))) firmwareText += ver.display_version;
            else firmwareText += "list_not"_lang;
            this->description->setText(firmwareText);
            break;
        case app:
            std::get<0>(links).push_back("list_latest"_lang);
            std::get<1>(links).push_back(APP_URL);
            operation += "list_app"_lang;
            break;
        case cfw:
            links = fetchLinks(CFW_URL);
            operation += "list_cfw"_lang;
            this->description->setText(
                "list_main"_lang
            );
            break;
        case cheats:
            std::string cheatsVer = fetchTitle(CHEATS_RELEASE_URL);
            if(cheatsVer != "-1"){
                std::get<0>(links).push_back("list_latest_ver"_lang + cheatsVer + ")");
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
            operation += "list_cheats"_lang;
            currentCheatsVer += readVersion(CHEATS_VERSION);
            this->description->setText(currentCheatsVer);
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
            std::string text("list_down"_lang + std::get<0>(links)[i] + "list_from"_lang + url);
            linkItems[i] = new brls::ListItem(std::get<0>(links)[i]);
            linkItems[i]->getClickEvent()->subscribe([&, text, url, type, operation](brls::View* view) {
                brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
                stagedFrame->setTitle(operation);
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, text)
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "list_downing"_lang, [url, type](){downloadArchive(url, type);})
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "list_extracting"_lang, [type](){extractArchive(type);})
                );
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, "list_All"_lang, true)
                );
                brls::Application::pushView(stagedFrame);
            });
            this->addView(linkItems[i]);
        }
    }
    else{
        notFound = new brls::Label(
            brls::LabelStyle::DESCRIPTION,
            "list_could_done",
            true
        );
        notFound->setHorizontalAlign(NVG_ALIGN_CENTER);
        this->addView(notFound);
    }

}

ListDownloadTab::~ListDownloadTab(){
    
}
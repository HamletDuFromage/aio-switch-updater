#include "list_download_tab.hpp"
#include <string>
#include "download.hpp"
#include "extract.hpp"
#include "confirm_page.hpp"
#include "worker_page.hpp"
#include "current_cfw.hpp"
#include "utils.hpp"
#include "app_page.hpp"
#include <filesystem>
#include <fstream>

namespace i18n = brls::i18n;
using namespace i18n::literals;

ListDownloadTab::ListDownloadTab(const archiveType type) :
    brls::List()
{
    std::vector<std::pair<std::string, std::string>> links;
    std::vector<std::pair<std::string, std::string>> sxoslinks;
    std::string operation("menus/main/getting"_i18n);
    std::string firmwareText("menus/main/firmware_text"_i18n);

    std::string currentCheatsVer = "menus/main/cheats_text"_i18n ;

    this->description = new brls::Label(brls::LabelStyle::DESCRIPTION, "", true);
    switch(type){
        case sigpatches:
            links = getLinks(SIGPATCHES_URL);
            operation += "menus/main/sigpatches"_i18n ;
            this->description->setText(
                "menus/main/sigpatches_text"_i18n 
            );
            break;
        case fw:
            links = getLinks(FIRMWARE_URL);
            operation += "menus/main/firmware"_i18n ;
            SetSysFirmwareVersion ver;
            if (R_SUCCEEDED(setsysGetFirmwareVersion(&ver))) firmwareText += ver.display_version;
            else firmwareText += "menus/main/not_found"_i18n ;
            this->description->setText(firmwareText);
            break;
        case app:
            links.push_back(std::make_pair("menus/main/latest_cheats"_i18n, APP_URL));
            operation += "menus/main/app"_i18n ;
            break;
        case cfw:
            links = getLinks(CFW_URL);
            sxoslinks = getLinks(SXOS_URL);
            links.insert(links.end(), sxoslinks.begin(), sxoslinks.end());
            operation += "menus/main/cfw"_i18n ;
            this->description->setText(
                "menus/main/cfw_text"_i18n 
            );
            break;
        case cheats:
            auto cheatsVerVec = downloadFile(CHEATS_URL_VERSION);
            std::string cheatsVer(cheatsVerVec.begin(), cheatsVerVec.end());
            if(cheatsVer != ""){
                switch(running_cfw){
                    case sxos:
                        links.push_back(std::make_pair("menus/main/get_cheats"_i18n + cheatsVer + ")", CHEATS_URL_TITLES));
                        break;
                    case ams:
                        links.push_back(std::make_pair("menus/main/get_cheats"_i18n + cheatsVer + ")", CHEATS_URL_CONTENTS));
                        break;
                    case rnx:
                        links.push_back(std::make_pair("menus/main/get_cheats"_i18n + cheatsVer + ")", CHEATS_URL_CONTENTS));
                        break;
                }
            }
            operation += "menus/main/cheats"_i18n ;
            currentCheatsVer += readVersion(CHEATS_VERSION);
            this->description->setText(currentCheatsVer);
            break;
    }

    this->addView(description);

    int nbLinks = links.size();
    if(nbLinks){
        for (int i = 0; i<nbLinks; i++){
            std::string url = links[i].second;
            std::string text("menus/common/download"_i18n  + links[i].first + "menus/common/from"_i18n  + url);
            listItem = new brls::ListItem(links[i].first);
            listItem->setHeight(LISTITEM_HEIGHT);
            listItem->getClickEvent()->subscribe([&, text, url, type, operation](brls::View* view) {
                brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
                stagedFrame->setTitle(operation);
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, text)
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "menus/common/downloading"_i18n , [url, type](){downloadArchive(url, type);})
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "menus/common/extracting"_i18n , [type](){extractArchive(type);})
                );
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, 
                        (type == sigpatches) ? 
                            "menus/common/all_done"_i18n + "\n" + "menus/sigpatches/reboot"_i18n : 
                            "menus/common/all_done"_i18n,
                        true)
                );
                brls::Application::pushView(stagedFrame);
            });
            this->addView(listItem);
        }

    }

    else{
        notFound = new brls::Label(
            brls::LabelStyle::DESCRIPTION,
            "menus/main/links_not_found"_i18n ,
            true
        );
        notFound->setHorizontalAlign(NVG_ALIGN_CENTER);
        this->addView(notFound);
    }

    if(type == cheats){
        cheatSlipLabel = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/cheats/cheatslips_label"_i18n ,
            true
        );
        this->addView(cheatSlipLabel);
        cheatslipsItem = new brls::ListItem("menus/cheats/get_cheatslips"_i18n);
        cheatslipsItem->setHeight(LISTITEM_HEIGHT);
        cheatslipsItem->getClickEvent()->subscribe([&](brls::View* view) {
            if(std::filesystem::exists(TOKEN_PATH)) {
                brls::Application::pushView(new AppPage(true));
                return true;
            }
            else {
                SwkbdConfig kbd;
                char usr[0x100] = {0};
                char pwd[0x100] = {0};
                Result rc = swkbdCreate(&kbd, 0);
                if (R_SUCCEEDED(rc)) {
                    swkbdConfigMakePresetDefault(&kbd);
                    swkbdConfigSetOkButtonText(&kbd, "Submit");
                    swkbdConfigSetGuideText(&kbd, "www.cheatslips.com e-mail");
                    swkbdShow(&kbd, usr, sizeof(usr));
                    swkbdClose(&kbd);
                    rc = swkbdCreate(&kbd, 0);
                    if(R_SUCCEEDED(rc)){
                        swkbdConfigMakePresetPassword(&kbd);
                        swkbdConfigSetOkButtonText(&kbd, "Submit");
                        swkbdConfigSetGuideText(&kbd, "www.cheatslips.com password");
                        swkbdShow(&kbd, pwd, sizeof(pwd));
                        swkbdClose(&kbd);
                    }
                }
                std::string body =  "{\"email\":\"" + std::string(usr) 
                                    + "\",\"password\":\"" + std::string(pwd) + "\"}";
                nlohmann::json token = getRequest(CHEATSLIPS_TOKEN_URL, 
                    {"Accept: application/json", 
                    "Content-Type: application/json", 
                    "charset: utf-8"}, 
                body);
                if(token.find("token") != token.end()) {
                    std::ofstream tokenFile(TOKEN_PATH);
                    tokenFile << token.dump();
                    tokenFile.close();
                    brls::Application::pushView(new AppPage(true));
                    return true;
                }
                else {
                    brls::Dialog* dialog = new brls::Dialog("menus/cheats/cheatslips_wrong_id"_i18n + "\n" + "menus/cheats/kb_error"_i18n);
                    brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                        dialog->close();
                    };
                    dialog->addButton("menus/Ok_button"_i18n , callback);
                    dialog->setCancelable(true);
                    dialog->open();
                    return true;
                }
            }
        });
        this->addView(cheatslipsItem);
    }
}
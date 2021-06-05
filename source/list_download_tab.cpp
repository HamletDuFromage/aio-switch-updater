#include "list_download_tab.hpp"
#include <string>
#include <filesystem>
#include <fstream>
#include "utils.hpp"
#include "fs.hpp"
#include "download.hpp"
#include "extract.hpp"
#include "confirm_page.hpp"
#include "worker_page.hpp"
#include "current_cfw.hpp"
#include "app_page.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

ListDownloadTab::ListDownloadTab(const archiveType type) :
    brls::List()
{
    std::vector<std::pair<std::string, std::string>> links, sxoslinks;
    std::string operation("menus/main/getting"_i18n);
    std::string firmwareText("menus/main/firmware_text"_i18n);

    std::string currentCheatsVer = "menus/main/cheats_text"_i18n;

    this->description = new brls::Label(brls::LabelStyle::DESCRIPTION, "", true);
    switch(type){
        case archiveType::ams_cfw:
        case archiveType::sigpatches:
            links = download::getLinks(SIGPATCHES_URL);
            operation += "menus/main/sigpatches"_i18n;
            this->description->setText(
                "menus/main/sigpatches_text"_i18n 
            );
            break;
        case archiveType::fw:
            links = download::getLinks(FIRMWARE_URL);
            operation += "menus/main/firmware"_i18n;
            SetSysFirmwareVersion ver;
            if (R_SUCCEEDED(setsysGetFirmwareVersion(&ver))) firmwareText += ver.display_version;
            else firmwareText += "menus/main/not_found"_i18n;
            this->description->setText(firmwareText);
            break;
        case archiveType::app:
            links.push_back(std::make_pair("menus/main/latest_cheats"_i18n, APP_URL));
            operation += "menus/main/app"_i18n;
            break;
        case archiveType::cfw:
            links = download::getLinks(CFW_URL);
            sxoslinks = download::getLinks(SXOS_URL);
            links.insert(links.end(), sxoslinks.begin(), sxoslinks.end());
            operation += "menus/main/cfw"_i18n;
            this->description->setText(
                "menus/main/cfw_text"_i18n 
            );
            break;
        case archiveType::cheats:
            auto cheatsVerVec = download::downloadFile(CHEATS_URL_VERSION);
            std::string cheatsVer(cheatsVerVec.begin(), cheatsVerVec.end());
            if(cheatsVer != ""){
                switch(CurrentCfw::running_cfw){
                    case CFW::sxos:
                        links.push_back(std::make_pair("menus/main/get_cheats"_i18n + cheatsVer + ")", CHEATS_URL_TITLES));
                        break;
                    case CFW::ams:
                        links.push_back(std::make_pair("menus/main/get_cheats"_i18n + cheatsVer + ")", CHEATS_URL_CONTENTS));
                        break;
                    case CFW::rnx:
                        links.push_back(std::make_pair("menus/main/get_cheats"_i18n + cheatsVer + ")", CHEATS_URL_CONTENTS));
                        break;
                }
            }
            operation += "menus/main/cheats"_i18n;
            currentCheatsVer += util::readVersion(CHEATS_VERSION);
            this->description->setText(currentCheatsVer);
            break;
    }

    this->addView(description);

    this->size = links.size();
    if(this->size){
        for (const auto& link : links){
            std::string url = link.second;
            std::string text("menus/common/download"_i18n + link.first + "menus/common/from"_i18n + url);
            listItem = new brls::ListItem(link.first);
            listItem->setHeight(LISTITEM_HEIGHT);
            listItem->getClickEvent()->subscribe([&, text, url, type, operation](brls::View* view) {
                brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
                stagedFrame->setTitle(operation);
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, text)
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, [url, type](){util::downloadArchive(url, type);})
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "menus/common/extracting"_i18n, [type](){util::extractArchive(type);})
                );
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, 
                        (type == archiveType::sigpatches) ? 
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
            "menus/main/links_not_found"_i18n,
            true
        );
        notFound->setHorizontalAlign(NVG_ALIGN_CENTER);
        this->addView(notFound);
    }

    if(type == archiveType::cheats){
        cheatsLabel = new brls::Label(
            brls::LabelStyle::DESCRIPTION,
            "menus/cheats/cheats_label"_i18n,
                true
        );
        this->addView(cheatsLabel);
        creategbatempItem();
        createCheatSlipItem();
    }
}

void ListDownloadTab::createCheatSlipItem() {
    this->size += 1;
    cheatslipsItem = new brls::ListItem("menus/cheats/get_cheatslips"_i18n);
    cheatslipsItem->setHeight(LISTITEM_HEIGHT);
    cheatslipsItem->getClickEvent()->subscribe([&](brls::View* view) {
        if(std::filesystem::exists(TOKEN_PATH)) {
            brls::Application::pushView(new AppPage(appPageType::cheatSlips));
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
            nlohmann::json token = download::getRequest(CHEATSLIPS_TOKEN_URL, 
                {"Accept: application/json", 
                "Content-Type: application/json", 
                "charset: utf-8"}, 
            body);
            if(token.find("token") != token.end()) {
                std::ofstream tokenFile(TOKEN_PATH);
                tokenFile << token.dump();
                tokenFile.close();
                brls::Application::pushView(new AppPage(appPageType::cheatSlips));
                return true;
            }
            else {
                brls::Dialog* dialog = new brls::Dialog("menus/cheats/cheatslips_wrong_id"_i18n + "\n" + "menus/cheats/kb_error"_i18n);
                brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                    dialog->close();
                };
                dialog->addButton("menus/common/ok"_i18n, callback);
                dialog->setCancelable(true);
                dialog->open();
                return true;
            }
        }
    });
    this->addView(cheatslipsItem);
}


void ListDownloadTab::creategbatempItem() {
    this->size += 1;
    gbatempItem = new brls::ListItem("menus/cheats/get_gbatemp"_i18n);
    gbatempItem->setHeight(LISTITEM_HEIGHT);
    gbatempItem->getClickEvent()->subscribe([&](brls::View* view) {
        brls::Application::pushView(new AppPage(appPageType::gbatempCheats));
        return true;
    });
    this->addView(gbatempItem);
}

brls::View* ListDownloadTab::getDefaultFocus()
{
    if(this->size)
        return this->brls::List::getDefaultFocus();
    else
        return nullptr;
}
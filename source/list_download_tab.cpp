#include "list_download_tab.hpp"

#include <filesystem>
#include <fstream>
#include <string>

#include "app_page.hpp"
#include "confirm_page.hpp"
#include "current_cfw.hpp"
#include "dialogue_page.hpp"
#include "download.hpp"
#include "extract.hpp"
#include "fs.hpp"
#include "utils.hpp"
#include "worker_page.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

ListDownloadTab::ListDownloadTab(const contentType type, const nlohmann::ordered_json& nxlinks) : brls::List(), type(type), nxlinks(nxlinks)
{
    this->setDescription();

    this->createList();

    if (this->type == contentType::cheats) {
        brls::Label* cheatsLabel = new brls::Label(
            brls::LabelStyle::DESCRIPTION,
            "menus/cheats/cheats_label"_i18n,
            true);
        this->addView(cheatsLabel);
        this->createGbatempItem();
        this->createGfxItem();
        this->createCheatSlipItem();
    }

    if (this->type == contentType::bootloaders) {
        this->setDescription(contentType::hekate_ipl);
        this->createList(contentType::hekate_ipl);
        this->setDescription(contentType::payloads);
        this->createList(contentType::payloads);
    }
}

void ListDownloadTab::createList()
{
    ListDownloadTab::createList(this->type);
}

void ListDownloadTab::createList(contentType type)
{
    std::vector<std::pair<std::string, std::string>> links;
    if (type == contentType::cheats && this->newCheatsVer != "") {
        links.push_back(std::make_pair(fmt::format("menus/main/get_cheats"_i18n, this->newCheatsVer), CurrentCfw::running_cfw == CFW::sxos ? CHEATS_URL_TITLES : CHEATS_URL_CONTENTS));
        links.push_back(std::make_pair("menus/main/get_cheats_gfx"_i18n, CurrentCfw::running_cfw == CFW::sxos ? GFX_CHEATS_URL_TITLES : GFX_CHEATS_URL_CONTENTS));
    }
    else
        links = download::getLinksFromJson(util::getValueFromKey(this->nxlinks, contentTypeNames[(int)type].data()));

    if (links.size()) {
        for (const auto& link : links) {
            const std::string title = link.first;
            const std::string url = link.second;
            const std::string text("menus/common/download"_i18n + link.first + "menus/common/from"_i18n + url);
            listItem = new brls::ListItem(link.first);
            listItem->setHeight(LISTITEM_HEIGHT);
            listItem->getClickEvent()->subscribe([this, type, text, url, title](brls::View* view) {
                brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
                stagedFrame->setTitle(fmt::format("menus/main/getting"_i18n, contentTypeNames[(int)type].data()));
                stagedFrame->addStage(new ConfirmPage(stagedFrame, text));
                if (type != contentType::payloads && type != contentType::hekate_ipl) {
                    if (type != contentType::cheats || (this->newCheatsVer != this->currentCheatsVer && this->newCheatsVer != "offline")) {
                        stagedFrame->addStage(new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, [this, type, url]() { util::downloadArchive(url, type); }));
                    }
                    stagedFrame->addStage(new WorkerPage(stagedFrame, "menus/common/extracting"_i18n, [this, type]() { util::extractArchive(type, this->newCheatsVer); }));
                }
                else if (type == contentType::payloads) {
                    fs::createTree(BOOTLOADER_PL_PATH);
                    std::string path = std::string(BOOTLOADER_PL_PATH) + title;
                    stagedFrame->addStage(new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, [url, path]() { download::downloadFile(url, path, OFF); }));
                }
                else if (type == contentType::hekate_ipl) {
                    fs::createTree(BOOTLOADER_PATH);
                    std::string path = std::string(BOOTLOADER_PATH) + title;
                    stagedFrame->addStage(new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, [url, path]() { download::downloadFile(url, path, OFF); }));
                }

                std::string doneMsg = "menus/common/all_done"_i18n;
                switch (type) {
                    case contentType::fw: {
                        std::string contentsPath = util::getContentsPath();
                        for (const auto& tid : {"0100000000001000", "0100000000001007", "0100000000001013"}) {
                            if (std::filesystem::exists(contentsPath + tid) && !std::filesystem::is_empty(contentsPath + tid)) {
                                doneMsg += "\n" + "menus/main/theme_warning"_i18n;
                                break;
                            }
                        }
                        if (std::filesystem::exists(DAYBREAK_PATH)) {
                            stagedFrame->addStage(new DialoguePage_fw(stagedFrame, doneMsg));
                        }
                        else {
                            stagedFrame->addStage(new ConfirmPage_Done(stagedFrame, doneMsg));
                        }
                        break;
                    }
                    default:
                        stagedFrame->addStage(new ConfirmPage_Done(stagedFrame, doneMsg));
                        break;
                }
                brls::Application::pushView(stagedFrame);
            });
            this->addView(listItem);
        }
    }
    else {
        this->displayNotFound();
    }
}

void ListDownloadTab::displayNotFound()
{
    brls::Label* notFound = new brls::Label(
        brls::LabelStyle::SMALL,
        "menus/main/links_not_found"_i18n,
        true);
    notFound->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(notFound);
}

void ListDownloadTab::setDescription()
{
    this->setDescription(this->type);
}

void ListDownloadTab::setDescription(contentType type)
{
    brls::Label* description = new brls::Label(brls::LabelStyle::DESCRIPTION, "", true);

    switch (type) {
        case contentType::fw: {
            SetSysFirmwareVersion ver;
            description->setText(fmt::format("{}{}", "menus/main/firmware_text"_i18n, R_SUCCEEDED(setsysGetFirmwareVersion(&ver)) ? ver.display_version : "menus/main/not_found"_i18n));
            break;
        }
        case contentType::bootloaders:
            description->setText(
                "menus/main/bootloaders_text"_i18n);
            break;
        case contentType::cheats:
            this->newCheatsVer = util::getCheatsVersion();
            this->currentCheatsVer = util::readFile(CHEATS_VERSION);
            description->setText("menus/main/cheats_text"_i18n + this->currentCheatsVer);
            break;
        case contentType::payloads:
            description->setText(fmt::format("menus/main/payloads_label"_i18n, BOOTLOADER_PL_PATH));
            break;
        case contentType::hekate_ipl:
            description->setText("menus/main/hekate_ipl_label"_i18n);
            break;
        default:
            break;
    }

    this->addView(description);
}

void ListDownloadTab::createCheatSlipItem()
{
    brls::ListItem* cheatslipsItem = new brls::ListItem("menus/cheats/get_cheatslips"_i18n);
    cheatslipsItem->setHeight(LISTITEM_HEIGHT);
    cheatslipsItem->getClickEvent()->subscribe([](brls::View* view) {
        if (std::filesystem::exists(TOKEN_PATH)) {
            brls::Application::pushView(new AppPage_CheatSlips());
        }
        else {
            std::string usr, pwd;
            // Result rc = swkbdCreate(&kbd, 0);
            brls::Swkbd::openForText([&usr](std::string text) { usr = text; }, "cheatslips.com e-mail", "", 64, "", 0, "Submit", "cheatslips.com e-mail");
            brls::Swkbd::openForText([&pwd](std::string text) { pwd = text; }, "cheatslips.com password", "", 64, "", 0, "Submit", "cheatslips.com password", true);
            std::string body = "{\"email\":\"" + std::string(usr) + "\",\"password\":\"" + std::string(pwd) + "\"}";
            nlohmann::ordered_json token;
            download::getRequest(CHEATSLIPS_TOKEN_URL, token,
                                 {"Accept: application/json",
                                  "Content-Type: application/json",
                                  "charset: utf-8"},
                                 body);
            if (token.find("token") != token.end()) {
                std::ofstream tokenFile(TOKEN_PATH);
                tokenFile << token.dump();
                tokenFile.close();
                brls::Application::pushView(new AppPage_CheatSlips());
            }
            else {
                util::showDialogBoxInfo("menus/cheats/cheatslips_wrong_id"_i18n + "\n" + "menus/cheats/kb_error"_i18n);
            }
        }
        return true;
    });
    this->addView(cheatslipsItem);
}

void ListDownloadTab::createGbatempItem()
{
    brls::ListItem* gbatempItem = new brls::ListItem("menus/cheats/get_gbatemp"_i18n);
    gbatempItem->setHeight(LISTITEM_HEIGHT);
    gbatempItem->getClickEvent()->subscribe([](brls::View* view) {
        brls::Application::pushView(new AppPage_Gbatemp());
        return true;
    });
    this->addView(gbatempItem);
}

void ListDownloadTab::createGfxItem()
{
    brls::ListItem* gfxItem = new brls::ListItem("menus/cheats/get_gfx"_i18n);
    gfxItem->setHeight(LISTITEM_HEIGHT);
    gfxItem->getClickEvent()->subscribe([](brls::View* view) {
        brls::Application::pushView(new AppPage_Gfx());
        return true;
    });
    this->addView(gfxItem);
}
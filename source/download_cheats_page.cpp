#include "download_cheats_page.hpp"
#include <fstream>
#include <filesystem>
#include "constants.hpp"
#include "download.hpp"
#include "utils.hpp"
#include "fs.hpp"
#include "current_cfw.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::json;

DownloadCheatsPage::DownloadCheatsPage(uint64_t tid) : AppletFrame(true, true), tid(tid)
{
    GetVersion();
    GetBuildID();
    this->setTitle("menus/cheats/menu"_i18n);
    this->setFooterText("Game version: v" + std::to_string(this->version / 0x10000));
}

void DownloadCheatsPage::GetBuildID() {
    if(CurrentCfw::running_cfw == CFW::ams)
        GetBuildIDFromDmnt();
    if(this->bid == "")
        GetBuildIDFromFile();
}

void DownloadCheatsPage::GetBuildIDFromDmnt() {
    static Service g_dmntchtSrv;
    DmntCheatProcessMetadata metadata;
    smGetService(&g_dmntchtSrv, "dmnt:cht");
    serviceDispatch(&g_dmntchtSrv, 65003);
    serviceDispatchOut(&g_dmntchtSrv, 65002, metadata);
    serviceClose(&g_dmntchtSrv);
    if(metadata.title_id == this->tid){
        u8 buildID[0x20];
        memcpy(buildID, metadata.main_nso_build_id, 0x20);
        std::stringstream ss;
        for (u8 i = 0; i < 8; i++)
            ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (u16)buildID[i];
        this->bid = ss.str();
    }
    else{
        this->bid = "";
    }
}

void DownloadCheatsPage::GetVersion() {
    NsApplicationContentMetaStatus *MetaSatus = new NsApplicationContentMetaStatus[100U];
    s32 out;
    nsListApplicationContentMetaStatus(this->tid, 0, MetaSatus, 100, &out);
    for(int i = 0; i < out ; i++) {
        if(version < MetaSatus[i].version) this->version = MetaSatus[i].version;
    }
}

void DownloadCheatsPage::GetBuildIDFromFile() {
    json versions_json = download::getRequest(VERSIONS_DIRECTORY + util::formatApplicationId(this->tid) + ".json");

    std::string version_str = std::to_string(this->version);
    if(versions_json.find(version_str) != versions_json.end()) {
        this->bid = versions_json[version_str];
    }
    else {
        this->bid = "";
    }
}

void DownloadCheatsPage::WriteCheats(std::string cheatContent) {
    std::string path;
    std::string tidstr = util::formatApplicationId(this->tid);
    switch(CurrentCfw::running_cfw){
        case CFW::ams:
            path = std::string(AMS_PATH) + std::string(CONTENTS_PATH);
            break;
        case CFW::rnx:
            path = std::string(REINX_PATH) + std::string(CONTENTS_PATH);
            break;
        case CFW::sxos:
            path = std::string(SXOS_PATH) + std::string(TITLES_PATH);
            break;
    }
    path += tidstr + "/cheats/";
    fs::createTree(path);
    path += this->bid + ".txt";
    std::ofstream cheatFile;
    cheatFile.open(path, std::ios::app);
    cheatFile << "\n\n" << cheatContent;
    std::ofstream updated;
    updated.open(UPDATED_TITLES_PATH, std::ios::app);
    updated << "\n" << tidstr;
}

void DownloadCheatsPage::DeleteCheats() {
    std::string path;
    switch(CurrentCfw::running_cfw){
        case CFW::ams:
            path = std::string(AMS_PATH) + std::string(CONTENTS_PATH);
            break;
        case CFW::rnx:
            path = std::string(REINX_PATH) + std::string(CONTENTS_PATH);
            break;
        case CFW::sxos:
            path = std::string(SXOS_PATH) + std::string(TITLES_PATH);
            break;
    }
    std::filesystem::remove(path + util::formatApplicationId(this->tid) + "/cheats/" + this->bid + ".txt");
}

DownloadCheatsPage_CheatSlips::DownloadCheatsPage_CheatSlips(uint64_t tid) : DownloadCheatsPage(tid)
{
    this->setTitle("menus/cheats/menu"_i18n);
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/cheats/cheatslips_dl"_i18n + "\n\uE016  Build ID: " + this->bid,
        true
    );
    list->addView(label);

    if(this->bid != "") {
        std::vector<std::string> headers = {"accept: application/json"};
        json cheatsInfo = download::getRequest(CHEATSLIPS_CHEATS_URL + util::formatApplicationId(this->tid) + "/" + this->bid, headers);
        if(cheatsInfo.find("cheats") != cheatsInfo.end()) {
            for (const auto& p : cheatsInfo["cheats"].items()) {
                json cheat = p.value();
                try {
                    listItem = new::brls::ToggleListItem(GetCheatsTitle(cheat), 0, "", "\uE016", "o");
                } catch (const std::out_of_range& e) {
                    //Empty titles
                    continue;
                } catch (...) {
                    //Something else went wrong
                    continue;
                }
                listItem->registerAction("menus/cheats/cheatslips_see_more"_i18n, brls::Key::Y, [this, cheat] { 
                    if(cheat.find("titles") != cheat.end()) {
                        ShowCheatsContent(cheat["titles"]);
                    }
                    return true;
                });
                toggles.push_back(std::make_pair(listItem, cheat["id"]));
                list->addView(listItem);
            }
            if(list->getViewsCount() > 1)
                list->addView(new brls::ListItemGroupSpacing(true));
        }
    }

    else {
        label = new brls::Label(
            brls::LabelStyle::REGULAR,
            "menus/cheats/bid_not_found"_i18n,
            true
        );
        list->addView(label);
    }

    list->registerAction((this->bid != "") ? "menus/cheats/cheatslips_dl_cheats"_i18n : "brls/hints/back"_i18n, brls::Key::B, [this] { 
        std::vector<int> ids;
        for(auto& e : toggles){
            if(e.first->getToggleState()){
                ids.push_back(e.second);
            }
        }
        int error = 0;
        if(!ids.empty()) {
            json token;
            std::ifstream tokenFile(TOKEN_PATH);
            tokenFile >> token;
            tokenFile.close();
            std::vector<std::string> headers = {"accept: application/json"};
            if(token.find("token") != token.end()) {
                headers.push_back("X-API-TOKEN: " + token["token"].get<std::string>());
            }
            nlohmann::ordered_json cheatsInfo = download::getRequest("https://www.cheatslips.com/api/v1/cheats/" + util::formatApplicationId(this->tid) + "/" + this->bid, headers);
            if(cheatsInfo.find("cheats") != cheatsInfo.end()) {
                for (const auto& p : cheatsInfo["cheats"].items()) {
                    if(std::find(ids.begin(), ids.end(), p.value()["id"]) != ids.end()) {
                        if(p.value()["content"].get<std::string>() == "Quota exceeded for today !"){
                            error = 1;
                        }
                        else {
                            WriteCheats(p.value()["content"]);
                        }
                    }
                }
            }
            else {
                error = 2;
            }

            if(error != 0){
                brls::Dialog* dialog;
                switch(error){
                    case 1:
                        dialog = new brls::Dialog("menus/cheats/quota"_i18n);
                        break;
                    case 2:
                        dialog = new brls::Dialog("menus/cheats/cheatslips_error"_i18n);
                        break;
                }
                brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                    dialog->close();
                };
                dialog->addButton("menus/common/ok"_i18n, callback);
                dialog->setCancelable(true);
                dialog->open();
            }
        }
        if(error == 0) {
           /*  brls::Dialog* dialog = new brls::Dialog("menus/cheatslips_success"_i18n);
            bool dialogResult = false;
            bool result = false;
            brls::GenericEvent::Callback callback = [dialog, &dialogResult](brls::View* view) {
                dialogResult = true;
                dialog->close();
            };
            dialog->addButton("menus/common/ok"_i18n, callback);
            dialog->setCancelable(true);
            dialog->open();
            while(result == false){
                usleep(1);
                result = dialogResult;
            }
            dialogResult = false; */
            brls::Application::popView();
        }
        return true;
    });

    del = new brls::ListItem("menus/cheats/delete_file"_i18n);
    del->getClickEvent()->subscribe([this](brls::View* view) {
        DeleteCheats();
        brls::Dialog* dialog = new brls::Dialog("menus/common/all_done"_i18n);
        brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
            dialog->close();
        };
        dialog->addButton("menus/common/ok"_i18n, callback);
        dialog->setCancelable(true);
        dialog->open();
    });
    list->addView(del);
    this->setContentView(list);
}

std::string DownloadCheatsPage_CheatSlips::GetCheatsTitle(json cheat) {
    std::string res = "";
    if(cheat.find("titles") != cheat.end()) {
        for(auto& p : cheat["titles"]){
            res += "[" + p.get<std::string>() + "]" + " - ";
        }
    }
    res.erase(res.length() - 3);
    if(res.size() > 80){
        res = res.substr(0, 79) + "\u2026";
    }
    return res;
}

void DownloadCheatsPage_CheatSlips::ShowCheatsContent(nlohmann::ordered_json titles) {
    brls::AppletFrame* appView = new brls::AppletFrame(true, true);
    brls::List* list = new brls::List();
    brls::ListItem* listItem;
    for(auto& p : titles){
        listItem = new brls::ListItem(p.get<std::string>());
        listItem->registerAction("", brls::Key::A, [this] { 
            return true;
        });
        list->addView(listItem);
    }
    appView->setContentView(list);
    brls::PopupFrame::open("menus/cheats/sheet_content"_i18n, appView, "", "");
}

DownloadCheatsPage_GbaTemp::DownloadCheatsPage_GbaTemp(uint64_t tid) : DownloadCheatsPage(tid)
{
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        fmt::format("menus/cheats/gbatemp_dl"_i18n, this->bid),
        true
    );
    list->addView(label);
    
    if(this->bid != "") {
        nlohmann::ordered_json cheatsJson = download::getRequest(CHEATS_DIRECTORY + util::formatApplicationId(this->tid) + ".json");
        if(cheatsJson.find(this->bid) != cheatsJson.end()) {
            for (const auto& p : cheatsJson[this->bid].items()) {
                json cheat = p.value();
                listItem = new::brls::ListItem(cheat["title"]);
                listItem->registerAction("menus/cheats/gbatemp_dl_cheatcode"_i18n, brls::Key::A, [this, cheat] {
                    WriteCheats(cheat["content"]);
                    brls::Dialog* dialog = new brls::Dialog(fmt::format("menus/cheats/gbatemp_dl_successful_dl"_i18n, cheat["title"]));
                    brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                        dialog->close();
                    };
                    dialog->addButton("menus/common/ok"_i18n, callback);
                    dialog->setCancelable(true);
                    dialog->open();
                    return true;
                });
                list->addView(listItem);
            }
            list->addView(new brls::ListItemGroupSpacing(true));
        }

    }

    else {
        label = new brls::Label(
            brls::LabelStyle::REGULAR,
            "menus/cheats/bid_not_found"_i18n,
            true
        );
        list->addView(label);
    }

    del = new brls::ListItem("menus/cheats/delete_file"_i18n);
    del->getClickEvent()->subscribe([this](brls::View* view) {
        DeleteCheats();
        brls::Dialog* dialog = new brls::Dialog("menus/common/all_done"_i18n);
        brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
            dialog->close();
        };
        dialog->addButton("menus/common/ok"_i18n, callback);
        dialog->setCancelable(true);
        dialog->open();
    });
    list->addView(del);
    this->setContentView(list);
}


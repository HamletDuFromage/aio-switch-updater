#include "download_cheats_page.hpp"
#include "constants.hpp"
#include "download.hpp"
#include "utils.hpp"
#include "current_cfw.hpp"
#include <fstream>
#include <filesystem>
//#include <iostream>
 
namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::json;

DownloadCheatsPage::DownloadCheatsPage(uint64_t tid) : AppletFrame(true, true)
{
    this->setTitle("menus/cheats/menu"_i18n );

    std::string bid = "";
    if(running_cfw == ams)
        bid = GetBuilID(tid);
    if(bid == "")
        bid = GetBuilIDFromFile(tid);

    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/cheats/cheatslips_dl"_i18n + "\n\uE016  Build ID: " + bid,
        true
    );
    list->addView(label);

    if(bid != "") {
        std::vector<std::string> headers = {"accept: application/json"};
        json cheatsInfo = getRequest((CHEATSLIPS_CHEATS_URL + formatApplicationId(tid) + "/" + bid).c_str(), headers);
        if(cheatsInfo.find("cheats") != cheatsInfo.end()) {
            for (const auto& p : cheatsInfo["cheats"].items()) {
                json cheat = p.value();
                listItem = new::brls::ToggleListItem(GetCheatsTitle(cheat), 0, "", "\uE016", "o");
                listItem->registerAction("menus/cheats/cheatslips_see_more"_i18n , brls::Key::Y, [this, cheat] { 
                    if(cheat.find("titles") != cheat.end()) {
                        ShowCheatsContent(cheat["titles"]);
                    }
                    return true;
                });
                toggles.push_back(std::make_pair(listItem, cheat["id"]));
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

    list->registerAction((bid != "") ? "menus/cheats/cheatslips_dl_cheats"_i18n : "brls/hints/back"_i18n, brls::Key::B, [this, bid, tid] { 
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
            json cheatsInfo = getRequest(("https://www.cheatslips.com/api/v1/cheats/" + formatApplicationId(tid) + "/" + bid).c_str(), headers);
            if(cheatsInfo.find("cheats") != cheatsInfo.end()) {
                for (const auto& p : cheatsInfo["cheats"].items()) {
                    if(std::find(ids.begin(), ids.end(), p.value()["id"]) != ids.end()) {
                        if(p.value()["content"].get<std::string>() == "Quota exceeded for today !"){
                            error = 1;
                        }
                        else {
                            WriteCheats(tid, bid, p.value()["content"]);
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
                dialog->addButton("menus/Ok_button"_i18n , callback);
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
            dialog->addButton("menus/Ok_button"_i18n , callback);
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
    del->getClickEvent()->subscribe([this, tid, bid](brls::View* view) {
        DeleteCheats(tid, bid);
        brls::Dialog* dialog = new brls::Dialog("menus/common/all_done"_i18n);
        brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
            dialog->close();
        };
        dialog->addButton("menus/Ok_button"_i18n , callback);
        dialog->setCancelable(true);
        dialog->open();
    });
    list->addView(del);
    this->setContentView(list);
}
std::string DownloadCheatsPage::GetBuilID(uint64_t tid) {
    static Service g_dmntchtSrv;
    DmntCheatProcessMetadata metadata;
    smGetService(&g_dmntchtSrv, "dmnt:cht");
    serviceDispatch(&g_dmntchtSrv, 65003);
    serviceDispatchOut(&g_dmntchtSrv, 65002, metadata);
    serviceClose(&g_dmntchtSrv);
    if(metadata.title_id == tid){
        u8 buildID[0x20];
        memcpy(buildID, metadata.main_nso_build_id, 0x20);
        std::stringstream ss;
        for (u8 i = 0; i < 8; i++)
            ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (u16)buildID[i];
        return ss.str();
    }
    else{
        return "";
    }
}

std::string DownloadCheatsPage::GetBuilIDFromFile(uint64_t tid) {
    NsApplicationContentMetaStatus *MetaSatus = new NsApplicationContentMetaStatus[100U];
    s32 out;
    nsListApplicationContentMetaStatus(tid, 0, MetaSatus, 100, &out);
    u32 version = 0;
    for(int i = 0; i < out ; i++){
        if(version < MetaSatus[i].version) version = MetaSatus[i].version;
    }

    this->setFooterText("Game version: v" + std::to_string(version / 0x10000));

    json lookupTable;
    try { 
        lookupTable = json::parse(std::string(json::from_cbor(downloadFile(LOOKUP_TABLE_CBOR))));
    }
    catch (json::parse_error& e)
    {
        //std::cout << "message: " << e.what() << '\n' << "exception id: " << e.id << '\n' << "byte position of error: " << e.byte << std::endl;
    }

    std::string tidstr = formatApplicationId(tid);
    std::string versionstr = std::to_string(version);
    if(lookupTable.find(tidstr) != lookupTable.end()) {
        json buildIDs = lookupTable[tidstr];
        if(buildIDs.find(versionstr) != buildIDs.end()) {
            return buildIDs[versionstr];
        }
    }
    return "";
}

std::string DownloadCheatsPage::GetCheatsTitle(json cheat) {
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

void DownloadCheatsPage::WriteCheats(uint64_t tid, std::string bid, std::string cheatContent) {
    std::string path;
    std::string tidstr = formatApplicationId(tid);
    switch(running_cfw){
        case ams:
            path = std::string(AMS_PATH) + std::string(CONTENTS_PATH);
            break;
        case rnx:
            path = std::string(REINX_PATH) + std::string(CONTENTS_PATH);
            break;
        case sxos:
            path = std::string(SXOS_PATH) + std::string(TITLES_PATH);
            break;
    }
    path += tidstr + "/cheats/";
    createTree(path);
    path += bid + ".txt";
    std::ofstream cheatFile;
    cheatFile.open(path, std::ios::app);
    cheatFile << "\n\n" << cheatContent;
    std::ofstream updated;
    updated.open(UPDATED_TITLES_PATH, std::ios::app);
    updated << "\n" << tidstr;
}

void DownloadCheatsPage::DeleteCheats(uint64_t tid, std::string bid) {
    std::string path;
    switch(running_cfw){
        case ams:
            path = std::string(AMS_PATH) + std::string(CONTENTS_PATH);
            break;
        case rnx:
            path = std::string(REINX_PATH) + std::string(CONTENTS_PATH);
            break;
        case sxos:
            path = std::string(SXOS_PATH) + std::string(TITLES_PATH);
            break;
    }
    std::filesystem::remove(path + formatApplicationId(tid) + "/cheats/" + bid + ".txt");
}

void DownloadCheatsPage::ShowCheatsContent(nlohmann::ordered_json titles) {
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
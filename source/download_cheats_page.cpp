#include "download_cheats_page.hpp"
#include "constants.hpp"
 
namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::json;

DownloadCheatsPage::DownloadCheatsPage(uint64_t tid) : AppletFrame(true, true)
{
    this->setTitle("menus/cheat_menu"_i18n );

    std::string bid = GetBuilID(tid);
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/download_cheatslips"_i18n + "\n\uE016  Build ID: " + bid,
        true
    );
    list->addView(label);

    if(bid != "") {
        std::vector<std::string> headers = {"accept: application/json"};
        json cheatsInfo = getRequest(("https://www.cheatslips.com/api/v1/cheats/" + formatApplicationId(tid) + "/" + bid).c_str(), headers);
        if(cheatsInfo.find("cheats") != cheatsInfo.end()) {
            for (const auto& p : cheatsInfo["cheats"].items()) {
                json cheat = p.value();
                listItem = new::brls::ToggleListItem(GetCheatsTitle(cheat), 0);
                listItem->registerAction("menus/see_more"_i18n , brls::Key::Y, [this, cheat] { 
                    std::string str = "menus/cheat_cheat_content"_i18n + "\n";
                    if(cheat.find("titles") != cheat.end()) {
                        for(auto& p : cheat["titles"]){
                            str += "[" + p.get<std::string>() + "]" + "\n";
                        }
                        str.pop_back();
                    }
                    brls::Dialog* dialog = new brls::Dialog(str);
                    brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                        dialog->close();
                    };
                    dialog->addButton("menus/Ok_button"_i18n , callback);
                    dialog->setCancelable(true);
                    dialog->open();
                    return true;
                });
                toggles.push_back(std::make_pair(listItem, cheat["id"]));
                list->addView(listItem);
            }
        }
        list->addView(new brls::ListItemGroupSpacing(true));
    }

    list->registerAction("menus/download_cheats"_i18n , brls::Key::B, [this, bid, tid] { 
        std::vector<int> ids;
        for(auto& e : toggles){
            if(e.first->getToggleState()){
                ids.push_back(e.second);
            }
        }
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
                        WriteCheats(tid, bid, p.value()["content"]);
                    }
                }
            }
            else {
                brls::Dialog* dialog = new brls::Dialog("menus/couldnt_dl_cheats"_i18n);
                brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                    dialog->close();
                };
                dialog->addButton("menus/Ok_button"_i18n , callback);
                dialog->setCancelable(true);
                dialog->open();
            }
        }
        brls::Application::popView();
        return true;
    });

    del = new brls::ListItem("menus/delete_cheat"_i18n);
    del->getClickEvent()->subscribe([this, tid, bid](brls::View* view) {
        DeleteCheats(tid, bid);
        brls::Dialog* dialog = new brls::Dialog("menus/All_done"_i18n);
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
    NsApplicationContentMetaStatus *MetaSatus = new NsApplicationContentMetaStatus[100U];
    s32 out;
    nsListApplicationContentMetaStatus(tid, 0, MetaSatus, 100, &out);
    u32 version = 0;
    for(int i = 0; i < out ; i++){
        if(version < MetaSatus[i].version) version = MetaSatus[i].version;
    }

    this->setFooterText("Game version: v" + std::to_string(version / 0x10000));

    json lookupTable = getRequest(LOOKUP_TABLE_URL);
    
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
        res = res.substr(0, 79) + "...";
    }
    return res;
}

void DownloadCheatsPage::WriteCheats(uint64_t tid, std::string bid, std::string cheatContent) {
    std::string path;
    std::string tidstr = formatApplicationId(tid);
    switch(getCFW()){
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
    cheatFile << "\n" << cheatContent;
    std::ofstream updated;
    updated.open(UPDATED_TITLES_PATH, std::ios::app);
    updated << "\n" << tidstr;
}

void DownloadCheatsPage::DeleteCheats(uint64_t tid, std::string bid) {
    std::string path;
    switch(getCFW()){
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
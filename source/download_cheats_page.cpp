#include "download_cheats_page.hpp"

#include <filesystem>
#include <fstream>

#include "constants.hpp"
#include "current_cfw.hpp"
#include "download.hpp"
#include "extract.hpp"
#include "fs.hpp"
#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::json;

namespace show_cheats {

    void ShowCheatSheet(u64 tid, const std::string& bid, const std::string& name)
    {
        std::string path = fmt::format("{}{}/cheats/{}.txt", util::getContentsPath(), util::formatApplicationId(tid), bid);
        brls::AppletFrame* appView = new brls::AppletFrame(true, true);
        brls::List* cheatsList = new brls::List();
        if (std::filesystem::exists(path) && CreateCheatList(path, &cheatsList)) {
            appView->setContentView(cheatsList);
            appView->registerAction("menus/cheats/delete_file"_i18n, brls::Key::X, [tid, bid] {
                DeleteCheats(tid, bid);
                brls::Dialog* dialog = new brls::Dialog("menus/common/all_done"_i18n);
                brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                    dialog->close();
                };
                dialog->addButton("menus/common/ok"_i18n, callback);
                dialog->setCancelable(true);
                dialog->open();
                return true;
            });
            brls::PopupFrame::open(name, appView, "");
        }
        else {
            NoCheatsFoundPopup();
        }
    }

    void ShowCheatFiles(u64 tid, const std::string& name)
    {
        std::string path = util::getContentsPath();
        path += util::formatApplicationId(tid) + "/cheats/";

        brls::TabFrame* appView = new brls::TabFrame();
        bool is_populated = false;
        if (std::filesystem::exists(path)) {
            for (const auto& cheatFile : std::filesystem::directory_iterator(path)) {
                brls::List* cheatsList = new brls::List();
                is_populated |= CreateCheatList(cheatFile.path(), &cheatsList);
                if (is_populated) {
                    appView->addTab(util::upperCase(cheatFile.path().stem()), cheatsList);
                }
            }
        }
        if (is_populated) {
            brls::PopupFrame::open(name, appView, "");
        }
        else {
            NoCheatsFoundPopup();
        }
    }

    void NoCheatsFoundPopup()
    {
        brls::Dialog* dialog = new brls::Dialog("menus/cheats/not_found"_i18n);
        brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
            dialog->close();
        };
        dialog->addButton("menus/common/ok"_i18n, callback);
        dialog->setCancelable(true);
        dialog->open();
    }

    bool CreateCheatList(const std::filesystem::path& path, brls::List** cheatsList)
    {
        bool res = false;
        if (extract::isBID(path.filename().stem())) {
            (*cheatsList)->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, fmt::format("menus/cheats/cheatfile_label"_i18n, path.filename().string()), true));

            std::string str;
            std::regex cheats_expr(R"(\[.+\]|\{.+\})");
            std::ifstream in(path);
            if (in) {
                while (std::getline(in, str)) {
                    if (str.size() > 0) {
                        if (std::regex_search(str, cheats_expr)) {
                            (*cheatsList)->addView(new brls::ListItem(str));
                            res = true;
                        }
                    }
                }
            }
        }
        return res;
    }

    void DeleteCheats(u64 tid, const std::string& bid)
    {
        std::filesystem::remove(fmt::format("{}{:016X}/cheats/{}.txt", util::getContentsPath(), tid, bid));
    }

}  // namespace show_cheats

DownloadCheatsPage::DownloadCheatsPage(uint64_t tid, const std::string& name) : AppletFrame(true, true), tid(tid), name(name)
{
    this->list = new brls::List();
    this->GetVersion();
    this->GetBuildID();
    this->setTitle(this->name);
    this->setFooterText("v" + std::to_string(this->version / 0x10000));
    this->brls::AppletFrame::registerAction("menus/cheats/show_existing"_i18n, brls::Key::X, [this] {
        show_cheats::ShowCheatSheet(this->tid, this->bid, this->name);
        return true;
    });
    this->rebuildHints();
}

void DownloadCheatsPage::GetBuildID()
{
    if (CurrentCfw::running_cfw == CFW::ams)
        this->GetBuildIDFromDmnt();
    if (this->bid == "")
        this->GetBuildIDFromFile();
}

void DownloadCheatsPage::GetBuildIDFromDmnt()
{
    static Service g_dmntchtSrv;
    DmntCheatProcessMetadata metadata;
    smGetService(&g_dmntchtSrv, "dmnt:cht");
    serviceDispatch(&g_dmntchtSrv, 65003);
    serviceDispatchOut(&g_dmntchtSrv, 65002, metadata);
    serviceClose(&g_dmntchtSrv);
    if (metadata.title_id == this->tid) {
        u64 buildID = 0;
        memcpy(&buildID, metadata.main_nso_build_id, sizeof(u64));
        this->bid = fmt::format("{:016X}", __builtin_bswap64(buildID));
    }
}

void DownloadCheatsPage::GetVersion()
{
    NsApplicationContentMetaStatus* MetaSatus = new NsApplicationContentMetaStatus[100U];
    s32 out;
    nsListApplicationContentMetaStatus(this->tid, 0, MetaSatus, 100, &out);
    for (int i = 0; i < out; i++) {
        if (version < MetaSatus[i].version) this->version = MetaSatus[i].version;
    }
}

void DownloadCheatsPage::GetBuildIDFromFile()
{
    nlohmann::ordered_json versions_json;
    download::getRequest(VERSIONS_DIRECTORY + util::formatApplicationId(this->tid) + ".json", versions_json);

    std::string version_str = std::to_string(this->version);
    if (versions_json.find(version_str) != versions_json.end()) {
        this->bid = versions_json[version_str];
    }
    else {
        this->bid = "";
    }
}

void DownloadCheatsPage::WriteCheats(const std::string& cheatContent)
{
    std::string path = util::getContentsPath() + util::formatApplicationId(this->tid) + "/cheats/";
    fs::createTree(path);
    std::ofstream cheatFile(path + this->bid + ".txt", std::ios::app);
    cheatFile << "\n\n"
              << cheatContent;
}

void DownloadCheatsPage::AddCheatsfileListItem()
{
    brls::ListItem* item = new brls::ListItem("menus/cheats/show_cheat_files"_i18n);
    item->getClickEvent()->subscribe([this](brls::View* view) {
        show_cheats::ShowCheatFiles(this->tid, this->name);
    });
    this->list->addView(item);
}

void DownloadCheatsPage::ShowBidNotFound()
{
    brls::Label* label = new brls::Label(
        brls::LabelStyle::REGULAR,
        "menus/cheats/bid_not_found"_i18n,
        true);
    this->list->addView(label);
}

void DownloadCheatsPage::ShowCheatsNotFound(const std::string& versionsWithCheats)
{
    brls::Label* label = new brls::Label(
        brls::LabelStyle::REGULAR,
        "menus/cheats/cheats_not_found"_i18n + (!versionsWithCheats.empty() ? "\n" + fmt::format("menus/cheats/old_cheats_found"_i18n, versionsWithCheats) : ""),
        true);
    this->list->addView(label);
}

DownloadCheatsPage_CheatSlips::DownloadCheatsPage_CheatSlips(uint64_t tid, const std::string& name) : DownloadCheatsPage(tid, name)
{
    this->label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/cheats/cheatslips_dl"_i18n + "\n\uE016  Build ID: " + this->bid,
        true);
    this->list->addView(this->label);

    if (this->bid != "") {
        std::vector<std::string> headers = {"accept: application/json"};
        nlohmann::ordered_json cheatsInfo;
        download::getRequest(CHEATSLIPS_CHEATS_URL + util::formatApplicationId(this->tid) + "/" + this->bid, cheatsInfo, headers);
        if (cheatsInfo.find("cheats") != cheatsInfo.end()) {
            for (const auto& p : cheatsInfo.at("cheats").items()) {
                json cheat = p.value();
                listItem = new brls::ToggleListItem(GetCheatsTitle(cheat), 0, "", "\uE016", "o");
                this->listItem->registerAction("menus/cheats/cheatslips_see_more"_i18n, brls::Key::Y, [this, cheat] {
                    if (cheat.find("titles") != cheat.end()) {
                        ShowCheatsContent(cheat.at("titles"));
                    }
                    return true;
                });
                this->toggles.push_back(std::make_pair(listItem, cheat.at("id")));
                list->addView(listItem);
            }
            if (this->list->getViewsCount() > 1)
                this->list->addView(new brls::ListItemGroupSpacing(true));
        }
        else {
            ShowCheatsNotFound();
        }
    }

    else {
        ShowBidNotFound();
    }

    this->list->registerAction((this->bid != "") ? "menus/cheats/cheatslips_dl_cheats"_i18n : "brls/hints/back"_i18n, brls::Key::B, [this] {
        std::vector<int> ids;
        for (auto& e : toggles) {
            if (e.first->getToggleState()) {
                ids.push_back(e.second);
            }
        }
        int error = 0;
        if (!ids.empty()) {
            json token;
            std::ifstream tokenFile(TOKEN_PATH);
            tokenFile >> token;
            tokenFile.close();
            std::vector<std::string> headers = {"accept: application/json"};
            if (token.find("token") != token.end()) {
                headers.push_back("X-API-TOKEN: " + token.at("token").get<std::string>());
            }
            nlohmann::ordered_json cheatsInfo;
            download::getRequest("https://www.cheatslips.com/api/v1/cheats/" + util::formatApplicationId(this->tid) + "/" + this->bid, cheatsInfo, headers);
            if (cheatsInfo.find("cheats") != cheatsInfo.end()) {
                for (const auto& p : cheatsInfo.at("cheats").items()) {
                    if (std::find(ids.begin(), ids.end(), p.value().at("id")) != ids.end()) {
                        if (p.value().at("content").get<std::string>() == "Quota exceeded for today !") {
                            error = 1;
                        }
                        else {
                            WriteCheats(p.value().at("content"));
                        }
                    }
                }
            }
            else {
                error = 2;
            }

            if (error != 0) {
                brls::Dialog* dialog;
                switch (error) {
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
        if (error == 0) {
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
    this->AddCheatsfileListItem();
    this->setContentView(list);
}

std::string DownloadCheatsPage_CheatSlips::GetCheatsTitle(json cheat)
{
    std::string res = "";
    if (cheat.find("titles") != cheat.end()) {
        for (auto& p : cheat.at("titles")) {
            res += "[" + p.get<std::string>() + "]" + " - ";
        }
    }
    return res;
}

void DownloadCheatsPage_CheatSlips::ShowCheatsContent(nlohmann::ordered_json titles)
{
    brls::AppletFrame* appView = new brls::AppletFrame(true, true);
    brls::List* list = new brls::List();
    brls::ListItem* titlesItem;
    for (auto& p : titles) {
        titlesItem = new brls::ListItem(p.get<std::string>());
        titlesItem->registerAction("", brls::Key::A, [this] {
            return true;
        });
        list->addView(titlesItem);
    }
    appView->setContentView(list);
    brls::PopupFrame::open("menus/cheats/sheet_content"_i18n, appView, "", "");
}

DownloadCheatsPage_GbaTemp::DownloadCheatsPage_GbaTemp(uint64_t tid, const std::string& name) : DownloadCheatsPage(tid, name)
{
    this->label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        fmt::format("menus/cheats/gbatemp_dl"_i18n, this->bid),
        true);
    this->list->addView(label);

    if (this->bid != "") {
        nlohmann::ordered_json cheatsJson;
        download::getRequest(CHEATS_DIRECTORY + util::formatApplicationId(this->tid) + ".json", cheatsJson);
        if (cheatsJson.find(this->bid) != cheatsJson.end()) {
            for (const auto& p : cheatsJson[this->bid].items()) {
                json cheat = p.value();
                this->listItem = new brls::ListItem(cheat.at("title"));
                listItem->registerAction("menus/cheats/gbatemp_dl_cheatcode"_i18n, brls::Key::A, [this, cheat] {
                    WriteCheats(cheat.at("content"));
                    brls::Dialog* dialog = new brls::Dialog(fmt::format("menus/cheats/gbatemp_dl_successful_dl"_i18n, cheat.at("title")));
                    brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                        dialog->close();
                    };
                    dialog->addButton("menus/common/ok"_i18n, callback);
                    dialog->setCancelable(true);
                    dialog->open();
                    return true;
                });
                this->list->addView(listItem);
            }
            this->list->addView(new brls::ListItemGroupSpacing(true));
        }
        else {
            std::string versionsWithCheats;
            for (auto& [key, val] : cheatsJson.items()) {
                versionsWithCheats += key + " ";
            }
            ShowCheatsNotFound(versionsWithCheats);
        }
    }

    else {
        ShowBidNotFound();
    }
    this->AddCheatsfileListItem();
    this->setContentView(this->list);
}

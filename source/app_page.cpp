#include "app_page.hpp"
#include "current_cfw.hpp"
#include "worker_page.hpp"
#include "confirm_page.hpp"
#include "download_cheats_page.hpp"
#include "utils.hpp"
#include "extract.hpp"
#include "fs.hpp"
#include <switch.h>
#include <filesystem>
#include <fstream>
#include <regex>

namespace i18n = brls::i18n;
using namespace i18n::literals;

AppPage::AppPage() : AppletFrame(true, true)
{
    list = new brls::List();
}

void AppPage::PopulatePage()
{
    this->CreateLabel();

    NsApplicationRecord *records = new NsApplicationRecord[MaxTitleCount];
    NsApplicationControlData *controlData = NULL;
    std::string name;

    s32 recordCount     = 0;
    u64 controlSize     = 0;
    u64 tid;

    if (!util::isApplet()) {
        if (R_SUCCEEDED(nsListApplicationRecord(records, MaxTitleCount, 0, &recordCount))){
            for (s32 i = 0; i < recordCount; i++){
                controlSize = 0;

                if(R_FAILED(InitControlData(&controlData))) break;

                tid = records[i].application_id;

                if R_FAILED(GetControlData(tid, controlData, controlSize, name)) continue;

                listItem = new brls::ListItem(name, "", util::formatApplicationId(tid));
                this->DeclareGameListItem(name, tid, &controlData);

                free(controlData);
            }
            delete[] records;
        }
    }
    else {
        tid = GetCurrentApplicationId();
        if (R_SUCCEEDED(InitControlData(&controlData)) && R_SUCCEEDED(GetControlData(tid, controlData, controlSize, name))) {
            this->DeclareGameListItem(name, tid, &controlData);
        }
        label = new brls::Label(brls::LabelStyle::SMALL, "menus/cheats/applet_mode_not_supported"_i18n, true);
        list->addView(label);
    }

    this->CreateDownloadAllButton();

    this->setContentView(list);
}

void AppPage::CreateDownloadAllButton()
{
    std::string text("menus/cheats/downloading"_i18n);
    std::string url = "";
    switch(CurrentCfw::running_cfw){
        case CFW::ams:
            url += CHEATS_URL_CONTENTS;
            break;
        case CFW::rnx:
            url += CHEATS_URL_CONTENTS;
            break;
        case CFW::sxos:
            url += CHEATS_URL_CONTENTS;
            break;
    }
    text += url;
    download = new brls::ListItem("menus/cheats/dl_latest"_i18n);
    download->getClickEvent()->subscribe([&, url, text](brls::View* view) {
        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("menus/cheats/getting_cheats"_i18n);
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, text)
        );
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, [url](){util::downloadArchive(url, archiveType::cheats);})
        );
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/common/extracting"_i18n, [](){util::extractArchive(archiveType::cheats);})
        );
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "menus/common/all_done"_i18n, true)
        );
        brls::Application::pushView(stagedFrame);
    });
    list->addView(download);
}

u32 AppPage::InitControlData(NsApplicationControlData** controlData) {
    *controlData = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
    if(*controlData == NULL) {
        free(*controlData);
        return 300;
    }
    else {
        memset(*controlData, 0, sizeof(NsApplicationControlData));
        return 0;
    }
}

u32 AppPage::GetControlData(u64 tid, NsApplicationControlData* controlData, u64& controlSize, std::string& name)
{
    Result rc;
    NacpLanguageEntry* langEntry = NULL;
    rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, tid, controlData, sizeof(NsApplicationControlData), &controlSize);
    if(R_FAILED(rc)) return rc;

    if(controlSize < sizeof(controlData->nacp)) return 100;

    rc = nacpGetLanguageEntry(&controlData->nacp, &langEntry);
    if(R_FAILED(rc)) return rc;

    if (!langEntry->name) return 200;

    name = langEntry->name;

    return 0;
}

void AppPage::DeclareGameListItem(const std::string& name, u64 tid, NsApplicationControlData **controlData)
{
    listItem->setThumbnail((*controlData)->icon, sizeof((*controlData)->icon));
    list->addView(listItem);
}

uint64_t AppPage::GetCurrentApplicationId()
{
    Result rc = 0;
    uint64_t pid = 0;
    uint64_t tid = 0;

    rc = pmdmntGetApplicationProcessId(&pid);
    if (rc == 0x20f || R_FAILED(rc)) return 0;

    rc = pminfoGetProgramId(&tid, pid);
    if (rc == 0x20f || R_FAILED(rc)) return 0;

    return tid;
}

AppPage_CheatSlips::AppPage_CheatSlips() : AppPage()
{
    this->PopulatePage();
}

void AppPage_CheatSlips::CreateLabel()
{
    this->setTitle("menus/cheats/cheastlips_title"_i18n);
    label = new brls::Label(brls::LabelStyle::DESCRIPTION, "menus/cheats/cheatslips_select"_i18n, true);
    list->addView(label);
}

void AppPage_CheatSlips::DeclareGameListItem(const std::string& name, u64 tid, NsApplicationControlData **controlData)
{
    listItem->getClickEvent()->subscribe([&, tid, name](brls::View* view) { brls::Application::pushView(new DownloadCheatsPage_CheatSlips(tid, name)); });
    AppPage::DeclareGameListItem(name, tid, controlData);
}

AppPage_Gbatemp::AppPage_Gbatemp() : AppPage()
{
    this->PopulatePage();
    this->setIcon("romfs:/gbatemp_icon.png");
}

void AppPage_Gbatemp::CreateLabel()
{
    this->setTitle("menus/cheats/gbatemp_title"_i18n);
    label = new brls::Label( brls::LabelStyle::DESCRIPTION,"menus/cheats/cheatslips_select"_i18n,true);
    list->addView(label);
}

void AppPage_Gbatemp::DeclareGameListItem(const std::string& name, u64 tid, NsApplicationControlData **controlData)
{
    listItem->getClickEvent()->subscribe([&, tid, name](brls::View* view) { brls::Application::pushView(new DownloadCheatsPage_GbaTemp(tid, name)); });
    AppPage::DeclareGameListItem(name, tid, controlData);
}

AppPage_Exclude::AppPage_Exclude() : AppPage()
{
    this->PopulatePage();
}

void AppPage_Exclude::CreateLabel()
{
    this->setTitle("menus/cheats/exclude_titles"_i18n);
    label = new brls::Label( brls::LabelStyle::DESCRIPTION, "menus/cheats/exclude_titles_desc"_i18n, true);
    list->addView(label);
}

void AppPage_Exclude::PopulatePage()
{
    this->CreateLabel();

    NsApplicationRecord *records = new NsApplicationRecord[MaxTitleCount];
    NsApplicationControlData *controlData = NULL;
    std::string name;

    s32 recordCount     = 0;
    u64 controlSize     = 0;
    u64 tid;

    auto titles = fs::readLineByLine(CHEATS_EXCLUDE);

    if (!util::isApplet()) {
        if (R_SUCCEEDED(nsListApplicationRecord(records, MaxTitleCount, 0, &recordCount))){
            for (s32 i = 0; i < recordCount; i++){
                controlSize = 0;

                if(R_FAILED(InitControlData(&controlData))) break;

                tid = records[i].application_id;

                if R_FAILED(GetControlData(tid, controlData, controlSize, name)) continue;

                brls::ToggleListItem *listItem;
                listItem = new brls::ToggleListItem(util::formatListItemTitle(std::string(name)), titles.find(util::formatApplicationId(tid)) != titles.end() ? 0 : 1);

                listItem->setThumbnail(controlData->icon, sizeof(controlData->icon));
                items.insert(std::make_pair(listItem, util::formatApplicationId(tid)));
                list->addView(listItem);

                free(controlData);
            }
            delete[] records;
        }
    }
    else {
        label = new brls::Label(brls::LabelStyle::SMALL, "menus/common/applet_mode_not_supported"_i18n, true);
        list->addView(label);
    }

    list->registerAction("menus/cheats/exclude_titles_save"_i18n, brls::Key::B, [this] { 
        std::set<std::string> exclude;
        for (const auto& item : items) {
            if(!item.first->getToggleState()) {
                exclude.insert(item.second);
            }
        }
        extract::writeTitlesToFile(exclude, CHEATS_EXCLUDE);
        brls::Application::popView();
        return true;
    });

    this->CreateDownloadAllButton();

    this->setContentView(list);
}

AppPage_DownloadedCheats::AppPage_DownloadedCheats() : AppPage()
{
    GetExistingCheatsTids();
    this->PopulatePage();
}

void AppPage_DownloadedCheats::CreateLabel()
{
    this->setTitle("menus/cheats/installed"_i18n);
    label = new brls::Label(brls::LabelStyle::DESCRIPTION, "menus/cheats/label"_i18n, true);
    list->addView(label);
}

void AppPage_DownloadedCheats::DeclareGameListItem(const std::string& name, u64 tid, NsApplicationControlData **controlData)
{
    if (titles.find(util::formatApplicationId(tid)) != titles.end()) {
        listItem->getClickEvent()->subscribe([this, tid, name](brls::View* view) { ShowCheatFiles(tid, name); });
        AppPage::DeclareGameListItem(name, tid, controlData);
    }
}

void AppPage_DownloadedCheats::GetExistingCheatsTids() {
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
    for(const auto& entry : std::filesystem::directory_iterator(path)) {
        std::string cheatsPath =  entry.path().string() + "/cheats";
        if(std::filesystem::exists(cheatsPath) && !std::filesystem::is_empty(cheatsPath)) {
            for(const auto& cheatFile : std::filesystem::directory_iterator(cheatsPath)) {
                if(extract::isBID(cheatFile.path().filename().stem())) {
                    titles.insert(util::upperCase(cheatsPath.substr(cheatsPath.length() - 7 - 16, 16)));
                    break;
                }
            }
        }
    }
}

void AppPage_DownloadedCheats::ShowCheatFiles(u64 tid, const std::string& name) {
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
    path += util::formatApplicationId(tid) + "/cheats/";

    brls::TabFrame* appView = new brls::TabFrame();
    bool is_populated = false;
    if(std::filesystem::exists(path)) {
        for(const auto& cheatFile : std::filesystem::directory_iterator(path)){
            is_populated |= CreateCheatList(cheatFile.path(), &appView);
        }
    }
    if(is_populated) {
        brls::PopupFrame::open(name, appView, "");
    }
    else {
        brls::Dialog* dialog = new brls::Dialog("menus/cheats/not_found"_i18n);
        brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
            dialog->close();
        };
        dialog->addButton("menus/common/ok"_i18n, callback);
        dialog->setCancelable(true);
        dialog->open();
    }
}

bool AppPage_DownloadedCheats::CreateCheatList(const std::filesystem::path& path, brls::TabFrame** appView) {
    bool res = false;
    brls::List* cheatsList = new brls::List();
    if(extract::isBID(path.filename().stem())) {
        cheatsList->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, fmt::format("menus/cheats/cheatfile_label"_i18n, path.filename().string()), true));

        std::string str;
        std::regex cheats_expr(R"(\[.+\])");
        std::ifstream in(path);
        if(in) {
            while (std::getline(in, str)) {
                if(str.size() > 0) {
                    if(std::regex_search(str, cheats_expr)) {
                        cheatsList->addView(new brls::ListItem(str));
                        res = true;
                    }
                }
            }
        }
    }
    if(res) {
        (*appView)->addTab(path.filename().stem(), cheatsList);
    }
    return res;
}
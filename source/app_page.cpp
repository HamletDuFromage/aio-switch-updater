#include "app_page.hpp"
#include <switch.h>
#include <filesystem>
#include <fstream>
#include "current_cfw.hpp"
#include "worker_page.hpp"
#include "confirm_page.hpp"
#include "download_cheats_page.hpp"
#include "utils.hpp"
#include "fs.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
AppPage::AppPage(const appPageType type) : AppletFrame(true, true)
{
    list = new brls::List();
    switch(type){
        case appPageType::base:
            this->setTitle("menus/cheats/installed"_i18n);
            label = new brls::Label(brls::LabelStyle::DESCRIPTION, "menus/cheats/label"_i18n, true);
            break;
        case appPageType::cheatSlips:
            this->setTitle("menus/cheats/cheastlips_title"_i18n);
            label = new brls::Label(brls::LabelStyle::DESCRIPTION, "menus/cheats/cheatslips_select"_i18n, true);
            break;
        case appPageType::gbatempCheats:
            this->setTitle("menus/cheats/gbatemp_title"_i18n);
            label = new brls::Label( brls::LabelStyle::DESCRIPTION,"menus/cheats/cheatslips_select"_i18n,true);
            break;
    }
    list->addView(label);

    NsApplicationRecord record;
    uint64_t tid;
    NsApplicationControlData controlData;
    NacpLanguageEntry* langEntry = NULL;

    Result rc;
    size_t i            = 0;
    int recordCount     = 0;
    size_t controlSize  = 0;

    titles = fs::readLineByLine(UPDATED_TITLES_PATH);

    if(!titles.empty() || type == appPageType::cheatSlips || type == appPageType::gbatempCheats) {
        while (true)
        {
            rc = nsListApplicationRecord(&record, sizeof(record), i, &recordCount);
            if (R_FAILED(rc)) break;
            if(recordCount <= 0) break;

            tid = record.application_id;
            rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, tid, &controlData, sizeof(controlData), &controlSize);
            if (R_FAILED(rc)) break;
            rc = nacpGetLanguageEntry(&controlData.nacp, &langEntry);
            if (R_FAILED(rc)) break;
            
            if (!langEntry->name) {
                i++;
                continue;
            }
            if(type == appPageType::base && titles.find(util::formatApplicationId(tid)) == titles.end()) {
                i++;
                continue;
            }

            listItem = new brls::ListItem(std::string(langEntry->name), "", util::formatApplicationId(tid));
            listItem->setThumbnail(controlData.icon, sizeof(controlData.icon));

            switch(type){
                case appPageType::cheatSlips:
                    listItem->getClickEvent()->subscribe([&, tid](brls::View* view) { brls::Application::pushView(new DownloadCheatsPage_CheatSlips(tid)); });
                    break;
                case appPageType::gbatempCheats:
                    listItem->getClickEvent()->subscribe([&, tid](brls::View* view) { brls::Application::pushView(new DownloadCheatsPage_GbaTemp(tid)); });
                    break;
                case appPageType::base:
                    break;
            }

            list->addView(listItem);
            i++;
        }
    }
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
    this->setContentView(list);
}
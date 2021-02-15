#include "app_page.hpp"
#include <switch.h>
#include "utils.hpp"
#include <filesystem>
#include <fstream>
#include "current_cfw.hpp"
#include "worker_page.hpp"
#include "confirm_page.hpp"
#include "download_cheats_page.hpp"
 
namespace i18n = brls::i18n;
using namespace i18n::literals;
AppPage::AppPage(const bool cheatSlips) : AppletFrame(true, true)
{
    this->setTitle(cheatSlips ? "menus/cheastlips_title"_i18n : "menus/app_title"_i18n );
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        cheatSlips ? "menus/app_cheatslips_label"_i18n : "menus/app_label"_i18n,
        true
    );
    list->addView(label);

    NsApplicationRecord record;
    uint64_t tid;
    NsApplicationControlData controlData;
    NacpLanguageEntry* langEntry = NULL;

    Result rc;
    size_t i            = 0;
    int recordCount     = 0;
    size_t controlSize  = 0;

    titles = readLineByLine(UPDATED_TITLES_PATH);

    if(!titles.empty() || cheatSlips){
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
            if(!cheatSlips && titles.find(formatApplicationId(tid)) == titles.end()) {
                i++;
                continue;
            }

            listItem = new brls::ListItem(std::string(langEntry->name), "", formatApplicationId(tid));
            listItem->setThumbnail(controlData.icon, sizeof(controlData.icon));
            if(cheatSlips){
                listItem->getClickEvent()->subscribe([&, tid](brls::View* view) {
                    brls::Application::pushView(new DownloadCheatsPage(tid));
                });
            }

            list->addView(listItem);
            i++;
        }
    }
    std::string text("menus/text_download"_i18n);
    std::string url = "";
    switch(running_cfw){
        case ams:
            url += CHEATS_URL_CONTENTS;
            break;
        case rnx:
            url += CHEATS_URL_CONTENTS;
            break;
        case sxos:
            url += CHEATS_URL_CONTENTS;
            break;
    }
    text += url;
    download = new brls::ListItem("menus/text_download_list"_i18n );
    archiveType type = cheats;
    download->getClickEvent()->subscribe([&, url, text, type](brls::View* view) {
        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("menus/text_title"_i18n );
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, text)
        );
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/Downloading"_i18n , [url, type](){downloadArchive(url, type);})
        );
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/Extracting"_i18n , [type](){extractArchive(type);})
        );
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "menus/All_done"_i18n , true)
        );
        brls::Application::pushView(stagedFrame);
    });
    list->addView(download);
    this->setContentView(list);
}
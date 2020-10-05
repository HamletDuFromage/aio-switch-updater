#include "app_page.hpp"
//TODO: Serialize it in extract.cpp
#include "lang.hpp"
using namespace lang::literals;
AppPage::AppPage() : AppletFrame(true, true)
{
    this->setTitle("app_title"_lang);
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "app_label"_lang,
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

    if(!titles.empty()){
        while (true)
        {
            rc = nsListApplicationRecord(&record, sizeof(record), i, &recordCount);
            if (R_FAILED(rc)) break;

            if(recordCount <= 0)
                break;

            tid = record.application_id;
            rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, tid, &controlData, sizeof(controlData), &controlSize);
            if (R_FAILED(rc)) break;
            rc = nacpGetLanguageEntry(&controlData.nacp, &langEntry);
            if (R_FAILED(rc)) break;
            if (!langEntry->name || titles.find(formatApplicationId(tid)) == titles.end())
            {
                i++;
                continue;
            }
            App* app = (App*) malloc(sizeof(App));
            app->tid = tid;

            memset(app->name, 0, sizeof(app->name));
            strncpy(app->name, langEntry->name, sizeof(app->name)-1);

            memcpy(app->icon, controlData.icon, sizeof(app->icon));

            // Add the ListItem
            brls::ListItem *listItem = new brls::ListItem(formatListItemTitle(std::string(app->name)), "", formatApplicationId(app->tid));

            listItem->setThumbnail(app->icon, sizeof(app->icon));
            list->addView(listItem);
            i++;
        }
    }
    std::string text("text_download");
    std::string url = "";
    switch(getCFW()){
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
    download = new brls::ListItem("text_download_list"_lang);
    archiveType type = cheats;
    download->getClickEvent()->subscribe([&, url, text, type](brls::View* view) {
        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("text_title"_lang);
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, text)
        );
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "Downloading"_lang, [url, type](){downloadArchive(url, type);})
        );
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "Extracting"_lang, [type](){extractArchive(type);})
        );
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "All_done"_lang, true)
        );
        brls::Application::pushView(stagedFrame);
    });
    list->addView(download);
    this->setContentView(list);
}
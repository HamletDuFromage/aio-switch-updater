#include "exclude_page.hpp"

#include <switch.h>

#include <algorithm>
#include <filesystem>
#include <fstream>

#include "extract.hpp"
#include "fs.hpp"
#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
ExcludePage::ExcludePage() : AppletFrame(true, true)
{
    this->setTitle("menus/cheats/exclude_titles"_i18n);
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/cheats/exclude_titles_desc"_i18n,
        true);
    list->addView(label);

    NsApplicationRecord record;
    uint64_t tid;
    NsApplicationControlData controlData;
    NacpLanguageEntry* langEntry = NULL;

    Result rc;
    size_t i = 0;
    int recordCount = 0;
    size_t controlSize = 0;

    titles = fs::readLineByLine(CHEATS_EXCLUDE);

    while (true) {
        rc = nsListApplicationRecord(&record, sizeof(record), i, &recordCount);
        if (R_FAILED(rc)) break;

        if (recordCount <= 0)
            break;

        tid = record.application_id;
        rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, tid, &controlData, sizeof(controlData), &controlSize);
        if (R_FAILED(rc)) break;
        rc = nacpGetLanguageEntry(&controlData.nacp, &langEntry);
        if (R_FAILED(rc)) break;
        if (!langEntry->name) {
            i++;
            continue;
        }
        util::app* app = (util::app*)malloc(sizeof(util::app));
        app->tid = tid;

        memset(app->name, 0, sizeof(app->name));
        strncpy(app->name, langEntry->name, sizeof(app->name) - 1);

        memcpy(app->icon, controlData.icon, sizeof(app->icon));

        brls::ToggleListItem* listItem;
        if (titles.find(util::formatApplicationId(tid)) != titles.end())
            listItem = new brls::ToggleListItem(std::string(app->name), 0);
        else
            listItem = new brls::ToggleListItem(std::string(app->name), 1);

        listItem->setThumbnail(app->icon, sizeof(app->icon));
        items.insert(std::make_pair(listItem, util::formatApplicationId(app->tid)));
        list->addView(listItem);
        i++;
    }

    list->registerAction("menus/cheats/exclude_titles_save"_i18n, brls::Key::B, [this] {
        std::set<std::string> exclude;
        for (const auto& item : items) {
            if (!item.first->getToggleState()) {
                exclude.insert(item.second);
            }
        }
        extract::writeTitlesToFile(exclude, CHEATS_EXCLUDE);
        brls::Application::popView();
        return true;
    });
    this->setContentView(list);
}
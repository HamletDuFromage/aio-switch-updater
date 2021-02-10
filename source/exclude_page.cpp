#include "exclude_page.hpp"
#include <switch.h>
#include "utils.hpp"
#include "extract.hpp"
#include <filesystem>
#include <fstream>
#include <algorithm>
 
namespace i18n = brls::i18n;
using namespace i18n::literals;
ExcludePage::ExcludePage() : AppletFrame(true, true)
{
    this->setTitle("menus/exclude_titles"_i18n );
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/you_can"_i18n ,
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

    titles = readLineByLine(CHEATS_EXCLUDE);

    //std::tuple<std::vector<brls::ToggleListItem*>, std::vector<std::string>> items;

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
        if (!langEntry->name)
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
        brls::ToggleListItem *listItem;
        if(titles.find(formatApplicationId(tid)) != titles.end())
            listItem = new brls::ToggleListItem(formatListItemTitle(std::string(app->name)), 0);
        else
            listItem = new brls::ToggleListItem(formatListItemTitle(std::string(app->name)), 1);

        listItem->setThumbnail(app->icon, sizeof(app->icon));
        std::get<0>(items).push_back(listItem);
        std::get<1>(items).push_back(formatApplicationId(app->tid));
        list->addView(listItem);
        i++;
    }

    list->registerAction("menus/save"_i18n , brls::Key::B, [this] { 
        std::set<std::string> exclude{};
        for(int i = 0; i < (int) std::get<1>(items).size(); i++){
            if(!std::get<0>(items)[i]->getToggleState()){
                exclude.insert(std::get<1>(items)[i]);
            }
        }
        writeTitlesToFile(exclude, CHEATS_EXCLUDE);
        brls::Application::popView();
        return true;
    });
    this->setContentView(list);
}
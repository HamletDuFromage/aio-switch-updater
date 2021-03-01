#include "JC_page.hpp"
#include "JC_color_swapper.hpp"
#include "confirm_page.hpp"
#include "worker_page.hpp"
#include "constants.hpp"
 
namespace i18n = brls::i18n;
using namespace i18n::literals;
JCPage::JCPage() : AppletFrame(true, true)
{
    this->setTitle("menus/joy_con/title"_i18n );
    list = new brls::List();
    std::string labelText = "menus/joy_con/desc_1"_i18n  + std::string(COLOR_PROFILES_PATH) + "menus/joy_con/desc_2"_i18n +
                            "menus/joy_con/desc_3"_i18n ;
    label = new brls::Label(brls::LabelStyle::DESCRIPTION, labelText, true);
    list->addView(label);

    backup = new brls::ListItem("menus/joy_con/backup"_i18n );
    backup->getClickEvent()->subscribe([&](brls::View* view) {
        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("menus/joy_con/label"_i18n );
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/joy_con/backing_up"_i18n , 
            [](){backupJCColor(COLOR_PROFILES_PATH);})
        );
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "menus/common/all_done"_i18n , true)
        );
        brls::Application::pushView(stagedFrame);
    });
    list->addView(backup);

    list->addView(new brls::ListItemGroupSpacing(true));

    auto profiles = getProfiles(COLOR_PROFILES_PATH);
    std::vector<std::string> names = std::get<0>(profiles);
    int nbProfiles = names.size();
    items.reserve(nbProfiles);
    for (int i = nbProfiles - 1; i >= 0; i--){
        std::string name = std::get<0>(profiles)[i];
        std::vector<int> value = std::get<1>(profiles)[i];
        items[i] = new brls::ListItem(names[i]);
        items[i]->getClickEvent()->subscribe([&, value](brls::View* view) {
            brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
            stagedFrame->setTitle("menus/jc_con"_i18n );
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "menus/jc_changing"_i18n , 
                [value](){changeJCColor(value);})
            );
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "menus/joy_con/all_done"_i18n , true)
            );
            brls::Application::pushView(stagedFrame);
        });
        list->addView(items[i]);
    }
    this->setContentView(list);
}
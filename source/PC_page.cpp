#include "PC_page.hpp"
#include "PC_color_swapper.hpp"
#include "confirm_page.hpp"
#include "worker_page.hpp"
#include "constants.hpp"
 
namespace i18n = brls::i18n;
using namespace i18n::literals;
PCPage::PCPage() : AppletFrame(true, true)
{
    this->setTitle("menus/pro_con/title"_i18n);
    list = new brls::List();
    std::string labelText = "menus/pro_con/desc"_i18n;
    label = new brls::Label(brls::LabelStyle::DESCRIPTION, labelText, true);
    list->addView(label);

    backup = new brls::ListItem("menus/joy_con/backup"_i18n);
    backup->getClickEvent()->subscribe([&](brls::View* view) {
        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("menus/pro_con/label"_i18n);
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/pro_con/backing_up"_i18n, 
            [](){pc::backupPCColor(PC_COLOR_PATH);})
        );
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "menus/common/all_done"_i18n, true)
        );
        brls::Application::pushView(stagedFrame);
    });
    list->addView(backup);

    list->addView(new brls::ListItemGroupSpacing(true));

    auto profiles = pc::getProfiles(PC_COLOR_PATH);
    std::vector<std::string> names = std::get<0>(profiles);
    int nbProfiles = names.size();
    for (int i = nbProfiles - 1; i >= 0; i--){
        std::string name = std::get<0>(profiles)[i];
        std::vector<int> value = std::get<1>(profiles)[i];
        listItem = new brls::ListItem(names[i]);
        listItem->getClickEvent()->subscribe([&, value](brls::View* view) {
            brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
            stagedFrame->setTitle("menus/pro_con/label"_i18n);
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "menus/pro_con/changing"_i18n, 
                [value](){pc::changePCColor(value);})
            );
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "menus/pro_con/all_done"_i18n, true)
            );
            brls::Application::pushView(stagedFrame);
        });
        list->addView(listItem);
    }
    this->setContentView(list);
}
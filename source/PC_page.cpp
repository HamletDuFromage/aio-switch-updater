#include "PC_page.hpp"
 
namespace i18n = brls::i18n;
using namespace i18n::literals;
PCPage::PCPage() : AppletFrame(true, true)
{
    this->setTitle("menus/joy_con"_i18n );
    list = new brls::List();
    std::string labelText = "menus/jc_you_can_1"_i18n  + std::string(PC_COLOR_PATH) + "menus/jc_you_can_goto"\
                            "menus/jc_you_can_2"_i18n ;
    label = new brls::Label(brls::LabelStyle::DESCRIPTION, labelText, true);
    list->addView(label);

    backup = new brls::ListItem("menus/jc_backup"_i18n );
    backup->getClickEvent()->subscribe([&](brls::View* view) {
        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("menus/jc_color"_i18n );
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/jc_backing"_i18n , 
            [](){pc::backupPCColor(PC_COLOR_PATH);})
        );
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "menus/jc_all_done"_i18n , true)
        );
        brls::Application::pushView(stagedFrame);
    });
    list->addView(backup);

    list->addView(new brls::ListItemGroupSpacing(true));

    auto profiles = pc::getProfiles(PC_COLOR_PATH);
    std::vector<std::string> names = std::get<0>(profiles);
    int nbProfiles = names.size();
    items.reserve(nbProfiles);
    for (int i = nbProfiles - 1; i >= 0; i--){
        std::string name = std::get<0>(profiles)[i];
        std::vector<int> value = std::get<1>(profiles)[i];
        items[i] = new brls::ListItem(names[i]);
        items[i]->getClickEvent()->subscribe([&, value](brls::View* view) {
            brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
            stagedFrame->setTitle("menus/jc_concolor"_i18n );
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "menus/jc_changing"_i18n , 
                [value](){pc::changePCColor(value);})
            );
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "menus/jc_all_"_i18n , true)
            );
            brls::Application::pushView(stagedFrame);
        });
        list->addView(items[i]);
    }
    this->setContentView(list);
}
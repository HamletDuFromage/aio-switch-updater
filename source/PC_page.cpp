#include "PC_page.hpp"

#include "color_swapper.hpp"
#include "confirm_page.hpp"
#include "constants.hpp"
#include "worker_page.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
PCPage::PCPage() : AppletFrame(true, true)
{
    this->setTitle("menus/pro_con/title"_i18n);
    list = new brls::List();
    std::string labelText = fmt::format("{}\n{}", "menus/pro_con/desc"_i18n, "menus/pro_con/warning"_i18n);
    label = new brls::Label(brls::LabelStyle::DESCRIPTION, labelText, true);
    list->addView(label);

    backup = new brls::ListItem("menus/joy_con/backup"_i18n);
    backup->getClickEvent()->subscribe([](brls::View* view) {
        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("menus/pro_con/label"_i18n);
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/pro_con/backing_up"_i18n,
                           []() { PC::backupPCColor(PC_COLOR_PATH); }));
        stagedFrame->addStage(
            new ConfirmPage_Done(stagedFrame, "menus/common/all_done"_i18n));
        brls::Application::pushView(stagedFrame);
    });
    list->addView(backup);

    list->addView(new brls::ListItemGroupSpacing(true));

    auto profiles = PC::getProfiles(PC_COLOR_PATH);
    for (const auto& profile : profiles) {
        std::vector<int> value = profile.second;
        listItem = new brls::ListItem(profile.first);
        listItem->getClickEvent()->subscribe([value](brls::View* view) {
            brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
            stagedFrame->setTitle("menus/pro_con/label"_i18n);
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "menus/pro_con/changing"_i18n,
                               [value]() { PC::changePCColor(value); }));
            stagedFrame->addStage(
                new ConfirmPage_Done(stagedFrame, "menus/pro_con/all_done"_i18n));
            brls::Application::pushView(stagedFrame);
        });
        list->addView(listItem);
    }
    this->setContentView(list);
}
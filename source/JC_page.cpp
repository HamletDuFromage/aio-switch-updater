#include "JC_page.hpp"

#include "color_swapper.hpp"
#include "confirm_page.hpp"
#include "constants.hpp"
#include "worker_page.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
JCPage::JCPage() : AppletFrame(true, true)
{
    this->setTitle("menus/joy_con/title"_i18n);
    list = new brls::List();
    label = new brls::Label(brls::LabelStyle::DESCRIPTION, fmt::format("menus/joy_con/description"_i18n, COLOR_PROFILES_PATH, COLOR_PICKER_URL), true);
    list->addView(label);

    backup = new brls::ListItem("menus/joy_con/backup"_i18n);
    backup->getClickEvent()->subscribe([](brls::View* view) {
        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("menus/joy_con/label"_i18n);
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/joy_con/backing_up"_i18n,
                           []() { JC::backupJCColor(COLOR_PROFILES_PATH); }));
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "menus/common/all_done"_i18n, true));
        brls::Application::pushView(stagedFrame);
    });
    list->addView(backup);

    list->addView(new brls::ListItemGroupSpacing(true));

    auto profiles = JC::getProfiles(COLOR_PROFILES_PATH);
    for (int i = profiles.size() - 1; i >= 0; i--) {
        std::vector<int> value = profiles[i].second;
        listItem = new brls::ListItem(profiles[i].first);
        listItem->getClickEvent()->subscribe([value](brls::View* view) {
            brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
            stagedFrame->setTitle("menus/joy_con/label"_i18n);
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "menus/joy_con/changing"_i18n,
                               [value]() { JC::changeJCColor(value); }));
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "menus/joy_con/all_done"_i18n, true));
            brls::Application::pushView(stagedFrame);
        });
        list->addView(listItem);
    }
    this->setContentView(list);
}
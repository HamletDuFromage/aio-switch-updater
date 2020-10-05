#include "JC_page.hpp"
#include "lang.hpp"
using namespace lang::literals;
JCPage::JCPage() : AppletFrame(true, true)
{
    this->setTitle("joy_con"_lang);
    list = new brls::List();
    std::string labelText = "jc_you_can_1"_lang + std::string(COLOR_PROFILES_PATH) + "jc_you_can_goto"\
                            "jc_you_can_2"_lang;
    label = new brls::Label(brls::LabelStyle::DESCRIPTION, labelText, true);
    list->addView(label);

    backup = new brls::ListItem("jc_backup"_lang);
    backup->getClickEvent()->subscribe([&](brls::View* view) {
        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("jc_color"_lang);
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "jc_backing"_lang, 
            [](){backupJCColor(COLOR_PROFILES_PATH);})
        );
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "jc_all_done"_lang, true)
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
            stagedFrame->setTitle("jc_concolor"_lang);
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "jc_changing"_lang, 
                [value](){changeJCColor(value);})
            );
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "jc_all_"_lang, true)
            );
            brls::Application::pushView(stagedFrame);
        });
        list->addView(items[i]);
    }
    this->setContentView(list);
}
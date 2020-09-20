#include "JC_page.hpp"

JCPage::JCPage() : AppletFrame(true, true)
{
    this->setTitle("Joy-Con color swapper");
    list = new brls::List();
    std::string labelText = "You can change the internal color of your Joy-Cons. Make sure they're docked.\n"\
                            "Color profiles are stored in '" + std::string(COLOR_PROFILES_PATH) + "'. Go to http://bit.ly/JC-color "\
                            "to generate your own custom profiles.";
    label = new brls::Label(brls::LabelStyle::DESCRIPTION, labelText, true);
    list->addView(label);

    backup = new brls::ListItem("Backup current color profile");
    backup->getClickEvent()->subscribe([&](brls::View* view) {
        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("Joy-Con color swapper");
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "Backing up the current color profile. Make sure the Joy-Con are docked. "\
            "If the process hangs, try docking/undocking the JCs.", 
            [](){backupJCColor(COLOR_PROFILES_PATH);})
        );
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "All done!", true)
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
            stagedFrame->setTitle("Joy-Con color swapper");
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "Changing color. Make sure the Joy-Con are docked. "\
                "If the process hangs, try docking/undocking the JCs.", 
                [value](){changeJCColor(value);})
            );
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "All done! You may need to dock/undock your Joy-Cons for the change to take effect", true)
            );
            brls::Application::pushView(stagedFrame);
        });
        list->addView(items[i]);
    }
    this->setContentView(list);
}
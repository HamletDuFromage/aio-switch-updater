#include "tools_tab.hpp"

ToolsTab::ToolsTab() : brls::List()
{
    viewCheats = new brls::ListItem("View installed cheats");
    viewCheats->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new AppPage());
    });
    this->addView(viewCheats);

    deleteCheats = new brls::ListItem("Delete all existing cheat codes");
    deleteCheats->getClickEvent()->subscribe([&](brls::View* view){
        stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("Delete all cheats");
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "Deleting", [](){removeCheats(getCFW());})
        );
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "All done!", true)
        );
        brls::Application::pushView(stagedFrame);
    });
    this->addView(deleteCheats);

    JCcolor = new brls::ListItem("Change the Joy-Cons color");
    JCcolor->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new JCPage());
    });
    this->addView(JCcolor);

    downloadPaysload = new brls::ListItem("Dowload payloads to " + std::string(BOOTLOADER_PL_PATH));
    downloadPaysload->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new DownloadPayloadPage());
    });
    this->addView(downloadPaysload);

    rebootPayload = new brls::ListItem("Inject payload");
    rebootPayload->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new PayloadPage());
    });
    this->addView(rebootPayload);
    
    std::string tag = getLatestTag(TAGS_INFO);
    if(!tag.empty()){
        updateApp = new brls::ListItem("Update the app (v" + tag +")");
        std::string text("Downloading:\nAIO-switch-updater\n\nFrom:\n" + std::string(APP_URL));
        updateApp->getClickEvent()->subscribe([&, text](brls::View* view) {
            brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
            stagedFrame->setTitle("Updating app");
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, text)
            );
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "Downloading...", [](){downloadArchive(APP_URL, app);})
            );
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "Extracting....", [](){extractArchive(app);})
            );
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "All done!", true)
            );
            brls::Application::pushView(stagedFrame);
        });
        this->addView(updateApp);
    }

    changelog = new brls::ListItem("Changelog");
    changelog->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new ChangelogPage());
    });
    this->addView(changelog);
}
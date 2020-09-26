#include "tools_tab.hpp"

ToolsTab::ToolsTab() : brls::List()
{
    cheats = new brls::ListItem("Cheats menu");
    cheats->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new CheatsPage());
    });
    this->addView(cheats);

    JCcolor = new brls::ListItem("Change the Joy-Cons color");
    JCcolor->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new JCPage());
    });
    this->addView(JCcolor);

    downloadPayload = new brls::ListItem("Dowload payloads to " + std::string(BOOTLOADER_PL_PATH));
    downloadPayload->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new DownloadPayloadPage());
    });
    this->addView(downloadPayload);

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
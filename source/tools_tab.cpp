#include "tools_tab.hpp"
#include "lang.hpp"
using namespace lang::literals;
ToolsTab::ToolsTab(std::string tag) : brls::List()
{
    cheats = new brls::ListItem("tool_cheats"_lang);
    cheats->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new CheatsPage());
    });
    cheats->setHeight(LISTITEM_HEIGHT);
    this->addView(cheats);

    JCcolor = new brls::ListItem("tool_change"_lang);
    JCcolor->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new JCPage());
    });
    JCcolor->setHeight(LISTITEM_HEIGHT);
    this->addView(JCcolor);

    downloadPayload = new brls::ListItem("tool_download"_lang + std::string(BOOTLOADER_PL_PATH));
    downloadPayload->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new DownloadPayloadPage());
    });
    this->addView(downloadPayload);

    rebootPayload = new brls::ListItem("tool_inject"_lang);
    rebootPayload->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new PayloadPage());
    });
    rebootPayload->setHeight(LISTITEM_HEIGHT);
    this->addView(rebootPayload);
    
    if(!tag.empty() && tag != APP_VERSION){
        updateApp = new brls::ListItem("tool_update"_lang + tag +")");
        std::string text("tool_DownLoad"_lang + std::string(APP_URL));
        updateApp->getClickEvent()->subscribe([&, text](brls::View* view) {
            brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
            stagedFrame->setTitle("tool_updating"_lang);
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, text)
            );
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "tool_downloading"_lang, [](){downloadArchive(APP_URL, app);})
            );
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "tool_extracting"_lang, [](){extractArchive(app);})
            );
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "tool_all_done"_lang, true)
            );
            brls::Application::pushView(stagedFrame);
        });
        updateApp->setHeight(LISTITEM_HEIGHT);
        this->addView(updateApp);
    }

    changelog = new brls::ListItem("tool_changelog"_lang);
    changelog->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new ChangelogPage());
    });
    changelog->setHeight(LISTITEM_HEIGHT);
    this->addView(changelog);

    language = new brls::ListItem("Language_Option"_lang);
    language->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new LanguageOptionPage());
    });
    language->setHeight(LISTITEM_HEIGHT);
    this->addView(language);
}
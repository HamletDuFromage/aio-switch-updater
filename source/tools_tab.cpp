#include "tools_tab.hpp"
#include "confirm_page.hpp"
#include "worker_page.hpp"
#include "cheats_page.hpp"
#include "payload_page.hpp"
#include "download_payload_page.hpp"
#include "changelog_page.hpp"
#include "JC_page.hpp"
#include "PC_page.hpp"
#include "net_page.hpp"
#include "extract.hpp"
#include "utils.hpp"
#include "hide_tabs_page.hpp"
#include "json.hpp"
#include <filesystem>
#include <fstream>

namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::json;

ToolsTab::ToolsTab(std::string tag) : brls::List()
{
    if(!tag.empty() && tag != APP_VERSION){
        updateApp = new brls::ListItem("menus/tool_update"_i18n  + tag +")");
        std::string text("menus/tool_DownLoad"_i18n  + std::string(APP_URL));
        updateApp->getClickEvent()->subscribe([&, text, tag](brls::View* view) {
            brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
            stagedFrame->setTitle("menus/tool_updating"_i18n );
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, text)
            );
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "menus/tool_downloading"_i18n , [](){downloadArchive(APP_URL, app);})
            );
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "menus/tool_extracting"_i18n , [tag](){extractArchive(app, tag);})
            );
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "menus/tool_all_done"_i18n , true)
            );
            brls::Application::pushView(stagedFrame);
        });
        updateApp->setHeight(LISTITEM_HEIGHT);
        this->addView(updateApp);
    }

    cheats = new brls::ListItem("menus/tool_cheats"_i18n );
    cheats->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new CheatsPage());
    });
    cheats->setHeight(LISTITEM_HEIGHT);
    this->addView(cheats);

    JCcolor = new brls::ListItem("menus/tool_change"_i18n );
    JCcolor->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new JCPage());
    });
    JCcolor->setHeight(LISTITEM_HEIGHT);
    this->addView(JCcolor);

    PCcolor = new brls::ListItem("menus/tool_change_procon"_i18n );
    PCcolor->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new PCPage());
    });
    PCcolor->setHeight(LISTITEM_HEIGHT);
    this->addView(PCcolor);

    downloadPayload = new brls::ListItem("menus/tool_download"_i18n  + std::string(BOOTLOADER_PL_PATH));
    downloadPayload->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new DownloadPayloadPage());
    });
    downloadPayload->setHeight(LISTITEM_HEIGHT);
    this->addView(downloadPayload);

    rebootPayload = new brls::ListItem("menus/tool_inject"_i18n );
    rebootPayload->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new PayloadPage());
    });
    rebootPayload->setHeight(LISTITEM_HEIGHT);
    this->addView(rebootPayload);

    /* ntcp = new brls::ListItem("menus/ntcp"_i18n );
    ntcp->getClickEvent()->subscribe([&](brls::View* view){
        std::string res = syncTime();
        brls::Dialog* dialog = new brls::Dialog(res);
        brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
            dialog->close();
        };
        dialog->addButton("menus/Ok_button"_i18n , callback);
        dialog->setCancelable(true);
        dialog->open();

    });
    ntcp->setHeight(LISTITEM_HEIGHT);
    this->addView(ntcp); */

    netSettings = new brls::ListItem("menus/tool_net_settings"_i18n );
    netSettings->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new NetPage());
    });
    netSettings->setHeight(LISTITEM_HEIGHT);
    this->addView(netSettings);

    browser = new brls::ListItem("menus/tool_browser"_i18n );
    browser->getClickEvent()->subscribe([&](brls::View* view){
        char url[0xc00] = {0};
        SwkbdConfig kbd;
        Result rc = swkbdCreate(&kbd, 0);
        if (R_SUCCEEDED(rc)) {
            swkbdConfigMakePresetDefault(&kbd);
            swkbdConfigSetGuideText(&kbd, "www.cheatslips.com e-mail");
            swkbdConfigSetInitialText(&kbd, "https://duckduckgo.com");
            swkbdShow(&kbd, url, sizeof(url));
            swkbdClose(&kbd);
        }
        else {
            strcpy(url, "https://duckduckgo.com");
        }
        int at = appletGetAppletType();
        std::string error = "";
        if(at == AppletType_Application) { // Running as a title
            WebCommonConfig conf;
            WebCommonReply out;
            rc = webPageCreate(&conf, url);
            if (R_FAILED(rc))
                error += "\uE016 Error starting Browser\n\uE016 Lookup error code for more info " + rc;
            webConfigSetJsExtension(&conf, true);
            webConfigSetPageCache(&conf, true);
            webConfigSetBootLoadingIcon(&conf, true);
            webConfigSetWhitelist(&conf, ".*");
            rc = webConfigShow(&conf, &out);
            if (R_FAILED(rc))
                error += "\uE016 Error starting Browser\n\uE016 Lookup error code for more info " + rc;
        } else { // Running under applet
            error += "\uE016 Running in applet mode.\n\uE016 Please launch hbmenu by holding [R] on a game";
        }
        if(!error.empty()){
            brls::Dialog* dialog = new brls::Dialog(error);
            brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                dialog->close();
            };
            dialog->addButton("menus/Ok_button"_i18n , callback);
            dialog->setCancelable(true);
            dialog->open();
        }
        
    });
    browser->setHeight(LISTITEM_HEIGHT);
    this->addView(browser);

    move = new brls::ListItem("menus/tool_copyFiles"_i18n );
    move->getClickEvent()->subscribe([&](brls::View* view){
        chdir("/");
        std::string error = "";
        if(std::filesystem::exists(MOVE_FILES_JSON)){
            error = copyFiles(MOVE_FILES_JSON);
        }
        else{
            error = "menus/move_files_not_found"_i18n;
        }
        brls::Dialog* dialog = new brls::Dialog(error);
        brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
            dialog->close();
        };
        dialog->addButton("menus/Ok_button"_i18n , callback);
        dialog->setCancelable(true);
        dialog->open();
    });
    move->setHeight(LISTITEM_HEIGHT);
    this->addView(move);

    cleanUp = new brls::ListItem("menus/tool_cleanUp"_i18n );
    cleanUp->getClickEvent()->subscribe([&](brls::View* view){
        std::filesystem::remove(AMS_ZIP_PATH);
        std::filesystem::remove(APP_ZIP_PATH);
        std::filesystem::remove(CFW_ZIP_PATH);
        std::filesystem::remove(FW_ZIP_PATH);
        std::filesystem::remove(CHEATS_ZIP_PATH);
        std::filesystem::remove(SIGPATCHES_ZIP_PATH);
        removeDir(AMS_DIRECTORY_PATH);
        removeDir(SEPT_DIRECTORY_PATH);
        removeDir(FW_DIRECTORY_PATH);
        brls::Dialog* dialog = new brls::Dialog("menus/All_done"_i18n);
        brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
            dialog->close();
        };
        dialog->addButton("menus/Ok_button"_i18n , callback);
        dialog->setCancelable(true);
        dialog->open();
    });
    cleanUp->setHeight(LISTITEM_HEIGHT);
    this->addView(cleanUp);

    hideTabs = new brls::ListItem("menus/hide_tabs"_i18n );
    hideTabs->getClickEvent()->subscribe([&](brls::View* view) {
        brls::Application::pushView(new HideTabsPage());
    });
    hideTabs->setHeight(LISTITEM_HEIGHT);
    this->addView(hideTabs);

    changelog = new brls::ListItem("menus/tool_changelog"_i18n );
    changelog->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new ChangelogPage());
    });
    changelog->setHeight(LISTITEM_HEIGHT);
    this->addView(changelog);

/*     language = new brls::ListItem("menus/Language_Option"_i18n );
    language->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new LanguageOptionPage());
    });
    language->setHeight(LISTITEM_HEIGHT);
    this->addView(language); */
}


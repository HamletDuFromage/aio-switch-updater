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
#include "fs.hpp"
#include "hide_tabs_page.hpp"
#include <filesystem>
#include <fstream>

namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::json;

namespace {
    constexpr const char AppVersion[] = APP_VERSION;
}

ToolsTab::ToolsTab(const std::string& tag, bool erista, const nlohmann::json& hideStatus) : brls::List()
{
    if(!tag.empty() && tag != AppVersion){
        updateApp = new brls::ListItem("menus/tools/update_app"_i18n + tag +")");
        std::string text("menus/tools/dl_app"_i18n + std::string(APP_URL));
        updateApp->getClickEvent()->subscribe([&, text, tag](brls::View* view) {
            brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
            stagedFrame->setTitle("menus/common/updating"_i18n);
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, text)
            );
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, [](){util::downloadArchive(APP_URL, archiveType::app);})
            );
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "menus/common/extracting"_i18n, [tag](){util::extractArchive(archiveType::app, tag);})
            );
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "menus/common/all_done"_i18n, true)
            );
            brls::Application::pushView(stagedFrame);
        });
        updateApp->setHeight(LISTITEM_HEIGHT);
        this->addView(updateApp);
    }

    cheats = new brls::ListItem("menus/tools/cheats"_i18n);
    cheats->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new CheatsPage());
    });
    cheats->setHeight(LISTITEM_HEIGHT);

    JCcolor = new brls::ListItem("menus/tools/joy_cons"_i18n);
    JCcolor->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new JCPage());
    });
    JCcolor->setHeight(LISTITEM_HEIGHT);

    PCcolor = new brls::ListItem("menus/tools/pro_cons"_i18n);
    PCcolor->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new PCPage());
    });
    PCcolor->setHeight(LISTITEM_HEIGHT);

    downloadPayload = new brls::ListItem("menus/tools/dl_payloads"_i18n + std::string(BOOTLOADER_PL_PATH));
    downloadPayload->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new DownloadPayloadPage());
    });
    downloadPayload->setHeight(LISTITEM_HEIGHT);

    rebootPayload = new brls::ListItem("menus/tools/inject_payloads"_i18n);
    rebootPayload->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new PayloadPage());
    });
    rebootPayload->setHeight(LISTITEM_HEIGHT);


    /* ntcp = new brls::ListItem("menus/ntcp"_i18n);
    ntcp->getClickEvent()->subscribe([&](brls::View* view){
        std::string res = syncTime();
        brls::Dialog* dialog = new brls::Dialog(res);
        brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
            dialog->close();
        };
        dialog->addButton("menus/common/ok"_i18n, callback);
        dialog->setCancelable(true);
        dialog->open();

    });
    ntcp->setHeight(LISTITEM_HEIGHT);
    this->addView(ntcp); */

    netSettings = new brls::ListItem("menus/tools/internet_settings"_i18n);
    netSettings->getClickEvent()->subscribe([&](brls::View* view){
        brls::PopupFrame::open("menus/tools/internet_settings"_i18n, new NetPage(), "", "");
    });
    netSettings->setHeight(LISTITEM_HEIGHT);

    browser = new brls::ListItem("menus/tools/browser"_i18n);
    browser->getClickEvent()->subscribe([&](brls::View* view){
        std::string url;
        if (brls::Swkbd::openForText([&](std::string text) { url = text; }, "cheatslips.com e-mail", "", 64, "https://duckduckgo.com", 0, "Submit", "https://website.tld")) {
            std::string error = "";
            int at = appletGetAppletType();
            if(at == AppletType_Application) { // Running as a title
                WebCommonConfig conf;
                WebCommonReply out;
                Result rc = webPageCreate(&conf, url.c_str());
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
                error += "\uE016 Running in applet mode/through a forwarder.\n\uE016 Please launch hbmenu by holding [R] on a game";
            }
            if(!error.empty()){
                brls::Dialog* dialog = new brls::Dialog(error);
                brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                    dialog->close();
                };
                dialog->addButton("menus/common/ok"_i18n, callback);
                dialog->setCancelable(true);
                dialog->open();
            }
        }
        
    });
    browser->setHeight(LISTITEM_HEIGHT);

    move = new brls::ListItem("menus/tools/batch_copy"_i18n);
    move->getClickEvent()->subscribe([&](brls::View* view){
        chdir("/");
        std::string error = "";
        if(std::filesystem::exists(COPY_FILES_TXT)){
            error = fs::copyFiles(COPY_FILES_TXT);
        }
        else{
            error = "menus/tools/batch_copy_config_not_found"_i18n;
        }
        brls::Dialog* dialog = new brls::Dialog(error);
        brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
            dialog->close();
        };
        dialog->addButton("menus/common/ok"_i18n, callback);
        dialog->setCancelable(true);
        dialog->open();
    });
    move->setHeight(LISTITEM_HEIGHT);

    cleanUp = new brls::ListItem("menus/tools/clean_up"_i18n);
    cleanUp->getClickEvent()->subscribe([&](brls::View* view){
        std::filesystem::remove(AMS_ZIP_PATH);
        std::filesystem::remove(APP_ZIP_PATH);
        std::filesystem::remove(CFW_ZIP_PATH);
        std::filesystem::remove(FW_ZIP_PATH);
        std::filesystem::remove(CHEATS_ZIP_PATH);
        std::filesystem::remove(SIGPATCHES_ZIP_PATH);
        fs::removeDir(AMS_DIRECTORY_PATH);
        fs::removeDir(SEPT_DIRECTORY_PATH);
        fs::removeDir(FW_DIRECTORY_PATH);
        brls::Dialog* dialog = new brls::Dialog("menus/common/all_done"_i18n);
        brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
            dialog->close();
        };
        dialog->addButton("menus/common/ok"_i18n, callback);
        dialog->setCancelable(true);
        dialog->open();
    });
    cleanUp->setHeight(LISTITEM_HEIGHT);

    language = new brls::ListItem("menus/tools/language"_i18n);
    language->getClickEvent()->subscribe([&](brls::View* view){
        std::vector<std::pair<std::string, std::string>> languages{
            std::make_pair("menus/language/en-US"_i18n, "en-US"),
            std::make_pair("menus/language/ja"_i18n, "ja"),
            std::make_pair("menus/language/fr"_i18n, "fr"),
            std::make_pair("menus/language/de"_i18n, "de"),
            std::make_pair("menus/language/it"_i18n, "it"),
            std::make_pair("menus/language/es"_i18n, "es"),
            std::make_pair("menus/language/pl"_i18n, "pl"),
            std::make_pair("menus/language/zh-CN"_i18n, "zh-CN"),
            std::make_pair("menus/language/zh-TW"_i18n, "zh-TW"),
            std::make_pair("menus/language/es-419"_i18n, "es-419"),
            std::make_pair("menus/language/zh-Hant"_i18n, "zh-Hant"),
            std::make_pair("menus/language/zh-Hans"_i18n, "zh-Hans")
        };
        brls::AppletFrame* appView = new brls::AppletFrame(true, true);
        brls::List* list = new brls::List();
        brls::ListItem* listItem;
        listItem = new brls::ListItem(fmt::format("{} ({})", "menus/language/system_default"_i18n, i18n::getCurrentLocale()));
        listItem->registerAction("menus/tools/language"_i18n, brls::Key::A, [this] {
            std::filesystem::remove(LANGUAGE_JSON);
            brls::Application::quit();
            return true;
        });
        list->addView(listItem);
        for(auto& l : languages){
            listItem = new brls::ListItem(l.first);
            listItem->registerAction("menus/tools/language"_i18n, brls::Key::A, [this, l] { 
                json updatedLanguage = json::object();
                updatedLanguage["language"] = l.second;
                std::ofstream out(LANGUAGE_JSON);
                out << updatedLanguage.dump();
                brls::Application::quit();
                return true;
            });
            list->addView(listItem);
        }
        appView->setContentView(list);
        brls::PopupFrame::open("menus/tools/language"_i18n, appView, "", "");
    });
    language->setHeight(LISTITEM_HEIGHT);

    hideTabs = new brls::ListItem("menus/tools/hide_tabs"_i18n);
    hideTabs->getClickEvent()->subscribe([&](brls::View* view) {
        brls::PopupFrame::open("menus/tools/hide_tabs"_i18n, new HideTabsPage(), "", "");
    });
    hideTabs->setHeight(LISTITEM_HEIGHT);

    changelog = new brls::ListItem("menus/tools/changelog"_i18n);
    changelog->getClickEvent()->subscribe([&](brls::View* view){
        brls::PopupFrame::open("menus/tools/changelog"_i18n, new ChangelogPage(), "", "");
    });
    changelog->setHeight(LISTITEM_HEIGHT);

    if(!util::getBoolValue(hideStatus, "cheats")) this->addView(cheats);
    if(!util::getBoolValue(hideStatus, "jccolor")) this->addView(JCcolor);
    if(!util::getBoolValue(hideStatus, "pccolor")) this->addView(PCcolor);
    if(!util::getBoolValue(hideStatus, "downloadpayload")) this->addView(downloadPayload);
    if(erista && !util::getBoolValue(hideStatus, "rebootpayload")) this->addView(rebootPayload);
    if(!util::getBoolValue(hideStatus, "netsettings")) this->addView(netSettings);
    if(!util::getBoolValue(hideStatus, "browser")) this->addView(browser);
    if(!util::getBoolValue(hideStatus, "move")) this->addView(move);
    if(!util::getBoolValue(hideStatus, "cleanup")) this->addView(cleanUp);
    if(!util::getBoolValue(hideStatus, "language")) this->addView(language);
    if(!util::getBoolValue(hideStatus, "hidetabs")) this->addView(hideTabs);
    this->addView(changelog);
}


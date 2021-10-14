#include "cheats_page.hpp"

#include "app_page.hpp"
#include "confirm_page.hpp"
#include "constants.hpp"
#include "current_cfw.hpp"
#include "download.hpp"
#include "exclude_page.hpp"
#include "extract.hpp"
#include "utils.hpp"
#include "worker_page.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
CheatsPage::CheatsPage() : AppletFrame(true, true)
{
    this->setTitle("menus/cheats/menu"_i18n);
    list = new brls::List();

    item = new brls::ListItem("menus/cheats/view"_i18n);
    item->getClickEvent()->subscribe([](brls::View* view) {
        brls::Application::pushView(new AppPage_DownloadedCheats());
    });
    list->addView(item);

    item = new brls::ListItem("menus/cheats/exclude"_i18n);
    item->getClickEvent()->subscribe([](brls::View* view) {
        brls::Application::pushView(new AppPage_Exclude());
    });
    list->addView(item);

    item = new brls::ListItem("menus/cheats/delete_existing"_i18n);
    item->getClickEvent()->subscribe([](brls::View* view) {
        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("menus/cheats/delete_existing"_i18n);
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/cheats/deleting"_i18n, []() { extract::removeCheats(); }));
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "menus/common/all_done"_i18n, true));
        brls::Application::pushView(stagedFrame);
    });
    list->addView(item);

    item = new brls::ListItem("menus/cheats/delete_orphaned"_i18n);
    item->getClickEvent()->subscribe([](brls::View* view) {
        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("menus/cheats/delete_orphaned"_i18n);
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/cheats/deleting"_i18n, []() { extract::removeOrphanedCheats(); }));
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "menus/common/all_done"_i18n, true));
        brls::Application::pushView(stagedFrame);
    });
    list->addView(item);

    std::string cheatsVer = util::downloadFileToString(CHEATS_URL_VERSION);
    if (cheatsVer != "") {
        item = new brls::ListItem("menus/cheats/dl_all"_i18n);
        item->getClickEvent()->subscribe([cheatsVer](brls::View* view) {
            std::string url;
            switch (CurrentCfw::running_cfw) {
                case CFW::sxos:
                    url = CHEATS_URL_TITLES;
                    break;
                case CFW::ams:
                    url = CHEATS_URL_CONTENTS;
                    break;
                case CFW::rnx:
                    url = CHEATS_URL_CONTENTS;
                    break;
            }
            std::string text("menus/main/get_cheats"_i18n + cheatsVer + ")" + "menus/common/from"_i18n + url);
            brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
            stagedFrame->setTitle("menus/cheats/dl_all"_i18n);
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, text));
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, [url]() { util::downloadArchive(url, contentType::cheats); }));
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "menus/common/extracting"_i18n, []() { extract::extractAllCheats(CHEATS_ZIP_PATH, CurrentCfw::running_cfw); }));
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "menus/common/all_done"_i18n, true));
            brls::Application::pushView(stagedFrame);
        });
        list->addView(item);
    }
    this->setContentView(list);
}
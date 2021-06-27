#include "cheats_page.hpp"
#include "app_page.hpp"
#include "exclude_page.hpp"
#include "worker_page.hpp"
#include "confirm_page.hpp"
#include "current_cfw.hpp"
#include "extract.hpp"
#include "constants.hpp"
#include "current_cfw.hpp"
#include "download.hpp"
#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
CheatsPage::CheatsPage() : AppletFrame(true, true)
{
    this->setTitle("menus/cheats/menu"_i18n);
    list = new brls::List();

    /* view = new brls::ListItem("menus/cheats/view"_i18n);
    view->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new AppPage_DownloadedCheats());
    });
    list->addView(view); */

    exclude = new brls::ListItem("menus/cheats/exclude"_i18n);
    exclude->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new AppPage_Exclude());
    });
    list->addView(exclude);
 
    deleteCheats = new brls::ListItem("menus/cheats/delete_existing"_i18n);
    deleteCheats->getClickEvent()->subscribe([&](brls::View* view){
        stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("menus/cheats/delete_all"_i18n);
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/cheats/deleting"_i18n, [](){extract::removeCheats(CurrentCfw::running_cfw);})
        );
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "menus/common/all_done"_i18n, true)
        );
        brls::Application::pushView(stagedFrame);
    });
    list->addView(deleteCheats);

    std::string cheatsVer = util::downloadFileToString(CHEATS_URL_VERSION);
    if(cheatsVer != "") {
        dlAll = new brls::ListItem("menus/cheats/dl_all"_i18n);
        dlAll->getClickEvent()->subscribe([&, cheatsVer](brls::View* view) {
            std::string url;
            switch(CurrentCfw::running_cfw){
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
                new ConfirmPage(stagedFrame, text)
            );
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, [url](){util::downloadArchive(url, archiveType::cheats);})
            );
            stagedFrame->addStage(
                new WorkerPage(stagedFrame, "menus/common/extracting"_i18n, [](){extract::extractAllCheats(CHEATS_ZIP_PATH, CurrentCfw::running_cfw);})
            );
            stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "menus/common/all_done"_i18n, true)
            );
            brls::Application::pushView(stagedFrame);
        });
        list->addView(dlAll);
    }
    this->setContentView(list);
}
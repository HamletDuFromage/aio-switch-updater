#include "cheats_page.hpp"
 
namespace i18n = brls::i18n;
using namespace i18n::literals;
CheatsPage::CheatsPage() : AppletFrame(true, true)
{
    this->setTitle("menus/cheat_menu"_i18n );
    list = new brls::List();

    view = new brls::ListItem("menus/cheat_view"_i18n );
    view->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new AppPage());
    });
    list->addView(view);

    exclude = new brls::ListItem("menus/cheat_exclude"_i18n );
    exclude->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new ExcludePage());
    });
    list->addView(exclude);

 
    deleteCheats = new brls::ListItem("menus/cheat_delete_all_ex"_i18n );
    deleteCheats->getClickEvent()->subscribe([&](brls::View* view){
        stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("menus/cheat_delete_all_cheat"_i18n );
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/cheat_Deleting"_i18n  , [](){removeCheats(getCFW());})
        );
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "menus/cheat_All_done"_i18n , true)
        );
        brls::Application::pushView(stagedFrame);
    });
    list->addView(deleteCheats);

    this->setContentView(list);
}
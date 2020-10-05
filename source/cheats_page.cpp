#include "cheats_page.hpp"
#include "lang.hpp"
using namespace lang::literals;
CheatsPage::CheatsPage() : AppletFrame(true, true)
{
    this->setTitle("cheat_menu"_lang);
    list = new brls::List();

    view = new brls::ListItem("cheat_view"_lang);
    view->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new AppPage());
    });
    list->addView(view);

    exclude = new brls::ListItem("cheat_exclude"_lang);
    exclude->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new ExcludePage());
    });
    list->addView(exclude);

 
    deleteCheats = new brls::ListItem("cheat_delete_all_ex"_lang);
    deleteCheats->getClickEvent()->subscribe([&](brls::View* view){
        stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->setTitle("cheat_delete_all_cheat"_lang);
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "cheat_Deleting"_lang , [](){removeCheats(getCFW());})
        );
        stagedFrame->addStage(
            new ConfirmPage(stagedFrame, "cheat_All_done"_lang, true)
        );
        brls::Application::pushView(stagedFrame);
    });
    list->addView(deleteCheats);

    this->setContentView(list);
}
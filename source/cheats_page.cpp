#include "cheats_page.hpp"

CheatsPage::CheatsPage() : AppletFrame(true, true)
{
    this->setTitle("Cheats menu");
    list = new brls::List();

    view = new brls::ListItem("View installed cheats");
    view->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new AppPage());
    });
    list->addView(view);

    exclude = new brls::ListItem("Exclude games from recieving cheat updates");
    exclude->getClickEvent()->subscribe([&](brls::View* view){
        brls::Application::pushView(new ExcludePage());
    });
    list->addView(exclude);


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
    list->addView(deleteCheats);

    this->setContentView(list);
}
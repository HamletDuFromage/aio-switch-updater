#include "ams_tab.hpp"
 
namespace i18n = brls::i18n;
using namespace i18n::literals;

AmsTab::AmsTab() :
    brls::List()
{
    std::tuple<std::vector<std::string>, std::vector<std::string>> links;
    std::string operation = "menus/Getting"_i18n ;
    this->description = new brls::Label(brls::LabelStyle::DESCRIPTION, "", true);
    links = fetchLinks(AMS_URL);
    operation += "menus/list_cfw"_i18n ;
    this->description->setText(
        "menus/list_ams"_i18n 
    );

    this->addView(description);

    

    int nbLinks = std::get<0>(links).size();
    if(nbLinks){
        auto hekate_link = fetchLinks(HEKATE_URL);
        std::string hekate_url = std::get<1>(hekate_link)[0];
        std::string text_hekate = "menus/list_down"_i18n  + std::get<0>(hekate_link)[0];

        linkItems.reserve(nbLinks);
        for (int i = 0; i<nbLinks; i++){
            std::string url = std::get<1>(links)[i];
            std::string text("menus/list_down"_i18n  + std::get<0>(links)[i] + "menus/list_from"_i18n  + url);
            linkItems[i] = new brls::ListItem(std::get<0>(links)[i]);
            linkItems[i]->setHeight(LISTITEM_HEIGHT);
            linkItems[i]->getClickEvent()->subscribe([&, text, text_hekate, url, hekate_url, operation](brls::View* view) {
                brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
                stagedFrame->setTitle(operation);
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, text)
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "menus/list_downing"_i18n , [url](){downloadArchive(url, ams_cfw);})
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "menus/list_extracting"_i18n , [](){extractArchive(ams_cfw);})
                );
                stagedFrame->addStage(
                    new DialoguePage(stagedFrame, text_hekate)
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "menus/list_downing"_i18n , [hekate_url](){downloadArchive(hekate_url, cfw);})
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "menus/list_extracting"_i18n , [](){extractArchive(cfw);})
                );
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, "menus/reboot_rcm"_i18n , false, true)
                );
                brls::Application::pushView(stagedFrame);
            });
            this->addView(linkItems[i]);
        }
    }
    else{
        notFound = new brls::Label(
            brls::LabelStyle::DESCRIPTION,
            "menus/list_could_done"_i18n ,
            true
        );
        notFound->setHorizontalAlign(NVG_ALIGN_CENTER);
        this->addView(notFound);
    }

}

AmsTab::~AmsTab(){
    
}

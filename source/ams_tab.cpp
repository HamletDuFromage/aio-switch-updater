#include "ams_tab.hpp"
#include <string>
#include "download.hpp"
#include "extract.hpp"
#include "confirm_page.hpp"
#include "dialogue_page.hpp"
#include "worker_page.hpp"
#include "utils.hpp"
 
namespace i18n = brls::i18n;
using namespace i18n::literals;

AmsTab::AmsTab() :
    brls::List()
{
    std::vector<std::pair<std::string, std::string>> links;
    std::string operation = "menus/main/getting"_i18n ;
    this->description = new brls::Label(brls::LabelStyle::DESCRIPTION, "", true);
    links = getLinks(AMS_URL);
    operation += "menus/main/cfw"_i18n ;
    this->description->setText(
        "menus/main/ams_text"_i18n 
    );

    this->addView(description);

    

    int nbLinks = links.size();
    if(nbLinks){
        auto hekate_link = getLinks(HEKATE_URL);
        std::string hekate_url = hekate_link[0].second;
        std::string text_hekate = "menus/common/download"_i18n  + hekate_link[0].first;

        for (int i = 0; i < nbLinks; i++){
            std::string url = links[i].second;
            std::string text("menus/common/download"_i18n  + links[i].first + "menus/common/from"_i18n  + url);
            listItem = new brls::ListItem(links[i].first);
            listItem->setHeight(LISTITEM_HEIGHT);
            listItem->getClickEvent()->subscribe([&, text, text_hekate, url, hekate_url, operation](brls::View* view) {
                brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
                stagedFrame->setTitle(operation);
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, text)
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "menus/common/downloading"_i18n , [url](){downloadArchive(url, ams_cfw);})
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "menus/common/extracting"_i18n , [](){extractArchive(ams_cfw);})
                );
                stagedFrame->addStage(
                    new DialoguePage(stagedFrame, text_hekate)
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "menus/common/downloading"_i18n , [hekate_url](){downloadArchive(hekate_url, cfw);})
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "menus/common/extracting"_i18n , [](){extractArchive(cfw);})
                );
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, "menus/ams_update/reboot_rcm"_i18n , false, true)
                );
                brls::Application::pushView(stagedFrame);
            });
            this->addView(listItem);
        }
    }
    else{
        notFound = new brls::Label(
            brls::LabelStyle::DESCRIPTION,
            "menus/main/links_not_found"_i18n ,
            true
        );
        notFound->setHorizontalAlign(NVG_ALIGN_CENTER);
        this->addView(notFound);
    }

}
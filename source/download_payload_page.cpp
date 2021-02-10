#include "download_payload_page.hpp"
#include "utils.hpp"
#include "confirm_page.hpp"
#include "worker_page.hpp"
#include "download.hpp"
 
namespace i18n = brls::i18n;
using namespace i18n::literals;
DownloadPayloadPage::DownloadPayloadPage() : AppletFrame(true, true)
{
    this->setTitle("menus/Download_payloads"_i18n );
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/select"_i18n  + std::string(BOOTLOADER_PL_PATH) + "." ,
        true
    );
    list->addView(label);
    
    auto links =  getLinks(PAYLOAD_URL);
    int nbLinks = links.size();
    if(nbLinks){
        for (int i = 0; i<nbLinks; i++){
            std::string url = links[i].second;
            std::string path = std::string(BOOTLOADER_PL_PATH) + links[i].first;
            std::string text("menus/Download"_i18n  + links[i].first + "menus/from"_i18n + url);
            listItem = new brls::ListItem(links[i].first);
            listItem->getClickEvent()->subscribe([&, text, url, path](brls::View* view) {
                createTree(BOOTLOADER_PL_PATH);
                brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
                stagedFrame->setTitle("menus/getting_paylaod"_i18n );
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, text)
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "menus/down"_i18n , [url, path](){downloadFile(url.c_str(), path.c_str(), OFF);})
                );
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, "menus/download_all_done"_i18n , true)
                );
                brls::Application::pushView(stagedFrame);
            });
            list->addView(listItem);
        }
    }
    else{
        notFound = new brls::Label(
            brls::LabelStyle::DESCRIPTION,
            "menus/description"_i18n ,
            true
        );
        notFound->setHorizontalAlign(NVG_ALIGN_CENTER);
        list->addView(notFound);
        brls::ListItem* back = new brls::ListItem("menus/back"_i18n );
        back->getClickEvent()->subscribe([&](brls::View* view) {
            brls::Application::popView();
        });
    list->addView(back);
    }
    this->setContentView(list);
}
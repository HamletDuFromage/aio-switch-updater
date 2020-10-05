#include "download_payload_page.hpp"
 
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
    std::tuple<std::vector<std::string>, std::vector<std::string>> links =  fetchLinks(PAYLOAD_URL);
    int nbLinks = std::get<0>(links).size();
    if(nbLinks){
        items.reserve(nbLinks);
        for (int i = 0; i<nbLinks; i++){
            std::string url = std::get<1>(links)[i];
            std::string path = std::string(BOOTLOADER_PL_PATH) + std::get<0>(links)[i];
            std::string text("menus/Download"_i18n  + std::get<0>(links)[i] + "menus/from"_i18n + url);
            items[i] = new brls::ListItem(std::get<0>(links)[i]);
            items[i]->getClickEvent()->subscribe([&, text, url, path](brls::View* view) {
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
            list->addView(items[i]);
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
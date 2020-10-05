#include "download_payload_page.hpp"
#include "lang.hpp"
using namespace lang::literals;
DownloadPayloadPage::DownloadPayloadPage() : AppletFrame(true, true)
{
    this->setTitle("Download_payloads"_lang);
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "select"_lang + std::string(BOOTLOADER_PL_PATH) + "'."_lang,
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
            std::string text("Download"_lang + std::get<0>(links)[i] + "from"_lang+ url);
            items[i] = new brls::ListItem(std::get<0>(links)[i]);
            items[i]->getClickEvent()->subscribe([&, text, url, path](brls::View* view) {
                createTree(BOOTLOADER_PL_PATH);
                brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
                stagedFrame->setTitle("getting_paylaod"_lang);
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, text)
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "down"_lang, [url, path](){downloadFile(url.c_str(), path.c_str(), OFF);})
                );
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, "download_all_done"_lang, true)
                );
                brls::Application::pushView(stagedFrame);
            });
            list->addView(items[i]);
        }
    }
    else{
        notFound = new brls::Label(
            brls::LabelStyle::DESCRIPTION,
            "description"_lang,
            true
        );
        notFound->setHorizontalAlign(NVG_ALIGN_CENTER);
        list->addView(notFound);
        brls::ListItem* back = new brls::ListItem("back"_lang);
        back->getClickEvent()->subscribe([&](brls::View* view) {
            brls::Application::popView();
        });
    list->addView(back);
    }
    this->setContentView(list);
}
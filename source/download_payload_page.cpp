#include "download_payload_page.hpp"

DownloadPayloadPage::DownloadPayloadPage() : AppletFrame(true, true)
{
    this->setTitle("Download payloads");
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "Select a payload to download to " + std::string(BOOTLOADER_PL_PATH) + ".",
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
            std::string text("Downloading:\n" + std::get<0>(links)[i] + "\n\nFrom:\n" + url);
            items[i] = new brls::ListItem(std::get<0>(links)[i]);
            items[i]->getClickEvent()->subscribe([&, text, url, path](brls::View* view) {
                createTree(BOOTLOADER_PL_PATH);
                brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
                stagedFrame->setTitle("Getting payload");
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, text)
                );
                stagedFrame->addStage(
                    new WorkerPage(stagedFrame, "Downloading...", [url, path](){downloadFile(url.c_str(), path.c_str(), OFF);})
                );
                stagedFrame->addStage(
                    new ConfirmPage(stagedFrame, "All done!", true)
                );
                brls::Application::pushView(stagedFrame);
            });
            list->addView(items[i]);
        }
    }
    else{
        notFound = new brls::Label(
            brls::LabelStyle::DESCRIPTION,
            "Could not find a download link, make sure the Switch has access to the internet.\n"\
            "If this problem persists, please open an issue on Github",
            true
        );
        notFound->setHorizontalAlign(NVG_ALIGN_CENTER);
        list->addView(notFound);
        brls::ListItem* back = new brls::ListItem("Back");
        back->getClickEvent()->subscribe([&](brls::View* view) {
            brls::Application::popView();
        });
    list->addView(back);
    }
    this->setContentView(list);
}
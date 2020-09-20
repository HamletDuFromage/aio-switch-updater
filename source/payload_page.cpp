#include "payload_page.hpp"

PayloadPage::PayloadPage() : AppletFrame(true, true)
{

    this->setTitle("Reboot menu");
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "Select a payload to reboot to.",
        true
    );
    list->addView(label);
    std::vector<std::string> payloads = fetchPayloads();
    int nbPayloads = payloads.size();
    items.reserve(nbPayloads);
    for (int i = 0; i < nbPayloads; i++){
        std::string payload = payloads[i];
        items[i] = new brls::ListItem(payload);
        items[i]->getClickEvent()->subscribe([&, payload](brls::View* view) {
            reboot_to_payload(payload.c_str());
            brls::Application::popView();
        });
        list->addView(items[i]);
    }
    list->addView(new brls::ListItemGroupSpacing(true));

    shutDown = new brls::ListItem("Shut Down");
    shutDown->getClickEvent()->subscribe([](brls::View* view) {
        shut_down(false);
        brls::Application::popView();
    });
    list->addView(shutDown);

    reboot = new brls::ListItem("Reboot");
    reboot->getClickEvent()->subscribe([](brls::View* view) {
        shut_down(true);
        brls::Application::popView();
    });
    list->addView(reboot);
    this->setContentView(list);

}
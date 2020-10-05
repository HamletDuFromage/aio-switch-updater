#include "payload_page.hpp"
#include "lang.hpp"
using namespace lang::literals;
PayloadPage::PayloadPage() : AppletFrame(true, true)
{
    CFW cfw = getCFW();
    this->setTitle("payload_reboot"_lang);
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "payload_select"_lang,
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
        if(cfw == ams){
            items[i]->registerAction("payload_set"_lang, brls::Key::X, [this, payload] { 
                if(R_SUCCEEDED(CopyFile(payload.c_str(), REBOOT_PAYLOAD_PATH))){
                    brls::Dialog* dialog = new brls::Dialog("payload_success"_lang + payload + "payload_to"_lang + std::string(REBOOT_PAYLOAD_PATH) + "'.");
                    brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                        dialog->close();
                    };
                    dialog->addButton("payload_ok"_lang, callback);
                    dialog->setCancelable(true);
                    dialog->open();
                }
                return true;
            });
        }
        list->addView(items[i]);
        
    }
    list->addView(new brls::ListItemGroupSpacing(true));

    shutDown = new brls::ListItem("payload_shut"_lang);
    shutDown->getClickEvent()->subscribe([](brls::View* view) {
        shut_down(false);
        brls::Application::popView();
    });
    list->addView(shutDown);

    reboot = new brls::ListItem("payload_reboot_2"_lang);
    reboot->getClickEvent()->subscribe([](brls::View* view) {
        shut_down(true);
        brls::Application::popView();
    });
    list->addView(reboot);

    this->setContentView(list);

}
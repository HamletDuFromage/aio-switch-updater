#include "payload_page.hpp"

#include "current_cfw.hpp"
#include "fs.hpp"
#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

PayloadPage::PayloadPage() : AppletFrame(true, true)
{
    this->updateActionHint(brls::Key::B, "");
    this->updateActionHint(brls::Key::PLUS, "");

    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/payloads/select"_i18n,
        true);
    list->addView(label);
    std::vector<std::string> payloads = util::fetchPayloads();
    for (const auto& payload : payloads) {
        std::string payload_path = payload;
        listItem = new brls::ListItem(payload_path);
        listItem->getClickEvent()->subscribe([payload](brls::View* view) {
            util::rebootToPayload(payload);
            brls::Application::popView();
        });
        if (CurrentCfw::running_cfw == CFW::ams) {
            listItem->registerAction("menus/payloads/set_reboot_payload"_i18n, brls::Key::X, [payload_path] {
                std::string res = fs::copyFile(payload_path, REBOOT_PAYLOAD_PATH)
                                      ? "menus/payloads/copy_success"_i18n + payload_path + "menus/payloads/to"_i18n + std::string(REBOOT_PAYLOAD_PATH) + "'."
                                      : "Failed.";
                brls::Dialog* dialog = new brls::Dialog(res);
                brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                    dialog->close();
                };
                dialog->addButton("menus/common/ok"_i18n, callback);
                dialog->setCancelable(true);
                dialog->open();
                return true;
            });
        }
        listItem->registerAction("menus/payloads/set_update_bin"_i18n, brls::Key::Y, [payload] {
            std::string res = fs::copyFile(payload, UPDATE_BIN_PATH)
                                  ? "menus/payloads/copy_success"_i18n + payload + "menus/payloads/to"_i18n + std::string(UPDATE_BIN_PATH) + "'."
                                  : "Failed.";
            brls::Dialog* dialog = new brls::Dialog(res);
            brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                dialog->close();
            };
            dialog->addButton("menus/common/ok"_i18n, callback);
            dialog->setCancelable(true);
            dialog->open();
            return true;
        });
        list->addView(listItem);
    }
    list->addView(new brls::ListItemGroupSpacing(true));

    shutDown = new brls::ListItem("menus/common/shut_down"_i18n);
    shutDown->getClickEvent()->subscribe([](brls::View* view) {
        util::shutDown(false);
        brls::Application::popView();
    });
    list->addView(shutDown);

    reboot = new brls::ListItem("menus/common/reboot"_i18n);
    reboot->getClickEvent()->subscribe([](brls::View* view) {
        util::shutDown(true);
        brls::Application::popView();
    });
    list->addView(reboot);

    this->setContentView(list);
}
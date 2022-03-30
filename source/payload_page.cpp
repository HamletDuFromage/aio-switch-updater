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

    this->list = new brls::List();
    this->label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/payloads/select"_i18n,
        true);
    this->list->addView(this->label);
    std::vector<std::string> payloads = util::fetchPayloads();
    for (const auto& payload : payloads) {
        std::string payload_path = payload;
        this->listItem = new brls::ListItem(payload_path);
        this->listItem->getClickEvent()->subscribe([payload_path](brls::View* view) {
            util::rebootToPayload(payload_path);
            brls::Application::popView();
        });
        if (CurrentCfw::running_cfw == CFW::ams) {
            this->RegisterCopyAction(brls::Key::X, payload_path, REBOOT_PAYLOAD_PATH, "menus/payloads/set_reboot_payload"_i18n);
        }
        this->RegisterCopyAction(brls::Key::Y, payload_path, UPDATE_BIN_PATH, "menus/payloads/set_update_bin"_i18n);
        this->list->addView(this->listItem);
    }
    this->list->addView(new brls::ListItemGroupSpacing(true));

    this->listItem = new brls::ListItem("menus/common/shut_down"_i18n);
    this->listItem->getClickEvent()->subscribe([](brls::View* view) {
        util::shutDown(false);
        brls::Application::popView();
    });
    this->list->addView(this->listItem);

    this->listItem = new brls::ListItem("menus/common/reboot"_i18n);
    this->listItem->getClickEvent()->subscribe([](brls::View* view) {
        util::shutDown(true);
        brls::Application::popView();
    });
    list->addView(this->listItem);

    this->setContentView(list);
}

void PayloadPage::RegisterCopyAction(brls::Key key, const std::string& payload_path, const std::string& payload_dest, const std::string& action_name)
{
    this->listItem->registerAction(action_name, key, [payload_path, payload_dest] {
        std::string res = fs::copyFile(payload_path, payload_dest)
                              ? fmt::format("menus/payloads/copy_success"_i18n, payload_path, payload_dest)
                              : "Failed.";
        util::showDialogBoxInfo(res);
        return true;
    });
}
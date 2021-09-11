#include "hide_tabs_page.hpp"

#include <fstream>
#include <json.hpp>

#include "constants.hpp"
#include "fs.hpp"
#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::json;

HideTabsPage::HideTabsPage() : AppletFrame(true, true)
{
    this->setTitle("menus/hide/title"_i18n);
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/hide/desc"_i18n,
        true);
    list->addView(label);

    json hideStatus = fs::parseJsonFile(HIDE_TABS_JSON);

    about = new brls::ToggleListItem("menus/main/about"_i18n, util::getBoolValue(hideStatus, "about"));
    list->addView(about);

    ams = new brls::ToggleListItem("menus/main/update_ams"_i18n, util::getBoolValue(hideStatus, "atmosphere"));
    list->addView(ams);

    cfws = new brls::ToggleListItem("menus/main/update_bootloaders"_i18n, util::getBoolValue(hideStatus, "cfw"));
    list->addView(cfws);

    sigpatches = new brls::ToggleListItem("menus/main/update_sigpatches"_i18n, util::getBoolValue(hideStatus, "sigpatches"));
    list->addView(sigpatches);

    fws = new brls::ToggleListItem("menus/main/download_firmware"_i18n, util::getBoolValue(hideStatus, "firmwares"));
    list->addView(fws);

    cheats = new brls::ToggleListItem("menus/main/download_cheats"_i18n, util::getBoolValue(hideStatus, "cheats"));
    list->addView(cheats);

    list->registerAction("menus/cheats/exclude_titles_save"_i18n, brls::Key::B, [this] {
        json updatedStatus = json::object();
        updatedStatus["about"] = about->getToggleState();
        updatedStatus["atmosphere"] = ams->getToggleState();
        updatedStatus["cfw"] = cfws->getToggleState();
        updatedStatus["sigpatches"] = sigpatches->getToggleState();
        updatedStatus["firmwares"] = fws->getToggleState();
        updatedStatus["cheats"] = cheats->getToggleState();
        fs::writeJsonToFile(updatedStatus, HIDE_TABS_JSON);
        brls::Application::popView();
        return true;
    });
    this->setContentView(list);
}
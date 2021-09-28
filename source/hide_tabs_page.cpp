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

    jccolor = new brls::ToggleListItem("menus/tools/joy_cons"_i18n, util::getBoolValue(hideStatus, "jccolor"));
    list->addView(jccolor);

    pccolor = new brls::ToggleListItem("menus/tools/pro_cons"_i18n, util::getBoolValue(hideStatus, "pccolor"));
    list->addView(pccolor);

    downloadpayload = new brls::ToggleListItem("menus/tools/dl_payloads"_i18n, util::getBoolValue(hideStatus, "downloadpayload"));
    list->addView(downloadpayload);

    rebootpayload = new brls::ToggleListItem("menus/tools/inject_payloads"_i18n, util::getBoolValue(hideStatus, "rebootpayload"));
    list->addView(rebootpayload);

    netsettings = new brls::ToggleListItem("menus/tools/internet_settings"_i18n, util::getBoolValue(hideStatus, "netsettings"));
    list->addView(netsettings);

    browser = new brls::ToggleListItem("menus/tools/browser"_i18n, util::getBoolValue(hideStatus, "browser"));
    list->addView(browser);

    move = new brls::ToggleListItem("menus/tools/batch_copy"_i18n, util::getBoolValue(hideStatus, "move"));
    list->addView(move);

    cleanup = new brls::ToggleListItem("menus/tools/clean_up"_i18n, util::getBoolValue(hideStatus, "cleanup"));
    list->addView(cleanup);

    language = new brls::ToggleListItem("menus/tools/language"_i18n, util::getBoolValue(hideStatus, "language"));
    list->addView(language);

    atmosphereentries = new brls::ToggleListItem("menus/hide/update_ams_deepsea"_i18n, util::getBoolValue(hideStatus, "atmosphereentries"));
    list->addView(atmosphereentries);

    list->registerAction("menus/cheats/exclude_titles_save"_i18n, brls::Key::B, [this] {
        json updatedStatus = json::object();
        updatedStatus["about"] = about->getToggleState();
        updatedStatus["atmosphere"] = ams->getToggleState();
        updatedStatus["atmosphereentries"] = atmosphereentries->getToggleState();
        updatedStatus["cfw"] = cfws->getToggleState();
        updatedStatus["sigpatches"] = sigpatches->getToggleState();
        updatedStatus["firmwares"] = fws->getToggleState();
        updatedStatus["cheats"] = cheats->getToggleState();
        updatedStatus["jccolor"] = jccolor->getToggleState();
        updatedStatus["pccolor"] = pccolor->getToggleState();
        updatedStatus["downloadpayload"] = downloadpayload->getToggleState();
        updatedStatus["rebootpayload"] = rebootpayload->getToggleState();
        updatedStatus["netsettings"] = netsettings->getToggleState();
        updatedStatus["browser"] = browser->getToggleState();
        updatedStatus["move"] = move->getToggleState();
        updatedStatus["cleanup"] = cleanup->getToggleState();
        updatedStatus["language"] = language->getToggleState();

        fs::writeJsonToFile(updatedStatus, HIDE_TABS_JSON);
        brls::Application::popView();
        return true;
    });
    this->setContentView(list);
}

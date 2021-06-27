#include "hide_tabs_page.hpp"
#include <json.hpp>
#include <fstream>
#include "constants.hpp"
#include "fs.hpp"

 
namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::json;

HideTabsPage::HideTabsPage() : AppletFrame(true, true) {
    this->setTitle("menus/hide/title"_i18n);
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/hide/desc"_i18n,
        true
    );
    list->addView(label);

    json hideStatus = fs::parseJsonFile(HIDE_TABS_JSON);

    bool status = false;
    if(hideStatus.find("about") != hideStatus.end()) {
        status = hideStatus["about"];
    }
    about = new brls::ToggleListItem("menus/main/about"_i18n, status);
    list->addView(about);

    status = false;
    if(hideStatus.find("atmosphere") != hideStatus.end()) {
        status = hideStatus["atmosphere"];
    }
    ams = new brls::ToggleListItem("menus/main/update_ams"_i18n, status);
    list->addView(ams);

    status = false;
    if(hideStatus.find("cfw") != hideStatus.end()) {
        status = hideStatus["cfw"];
    }
    cfws = new brls::ToggleListItem("menus/main/update_bootloaders"_i18n, status);
    list->addView(cfws);

    status = false;
    if(hideStatus.find("sigpatches") != hideStatus.end()) {
        status = hideStatus["sigpatches"];
    }
    sigpatches = new brls::ToggleListItem("menus/main/update_sigpatches"_i18n, status);
    list->addView(sigpatches);

    status = false;
    if(hideStatus.find("firmwares") != hideStatus.end()) {
        status = hideStatus["firmwares"];
    }
    fws = new brls::ToggleListItem("menus/main/download_firmware"_i18n, status);
    list->addView(fws);

    status = false;
    if(hideStatus.find("cheats") != hideStatus.end()) {
        status = hideStatus["cheats"];
    }
    cheats = new brls::ToggleListItem("menus/main/download_cheats"_i18n, status);
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
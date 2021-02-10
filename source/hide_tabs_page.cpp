#include "hide_tabs_page.hpp"
#include "json.hpp"
#include "constants.hpp"
#include <fstream>
 
namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::json;

HideTabsPage::HideTabsPage() : AppletFrame(true, true) {
    this->setTitle("menus/hide_tabs_page"_i18n );
    list = new brls::List();
    label = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/hide_tabs_label"_i18n ,
        true
    );
    list->addView(label);

    json hideStatus;
    std::ifstream hideFile(HIDE_TABS_JSON);

    std::string fileContent((std::istreambuf_iterator<char>(hideFile) ),
                            (std::istreambuf_iterator<char>()    ));

    if(json::accept(fileContent))   hideStatus = json::parse(fileContent);
    else                            hideStatus = json::object();

    bool status = false;
    if(hideStatus.find("about") != hideStatus.end()) {
        status = hideStatus["about"];
    }
    about = new brls::ToggleListItem("menus/main_about"_i18n, status);
    list->addView(about);

    status = false;
    if(hideStatus.find("atmosphere") != hideStatus.end()) {
        status = hideStatus["atmosphere"];
    }
    ams = new brls::ToggleListItem("menus/main_update_ams"_i18n, status);
    list->addView(ams);

    status = false;
    if(hideStatus.find("cfw") != hideStatus.end()) {
        status = hideStatus["cfw"];
    }
    cfws = new brls::ToggleListItem("menus/main_update_cfw"_i18n, status);
    list->addView(cfws);

    status = false;
    if(hideStatus.find("sigpatches") != hideStatus.end()) {
        status = hideStatus["sigpatches"];
    }
    sigpatches = new brls::ToggleListItem("menus/main_update_si"_i18n, status);
    list->addView(sigpatches);

    status = false;
    if(hideStatus.find("firmwares") != hideStatus.end()) {
        status = hideStatus["firmwares"];
    }
    fws = new brls::ToggleListItem("menus/main_firmwares"_i18n, status);
    list->addView(fws);

    status = false;
    if(hideStatus.find("cheats") != hideStatus.end()) {
        status = hideStatus["cheats"];
    }
    cheats = new brls::ToggleListItem("menus/main_cheats"_i18n, status);
    list->addView(cheats);

    list->registerAction("menus/save"_i18n , brls::Key::B, [this] { 
        json updatedStatus = json::object();
        updatedStatus["about"] = about->getToggleState();
        updatedStatus["atmosphere"] = ams->getToggleState();
        updatedStatus["cfw"] = cfws->getToggleState();
        updatedStatus["sigpatches"] = sigpatches->getToggleState();
        updatedStatus["firmwares"] = fws->getToggleState();
        updatedStatus["cheats"] = cheats->getToggleState();
        std::ofstream out(HIDE_TABS_JSON);
        out << updatedStatus.dump(4);
        out.close();
        brls::Application::popView();
        return true;
    });
    this->setContentView(list);
}
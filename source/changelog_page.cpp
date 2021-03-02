#include "changelog_page.hpp"
 
namespace i18n = brls::i18n;
using namespace i18n::literals;
ChangelogPage::ChangelogPage() : AppletFrame(true, true)
{
    this->setTitle("menus/changelog/changelog"_i18n );
    list = new brls::List();
    std::vector<std::string> verTitles;
    std::string change;
    std::vector<std::string> changes;

    verTitles.push_back("v1.0.1");
    changes.push_back("\uE016 Added dialogue box asking about ini files.\n\uE016 Fixed update app link when not connected to the internet.\n\uE016 Minor fixes here and there.");

    verTitles.push_back("v1.0.2");
    changes.push_back("\uE016 Fixed .ini files being handled poorly when installing sigpatches. Now prompts the user if they want to replace hetake_ipl.ini.");

    verTitles.push_back("v1.0.3");
    changes.push_back("\uE016 Fixed progress bar sometimes being stuck when extracting.");

    verTitles.push_back("v1.1.0");
    changes.push_back("\uE016 Added an option to download payloads to '/bootloader/payloads'.\n\uE016 Cleaned up some stuff, made .ini overwriting cleaner.\n");

    verTitles.push_back("v1.1.1");
    changes.push_back("\uE016 Added some safety checks before downloading/extracting.\n\uE016 Added the possibility to copy a payload to '/atmosphere/reboot_payload.bin'\n\uE016 Added changelog in 'Tools'\n");

    verTitles.push_back("v1.1.2" );
    changes.push_back("\uE016 Added GUI to disable cheat updates for specific titles.");

    verTitles.push_back("v1.1.3");
    changes.push_back("\uE016 Now displays the latest installed cheat version.\n\uE016 Now warns in the app title if a new update is available.");

    verTitles.push_back("v1.2.3");
    changes.push_back("\uE016 Now multilingual (thanks to 'github.com/tiansongyu').\n\uE016 Chinese and French are currently supported.");

    verTitles.push_back("v1.2.1");
    changes.push_back("\uE016 Added option to set a payload to /bootloader/update.bin");

    verTitles.push_back("v1.2.2");
    changes.push_back("\uE016 Added japanese localisation (thanks to github.com/yyoossk).");

    verTitles.push_back("v1.2.4");
    changes.push_back("\uE016 Fixed app self-update");

    verTitles.push_back("v1.2.5");
    changes.push_back("\uE016 Improved japanese translation");

    verTitles.push_back("v1.2.6");
    changes.push_back("\uE016 Added spanish translation (thanks to 'https://github.com/sergiou87'");

    verTitles.push_back("v1.3.0");
    changes.push_back("\uE016 Added back the ability to update CFWs");

    verTitles.push_back("v1.3.1");
    changes.push_back("\uE016 Small fixes for bugs causing hangs.");

    verTitles.push_back("v1.3.2");
    changes.push_back("\uE016 Added ability to change the internet color of Pro Controllers.\n\uE016 Added the ability to preserve specific files when updating.\n\uE016 Added the ability to clean up downloaded archives.");

    verTitles.push_back("v1.4.0");
    changes.push_back("\uE016 Added possibility to use pre-set network settings. Add your own in `config/aio-switch-updater/internet.json.\n\uE016 Added web browser.");

    verTitles.push_back("v1.4.1");
    changes.push_back("\uE016 Fixed misleading display in internet settings page.");

    verTitles.push_back("v1.4.2");
    changes.push_back("\uE016 (hopefully) fixed unreliable copy of payloads.");

    verTitles.push_back("v1.4.3");
    changes.push_back("\uE016 Fixed switch not rebooting to hekate payload after updating.");

    verTitles.push_back("v1.5.0");
    changes.push_back("\uE016 Overhauled the app update feature.");

    verTitles.push_back("v1.5.1");
    changes.push_back("\uE016 A few graphical tweaks.");

    verTitles.push_back("v1.5.2");
    changes.push_back("\uE016 Added support for Traditional Chinese (thanks to 'https://github.com/qazrfv1234')");

    verTitles.push_back("v2.0.0");
    changes.push_back("\uE016 Added ability to download cheatslips cheats.\n\uE016 Added way to toggle out tabs.\n\uE016 Various code improvements.");

    verTitles.push_back("v2.0.1");
    changes.push_back("\uE016 Fixed a bug not letting the user click no when asked if they want to install hekate.\n\uE016 Correct a bunch of typos.");

    verTitles.push_back("v2.1.0");
    changes.push_back("\uE016 Switched to a better way to get links.");

    verTitles.push_back("v2.1.1");
    changes.push_back("\uE016 Added a friendly reminder to reboot for new sigpatches to apply.\n\uE016 Changed the cheatslips cheatsheet view.");

    verTitles.push_back("v2.1.2");
    changes.push_back("\uE016 Fixed wronge hekate link.");

    verTitles.push_back("v2.2.0");
    changes.push_back("\uE016 For cheatslips.com, the build ID of a running game will now be fetched, allowing you to access cheatsheets even if the build ID isn't listed in the versions database.");

    verTitles.push_back("v2.2.1");
    changes.push_back("\uE016 Fixed non Atmosphere CFW being wrongly categorised as ams.");

    verTitles.push_back("v2.3.0");
    changes.push_back("\uE016 Added possibility to copy a list of files to custom locations. This may come in handy for trinket/esoteric bootloader users.\n\uE016 Added warning screen on launch.\n\uE016 Few UI tweaks.\n\uE016 Traditional Chinese translation updated (qazrfv1234)");

    verTitles.push_back("v2.3.1");
    changes.push_back("\uE016 Files for batch copy are now copied according to the json order (formerly alphabetical order).");

    verTitles.push_back("v2.3.2");
    changes.push_back("\uE016 Fixed batch copy not copying :^)");

    verTitles.push_back("v2.3.3");
    changes.push_back("\uE016 Hide unsupported features for Mariko Switches");

    verTitles.push_back("v2.3.4");
    changes.push_back("\uE016 Minor fixes, refactored old switch-cheats updater code and changed way to detect CFW");

    verTitles.push_back("v2.3.5");
    changes.push_back("\uE016 Added German localisation (thanks to github.com/Slluxx).\n\uE016 Added option to disable sysmodules prior to updating.\n\uE016 Improved Japanese localisation (thanks to github.com/yyoossk).");

    for(int i = verTitles.size() -1 ; i >= 0; i--){
        listItem = new brls::ListItem(verTitles[i]);
        change = changes[i];
        listItem->getClickEvent()->subscribe([&, change](brls::View* view){
            brls::Dialog* dialog = new brls::Dialog(change);
            brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                dialog->close();
            };
            dialog->addButton("menus/Ok_button"_i18n , callback);
            dialog->setCancelable(true);
            dialog->open();
        });
        list->addView(listItem);
    }
    this->setContentView(list);
}

void ChangelogPage::ShowChangelogContent(const std::string version, const std::string content) {
    listItem = new brls::ListItem(version);
    listItem->getClickEvent()->subscribe([&, content](brls::View* view) {
        brls::AppletFrame* appView = new brls::AppletFrame(true, true);

        brls::PopupFrame::open(version, appView, "", "");
    });
    list->addView(listItem);
}
#include "changelog_page.hpp"

#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
ChangelogPage::ChangelogPage() : AppletFrame(true, true)
{
    this->setTitle("menus/changelog/changelog"_i18n);
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

    verTitles.push_back("v1.1.2");
    changes.push_back("\uE016 Added GUI to disable cheat updates for specific titles.");

    verTitles.push_back("v1.1.3");
    changes.push_back("\uE016 Now displays the latest installed cheat version.\n\uE016 Now warns in the app title if a new update is available.");

    verTitles.push_back("v1.2.3");
    changes.push_back("\uE016 Now multilingual (credits to github.com/tiansongyu).\n\uE016 Chinese and French are currently supported.");

    verTitles.push_back("v1.2.1");
    changes.push_back("\uE016 Added option to set a payload to /bootloader/update.bin");

    verTitles.push_back("v1.2.2");
    changes.push_back("\uE016 Added Japanese localisation (credits to github.com/yyoossk).");

    verTitles.push_back("v1.2.4");
    changes.push_back("\uE016 Fixed app self-update");

    verTitles.push_back("v1.2.5");
    changes.push_back("\uE016 Improved Japanese translation");

    verTitles.push_back("v1.2.6");
    changes.push_back("\uE016 Added spanish translation (credits to github.com/sergiou87");

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
    changes.push_back("\uE016 Added support for Traditional Chinese (credits to github.com/qazrfv1234)");

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

    verTitles.push_back("v2.4.0");
    changes.push_back("\uE016 Added option to extract the entire gbatemp cheats archive.\n\uE016 Shortened load times.\n\uE016 Added German localisation (credits to github.com/Slluxx).\n\uE016 Added option to disable sysmodules prior to updating.\n\uE016 Improved Japanese localisation (credits to github.com/yyoossk).\n\uE016 Added current AMS version display.");

    verTitles.push_back("v2.4.1");
    changes.push_back("\uE016 Added information about download progress.");

    verTitles.push_back("v2.4.2");
    changes.push_back("\uE016 Added option to manually change language.\n\uE016 Fixed broken strings.\n\uE016 Few visual tweaks.");

    verTitles.push_back("v2.4.3");
    changes.push_back("\uE016 Added way to select system default in language selection.\n\uE016 Display local IP in internet settings.\n\uE016 Added Polish localisation (credits to github.com/teddy74eva).\n\uE016 Updated Traditional Chinese localisation (credits to https://github.com/qazrfv1234).\n\uE016 Minor code changes.");

    verTitles.push_back("v2.4.4");
    changes.push_back("\uE016 Significantly reduced time to retrieve build IDs.\n\uE016 Updated Polish localisation (credits to github.com/teddy74eva).");

    verTitles.push_back("v2.4.5-fix_ams");
    changes.push_back("\uE016 Atmosphère 0.19+ support. Note that this version assumes your switch is running Atmosphère. I should fix this soon, but if you're on SXOS, please stay on 2.4.4.");

    verTitles.push_back("v2.4.6");
    changes.push_back("\uE016 Added back support for SXOS.\n\uE016 Removed dialogue asking to update Hekate if downloading DeepSea.\n\uE016 Fixed incorrect description in inject payload menu.");

    verTitles.push_back("v2.4.7");
    changes.push_back("\uE016 Fixed app only working on 0.19 AMS.\n\uE016 Proper replacement of stratosphere.romfs when updating.");

    verTitles.push_back("v2.4.8");
    changes.push_back("\uE016 Fixed crashes when trying to focus empty lists (typically when in airplane mode).");

    verTitles.push_back("v2.4.9");
    changes.push_back("\uE016 Fixed some read-only files not being properly extracted thus preventing updating.");

    verTitles.push_back("v2.5.0");
    changes.push_back("\uE016 Added ability to update Atmosphère for Mariko (ie patched) Switches. Thanks to SkyDark for testing it out.\n\uE016 Fixed batch copy to be able to copy the same file multiple times (see https://git.io/aiosu_copyfiles). Existing copy_files.json files won't work anymore.");

    verTitles.push_back("v2.5.1");
    changes.push_back("\uE016 Fixed bug copying Mariko payloads on Erista.");

    verTitles.push_back("v2.5.2");
    changes.push_back("\uE016 Updated Japanese and Traditional Chinese translations (credits to github.com/yyoossk and github.com/qazrfv1234).\n\uE016 Updated payload so that it doesn't throw errors if the bootloader folder is missing.\n\uE016 Minor code changes that may or may not result in performace improvements.");

    verTitles.push_back("v2.5.3");
    changes.push_back("\uE016 Check available size before downloading and extracting files.\n\uE016 Now displaying a message when the custom payload is running.");

    verTitles.push_back("v2.6.0");
    changes.push_back("\uE016 Added ability to download individual cheat codes from the GBAtemp.net archive.\n\uE016 Updated Polish translation (credits to github.com/teddy74eva).");

    verTitles.push_back("v2.6.1");
    changes.push_back("\uE016 Added a safer way to inject payloads (should prevent corruption on exFAT system, not that you should be using exFAT)");

    verTitles.push_back("v2.7.0");
    changes.push_back("\uE016 Fixed Atmosphère updates failing when updating from a system without stratosphere.romfs present on the SD card.\n\uE016 Disabled the option to download individual cheat codes while in applet mode, except for the one game currently running.\n\uE016 UI tweaks.\n\uE016 Prevents crashes when requesting invalid CheatSlips cheat sheets.");

    verTitles.push_back("v2.8.0");
    changes.push_back("\uE016 Restored and improved \"View installed cheats\" menu. You can now browse the content of your cheatsheets from the app.");

    verTitles.push_back("v2.9.0");
    changes.push_back("\uE016 Added support for Team-Neptune's custom pack builder.");

    verTitles.push_back("v2.10.0");
    changes.push_back("\uE016 Added support custom packs through a json file.");

    verTitles.push_back("v2.10.1");
    changes.push_back("\uE016 Fixed crashes in airplane mode introduced in 2.10.0");

    verTitles.push_back("v2.11.0");
    changes.push_back("\uE016 Borealis changes for visual tweaks (tickering labels, scroll bar etc).\n\uE016 When needed, issue a warning about custom themes after downloading a new firmware.\n\uE016 Abort AMS update process for Mariko Switches when payload.bin cannot be found.");

    verTitles.push_back("v2.11.1");
    changes.push_back("\uE016 Added ability to view existing cheats when downloading cheat codes/sheets.\n\uE016 Fixed wrong tid for theme detection.");

    verTitles.push_back("v2.11.2");
    changes.push_back("\uE016 Fixed crash when downloading cheats in applet mode.\n\uE016 Fixed progress bar percentage not showing past 10%.");

    verTitles.push_back("v2.11.3");
    changes.push_back("\uE016 Don't download the cheat archive when already cached.\n\uE016 Added ability to hide more entries through hide_tabs.json.\n\uE016 Better support for non-UTF-8 characters.");

    verTitles.push_back("v2.11.4");
    changes.push_back("\uE016 Fix Traditional Chinese strings not showing.\n\uE016 Remove full RAM access check for firmware downloads.");

    verTitles.push_back("v2.11.5");
    changes.push_back("\uE016 Update Japanese translation.\n\uE016 Do not show theme warning when downloading something that is not firmware files.");

    verTitles.push_back("v2.12.0");
    changes.push_back("\uE016 Prepare for AMS 1.0.0.\n\uE016 Updated Japanese and French translations.\n\uE016 Small bugfixes.");

    verTitles.push_back("v2.12.1");
    changes.push_back("\uE016 Fix crashes in exclude page.");

    verTitles.push_back("v2.13.0");
    changes.push_back("\uE016 Added option to delete all cheat files for a specific game.\n\uE016 Now blocks the home menu during downloads/extraction, and add an option to cancel it.\n\uE016 Updated traditional Chinese translations.");

    verTitles.push_back("v2.14.0");
    changes.push_back("\uE016 Add option to launch Daybreak after downloading a sysupdate.\n\uE016 Download all links at once to cut down on boot times.\n\uE016 Randomize IPs for lan-play in the internet settings page.\n\uE016 Fix default color scheme for pro-cons.");

    verTitles.push_back("v2.15.0");
    changes.push_back("\uE016 Implement cheat codes support for multi-content games.\n\uE016 Move payload downloads to bootloader tab.");

    verTitles.push_back("v2.16.0");
    changes.push_back("\uE016 Ask user about replacing reboot_payload.bin with hekate when updating hekate.\n\uE016 Added option to deleted orphaned cheats.\n\uE016 UX tweaks.");

    verTitles.push_back("v2.16.1");
    changes.push_back("\uE016 Updated Japanese localisation (https://github.com/yyoossk).\n\uE016 Updated German localisation (https://github.com/MSco).\n\uE016 Added ability to manually launch aiosu_rcm.bin.");

    verTitles.push_back("v2.16.2");
    changes.push_back("\uE016 Allow offline extraction of cheats.\n\uE016 Fixed extraction of the the complete cheat archive.\n\uE016 Fixed display bug for cheats extraction.");

    verTitles.push_back("v2.16.3");
    changes.push_back("\uE016 Fetch default profiles online for joy-con and pro-con color swaps.\n\uE016 Updated French localization (https://github.com/NotaInutilis).");

    verTitles.push_back("v2.17.0");
    changes.push_back("\uE016 Add ability to download a basic hekate_ipl file.\n\uE016 Add error messages when cheats are not found for a game.");

    verTitles.push_back("v2.17.1");
    changes.push_back("\uE016 Add disclaimer when cheat codes are found for a different game version.\n\uE016 Improve French and Japanese localization.");

    verTitles.push_back("v2.18.0");
    changes.push_back("\uE016 Add mega.nz support (https://github.com/aedalzotto).\n\uE016 Add Korean localization (https://github.com/DDinghoya).\n\uE016 Improve Spanish localization (https://github.com/Armi-Heavy).");

    verTitles.push_back("v2.18.1");
    changes.push_back("\uE016 Fix some pop-up related bugs.");

    verTitles.push_back("v2.19.0");
    changes.push_back("\uE016 Added ability to view games with missing updates.\n\uE016 Updated Japanese localisation (https://github.com/yyoossk).\n\uE016 Updated Chinese localisation (https://github.com/Physton).");

    verTitles.push_back("v2.19.1");
    changes.push_back("\uE016 Fixed crash when trying to download cheats in applet mode.\n\uE016 Updated Japanese localisation (https://github.com/yyoossk).\n\uE016 Updated Chinese localisation (https://github.com/qazrfv1234).");

    verTitles.push_back("v2.19.2");
    changes.push_back("\uE016 Fixed Japanese locale.");

    verTitles.push_back("v2.19.3");
    changes.push_back("\uE016 Fix wrong bid for titles overriden by HBL.\n\uE016 Improve Korean localisation (https://github.com/DDinghoya).\n\uE016 Improve Italian localisation (https://github.com/clamintus).");

    verTitles.push_back("v2.20.0");
    changes.push_back("\uE016 Significantly increase extraction speed (https://github.com/PoloNX).\n\uE016 Create a \"Custom Downloads\" tab that supports user-provided links for Atmosphère packs as well as regular downloads.");


    for (int i = verTitles.size() - 1; i >= 0; i--) {
        listItem = new brls::ListItem(verTitles[i]);
        change = changes[i];
        listItem->getClickEvent()->subscribe([change](brls::View* view) {
            util::showDialogBoxInfo(change);
        });
        list->addView(listItem);
    }
    this->setContentView(list);
}

void ChangelogPage::ShowChangelogContent(const std::string version, const std::string content)
{
    listItem = new brls::ListItem(version);
    listItem->getClickEvent()->subscribe([version, content](brls::View* view) {
        brls::AppletFrame* appView = new brls::AppletFrame(true, true);

        brls::PopupFrame::open(version, appView, "", "");
    });
    list->addView(listItem);
}
#include "changelog_page.hpp"

ChangelogPage::ChangelogPage() : AppletFrame(true, true)
{
    this->setTitle("Changelog");
    list = new brls::List();
    std::vector<std::string> verTitles;
    std::string change;
    std::vector<std::string> changes;

    verTitles.push_back("v1.0.1");
    changes.push_back("\uE016  Added dialogue box asking about ini files.\n"\
    "\uE016  Fixed update app link when not connected to the internet.\n"\
    "\uE016  Minor fixes here and there.");

    verTitles.push_back("v1.0.2");
    changes.push_back("\uE016  Fixed .ini files being handled poorly when installing sigpatches. Now prompts the user if they want to replace hetake_ipl.ini.");

    verTitles.push_back("v1.0.3");
    changes.push_back("\uE016  Fixed progress bar sometimes being stuck when extracting.");

    verTitles.push_back("v1.1.0");
    changes.push_back("\uE016  Added an option to download payloads to '/bootloader/payloads'.\n"\
    "\uE016  Cleaned up some stuff, made .ini overwriting cleaner.\n");

    verTitles.push_back("v1.1.1");
    changes.push_back("\uE016  Added some safety checks before downloading/extracting.\n"\
    "\uE016  Added the possibility to copy a payload to '/atmosphere/reboot_payload.bin'\n"\
    "\uE016  Added changelog in 'Tools'\n");

    verTitles.push_back("v1.1.2");
    changes.push_back("\uE016  Added GUI to disable cheat updates for specific titles.");

    verTitles.push_back("v1.1.3");
    changes.push_back("\uE016  Now displays the latest installed cheat version.\n"\
    "\uE016  Now warns if in the app title if a new update is available.");

    int nbVersions = verTitles.size();
    items.reserve(nbVersions);
    for(int i = nbVersions -1 ; i >= 0; i--){
        items[i] = new brls::ListItem(verTitles[i]);
        change = changes[i];
        items[i]->getClickEvent()->subscribe([&, change](brls::View* view){
            brls::Dialog* dialog = new brls::Dialog(change);
            brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
                dialog->close();
            };
            dialog->addButton("Ok", callback);
            dialog->setCancelable(true);
            dialog->open();
        });
        list->addView(items[i]);
    }
    this->setContentView(list);
}
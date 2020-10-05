#include "changelog_page.hpp"
#include "lang.hpp"
using namespace lang::literals;
ChangelogPage::ChangelogPage() : AppletFrame(true, true)
{
    this->setTitle("Changelog"_lang);
    list = new brls::List();
    std::vector<std::string> verTitles;
    std::string change;
    std::vector<std::string> changes;

    verTitles.push_back("v1_0_1"_lang);
    changes.push_back("v1_0_1_text"_lang);

    verTitles.push_back("v1_0_2"_lang);
    changes.push_back("v1_0_2_text"_lang);

    verTitles.push_back("v1_0_3"_lang);
    changes.push_back("v1_0_3_text"_lang);

    verTitles.push_back("v1_1_0"_lang);
    changes.push_back("v1_1_0_text"_lang);

    verTitles.push_back("v1_1_1"_lang);
    changes.push_back("v1_1_1_text"_lang);

    verTitles.push_back("v1_1_2"_lang);
    changes.push_back("v1_1_2_text"_lang);

    verTitles.push_back("v1_1_3"_lang);
    changes.push_back("v1_1_3_text"_lang);

    verTitles.push_back("v1_2_0"_lang);
    changes.push_back("v1_2_0_text"_lang);

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
            dialog->addButton("Ok_button"_lang, callback);
            dialog->setCancelable(true);
            dialog->open();
        });
        list->addView(items[i]);
    }
    this->setContentView(list);
}
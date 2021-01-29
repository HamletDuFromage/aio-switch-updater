#include "changelog_page.hpp"
 
namespace i18n = brls::i18n;
using namespace i18n::literals;
ChangelogPage::ChangelogPage() : AppletFrame(true, true)
{
    this->setTitle("menus/Changelog"_i18n );
    list = new brls::List();
    std::vector<std::string> verTitles;
    std::string change;
    std::vector<std::string> changes;

    verTitles.push_back("menus/v1_0_1"_i18n );
    changes.push_back("menus/v1_0_1_text"_i18n );

    verTitles.push_back("menus/v1_0_2"_i18n );
    changes.push_back("menus/v1_0_2_text"_i18n );

    verTitles.push_back("menus/v1_0_3"_i18n );
    changes.push_back("menus/v1_0_3_text"_i18n );

    verTitles.push_back("menus/v1_1_0"_i18n );
    changes.push_back("menus/v1_1_0_text"_i18n );

    verTitles.push_back("menus/v1_1_1"_i18n );
    changes.push_back("menus/v1_1_1_text"_i18n );

    verTitles.push_back("menus/v1_1_2"_i18n );
    changes.push_back("menus/v1_1_2_text"_i18n );

    verTitles.push_back("menus/v1_1_3"_i18n );
    changes.push_back("menus/v1_1_3_text"_i18n );

    verTitles.push_back("menus/v1_2_0"_i18n );
    changes.push_back("menus/v1_2_0_text"_i18n );

    verTitles.push_back("menus/v1_2_1"_i18n );
    changes.push_back("menus/v1_2_1_text"_i18n );

    verTitles.push_back("menus/v1_2_2"_i18n );
    changes.push_back("menus/v1_2_2_text"_i18n );

    verTitles.push_back("menus/v1_2_4"_i18n );
    changes.push_back("menus/v1_2_4_text"_i18n );

    verTitles.push_back("menus/v1_2_5"_i18n );
    changes.push_back("menus/v1_2_5_text"_i18n );

    verTitles.push_back("menus/v1_2_6"_i18n );
    changes.push_back("menus/v1_2_6_text"_i18n );

    verTitles.push_back("menus/v1_3_0"_i18n );
    changes.push_back("menus/v1_3_0_text"_i18n );

    verTitles.push_back("menus/v1_3_1"_i18n );
    changes.push_back("menus/v1_3_1_text"_i18n );

    verTitles.push_back("menus/v1_3_2"_i18n );
    changes.push_back("menus/v1_3_2_text"_i18n );

    verTitles.push_back("menus/v1_4_0"_i18n );
    changes.push_back("menus/v1_4_0_text"_i18n );

    verTitles.push_back("menus/v1_4_1"_i18n );
    changes.push_back("menus/v1_4_1_text"_i18n );

    verTitles.push_back("menus/v1_4_2"_i18n );
    changes.push_back("menus/v1_4_2_text"_i18n );

    verTitles.push_back("menus/v1_4_3"_i18n );
    changes.push_back("menus/v1_4_3_text"_i18n );

    verTitles.push_back("menus/v1_5_0"_i18n );
    changes.push_back("menus/v1_5_0_text"_i18n );

    verTitles.push_back("menus/v1_5_1"_i18n );
    changes.push_back("menus/v1_5_1_text"_i18n );

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
            dialog->addButton("menus/Ok_button"_i18n , callback);
            dialog->setCancelable(true);
            dialog->open();
        });
        list->addView(items[i]);
    }
    this->setContentView(list);
}
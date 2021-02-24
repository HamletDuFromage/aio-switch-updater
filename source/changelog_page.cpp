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

    verTitles.push_back("menus/v1_5_2"_i18n );
    changes.push_back("menus/v1_5_2_text"_i18n );

    verTitles.push_back("menus/v2_0_0"_i18n );
    changes.push_back("menus/v2_0_0_text"_i18n );

    verTitles.push_back("menus/v2_0_1"_i18n );
    changes.push_back("menus/v2_0_1_text"_i18n );

    verTitles.push_back("v2.1.0");
    changes.push_back("menus/v2_1_0_text"_i18n );

    verTitles.push_back("v2.1.1");
    changes.push_back("menus/v2_1_1_text"_i18n );

    verTitles.push_back("v2.1.2");
    changes.push_back("menus/v2_1_2_text"_i18n );

    verTitles.push_back("v2.2.0");
    changes.push_back("menus/v2_2_0_text"_i18n );

    verTitles.push_back("v2.2.1");
    changes.push_back("menus/v2_2_1_text"_i18n );

    verTitles.push_back("v2.3.0");
    changes.push_back("menus/v2_3_0_text"_i18n );

    verTitles.push_back("v2.3.1");
    changes.push_back("menus/v2_3_1_text"_i18n );

    verTitles.push_back("v2.3.2");
    changes.push_back("menus/v2_3_2_text"_i18n );

    verTitles.push_back("v2.3.3");
    changes.push_back("menus/v2_3_3_text"_i18n );

    verTitles.push_back("v2.3.4");
    changes.push_back("menus/v2_3_4_text"_i18n );

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
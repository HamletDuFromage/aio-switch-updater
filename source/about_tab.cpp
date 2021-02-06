#include "about_tab.hpp"
 

namespace i18n = brls::i18n;
using namespace i18n::literals;

AboutTab::AboutTab()
{
    // Subtitle
    brls::Label *subTitle = new brls::Label(
        brls::LabelStyle::REGULAR,
        "menus/About_Title"_i18n , 
        true
    );
    subTitle->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(subTitle);

    // Copyright
    brls::Label *copyright = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/copyright"_i18n,
        true
    );
    copyright->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(copyright);

    // Links
    this->addView(new brls::Header("Disclaimers"));
    brls::Label *links = new brls::Label(
        brls::LabelStyle::SMALL,
        "menus/Disclaimers"_i18n + "\n" + "menus/donate"_i18n,
        true
    );
    this->addView(links);
}
#include "about_tab.hpp"
#include "lang.hpp"
using namespace lang::literals;

AboutTab::AboutTab()
{
    // Subtitle
    brls::Label *subTitle = new brls::Label(
        brls::LabelStyle::REGULAR,
        "About_Title"_lang , 
        true
    );
    subTitle->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(subTitle);

    // Copyright
    brls::Label *copyright = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "copyright"_lang,
        true
    );
    copyright->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(copyright);

    // Links
    this->addView(new brls::Header("Disclaimers"));
    brls::Label *links = new brls::Label(
        brls::LabelStyle::SMALL,
        "Disclaimers"_lang,
        true
    );
    this->addView(links);
}

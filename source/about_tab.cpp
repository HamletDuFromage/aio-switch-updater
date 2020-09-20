#include "about_tab.hpp"

AboutTab::AboutTab()
{
    // Subtitle
    brls::Label *subTitle = new brls::Label(
        brls::LabelStyle::REGULAR,
        "All-in-One Nintendo Switch Updater", 
        true
    );
    subTitle->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(subTitle);

    // Copyright
    brls::Label *copyright = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "AIO-switch-updater is licensed under GPL-3.0\n" \
        "\u00A9 2020 HamletDuFromage",
        true
    );
    copyright->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(copyright);

    // Links
    this->addView(new brls::Header("Disclaimers"));
    brls::Label *links = new brls::Label(
        brls::LabelStyle::SMALL,
        "\uE016  Aside from cheat codes that are mirrored from the main Gbatemp thread, "\
        "HamletDuFromage isn't hosting anything. All credits go to respective owners\n"\
        "\uE016  Links are refreshed every three hours. If a link remains broken after 3 hours have passed, please open a Github issue\n",
        true
    );
    this->addView(links);
}

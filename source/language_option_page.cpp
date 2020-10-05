#include "language_option_page.hpp"
 
#include <cstdio>
namespace i18n = brls::i18n;
using namespace i18n::literals;

//this page is for language page 

LanguageOptionPage::LanguageOptionPage() : AppletFrame(true, true)
{ 
    /* this->setTitle("menus/Language_Option"_i18n );
    list = new brls::List();
    
    English = new brls::ListItem("English");
    English->getClickEvent()->subscribe([&](brls::View* view){

        nlohmann::json json_file;
        json_file["language"]=(int)lang::Language::English;
        std::ofstream o(APP_LANG);
        o<<std::setw(4)<<json_file<<std::endl;

        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "menus/reset_machine"_i18n , true)
            );
        brls::Application::pushView(stagedFrame);

    });
    list->addView(English);
    Chinese = new brls::ListItem("中文");
    Chinese->getClickEvent()->subscribe([&](brls::View* view){
        nlohmann::json json_file;
        json_file["language"]=(int)lang::Language::Chinese;
        std::ofstream o(APP_LANG);
        o<<std::setw(4)<<json_file<<std::endl;

        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "menus/reset_machine"_i18n , true)
            );
        brls::Application::pushView(stagedFrame);
    });
    list->addView(Chinese);

    French = new brls::ListItem("Français");
    French->getClickEvent()->subscribe([&](brls::View* view){
        nlohmann::json json_file;
        json_file["language"]=(int)lang::Language::French;
        std::ofstream o(APP_LANG);
        o<<std::setw(4)<<json_file<<std::endl;

        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "menus/reset_machine"_i18n , true)
            );
        brls::Application::pushView(stagedFrame);
    });
    list->addView(French);
    this->setContentView(list); */
}
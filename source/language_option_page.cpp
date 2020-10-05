#include "language_option_page.hpp"
#include "lang.hpp"
#include <cstdio>
using namespace lang::literals;

//this page is for language page 

LanguageOptionPage::LanguageOptionPage() : AppletFrame(true, true)
{ 
    this->setTitle("Language_Option"_lang);
    list = new brls::List();
    
    English = new brls::ListItem("English");
    English->getClickEvent()->subscribe([&](brls::View* view){

        nlohmann::json json_file;
        json_file["language"]=(int)lang::Language::English;
        std::ofstream o(APP_LANG);
        o<<std::setw(4)<<json_file<<std::endl;

        brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
        stagedFrame->addStage(
                new ConfirmPage(stagedFrame, "reset_machine"_lang, true)
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
                new ConfirmPage(stagedFrame, "reset_machine"_lang, true)
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
                new ConfirmPage(stagedFrame, "reset_machine"_lang, true)
            );
        brls::Application::pushView(stagedFrame);
    });
    list->addView(French);
    this->setContentView(list);
}
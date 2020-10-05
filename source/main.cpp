#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <switch.h>

#include <borealis.hpp>

#include "main_frame.hpp"
#include "constants.hpp"
#include "utils.hpp"
 

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <unistd.h>
#include <dirent.h>

#include <sstream>
#include<fstream>

namespace i18n = brls::i18n;
using namespace i18n::literals;

int main(int argc, char* argv[])
{
    // Init the app
    if (!brls::Application::init(APP_TITLE))
    {
        brls::Logger::error("Unable to init Borealis application");
        return EXIT_FAILURE;
    }
    i18n::loadTranslations();
    
    // Setup verbose logging on PC
#ifndef __SWITCH__
    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
#endif

    // Initialize services with a PC shim
    setsysInitialize();
    plInitialize(PlServiceType_User);
    romfsInit();

    nsInitialize();
    socketInitializeDefault();
    nxlinkStdio();

    splInitialize();
    createTree(CONFIG_PATH);


    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
    brls::Logger::debug("Start");


/*     //start code by tiansongyu
    if(std::filesystem::exists(APP_LANG))
    {
        std::ifstream i(APP_LANG);
        nlohmann::json lang_json;
        i>>lang_json;
        int tmp_number =lang_json["language"];
        lang::set_language((lang::Language)(tmp_number));
    }
    else 
    {
        //init system language automatically
        if (auto rc = lang::initialize_to_system_language(); R_FAILED(rc))
            brls::Logger::debug("Failed to init language: %#x, will fall back to key names\n", rc);
        int language_number = (int)lang::get_current_language();
        nlohmann::json json_file;
        json_file["language"]=language_number;
        std::ofstream o(APP_LANG);
        o<<std::setw(4)<<json_file<<std::endl;
    }
    //end by tiansongyu  */
    // Create root view
    MainFrame *mainFrame = new MainFrame();

    // Add the root view to the stack
    brls::Application::pushView(mainFrame);

    // Run the app
    while (brls::Application::mainLoop());

    // Exit
    splExit();
    socketExit();
    nsExit();
    setsysExit();
    plExit();
    romfsExit();
    return EXIT_SUCCESS;
}

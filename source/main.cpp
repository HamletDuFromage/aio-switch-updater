#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <switch.h>

#include <borealis.hpp>

#include "main_frame.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include "lang.hpp"

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <unistd.h>
#include <dirent.h>

#include <sstream>
#include<fstream>
int main(int argc, char* argv[])
{
    // Init the app
    if (!brls::Application::init(APP_TITLE))
    {
        brls::Logger::error("Unable to init Borealis application");
        return EXIT_FAILURE;
    }

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


    //start code by tiansongyu please clean the code if you want
    //checkout the /switch/AIO-switch-updater/config.ini
    //save the language mode
    //or you can use this
    /*
            if (auto rc = lang::initialize_to_system_language(); R_FAILED(rc))
            brls::Logger::debug("Failed to init language: %#x, will fall back to key names\n", rc);
    */
   //this code will automatically choose the machine language that the user use
   //there are two ways to set the language  . choose that you like way :D
    const char* switch_dir= "/switch/";
    if(opendir(switch_dir)==NULL)
        {
            mkdir(switch_dir,0755);
        }
    const char* aio_config_file = "/switch/AIO-switch-updater/";
        if(opendir(aio_config_file)==NULL)
        {
            mkdir(aio_config_file,0755);
        }
    if(access("/switch/AIO-switch-updater/config.ini",F_OK) ==0)
    {
        std::ifstream i("/switch/AIO-switch-updater/config.ini");
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
        std::ofstream o("/switch/AIO-switch-updater/config.ini");
        o<<std::setw(4)<<json_file<<std::endl;
    }
    //end by tiansongyu 
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

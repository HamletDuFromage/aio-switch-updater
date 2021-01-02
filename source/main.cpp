//#include <stdio.h>
//#include <stdlib.h>
#include <string>
#include <switch.h>

#include <borealis.hpp>

#include "main_frame.hpp"
#include "constants.hpp"
#include "utils.hpp"
 

//#include <string.h>
//#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <unistd.h>
#include <dirent.h>

#include <sstream>
#include <fstream>

namespace i18n = brls::i18n;
using namespace i18n::literals;

//TimeServiceType __nx_time_service_type = TimeServiceType_System;

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

    nsInitialize();
    socketInitializeDefault();
    nxlinkStdio();

    splInitialize();

    romfsInit();
    createTree(CONFIG_PATH);

    std::string appName = std::string(argv[0]).substr(5);
    bool star = false;
    //if(1){
    if(appName.find(APP_PATH) != std::string::npos){
        for(auto& p : std::filesystem::directory_iterator(APP_PATH)){
            if(p.path().extension().string() == ".nro" && p.path().string() != appName){
                std::cout << p.path().string() << std::endl;
                std::filesystem::remove(p.path());
            }
            if(p.path().extension().string() == ".star"){
                std::filesystem::remove(p.path());
                star = true;
            }
        }
    }
    if(star)  {
        std::ofstream starFile(std::string(APP_PATH) + ".aio-switch-updater-v" + APP_VERSION + ".nro.star", std::ofstream::out);
    }

    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
    brls::Logger::debug("Start");

    // Create root view
    MainFrame *mainFrame = new MainFrame();

    // Add the root view to the stack
    brls::Application::pushView(mainFrame);

    // Run the app
    while (brls::Application::mainLoop());

    // Exit
    romfsExit();
    splExit();
    socketExit();
    nsExit();
    setsysExit();
    plExit();
    return EXIT_SUCCESS;
}

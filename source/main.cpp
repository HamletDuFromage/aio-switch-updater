//#include <stdio.h>
//#include <stdlib.h>
#include <string>
#include <switch.h>
#include <borealis.hpp>
#include "main_frame.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include "current_cfw.hpp"
#include "warning_page.hpp"
#include <filesystem>

namespace i18n = brls::i18n;
using namespace i18n::literals;

//TimeServiceType __nx_time_service_type = TimeServiceType_System;

const CFW running_cfw = getCFW();

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

    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
    brls::Logger::debug("Start");

    if(std::filesystem::exists(HIDDEN_AIO_FILE)) {
        brls::Application::pushView(new MainFrame());
    }
    else {
        brls::Application::pushView(new WarningPage("menus/launch_warning"_i18n));
    }


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

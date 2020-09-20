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
    nsInitialize();
    socketInitializeDefault();
    nxlinkStdio();

    std::cout << R_SUCCEEDED(splInitialize()) << std::endl;
    createTree(CONFIG_PATH);

    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
    brls::Logger::debug("Start");

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
    return EXIT_SUCCESS;
}

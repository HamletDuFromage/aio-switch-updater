#pragma once 

#include <borealis.hpp>
#include <switch.h>
#include <json.hpp>
#include "constants.hpp"
#include "main_frame.hpp"
#include <fstream>
#include <filesystem>
#include <tuple>
#include <iomanip>

#define AF_INET     2

class NetPage : public brls::AppletFrame
{
    private:
        brls::List* list;
        brls::Label* label;
        std::vector<brls::ListItem*> listItems;
        brls::ListItem* cancel;

    public:
        NetPage();
        std::string ipToString(u8* ip);
        int stringToIp(std::string ip, u8* out);

};
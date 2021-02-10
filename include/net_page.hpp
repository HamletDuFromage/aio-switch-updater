#pragma once 

#include <borealis.hpp>

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
        std::string ipToString(unsigned char* ip);
        int stringToIp(std::string ip, unsigned char* out);

};
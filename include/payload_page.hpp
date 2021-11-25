#pragma once

#include <borealis.hpp>

class PayloadPage : public brls::AppletFrame
{
private:
    brls::Label* label;
    brls::List* list;
    brls::ListItem* listItem;
    brls::ListItem* reboot;
    brls::ListItem* shutDown;
    void RegisterCopyAction(brls::Key key, const std::string& payload_path, const std::string& payload_dest, const std::string& action_name);

public:
    PayloadPage();
};
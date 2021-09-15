#pragma once

#include <borealis.hpp>
#include <json.hpp>

class DownloadPayloadPage : public brls::AppletFrame
{
private:
    brls::Label* label;
    brls::Label* notFound;
    brls::List* list;
    brls::ListItem* listItem;

public:
    DownloadPayloadPage(const nlohmann::ordered_json& payloads);
};
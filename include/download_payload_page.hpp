#pragma once

#include <borealis.hpp>

class DownloadPayloadPage : public brls::AppletFrame
{
    private:
        brls::Label* label;
        brls::Label* notFound;
        brls::List* list;
        brls::ListItem* listItem;

    public:
        DownloadPayloadPage();
};
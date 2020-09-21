#pragma once

#include <borealis.hpp>
#include <string>
#include "download.hpp"
#include "extract.hpp"
#include "confirm_page.hpp"
#include "worker_page.hpp"

class ListDownloadTab : public brls::List
{
    private:
        std::vector<brls::ListItem*> linkItems;
        brls::Label *notFound;
        brls::Label *description;
    public:
        ListDownloadTab(archiveType type);
        ~ListDownloadTab();
};
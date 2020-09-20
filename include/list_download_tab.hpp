#pragma once

#include <borealis.hpp>
#include <string>
#include "download.hpp"
#include "extract.hpp"
#include "constants.hpp"
#include "confirm_page.hpp"
#include "worker_page.hpp"
#include "utils.hpp"

class ListDownloadTab : public brls::List
{
    private:
        std::tuple<std::vector<std::string>, std::vector<std::string>> links;
        std::vector<brls::ListItem*> linkItems;
        brls::Label *notFound;
        brls::Label *description;
    public:
        ListDownloadTab(archiveType type);
        ~ListDownloadTab();
};
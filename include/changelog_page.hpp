#pragma once

#include <borealis.hpp>

class ChangelogPage : public brls::AppletFrame
{
private:
    brls::List* list;
    brls::ListItem* listItem;

public:
    ChangelogPage();
    void ShowChangelogContent(const std::string version, const std::string content);
};
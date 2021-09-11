#pragma once

#include <borealis.hpp>

class AboutTab : public brls::List
{
public:
    AboutTab();

    View* getDefaultFocus() override
    {
        return nullptr;
    }
};

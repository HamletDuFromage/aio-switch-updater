#pragma once

#include <borealis.hpp>

#include <utils.hpp>
#include "reboot_payload.h"


class DialoguePage : public brls::View
{
    private:
        brls::Button* button1 = nullptr;
        brls::Button* button2 = nullptr;
        brls::Label* label = nullptr;
        brls::NavigationMap navigationMap;

    public:
        DialoguePage(brls::StagedAppletFrame* frame, std::string text);

        void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;
        void layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) override;
        brls::View* getDefaultFocus() override;
        brls::View* getNextFocus(brls::FocusDirection direction, brls::View* currentView);
};
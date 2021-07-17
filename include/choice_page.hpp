#pragma once

#include <borealis.hpp>

class ChoicePage: public brls::View
{
    private:
        brls::Button* yes = nullptr;
        brls::Button* no = nullptr;
        brls::Label* label = nullptr;

    public:
        ChoicePage(brls::StagedAppletFrame* frame, const std::string text);
        void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;
        void layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) override;
        brls::View* getDefaultFocus() override;
};
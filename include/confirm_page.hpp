#pragma once

#include <borealis.hpp>
#include <chrono>

class ConfirmPage : public brls::View
{
protected:
    brls::Button* button = nullptr;
    brls::Label* label = nullptr;
    std::chrono::system_clock::time_point start = std::chrono::high_resolution_clock::now();
    bool done = false;

public:
    ConfirmPage(brls::StagedAppletFrame* frame, const std::string& text);
    ~ConfirmPage();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;
    void layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) override;
    brls::View* getDefaultFocus() override;
};

class ConfirmPage_Done : public ConfirmPage
{
public:
    ConfirmPage_Done(brls::StagedAppletFrame* frame, const std::string& text);
};

class ConfirmPage_AppUpdate : public ConfirmPage_Done
{
public:
    ConfirmPage_AppUpdate(brls::StagedAppletFrame* frame, const std::string& text);
};

class ConfirmPage_AmsUpdate : public ConfirmPage_Done
{
public:
    ConfirmPage_AmsUpdate(brls::StagedAppletFrame* frame, const std::string& text, bool erista = true);
};
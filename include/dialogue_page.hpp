#pragma once

#include <borealis.hpp>
#include <chrono>

class DialoguePage : public brls::View
{
private:
    std::chrono::system_clock::time_point start = std::chrono::high_resolution_clock::now();
    brls::NavigationMap navigationMap;

protected:
    brls::Button* button1 = nullptr;
    brls::Button* button2 = nullptr;
    brls::Label* label = nullptr;
    virtual void CreateView();
    virtual void instantiateButtons(){};

public:
    DialoguePage(){};

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;
    void layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) override;
    brls::View* getDefaultFocus() override;
    brls::View* getNextFocus(brls::FocusDirection direction, brls::View* currentView);
};

class DialoguePage_fw : public DialoguePage
{
private:
    void instantiateButtons() override;
    std::string text;
    brls::StagedAppletFrame* frame;

public:
    DialoguePage_fw(brls::StagedAppletFrame* frame, const std::string& text) : DialoguePage(), text(text), frame(frame) { CreateView(); }
    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;
};

class DialoguePage_ams : public DialoguePage
{
private:
    void instantiateButtons() override;
    bool erista = true;
    std::string text;
    brls::StagedAppletFrame* frame;

public:
    DialoguePage_ams(brls::StagedAppletFrame* frame, const std::string& text, bool erista = true) : DialoguePage(), erista(erista), text(text), frame(frame) { CreateView(); }
};
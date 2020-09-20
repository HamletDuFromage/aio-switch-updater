#pragma once

#include <borealis.hpp>
#include <thread>
#include <string>
#include <functional>

#include "utils.hpp"
#include "download.hpp"
#include "extract.hpp"
#include "constants.hpp"
#include "progress_event.hpp"

typedef std::function<void()> worker_func_t;

class WorkerPage : public brls::View
{
  private:
    brls::ProgressDisplay* progressDisp = nullptr;
    brls::StagedAppletFrame* frame;
    brls::Button* button;
    brls::Label* label;
    int progressValue = 0;
    bool workStarted = false;
    std::thread* workerThread;
    worker_func_t workerFunc;

  public:
    WorkerPage(brls::StagedAppletFrame* frame, const std::string& text, worker_func_t worker_func);
    ~WorkerPage();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;
    void layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) override;
    void doWork();
    brls::View* getDefaultFocus() override;
};
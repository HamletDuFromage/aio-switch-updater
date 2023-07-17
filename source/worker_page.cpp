#include "worker_page.hpp"

#include <functional>
#include <string>

#include "constants.hpp"
#include "download.hpp"
#include "extract.hpp"
#include "main_frame.hpp"
#include "progress_event.hpp"
#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

WorkerPage::WorkerPage(brls::StagedAppletFrame* frame, const std::string& text, worker_func_t worker_func) : frame(frame), workerFunc(worker_func), text(text)
{
    this->progressDisp = new brls::ProgressDisplay();
    this->progressDisp->setParent(this);

    this->label = new brls::Label(brls::LabelStyle::DIALOG, text, true);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label->setParent(this);

    this->button = new brls::Button(brls::ButtonStyle::REGULAR);
    this->button->setParent(this);

    this->registerAction("menus/common/cancel"_i18n, brls::Key::B, [this] {
        ProgressEvent::instance().setInterupt(true);
        return true;
    });
    this->registerAction("", brls::Key::A, [this] { return true; });
    this->registerAction("", brls::Key::PLUS, [this] { return true; });
}

std::string formatLabelText(double speed, double fileSizeCurrent, double fileSizeFinal)
{
    double fileSizeCurrentMB = fileSizeCurrent / 0x100000;
    double fileSizeFinalMB = fileSizeFinal / 0x100000;
    double speedMB = speed / 0x100000;

    double timeRemaining = (fileSizeFinal - fileSizeCurrent) / speed;
    int hours = static_cast<int>(timeRemaining / 3600);
    int minutes = static_cast<int>((timeRemaining - hours * 3600) / 60);
    int seconds = static_cast<int>(timeRemaining - hours * 3600 - minutes * 60);

    std::string labelText = fmt::format("menus/worker/download_progress"_i18n, fileSizeCurrentMB, fileSizeFinalMB, speedMB);
    if (speedMB > 0) {
        std::string eta;
        if (hours > 0)
            eta += fmt::format("{}h ", hours);
        if (minutes > 0)
            eta += fmt::format("{}m ", minutes);

        eta += fmt::format("{}s", seconds);
        labelText += "\n" + fmt::format("menus/worker/time_left"_i18n, eta);
    }

    return labelText;
}

void WorkerPage::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx)
{
    if (this->draw_page) {
        if (!this->workStarted) {
            this->workStarted = true;
            appletSetMediaPlaybackState(true);
            appletBeginBlockingHomeButton(0);
            ProgressEvent::instance().reset();
            workerThread = new std::thread(&WorkerPage::doWork, this);
        }
        else if (ProgressEvent::instance().finished()) {
            appletEndBlockingHomeButton();
            appletSetMediaPlaybackState(false);
            if (ProgressEvent::instance().getStatusCode() > 399) {
                this->draw_page = false;
                brls::Application::crash(fmt::format("menus/errors/error_message"_i18n, util::getErrorMessage(ProgressEvent::instance().getStatusCode())));
            }
            if (ProgressEvent::instance().getInterupt()) {
                brls::Application::pushView(new MainFrame());
            }
            else {
                ProgressEvent::instance().setStatusCode(0);
                frame->nextStage();
            }
        }
        else {
            this->progressDisp->setProgress(ProgressEvent::instance().getStep(), ProgressEvent::instance().getMax());
            this->progressDisp->frame(ctx);
            if (ProgressEvent::instance().getTotal()) {
                this->label->setText(formatLabelText(ProgressEvent::instance().getSpeed(), ProgressEvent::instance().getNow(), ProgressEvent::instance().getTotal()));
            }
            this->label->frame(ctx);
        }
    }
}

void WorkerPage::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash)
{
    this->label->setWidth(roundf((float)this->width * style->CrashFrame.labelWidth));

    this->label->setBoundaries(
        this->x + this->width / 2 - this->label->getWidth() / 2,
        this->y + (this->height - style->AppletFrame.footerHeight) / 2 - 30,
        this->label->getWidth(),
        this->label->getHeight());

    this->progressDisp->setBoundaries(
        this->x + this->width / 2 - style->CrashFrame.buttonWidth,
        this->y + this->height / 2,
        style->CrashFrame.buttonWidth * 2,
        style->CrashFrame.buttonHeight);
}

void WorkerPage::doWork()
{
    if (this->workerFunc)
        this->workerFunc();
}

brls::View* WorkerPage::getDefaultFocus()
{
    return this->button;
}

WorkerPage::~WorkerPage()
{
    if (this->workStarted && workerThread->joinable()) {
        this->workerThread->join();
        if (this->workerThread)
            delete this->workerThread;
    }
    delete this->progressDisp;
    delete this->label;
    delete this->button;
}
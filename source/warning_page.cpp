#include "warning_page.hpp"
#include <algorithm>
#include <fstream>
#include "main_frame.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include "fs.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
WarningPage::WarningPage(std::string text)
{
    fs::createTree(CONFIG_PATH);
    std::ofstream(HIDDEN_AIO_FILE);
    this->button = (new brls::Button(brls::ButtonStyle::PRIMARY))->setLabel("menus/common/continue"_i18n);
    this->button->setParent(this);
    this->button->getClickEvent()->subscribe([this](View* view) {
        brls::Application::pushView(new MainFrame());
    });

    this->label = new brls::Label(brls::LabelStyle::REGULAR, text, true);
    this->label->setHorizontalAlign(NVG_ALIGN_LEFT);
    //this->setBackground(brls::ViewBackground::DEBUG);
    this->label->setParent(this);

    this->registerAction("", brls::Key::B, [this] { return true; });
}

void WarningPage::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx)
{

    auto end = std::chrono::high_resolution_clock::now();
    auto missing = std::max(1l - std::chrono::duration_cast<std::chrono::seconds>(end - start).count(), 0l);
    auto text =  std::string("menus/common/continue"_i18n);
    if (missing > 0) {
        this->button->setLabel(text + " (" + std::to_string(missing) + ")");
        this->button->setState(brls::ButtonState::DISABLED);
    } else {
        this->button->setLabel(text);
        this->button->setState(brls::ButtonState::ENABLED);
    }
    this->button->invalidate();

    this->label->frame(ctx);
    this->button->frame(ctx);
}

brls::View* WarningPage::getDefaultFocus()
{
    return this->button;
}

void WarningPage::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash)
{
    this->label->setWidth(0.8f * this->width);
    this->label->invalidate(true);
    this->label->setBoundaries(
        this->x + this->width / 2 - this->label->getWidth() / 2,
        this->y + (this->height - this->label->getHeight() - this->y - style->CrashFrame.buttonHeight)/2,
        this->label->getWidth(),
        this->label->getHeight());

    this->button->setBoundaries(
        this->x + this->width / 2 - style->CrashFrame.buttonWidth / 2,
        this->y + (this->height-style->CrashFrame.buttonHeight*3),
        style->CrashFrame.buttonWidth,
        style->CrashFrame.buttonHeight);
    this->button->invalidate();

    start = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(150);
}

WarningPage::~WarningPage()
{
    delete this->label;
    delete this->button;
}
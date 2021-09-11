#include "choice_page.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
ChoicePage::ChoicePage(brls::StagedAppletFrame* frame, const std::string text)
{
    this->yes = (new brls::Button(brls::ButtonStyle::BORDERLESS))->setLabel("yes");
    this->yes->setParent(this);
    this->no = (new brls::Button(brls::ButtonStyle::BORDERLESS))->setLabel("no");
    this->no->setParent(this);

    this->label = new brls::Label(brls::LabelStyle::DIALOG, text, true);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label->setParent(this);
}

void ChoicePage::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx)
{
}

brls::View* ChoicePage::getDefaultFocus()
{
    return this->no;
}

void ChoicePage::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash)
{
    this->label->setWidth(this->width);
    this->label->invalidate(true);

    this->label->setBoundaries(
        this->x + this->width / 2 - this->label->getWidth() / 2,
        this->y + (this->height - this->label->getHeight() - this->y - style->CrashFrame.buttonHeight) / 2,
        this->label->getWidth(),
        this->label->getHeight());

    this->yes->setBoundaries(
        this->x + this->width / 2 - style->CrashFrame.buttonWidth / 2,
        this->y + (this->height - style->CrashFrame.buttonHeight * 3),
        style->CrashFrame.buttonWidth,
        style->CrashFrame.buttonHeight);
    this->yes->invalidate();

    this->no->setBoundaries(
        this->x + this->width / 2 - style->CrashFrame.buttonWidth / 2,
        this->y + (this->height - style->CrashFrame.buttonHeight * 3),
        style->CrashFrame.buttonWidth,
        style->CrashFrame.buttonHeight);
    this->no->invalidate();
}
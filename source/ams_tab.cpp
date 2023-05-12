#include "ams_tab.hpp"

#include <filesystem>
#include <iostream>
#include <string>

#include "confirm_page.hpp"
#include "current_cfw.hpp"
#include "dialogue_page.hpp"
#include "download.hpp"
#include "extract.hpp"
#include "fs.hpp"
#include "utils.hpp"
#include "worker_page.hpp"
namespace i18n = brls::i18n;
using namespace i18n::literals;

AmsTab::AmsTab(const nlohmann::ordered_json& nxlinks, const bool erista) : brls::List()
{
    this->erista = erista;
    this->nxlinks = nxlinks;
    this->hekate = util::getValueFromKey(nxlinks, "hekate");
}

void AmsTab::RegisterListItemAction(brls::ListItem* listItem) {}

bool AmsTab::CreateDownloadItems(const nlohmann::ordered_json& cfw_links, bool hekate, bool ams)
{
    std::vector<std::pair<std::string, std::string>> links;
    links = download::getLinksFromJson(cfw_links);
    if (links.size() && !this->hekate.empty()) {  // non-empty this->hekate indicates internet connection
        auto hekate_link = download::getLinksFromJson(this->hekate);
        std::string hekate_url = hekate_link[0].second;
        std::string text_hekate = "menus/common/download"_i18n + hekate_link[0].first;

        for (const auto& link : links) {
            std::string url = link.second;
            std::string text("menus/common/download"_i18n + link.first + "menus/common/from"_i18n + url);
            listItem = new brls::ListItem(link.first);
            listItem->setHeight(LISTITEM_HEIGHT);
            listItem->getClickEvent()->subscribe([this, text, text_hekate, url, hekate_url, hekate, ams](brls::View* view) {
                if (!erista && !std::filesystem::exists(MARIKO_PAYLOAD_PATH)) {
                    brls::Application::crash("menus/errors/mariko_payload_missing"_i18n);
                }
                else {
                    CreateStagedFrames(text, url, erista, ams, hekate, text_hekate, hekate_url);
                }
            });
            this->RegisterListItemAction(listItem);
            this->addView(listItem);
        }
        return true;
    }
    return false;
}

void AmsTab::CreateStagedFrames(const std::string& text, const std::string& url, bool erista, bool ams, bool hekate, const std::string& text_hekate, const std::string& hekate_url)
{
    brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
    stagedFrame->setTitle(this->type == contentType::ams_cfw ? "menus/ams_update/getting_ams"_i18n : "menus/ams_update/custom_download"_i18n);
    stagedFrame->addStage(
        new ConfirmPage(stagedFrame, text));
    stagedFrame->addStage(
        new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, [&, url]() { util::downloadArchive(url, this->type); }));
    stagedFrame->addStage(
        new WorkerPage(stagedFrame, "menus/common/extracting"_i18n, [&]() { util::extractArchive(this->type); }));
    if (hekate) {
        stagedFrame->addStage(
            new DialoguePage_ams(stagedFrame, text_hekate, erista));
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/common/downloading"_i18n, [hekate_url]() { util::downloadArchive(hekate_url, contentType::bootloaders); }));
        stagedFrame->addStage(
            new WorkerPage(stagedFrame, "menus/common/extracting"_i18n, []() { util::extractArchive(contentType::bootloaders); }));
    }
    if (ams)
        stagedFrame->addStage(new ConfirmPage_AmsUpdate(stagedFrame, "menus/ams_update/reboot_rcm"_i18n, erista));
    else
        stagedFrame->addStage(new ConfirmPage_Done(stagedFrame, "menus/common/all_done"_i18n));
    brls::Application::pushView(stagedFrame);
}

AmsTab_Regular::AmsTab_Regular(const nlohmann::ordered_json& nxlinks, const bool erista) : AmsTab(nxlinks, erista)
{
    this->CreateLists();
}

bool AmsTab_Regular::CreateDownloadItems(const nlohmann::ordered_json& cfw_links, bool hekate, bool ams)
{
    if (!AmsTab::CreateDownloadItems(cfw_links, hekate, ams)) {
        brls::Label* description = new brls::Label(
            brls::LabelStyle::SMALL,
            "menus/main/links_not_found"_i18n,
            true);
        description->setHorizontalAlign(NVG_ALIGN_CENTER);
        this->addView(description);
        return true;
    }
    return false;
}

void AmsTab_Regular::CreateLists()
{
    this->type = contentType::ams_cfw;
    auto cfws = util::getValueFromKey(this->nxlinks, "cfws");

    this->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "menus/main/ams_text"_i18n + (CurrentCfw::running_cfw == CFW::ams ? "\n" + "menus/ams_update/current_ams"_i18n + CurrentCfw::getAmsInfo() : "") + (erista ? "\n" + "menus/ams_update/erista_rev"_i18n : "\n" + "menus/ams_update/mariko_rev"_i18n), true));
    CreateDownloadItems(util::getValueFromKey(cfws, "Atmosphere"));

    this->addView(new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/ams_update/deepsea_label"_i18n,
        true));
    listItem = new brls::ListItem("menus/ams_update/get_custom_deepsea"_i18n);
    listItem->setHeight(LISTITEM_HEIGHT);
    listItem->getClickEvent()->subscribe([this](brls::View* view) {
        nlohmann::ordered_json modules;
        download::getRequest(DEEPSEA_META_JSON, modules);
        this->ShowCustomDeepseaBuilder(modules);
    });
    this->addView(listItem);
    CreateDownloadItems(util::getValueFromKey(cfws, "DeepSea"), false);
}

std::string AmsTab_Regular::GetRepoName(const std::string& repo)
{
    return repo.substr(repo.find("/") + 1, repo.length());
}

std::set<std::string> AmsTab_Regular::GetLastDownloadedModules(const std::string& json_path)
{
    nlohmann::ordered_json package = fs::parseJsonFile(json_path);
    std::set<std::string> res;
    if (package.find("modules") != package.end()) {
        for (const auto& module : package.at("modules")) {
            res.insert(module.get<std::string>());
        }
    }
    return res;
}

nlohmann::ordered_json AmsTab_Regular::SortDeepseaModules(const nlohmann::ordered_json& modules)
{
    nlohmann::ordered_json sorted_modules = nlohmann::ordered_json::object();
    if (modules.find("modules") != modules.end()) {
        for (const auto& module : modules.at("modules").items()) {
            sorted_modules[std::string(module.value().at("category"))][module.key()] = module.value();
        }
    }
    return sorted_modules;
}

void AmsTab_Regular::ShowCustomDeepseaBuilder(nlohmann::ordered_json& modules)
{
    modules = SortDeepseaModules(modules);
    std::map<std::string, std::string> name_map;

    brls::TabFrame* appView = new brls::TabFrame();
    appView->setIcon("romfs:/deepsea_icon.png");

    std::vector<brls::List*> lists;
    std::set<std::string> old_modules = GetLastDownloadedModules(DEEPSEA_PACKAGE_PATH);

    brls::ToggleListItem* deepseaListItem;
    for (const auto& category : modules.items()) {
        brls::List* list = new brls::List();

        for (const auto& module : category.value().items()) {
            auto module_value = module.value();
            std::string requirements = "";
            if (!module_value.at("requires").empty()) {
                requirements = "menus/ams_update/depends_on"_i18n;
                for (const auto& r : module.value().at("requires")) {
                    requirements += " " + r.get<std::string>() + ",";
                }
                requirements.pop_back();
            }
            if (module_value.at("required")) {
                deepseaListItem = new UnTogglableListItem(module_value.at("displayName"), 1, requirements, "Required", "o");
            }
            else {
                deepseaListItem = new ::brls::ToggleListItem(module_value.at("displayName"),
                                                             old_modules.find(module.key()) != old_modules.end() ? 1 : 0,
                                                             requirements,
                                                             "menus/common/selected"_i18n,
                                                             "menus/common/off"_i18n);
            }
            name_map.insert(std::pair(module_value.at("displayName"), module.key()));
            deepseaListItem->registerAction("menus/ams_update/show_module_description"_i18n, brls::Key::Y, [module_value] {
                util::showDialogBoxInfo(fmt::format("{}:\n{}", module_value.at("repo"), module_value.at("description")));
                return true;
            });
            list->addView(deepseaListItem);
        }
        lists.push_back(list);
        appView->addTab(category.key(), list);
    }

    appView->registerAction("menus/ams_update/download_deepsea_package"_i18n, brls::Key::X, [this, lists, name_map] {
        std::set<std::string> desired_modules;
        for (const auto& list : lists) {
            for (size_t i = 0; i < list->getViewsCount(); i++) {
                if (brls::ToggleListItem* item = dynamic_cast<brls::ToggleListItem*>(list->getChild(i))) {
                    if (item->getToggleState()) {
                        desired_modules.insert(name_map.at(item->getLabel()));
                    }
                }
            }
        }

        std::string request_url = DEEPSEA_BUILD_URL;
        for (const auto& e : desired_modules)
            request_url += e + ";";

        this->CreateStagedFrames("menus/common/download"_i18n + "Custom DeepSea package" + "menus/common/from"_i18n + request_url,
                                 request_url,
                                 this->erista);
        return true;
    });
    appView->registerAction("", brls::Key::PLUS, [this] { return true; });

    brls::PopupFrame::open("menus/ams_update/deepsea_builder"_i18n, appView, modules.empty() ? "menus/ams_update/cant_fetch_deepsea"_i18n : "menus/ams_update/build_your_deepsea"_i18n, "");
}

AmsTab_Custom::AmsTab_Custom(const nlohmann::ordered_json& nxlinks, const bool erista) : AmsTab(nxlinks, erista)
{
    this->custom_packs = fs::parseJsonFile(CUSTOM_PACKS_PATH);
    this->CreateLists();
}

void AmsTab_Custom::CreateLists()
{
    this->addView(new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        fmt::format("menus/ams_update/custom_packs_label"_i18n, CUSTOM_PACKS_PATH),
        true));

    this->type = contentType::ams_cfw;
    this->addView(new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/ams_update/custom_packs_ams"_i18n,
        true));
    CreateDownloadItems(util::getValueFromKey(this->custom_packs, "ams"), true);
    this->AddLinkCreator();

    this->type = contentType::custom;
    this->addView(new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "menus/ams_update/custom_packs_misc"_i18n,
        true));
    CreateDownloadItems(util::getValueFromKey(this->custom_packs, "misc"), false, false);
    this->AddLinkCreator();
}

void AmsTab_Custom::AddLinkCreator()
{
    std::string category = this->type == contentType::ams_cfw ? "ams" : "misc";
    listItem = new brls::ListItem("menus/ams_update/add_custom_link"_i18n);
    listItem->setHeight(LISTITEM_HEIGHT);
    listItem->getClickEvent()->subscribe([this, category](brls::View* view) {
        std::string title, link;
        brls::Swkbd::openForText([&title](std::string text) { title = text; }, "Enter title", "", 256, "", 0, "Submit", "Title");
        brls::Swkbd::openForText([&link](std::string text) { link = text; }, "Enter direct link", "", 256, "", 0, "Submit", "https://site/download.zip");
        auto links = util::getValueFromKey(this->custom_packs, category);
        links[title] = link;
        this->custom_packs[category] = links;
        fs::writeJsonToFile(this->custom_packs, CUSTOM_PACKS_PATH);
        util::restartApp();
    });
    this->addView(listItem);
}

void AmsTab_Custom::RegisterListItemAction(brls::ListItem* listItem)
{
    std::string label = listItem->getLabel();
    std::string category = this->type == contentType::ams_cfw ? "ams" : "misc";
    listItem->registerAction("menus/ams_update/delete_custom_link"_i18n, brls::Key::X, [this, label, category] {
        auto& links = this->custom_packs.at(category);
        links.erase(label);
        fs::writeJsonToFile(this->custom_packs, CUSTOM_PACKS_PATH);
        util::restartApp();
        return true;
    });
}

bool UnTogglableListItem::onClick()
{
    return true;
}

#include "utils.hpp"
#include "fs.hpp"
#include "current_cfw.hpp"
#include "download.hpp"
#include "extract.hpp"
#include "progress_event.hpp"
#include "main_frame.hpp"
#include "reboot_payload.h"
#include "unistd.h"
#include "progress_event.hpp"
#include <switch.h>
#include <filesystem>
#include <fstream>

namespace i18n = brls::i18n;
using namespace i18n::literals;

namespace util {

bool isArchive(const std::string&  path){
    std::fstream file;
    std::string fileContent;
    if(std::filesystem::exists(path)){
        file.open(path, std::fstream::in);
        file >> fileContent;
    }
    return fileContent.find("DOCTYPE") == std::string::npos;
}

void downloadArchive(const std::string& url, archiveType type) {
    long status_code;
    downloadArchive(url, type, status_code);
}

void downloadArchive(const std::string& url, archiveType type, long& status_code) {
    fs::createTree(DOWNLOAD_PATH);
    switch(type){
        case archiveType::sigpatches:
            status_code = download::downloadFile(url, SIGPATCHES_FILENAME, OFF);
            break;
        case archiveType::cheats:
            status_code = download::downloadFile(url, CHEATS_FILENAME, OFF);
            break;
        case archiveType::fw:
            status_code = download::downloadFile(url, FIRMWARE_FILENAME, OFF);
            break;
        case archiveType::app:
            status_code = download::downloadFile(url, APP_FILENAME, OFF);
            break;
        case archiveType::cfw:
            status_code = download::downloadFile(url, CFW_FILENAME, OFF);
            break;
        case archiveType::ams_cfw:
            status_code = download::downloadFile(url, AMS_FILENAME, OFF);
    }
    ProgressEvent::instance().setStatusCode(status_code);
}

int showDialogBox(const std::string& text, const std::string& opt){
    int dialogResult = -1;
    int result = -1;
    brls::Dialog* dialog = new brls::Dialog(text);
    brls::GenericEvent::Callback callback = [dialog, &dialogResult](brls::View* view) {
        dialogResult = 0;
        dialog->close();
    };
    dialog->addButton(opt, callback);
    dialog->setCancelable(false);
    dialog->open();
    while(result == -1){
        usleep(1);
        result = dialogResult;
    }
    return result;
}

int showDialogBox(const std::string& text, const std::string& opt1, const std::string& opt2){
    int dialogResult = -1;
    int result = -1;
    brls::Dialog* dialog = new brls::Dialog(text);
    brls::GenericEvent::Callback callback1 = [dialog, &dialogResult](brls::View* view) {
        dialogResult = 0;
        dialog->close();
    };
    brls::GenericEvent::Callback callback2 = [dialog, &dialogResult](brls::View* view) {
        dialogResult = 1;
        dialog->close();
    };
    dialog->addButton(opt1, callback1);
    dialog->addButton(opt2, callback2);
    dialog->setCancelable(false);
    dialog->open();
    while(result == -1){
        usleep(1);
        result = dialogResult;
    }
    return result;
}

void extractArchive(archiveType type, const std::string& tag){
    int overwriteInis = 0;
    chdir(ROOT_PATH);
    switch(type){
        case archiveType::sigpatches:
            if(isArchive(SIGPATCHES_FILENAME)) {
                /* if(std::filesystem::exists(HEKATE_IPL_PATH)){
                    overwriteInis = showDialogBox("menus/utils/overwrite"_i18n + std::string(HEKATE_IPL_PATH) +"?", "menus/common/no"_i18n, "menus/common/yes"_i18n);
                    if(overwriteInis == 0){
                        extract::extract(SIGPATCHES_FILENAME, ROOT_PATH, HEKATE_IPL_PATH);
                    }
                    else{
                        extract::extract(SIGPATCHES_FILENAME);
                    }
                } */
                //else{
                    extract::extract(SIGPATCHES_FILENAME);
                //}
            }
            else{
                brls::Application::crash("menus/utils/wrong_type_sigpatches"_i18n);
            }
            break;
        case archiveType::cheats: {
            std::vector<std::string> titles = extract::getInstalledTitlesNs();
            titles = extract::excludeTitles(CHEATS_EXCLUDE, titles);
            extract::extractCheats(CHEATS_FILENAME, titles, CurrentCfw::running_cfw);
            break;
        }
        case archiveType::fw:
            if(std::filesystem::file_size(FIRMWARE_FILENAME) < 200000){
                brls::Application::crash("menus/utils/wrong_type_sigpatches_downloaded"_i18n);
            }
            else{
                if (std::filesystem::exists(FIRMWARE_PATH)) std::filesystem::remove_all(FIRMWARE_PATH);
                fs::createTree(FIRMWARE_PATH);
                extract::extract(FIRMWARE_FILENAME, FIRMWARE_PATH);
            }
            break;
        case archiveType::app:
            extract::extract(APP_FILENAME, CONFIG_PATH);
            fs::copyFile(ROMFS_FORWARDER, FORWARDER_PATH);
            envSetNextLoad(FORWARDER_PATH, ("\"" + std::string(FORWARDER_PATH) + "\"").c_str());
            romfsExit();
            brls::Application::quit();
            break;
        case archiveType::cfw:
            if(isArchive(CFW_FILENAME)){
                overwriteInis = showDialogBox("menus/utils/overwrite_inis"_i18n, "menus/common/no"_i18n, "menus/common/yes"_i18n);
                extract::extract(CFW_FILENAME, ROOT_PATH, overwriteInis);
            }
            else{
                brls::Application::crash("menus/utils/wrong_type_cfw"_i18n);
            }
            break;
        case archiveType::ams_cfw:
            if(isArchive(AMS_FILENAME)){
                overwriteInis = showDialogBox("menus/utils/overwrite_inis"_i18n, "menus/common/no"_i18n, "menus/common/yes"_i18n);
                usleep(800000);
                int deleteContents = showDialogBox("menus/ams_update/delete_sysmodules_flags"_i18n, "menus/common/no"_i18n, "menus/common/yes"_i18n);
                if(deleteContents == 1)
                    removeSysmodulesFlags(AMS_CONTENTS);
                extract::extract(AMS_FILENAME, ROOT_PATH, overwriteInis);
            }
            break;
    }
    if(type == archiveType::ams_cfw || type == archiveType::cfw)
        fs::copyFiles(COPY_FILES_TXT);
}

std::string formatListItemTitle(const std::string &str, size_t maxScore) {
    size_t score = 0;
    for (size_t i = 0; i < str.length(); i++) {
        score += std::isupper(str[i]) ? 4 : 3;
        if(score > maxScore) {
            return str.substr(0, i-1) + "\u2026";
        }
    }
    return str;
}

std::string formatApplicationId(u64 ApplicationId){
    return fmt::format("{:016X}", ApplicationId);
}

std::vector<std::string> fetchPayloads(){
    std::vector<std::string> payloadPaths;
    payloadPaths.push_back(ROOT_PATH);
    if(std::filesystem::exists(PAYLOAD_PATH))       payloadPaths.push_back(PAYLOAD_PATH);
    if(std::filesystem::exists(AMS_PATH))           payloadPaths.push_back(AMS_PATH);
    if(std::filesystem::exists(REINX_PATH))         payloadPaths.push_back(REINX_PATH);
    if(std::filesystem::exists(BOOTLOADER_PATH))    payloadPaths.push_back(BOOTLOADER_PATH);
    if(std::filesystem::exists(BOOTLOADER_PL_PATH)) payloadPaths.push_back(BOOTLOADER_PL_PATH);
    if(std::filesystem::exists(SXOS_PATH))          payloadPaths.push_back(SXOS_PATH);
    std::vector<std::string> res;
    for (const auto& path : payloadPaths){
        for (const auto& entry : std::filesystem::directory_iterator(path)){
            if(entry.path().extension().string() == ".bin"){
                if(entry.path().string() != FUSEE_SECONDARY && entry.path().string() != FUSEE_MTC)
                    res.push_back(entry.path().string().c_str());
            }
        }

    }
    return res;
}

void shutDown(bool reboot){
    spsmInitialize();
    spsmShutdown(reboot);
}

void rebootToPayload(const std::string& path) {
    reboot_to_payload(path.c_str(), CurrentCfw::running_cfw != CFW::ams);
}

std::string getLatestTag(const std::string& url) {
    nlohmann::ordered_json tag;
    download::getRequest(url, tag, {"accept: application/vnd.github.v3+json"});
    if(tag.find("tag_name") != tag.end())
        return tag["tag_name"];
    else
        return "";
}

std::string downloadFileToString(const std::string& url) {
    std::vector<uint8_t> bytes;
    download::downloadFile(url, bytes);
    std::string str(bytes.begin(), bytes.end());
    return str;
}

void saveVersion(const std::string& version, const std::string& path){
    std::ofstream newVersion(path);
    newVersion << version << std::endl;
}

std::string readVersion(const std::string& path){
    std::fstream versionFile;
    std::string version = "0";
    if(std::filesystem::exists(path)){
        versionFile.open(path, std::fstream::in);
        versionFile >> version;
        versionFile.close();
    }
    return version;
}

bool isErista() {
    SetSysProductModel model;
    setsysGetProductModel(&model);
    return (model == SetSysProductModel_Nx || model == SetSysProductModel_Copper);
}

void removeSysmodulesFlags(const std::string&  directory) {
    for (const auto & e : std::filesystem::recursive_directory_iterator(directory)) {
        if(e.path().string().find("boot2.flag") != std::string::npos) {
                std::filesystem::remove(e.path());
        }
    }
}

std::string lowerCase(const std::string& str) {
    std::string res = str;
    std::for_each(res.begin(), res.end(), [](char & c){
        c = std::tolower(c);
    });
    return res;
}

std::string upperCase(const std::string& str) {
    std::string res = str;
    std::for_each(res.begin(), res.end(), [](char & c){
        c = std::toupper(c);
    });
    return res;
}

std::string getErrorMessage(long status_code) {
    std::string res;
    switch(status_code) {
        case 500:
            res = fmt::format("{0:}: Internal Server Error", status_code);
            break;
        case 503: 
            res = fmt::format("{0:}: Service Temporarily Unavailable", status_code);
            break;
        default:
            res = fmt::format("error: {0:}", status_code);
            break;
    }
    return res;
}

bool isApplet() {
    AppletType at = appletGetAppletType();
    return at != AppletType_Application && at != AppletType_SystemApplication;
}

std::set<std::string> getExistingCheatsTids() {
    std::string path = getContentsPath();
    std::set<std::string> res;
    for(const auto& entry : std::filesystem::directory_iterator(path)) {
        std::string cheatsPath =  entry.path().string() + "/cheats";
        if(std::filesystem::exists(cheatsPath)){
            res.insert(util::upperCase(cheatsPath.substr(cheatsPath.length() - 7 - 16, 16)));
        }
    }
    return res;
}

std::string getContentsPath() {
    std::string path;
    switch(CurrentCfw::running_cfw){
        case CFW::ams:
            path = std::string(AMS_PATH) + std::string(CONTENTS_PATH);
            break;
        case CFW::rnx:
            path = std::string(REINX_PATH) + std::string(CONTENTS_PATH);
            break;
        case CFW::sxos:
            path = std::string(SXOS_PATH) + std::string(TITLES_PATH);
            break;
    }
    return path;
}

bool getBoolValue(const nlohmann::json& jsonFile, const std::string& key) {
    /* try { return jsonFile.at(key); }
    catch (nlohmann::json::out_of_range& e) { return false; } */
    return (jsonFile.find(key) != jsonFile.end()) ? jsonFile.at(key).get<bool>() : false;
}

}
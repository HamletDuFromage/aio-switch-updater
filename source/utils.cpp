#include "utils.hpp"
#include "fs.hpp"
#include "current_cfw.hpp"
#include <switch.h>
#include "download.hpp"
#include "extract.hpp"
#include "progress_event.hpp"
#include "main_frame.hpp"
#include <filesystem>
#include <fstream>

namespace i18n = brls::i18n;
using namespace i18n::literals;

namespace util {

bool isArchive(const char * path){
    std::fstream file;
    std::string fileContent;
    if(std::filesystem::exists(path)){
        file.open(path, std::fstream::in);
        file >> fileContent;
        file.close();
    }
    return fileContent.find("DOCTYPE") == std::string::npos;
}

void downloadArchive(std::string url, archiveType type){
    fs::createTree(DOWNLOAD_PATH);
    AppletType at;
    switch(type){
        case archiveType::sigpatches:
            download::downloadFile(url.c_str(), SIGPATCHES_FILENAME, OFF);
            break;
        case archiveType::cheats:
            download::downloadFile(url.c_str(), CHEATS_FILENAME, OFF);
            break;
        case archiveType::fw:
            at = appletGetAppletType();
            if (at == AppletType_Application || at == AppletType_SystemApplication) {
                download::downloadFile(url.c_str(), FIRMWARE_FILENAME, OFF);
            }
            else{
                brls::Application::crash("menus/utils/fw_warning"_i18n);
            }
            break;
        case archiveType::app:
            download::downloadFile(url.c_str(), APP_FILENAME, OFF);
            break;
        case archiveType::cfw:
            download::downloadFile(url.c_str(), CFW_FILENAME, OFF);
            break;
        case archiveType::ams_cfw:
            download::downloadFile(url.c_str(), AMS_FILENAME, OFF);
    }
}


int showDialogBox(std::string text, std::string opt){
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

int showDialogBox(std::string text, std::string opt1, std::string opt2){
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

void extractArchive(archiveType type, std::string tag){
    int overwriteInis = 0;
    std::vector<std::string> titles;
    std::string nroPath ="sdmc:" + std::string(APP_PATH);
    chdir(ROOT_PATH);
    switch(type){
        case archiveType::sigpatches:
            if(isArchive(SIGPATCHES_FILENAME)) {
                /* std::string backup(HEKATE_IPL_PATH);
                backup += ".old"; */
                if(std::filesystem::exists(HEKATE_IPL_PATH)){
                    overwriteInis = showDialogBox("menus/utils/overwrite"_i18n + std::string(HEKATE_IPL_PATH) +"?", "menus/common/no"_i18n, "menus/common/yes"_i18n);
                    if(overwriteInis == 0){
                        extract::extract(SIGPATCHES_FILENAME, ROOT_PATH, HEKATE_IPL_PATH);
                    }
                    else{
                        extract::extract(SIGPATCHES_FILENAME);
                    }
                }
                else{
                    extract::extract(SIGPATCHES_FILENAME);
                }
            }
            else{
                brls::Application::crash("menus/utils/wrong_type_sigpatches"_i18n);
            }
            break;
        case archiveType::cheats: 
            titles = extract::getInstalledTitlesNs();
            titles = extract::excludeTitles(CHEATS_EXCLUDE, titles);
            extract::extractCheats(CHEATS_FILENAME, titles, CurrentCfw::running_cfw);
            break;
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
    if(std::filesystem::exists(COPY_FILES_JSON))
        fs::copyFiles(COPY_FILES_JSON);
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
    bpcInitialize();
    if(reboot) bpcRebootSystem();
    else bpcShutdownSystem();
    bpcExit();
}

std::string getLatestTag(const char *url){
    nlohmann::json tag = download::getRequest(url, {"accept: application/vnd.github.v3+json"});
    if(tag.find("tag_name") != tag.end())
        return tag["tag_name"];
    else
        return "";
}

void saveVersion(std::string version, const char* path){
    std::fstream newVersion;
    newVersion.open(path, std::fstream::out | std::fstream::trunc);
    newVersion << version << std::endl;
    newVersion.close();
}

std::string readVersion(const char* path){
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
    splInitialize();
    u64 hwType;
    Result rc = splGetConfig(SplConfigItem_HardwareType, &hwType);
    splExit();

    if(R_FAILED(rc))
        return true;

    switch (hwType)
    {
        case 0:
        case 1:
            return true;
        case 2:
        case 3:
        case 4:
        case 5:
            return false;
        default:
            return true;
    }
};

void removeSysmodulesFlags(const char * directory) {
    const std::set<std::string> AMS_SYSMODULES{
        "010000000000000D",
        "010000000000002B",
        "0100000000000032",
        "0100000000000036",
        "0100000000000042",
        "0100000000000008",
        "010000000000003C",
        "0100000000000034",
        "0100000000000037"
    };
    bool found = false;
    for (const auto & e : std::filesystem::recursive_directory_iterator(directory)) {
        if(e.path().string().find("boot2.flag") != std::string::npos) {
            for(const auto & c : AMS_SYSMODULES) {
                if(e.path().string().find(c) != std::string::npos) {
                    found = true;
                    break;
                }
            }
            if(!found)
                std::filesystem::remove(e.path());
        }
        found = false;
    }
}

}
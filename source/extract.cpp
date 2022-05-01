#include "extract.hpp"

#include <unzipper.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "current_cfw.hpp"
#include "download.hpp"
#include "fs.hpp"
#include "main_frame.hpp"
#include "progress_event.hpp"
#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

namespace extract {

    namespace {
        bool caselessCompare(const std::string& a, const std::string& b)
        {
            return strcasecmp(a.c_str(), b.c_str()) == 0;
        }

        void preWork(zipper::Unzipper& unzipper, const std::string& workingPath, std::vector<zipper::ZipEntry>& entries)
        {
            chdir(workingPath.c_str());
            entries = unzipper.entries();
            s64 uncompressedSize = 0;
            s64 freeStorage;
            for (const auto& entry : entries)
                uncompressedSize += entry.uncompressedSize;

            if (R_SUCCEEDED(fs::getFreeStorageSD(freeStorage))) {
                if (uncompressedSize * 1.1 > freeStorage) {
                    unzipper.close();
                    brls::Application::crash("menus/errors/insufficient_storage"_i18n);
                    std::this_thread::sleep_for(std::chrono::microseconds(2000000));
                    brls::Application::quit();
                }
            }
            ProgressEvent::instance().setTotalSteps(entries.size() + 1);
            ProgressEvent::instance().setStep(0);
        }
    }  // namespace

    void extract(const std::string& filename, const std::string& workingPath, int overwriteInis, std::function<void()> func)
    {
        zipper::Unzipper unzipper(filename);
        std::vector<zipper::ZipEntry> entries;

        preWork(unzipper, workingPath, entries);
        std::set<std::string> ignoreList = fs::readLineByLine(FILES_IGNORE);

        for (const auto& entry : entries) {
            if (ProgressEvent::instance().getInterupt()) {
                break;
            }
            if ((overwriteInis == 0 && entry.name.substr(entry.name.length() - 4) == ".ini") || find_if(ignoreList.begin(), ignoreList.end(), [&entry](std::string ignored) {
                                                            u8 res = ("/" + entry.name).find(ignored);
                                                            return (res == 0 || res == 1); }) != ignoreList.end()) {
                if (!std::filesystem::exists("/" + entry.name)) {
                    unzipper.extractEntry(entry.name);
                }
            }
            else if (entry.name == "atmosphere/stratosphere.romfs" || entry.name == "atmosphere/package3") {
                std::ofstream readonlyFile(entry.name + ".aio");
                unzipper.extractEntryToStream(entry.name, readonlyFile);
            }
            else {
                unzipper.extractEntry(entry.name);
                if (entry.name.substr(0, 13) == "hekate_ctcaer") {
                    fs::copyFile("/" + entry.name, UPDATE_BIN_PATH);
                    if (CurrentCfw::running_cfw == CFW::ams && util::showDialogBoxBlocking(fmt::format("menus/utils/set_hekate_reboot_payload"_i18n, UPDATE_BIN_PATH, REBOOT_PAYLOAD_PATH), "menus/common/yes"_i18n, "menus/common/no"_i18n) == 0) {
                        fs::copyFile(UPDATE_BIN_PATH, REBOOT_PAYLOAD_PATH);
                    }
                }
            }
            ProgressEvent::instance().incrementStep(1);
        }
        unzipper.close();
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
    }

    std::vector<std::string> getInstalledTitlesNs()
    {
        std::vector<std::string> titles;

        NsApplicationRecord* records = new NsApplicationRecord[MaxTitleCount]();
        NsApplicationControlData* controlData = NULL;

        s32 recordCount = 0;
        u64 controlSize = 0;

        if (R_SUCCEEDED(nsListApplicationRecord(records, MaxTitleCount, 0, &recordCount))) {
            for (s32 i = 0; i < recordCount; i++) {
                controlSize = 0;
                free(controlData);
                controlData = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
                if (controlData == NULL) {
                    break;
                }
                else {
                    memset(controlData, 0, sizeof(NsApplicationControlData));
                }

                if (R_FAILED(nsGetApplicationControlData(NsApplicationControlSource_Storage, records[i].application_id, controlData, sizeof(NsApplicationControlData), &controlSize))) continue;

                if (controlSize < sizeof(controlData->nacp)) {
                    continue;
                }

                titles.push_back(util::formatApplicationId(records[i].application_id));
            }
            free(controlData);
        }
        delete[] records;
        std::sort(titles.begin(), titles.end());
        return titles;
    }

    std::vector<std::string> excludeTitles(const std::string& path, const std::vector<std::string>& listedTitles)
    {
        std::vector<std::string> titles;
        std::ifstream file(path);
        std::string name;

        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::transform(line.begin(), line.end(), line.begin(), ::toupper);
                for (size_t i = 0; i < listedTitles.size(); i++) {
                    if (line == listedTitles[i]) {
                        titles.push_back(line);
                        break;
                    }
                }
            }
        }

        std::sort(titles.begin(), titles.end());

        std::vector<std::string> diff;
        std::set_difference(listedTitles.begin(), listedTitles.end(), titles.begin(), titles.end(),
                            std::inserter(diff, diff.begin()));
        return diff;
    }

    int computeOffset(CFW cfw)
    {
        switch (cfw) {
            case CFW::ams:
                std::filesystem::create_directory(AMS_PATH);
                std::filesystem::create_directory(AMS_CONTENTS);
                chdir(AMS_PATH);
                return std::string(CONTENTS_PATH).length();
                break;
            case CFW::rnx:
                std::filesystem::create_directory(REINX_PATH);
                std::filesystem::create_directory(REINX_CONTENTS);
                chdir(REINX_PATH);
                return std::string(CONTENTS_PATH).length();
                break;
            case CFW::sxos:
                std::filesystem::create_directory(SXOS_PATH);
                std::filesystem::create_directory(SXOS_TITLES);
                chdir(SXOS_PATH);
                return std::string(TITLES_PATH).length();
                break;
        }
        return 0;
    }

    void extractCheats(const std::string& zipPath, const std::vector<std::string>& titles, CFW cfw, const std::string& version, bool extractAll)
    {
        zipper::Unzipper unzipper(zipPath);
        std::vector<zipper::ZipEntry> entries = unzipper.entries();
        int offset = computeOffset(cfw);

        if (!extractAll) {
            ProgressEvent::instance().setTotalSteps(titles.size() + 1);
            for (const auto& title : titles) {
                if (ProgressEvent::instance().getInterupt()) {
                    break;
                }
                auto matches = entries | std::views::filter([&title, offset](zipper::ZipEntry entry) {
                                   if ((int)entry.name.size() > offset + 16 + 7) {
                                       return caselessCompare((title.substr(0, 13)), entry.name.substr(offset, 13)) && caselessCompare(entry.name.substr(offset + 16, 7), "/cheats");
                                   }
                                   else {
                                       return false;
                                   }
                               });
                for (const auto& match : matches) {
                    unzipper.extractEntry(match.name);
                }
                ProgressEvent::instance().incrementStep(1);
            }
        }
        else {
            ProgressEvent::instance().setTotalSteps(entries.size() + 1);
            for (const auto& entry : entries) {
                if (ProgressEvent::instance().getInterupt()) {
                    break;
                }

                if ((int)entry.name.size() > offset + 16 + 7 && caselessCompare(entry.name.substr(offset + 16, 7), "/cheats")) {
                    unzipper.extractEntry(entry.name);
                }
                ProgressEvent::instance().incrementStep(1);
            }
        }
        unzipper.close();
        if (version != "offline" && version != "") {
            util::saveToFile(version, CHEATS_VERSION);
        }
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
    }

    void extractAllCheats(const std::string& zipPath, CFW cfw, const std::string& version)
    {
        extractCheats(zipPath, {}, cfw, version, true);
    }

    bool isBID(const std::string& bid)
    {
        for (char const& c : bid) {
            if (!isxdigit(c)) return false;
        }
        return true;
    }

    void writeTitlesToFile(const std::set<std::string>& titles, const std::string& path)
    {
        std::ofstream updatedTitlesFile;
        std::set<std::string>::iterator it = titles.begin();
        updatedTitlesFile.open(path, std::ofstream::out | std::ofstream::trunc);
        if (updatedTitlesFile.is_open()) {
            while (it != titles.end()) {
                updatedTitlesFile << (*it) << std::endl;
                it++;
            }
            updatedTitlesFile.close();
        }
    }

    void removeCheats()
    {
        std::string path = util::getContentsPath();
        ProgressEvent::instance().setTotalSteps(std::distance(std::filesystem::directory_iterator(path), std::filesystem::directory_iterator()) + 1);
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (ProgressEvent::instance().getInterupt()) {
                break;
            }
            removeCheatsDirectory(entry.path().string());
            ProgressEvent::instance().incrementStep(1);
        }
        std::filesystem::remove(CHEATS_VERSION);
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
    }

    void removeOrphanedCheats()
    {
        auto path = util::getContentsPath();
        std::vector<std::string> titles = getInstalledTitlesNs();
        ProgressEvent::instance().setTotalSteps(std::distance(std::filesystem::directory_iterator(path), std::filesystem::directory_iterator()) + 1);
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (ProgressEvent::instance().getInterupt()) {
                break;
            }
            if (std::find_if(titles.begin(), titles.end(), [&entry](std::string title) {
                    return caselessCompare(entry.path().filename(), title);
                }) == titles.end()) {
                removeCheatsDirectory(entry.path().string());
            }
            ProgressEvent::instance().incrementStep(1);
        }
        std::filesystem::remove(CHEATS_VERSION);
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
    }

    bool removeCheatsDirectory(const std::string& entry)
    {
        bool res = true;
        std::string cheatsPath = fmt::format("{}/cheats", entry);
        if (std::filesystem::exists(cheatsPath)) res &= fs::removeDir(cheatsPath);
        if (std::filesystem::is_empty(entry)) res &= fs::removeDir(entry);
        return res;
    }

}  // namespace extract
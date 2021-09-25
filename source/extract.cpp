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
#include <vector>

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
            return strcasecmp(a.c_str(), b.c_str()) < 0;
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
                    usleep(2000000);
                    brls::Application::quit();
                }
            }
            ProgressEvent::instance().setStep(1);
            ProgressEvent::instance().setTotalSteps(entries.size() + 1);
        }
    }  // namespace

    void extract(const std::string& filename, const std::string& workingPath, int overwriteInis)
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
            else if (entry.name == "sept/payload.bin" || entry.name == "atmosphere/fusee-secondary.bin" || entry.name == "atmosphere/stratosphere.romfs" || entry.name == "atmosphere/package3") {
                std::ofstream readonlyFile(entry.name + ".aio");
                unzipper.extractEntryToStream(entry.name, readonlyFile);
            }
            else {
                unzipper.extractEntry(entry.name);
                if (entry.name.substr(0, 13) == "hekate_ctcaer") {
                    fs::copyFile("/" + entry.name, UPDATE_BIN_PATH);
                }
            }
            ProgressEvent::instance().incrementStep(1);
        }
        unzipper.close();
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
    }

    void extract(const std::string& filename, const std::string& workingPath, const std::string& toExclude)
    {
        zipper::Unzipper unzipper(filename);
        std::vector<zipper::ZipEntry> entries;
        preWork(unzipper, workingPath, entries);

        std::set<std::string> ignoreList = fs::readLineByLine(FILES_IGNORE);
        ignoreList.insert(toExclude);

        for (const auto& entry : entries) {
            if (ProgressEvent::instance().getInterupt()) {
                break;
            }
            if (find_if(ignoreList.begin(), ignoreList.end(), [&entry](std::string ignored) {
                                                            u8 res = ("/" + entry.name).find(ignored);
                                                            return (res == 0 || res == 1); }) != ignoreList.end()) {
                if (!std::filesystem::exists("/" + entry.name)) {
                    unzipper.extractEntry(entry.name);
                }
            }
            else {
                unzipper.extractEntry(entry.name);
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

    void extractCheats(const std::string& zipPath, std::vector<std::string> titles, CFW cfw, bool credits)
    {
        zipper::Unzipper unzipper(zipPath);
        std::vector<zipper::ZipEntry> entries = unzipper.entries();
        int offset = computeOffset(cfw);

        ProgressEvent::instance().setTotalSteps(titles.size() + 1);
        for (const auto& title : titles) {
            if (ProgressEvent::instance().getInterupt()) {
                break;
            }
            auto matches = entries | std::views::filter([&title, offset](zipper::ZipEntry entry) {
                               return strcasecmp((title.substr(0, 13)).c_str(), entry.name.substr(offset, 13).c_str()) == 0 && strcasecmp(entry.name.substr(offset + 16, 7).c_str(), "/cheats") == 0;
                           });
            for (const auto& match : matches) {
                unzipper.extractEntry(match.name);
                ProgressEvent::instance().incrementStep(1);
            }
        }
        unzipper.close();
        download::downloadFile(CHEATS_URL_VERSION, CHEATS_VERSION, OFF);
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
    }

    void extractAllCheats(const std::string& zipPath, CFW cfw)
    {
        zipper::Unzipper unzipper(zipPath);
        std::vector<zipper::ZipEntry> entries = unzipper.entries();
        int offset = computeOffset(cfw);

        ProgressEvent::instance().setTotalSteps(entries.size() + 1);
        for (const auto& entry : entries) {
            if (ProgressEvent::instance().getInterupt()) {
                break;
            }
            if (((int)entry.name.size() == offset + 16 + 4) && (isBID(entry.name.substr(offset, 16)))) {
                unzipper.extractEntry(entry.name);
            }
            ProgressEvent::instance().incrementStep(1);
        }
        unzipper.close();
        download::downloadFile(CHEATS_URL_VERSION, CHEATS_VERSION, OFF);
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
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
        ProgressEvent::instance().setTotalSteps(std::distance(std::filesystem::directory_iterator(path), std::filesystem::directory_iterator()));
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            removeCheatsDirectory(entry.path().string());
            ProgressEvent::instance().incrementStep(1);
        }
        std::filesystem::remove(CHEATS_VERSION);
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
    }

    bool removeCheatsDirectory(const std::string& entry)
    {
        bool res = true;
        std::string cheatsPath = fmt::format("{}/cheats", entry);
        if (std::filesystem::exists(cheatsPath)) {
            res &= fs::removeDir(cheatsPath);
            if (std::filesystem::is_empty(entry)) {
                res &= fs::removeDir(entry);
            }
            return res;
        }
        return false;
    }

}  // namespace extract
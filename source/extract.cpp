#include "extract.hpp"

#include <minizip/unzip.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <iostream>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <dirent.h>
#include <unzipper.h>

#include "current_cfw.hpp"
#include "download.hpp"
#include "fs.hpp"
#include "main_frame.hpp"
#include "progress_event.hpp"
#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

constexpr size_t WRITE_BUFFER_SIZE = 0x10000;

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
        }
    }  // namespace

    void extract(const std::string& filename, const std::string& workingPath, int overwriteInis, std::function<void()> func)
    {
        zipper::Unzipper unzipper(filename);
        std::vector<zipper::ZipEntry> entries;

        preWork(unzipper, workingPath, entries);
        unzipper.close();

        chdir(workingPath.c_str());
        unzFile zfile = unzOpen(filename.c_str());
        unz_global_info gi = {0};
        unzGetGlobalInfo(zfile, &gi);

        ProgressEvent::instance().setTotalSteps(gi.number_entry);
        ProgressEvent::instance().setStep(0);

        std::set<std::string> ignoreList = fs::readLineByLine(FILES_IGNORE);


        for (uLong i = 0; i < gi.number_entry; ++i) {
            char filename_inzip[0x301] = {0};
            unz_file_info file_info = {0};
            unzOpenCurrentFile(zfile);
            unzGetCurrentFileInfo(zfile, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
            std::string filename_inzip_s = filename_inzip;
            std::cout << filename_inzip_s << std::endl;

            if (filename_inzip[strlen(filename_inzip) - 1] == '/') {
                DIR *dir = opendir(filename_inzip);
                if(dir) closedir(dir);
                else{
                    std::cout << "\rCreation du repertoir : " << filename_inzip;
                    mkdir(filename_inzip, 0777);
                }
            }

            if ((overwriteInis == 0 && filename_inzip_s.substr(filename_inzip_s.length() - 4) == ".ini") || std::find_if(ignoreList.begin(), ignoreList.end(), [&filename_inzip_s](std::string ignored){
                                                                                                                    u8 res = ("/" + filename_inzip_s).find(ignored);                                                                                             return (res == 0 || res == 1); }) != ignoreList.end()) {
                if (!std::filesystem::exists("/" + filename_inzip_s)) {
                    void *buf = malloc(WRITE_BUFFER_SIZE);
                    FILE *outfile;
                    outfile = fopen((filename_inzip_s + ".aio").c_str(), "wb");
                    for (int j = unzReadCurrentFile(zfile, buf, WRITE_BUFFER_SIZE); j > 0; j = unzReadCurrentFile(zfile, buf, WRITE_BUFFER_SIZE)) {
                        fwrite(buf, 1, j, outfile);
                    }
                    free(buf);
                    fclose(outfile);
                }
                    
            }
            

            else {
                FILE *outfile;
                void *buf = malloc(WRITE_BUFFER_SIZE);

                if ((filename_inzip_s == "atmosphere/package3") || (filename_inzip_s == "atmosphere/stratosphere.romfs")) {
                    outfile = fopen((filename_inzip_s + ".aio").c_str(), "wb");
                }

                else {
                    outfile = fopen(filename_inzip, "wb");
                    if (filename_inzip_s.substr(0, 13) == "hekate_ctcaer") {
                        fs::copyFile("/" + filename_inzip_s, UPDATE_BIN_PATH);
                        if (CurrentCfw::running_cfw == CFW::ams && util::showDialogBoxBlocking(fmt::format("menus/utils/set_hekate_reboot_payload"_i18n, UPDATE_BIN_PATH, REBOOT_PAYLOAD_PATH), "menus/common/yes"_i18n, "menus/common/no"_i18n) == 0) {
                            fs::copyFile(UPDATE_BIN_PATH, REBOOT_PAYLOAD_PATH);
                        }
                    }   
                }

                for (int j = unzReadCurrentFile(zfile, buf, WRITE_BUFFER_SIZE); j > 0; j = unzReadCurrentFile(zfile, buf, WRITE_BUFFER_SIZE)) {
                    std::cout << outfile << std::endl;
                    fwrite(buf, 1, j, outfile);
                }
    
                fclose(outfile);
                free(buf);
            }

            ProgressEvent::instance().incrementStep(1);
            unzCloseCurrentFile(zfile);
            unzGoToNextFile(zfile);
        }
        
        unzClose(zfile);
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

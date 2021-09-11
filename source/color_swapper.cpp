#include "color_swapper.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include "constants.hpp"
#include "fs.hpp"
#include "progress_event.hpp"
#include "utils.hpp"

using json = nlohmann::json;

namespace ColorSwapper {

    int hexToBGR(const std::string& hex)
    {
        std::string R = hex.substr(0, 2);
        std::string G = hex.substr(2, 2);
        std::string B = hex.substr(4, 2);
        return std::stoi(B + G + R, 0, 16);
    }

    std::string BGRToHex(int v)
    {
        std::stringstream ss;
        v = ((v & 0xFF) << 16) + (v & 0xFF00) + (v >> 16) + 256;
        ss << std::setfill('0') << std::setw(6) << std::right << std::hex << v;
        return ss.str();
    }

    bool isHexaAnd3Bytes(const std::string& str)
    {
        if (str.size() != 6) return false;
        for (char const& c : str) {
            if (!isxdigit(c)) return false;
        }
        return true;
    }

}  // namespace ColorSwapper

namespace JC {

    int setColor(const std::vector<int>& colors)
    {
        Result pads, ljc, rjc;
        int res = 0;
        s32 nbEntries;
        HidsysUniquePadId UniquePadIds[2] = {};

        pads = hidsysGetUniquePadsFromNpad(HidNpadIdType_Handheld, UniquePadIds, 2, &nbEntries);
        if (R_SUCCEEDED(pads)) {
            ljc = hiddbgUpdateControllerColor(colors[0], colors[1], UniquePadIds[0]);
            if (R_FAILED(ljc)) res += 1;
            rjc = hiddbgUpdateControllerColor(colors[2], colors[3], UniquePadIds[1]);
            if (R_FAILED(rjc)) res += 2;
        }
        else {
            res += 4;
        }
        return res;
    }

    int backupToJSON(json& profiles, const std::string& path)
    {
        HidNpadControllerColor color_left;
        HidNpadControllerColor color_right;
        Result res = hidGetNpadControllerColorSplit(HidNpadIdType_Handheld, &color_left, &color_right);
        std::vector<int> oldBackups;
        if (R_SUCCEEDED(res)) {
            int i = 0;
            for (const auto& x : profiles.items()) {
                if (x.value()["name"] == "_backup") {
                    oldBackups.push_back(i);
                }
                i++;
            }
            for (auto& k : oldBackups) {
                profiles.erase(profiles.begin() + k);
            }
            json newBackup = json::object(
                {{"name", "_backup"},
                 {"L_JC", ColorSwapper::BGRToHex(color_left.main)},
                 {"L_BTN", ColorSwapper::BGRToHex(color_left.sub)},
                 {"R_JC", ColorSwapper::BGRToHex(color_right.main)},
                 {"R_BTN", ColorSwapper::BGRToHex(color_right.sub)}});
            profiles.push_back(newBackup);
            fs::writeJsonToFile(profiles, path);
            return 0;
        }
        else {
            return -1;
        }
    }

    json backupProfile()
    {
        json newBackup;
        HidNpadControllerColor color_left;
        HidNpadControllerColor color_right;
        Result res = hidGetNpadControllerColorSplit(HidNpadIdType_Handheld, &color_left, &color_right);
        if (R_SUCCEEDED(res)) {
            newBackup = json::object(
                {{"name", "_backup"},
                 {"L_JC", ColorSwapper::BGRToHex(color_left.main)},
                 {"L_BTN", ColorSwapper::BGRToHex(color_left.sub)},
                 {"R_JC", ColorSwapper::BGRToHex(color_right.main)},
                 {"R_BTN", ColorSwapper::BGRToHex(color_right.sub)}});
        }
        return newBackup;
    }

    std::vector<std::pair<std::string, std::vector<int>>> getProfiles(const std::string& path)
    {
        std::vector<std::pair<std::string, std::vector<int>>> res;
        bool properData;
        std::fstream profilesFile;
        json profilesJson = fs::parseJsonFile(path);
        if (profilesJson.empty()) {
            profilesJson = {{{"L_BTN", "0A1E0A"},
                             {"L_JC", "82FF96"},
                             {"R_BTN", "0A1E28"},
                             {"R_JC", "96F5F5"},
                             {"name", "Animal Crossing: New Horizons"}}};
        }
        for (const auto& x : profilesJson.items()) {
            std::string name = x.value()["name"];
            std::vector<std::string> values = {
                std::string(x.value()["L_JC"]),
                std::string(x.value()["L_BTN"]),
                std::string(x.value()["R_JC"]),
                std::string(x.value()["R_BTN"])};
            properData = true;
            for (auto& str : values) {
                if (!ColorSwapper::isHexaAnd3Bytes(str)) {
                    properData = false;
                }
            }
            if (properData) {
                if (name == "") name = "Unamed";
                res.push_back(std::make_pair(name, (std::vector<int>){
                                                       ColorSwapper::hexToBGR(values[0]),
                                                       ColorSwapper::hexToBGR(values[1]),
                                                       ColorSwapper::hexToBGR(values[2]),
                                                       ColorSwapper::hexToBGR(values[3])}));
            }
        }
        return res;
    }

    void changeJCColor(const std::vector<int>& values)
    {
        hiddbgInitialize();
        hidsysInitialize();
        ProgressEvent::instance().reset();
        ProgressEvent::instance().setStep(1);
        int res = setColor(values);
        if (res != 0) {
            util::showDialogBox("Could not change the Joy-Cons color. Make sure they're docked and try again.\nError :" + std::to_string(res), "Ok");
        }
        hiddbgExit();
        hidsysExit();
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
    }

    void backupJCColor(const std::string& path)
    {
        hiddbgInitialize();
        hidsysInitialize();
        ProgressEvent::instance().reset();
        ProgressEvent::instance().setStep(1);
        json backup;
        json profiles;
        std::fstream profilesFile;
        if (std::filesystem::exists(path)) {
            profilesFile.open(path, std::fstream::in);
            profilesFile >> profiles;
            profilesFile.close();
        }

        std::vector<int> oldBackups;
        int i = 0;
        for (const auto& x : profiles.items()) {
            if (x.value()["name"] == "_backup") {
                oldBackups.push_back(i);
            }
            i++;
        }
        for (auto& k : oldBackups) {
            profiles.erase(profiles.begin() + k);
        }

        while (backup.empty()) {
            backup = backupProfile();
        }

        profiles.push_back(backup);
        //backup.push_back(profiles);
        fs::writeJsonToFile(profiles, path);
        hiddbgExit();
        hidsysExit();
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
    }

}  // namespace JC

namespace PC {

    int setColor(const std::vector<int>& colors)
    {
        Result pads, pc;
        int res = 0;
        s32 nbEntries;
        HidsysUniquePadId UniquePadIds[1] = {};

        pads = hidsysGetUniquePadsFromNpad(HidNpadIdType_No1, UniquePadIds, 1, &nbEntries);
        if (R_SUCCEEDED(pads)) {
            pc = hiddbgUpdateControllerColor(colors[0], colors[1], UniquePadIds[0]);
            if (R_FAILED(pc)) res += 1;
        }
        else {
            res += 4;
        }
        return res;
    }

    int backupToJSON(json& profiles, const std::string& path)
    {
        HidNpadControllerColor color;
        Result res = hidGetNpadControllerColorSingle(HidNpadIdType_No1, &color);
        std::vector<int> oldBackups;
        if (R_SUCCEEDED(res)) {
            int i = 0;
            for (const auto& x : profiles.items()) {
                if (x.value()["name"] == "_backup") {
                    oldBackups.push_back(i);
                }
                i++;
            }
            for (auto& k : oldBackups) {
                profiles.erase(profiles.begin() + k);
            }
            json newBackup = json::object(
                {{"name", "_backup"},
                 {"BODY", ColorSwapper::BGRToHex(color.main)},
                 {"BTN", ColorSwapper::BGRToHex(color.sub)}});
            profiles.push_back(newBackup);
            fs::writeJsonToFile(profiles, path);
            return 0;
        }
        else {
            return -1;
        }
    }

    json backupProfile()
    {
        json newBackup;
        HidNpadControllerColor color;
        Result res = hidGetNpadControllerColorSingle(HidNpadIdType_No1, &color);
        if (R_SUCCEEDED(res)) {
            newBackup = json::object(
                {{"name", "_backup"},
                 {"BODY", ColorSwapper::BGRToHex(color.main)},
                 {"BTN", ColorSwapper::BGRToHex(color.sub)}});
        }
        return newBackup;
    }

    std::vector<std::pair<std::string, std::vector<int>>> getProfiles(const std::string& path)
    {
        std::vector<std::pair<std::string, std::vector<int>>> res;
        bool properData;
        std::fstream profilesFile;
        json profilesJson = fs::parseJsonFile(path);
        if (profilesJson.empty()) {
            profilesJson = {{{"BTN", "e6e6e6"},
                             {"BODY", "2d2d2d"},
                             {"name", "Default black"}}};
        }
        for (const auto& x : profilesJson.items()) {
            std::string name = x.value()["name"];
            std::vector<std::string> values = {
                std::string(x.value()["BODY"]),
                std::string(x.value()["BTN"])};
            properData = true;
            for (auto& str : values) {
                if (!ColorSwapper::isHexaAnd3Bytes(str)) {
                    properData = false;
                }
            }
            if (properData) {
                if (name == "") name = "Unamed";
                res.push_back(std::make_pair(name, (std::vector<int>){
                                                       ColorSwapper::hexToBGR(values[0]),
                                                       ColorSwapper::hexToBGR(values[1])}));
            }
        }
        return res;
    }

    void changePCColor(const std::vector<int>& values)
    {
        hiddbgInitialize();
        hidsysInitialize();
        ProgressEvent::instance().reset();
        ProgressEvent::instance().setStep(1);
        int res = setColor(values);
        if (res != 0) {
            util::showDialogBox("Could not change the Pro-Con color. Make they're connected to P1. This feature may not work on unofficial controllers. \nError :" + std::to_string(res), "Ok");
        }
        hiddbgExit();
        hidsysExit();
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
    }

    void backupPCColor(const std::string& path)
    {
        hiddbgInitialize();
        hidsysInitialize();
        ProgressEvent::instance().reset();
        ProgressEvent::instance().setStep(1);
        json backup;
        json profiles;
        std::fstream profilesFile;
        if (std::filesystem::exists(path)) {
            profilesFile.open(path, std::fstream::in);
            profilesFile >> profiles;
            profilesFile.close();
        }

        std::vector<int> oldBackups;
        int i = 0;
        for (const auto& x : profiles.items()) {
            if (x.value()["name"] == "_backup") {
                oldBackups.push_back(i);
            }
            i++;
        }
        for (auto& k : oldBackups) {
            profiles.erase(profiles.begin() + k);
        }

        while (backup.empty()) {
            backup = backupProfile();
        }

        profiles.push_back(backup);
        //backup.push_back(profiles);
        fs::writeJsonToFile(profiles, path);
        hiddbgExit();
        hidsysExit();
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
    }

}  // namespace PC
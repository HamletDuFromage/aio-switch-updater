#include "fs.hpp"

#include <borealis.hpp>
#include <filesystem>
#include <fstream>

#include "constants.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

namespace {
    std::vector<std::string> splitString(const std::string& s, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
}  // namespace

namespace fs {

    bool removeDir(const std::string& path)
    {
        Result ret = 0;
        FsFileSystem* fs = fsdevGetDeviceFileSystem("sdmc");
        if (R_FAILED(ret = fsFsDeleteDirectoryRecursively(fs, path.c_str())))
            return false;
        return true;
    }

    nlohmann::ordered_json parseJsonFile(const std::string& path)
    {
        std::ifstream file(path);

        std::string fileContent((std::istreambuf_iterator<char>(file)),
                                (std::istreambuf_iterator<char>()));

        if (nlohmann::ordered_json::accept(fileContent))
            return nlohmann::ordered_json::parse(fileContent);
        else
            return nlohmann::ordered_json::object();
    }

    void writeJsonToFile(nlohmann::json& data, const std::string& path)
    {
        std::ofstream out(path);
        out << data.dump(4);
    }

    bool copyFile(const std::string& from, const std::string& to)
    {
        std::ifstream src(from, std::ios::binary);
        std::ofstream dst(to, std::ios::binary);

        if (src.good() && dst.good()) {
            dst << src.rdbuf();
            return true;
        }
        return false;
    }

    void createTree(std::string path)
    {
        std::string delimiter = "/";
        size_t pos = 0;
        std::string token;
        std::string directories("");
        while ((pos = path.find(delimiter)) != std::string::npos) {
            token = path.substr(0, pos);
            directories += token + "/";
            std::filesystem::create_directory(directories);
            path.erase(0, pos + delimiter.length());
        }
    }

    std::string copyFiles(const std::string& path)
    {
        std::string error = "";
        if (std::filesystem::exists(path)) {
            std::string str;
            std::ifstream in(path);
            if (in) {
                while (std::getline(in, str)) {
                    if (str.size() > 0) {
                        auto toMove = splitString(str, '|');
                        if (std::filesystem::exists(toMove[0]) && toMove.size() > 1) {
                            copyFile(toMove[0], toMove[1]);
                        }
                        else {
                            error += toMove[0] + "\n";
                        }
                    }
                }
            }
        }
        if (error == "") {
            error = "menus/common/all_done"_i18n;
        }
        else {
            error = "menus/tools/batch_copy_not_found"_i18n + error;
        }
        return error;
    }

    std::set<std::string> readLineByLine(const std::string& path)
    {
        std::set<std::string> titles;
        std::string str;
        std::ifstream in(path);
        if (in) {
            while (std::getline(in, str)) {
                if (str.size() > 0)
                    titles.insert(str);
            }
        }
        return titles;
    }

    Result getFreeStorageSD(s64& free)
    {
        return nsGetFreeSpaceSize(NcmStorageId_SdCard, &free);
    }

}  // namespace fs
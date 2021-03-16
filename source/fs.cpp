#include "fs.hpp"
#include <borealis.hpp>
#include <fstream>
#include <filesystem>
#include "constants.hpp"


namespace i18n = brls::i18n;
using namespace i18n::literals;

namespace fs {

    int removeDir(const char* path) {
        Result ret = 0;
        FsFileSystem *fs = fsdevGetDeviceFileSystem("sdmc");
        if (R_FAILED(ret = fsFsDeleteDirectoryRecursively(fs, path)))
            return ret;
        return 0;
    }

    nlohmann::json parseJsonFile(const char* path) {
        std::ifstream file(path);

        std::string fileContent((std::istreambuf_iterator<char>(file) ),
                                (std::istreambuf_iterator<char>()    ));

        if(nlohmann::json::accept(fileContent))   return nlohmann::json::parse(fileContent);
        else                                      return nlohmann::json::object();
    }

    void writeJsonToFile(nlohmann::json &data, const char* path) {
        std::ofstream out(path);
        out << data.dump(4);
        out.close();
    }

    bool copyFile(const char *from, const char *to){
        std::ifstream src(from, std::ios::binary);
        std::ofstream dst(to, std::ios::binary);

        if (src.good() && dst.good()) {
            dst << src.rdbuf();
            return true;
        }
        return false;
    }

    void createTree(std::string path){
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

    std::string copyFiles(const char* path) {
        nlohmann::ordered_json toMove;
        std::ifstream f(COPY_FILES_JSON);
        f >> toMove;
        f.close();
        std::string error = "";
        for (auto it = toMove.begin(); it != toMove.end(); ++it) {
            if(std::filesystem::exists(it.key())) {
                createTree(std::string(std::filesystem::path(it.value().get<std::string>()).parent_path()) + "/");
                copyFile(it.key().c_str(), it.value().get<std::string>().c_str());
            }
            else {
                error += it.key() + "\n";
            }
        }
        if(error == "") {
            error = "menus/common/all_done"_i18n;
        }
        else {
            error = "menus/tools/batch_copy_not_found"_i18n + error;
        }
        return error;
    }

    std::set<std::string> readLineByLine(const char * path){
        std::set<std::string> titles;
        std::string str;
        std::ifstream in(path);
        if(in){
            while (std::getline(in, str))
            {
                if(str.size() > 0)
                    titles.insert(str);
            }
            in.close();
        }
        return titles;
    }

}
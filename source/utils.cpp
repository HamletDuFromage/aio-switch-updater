#include "utils.hpp"

bool isServiceRunning(const char *serviceName) {
    Handle handle;
    SmServiceName service_name = smEncodeName(serviceName);
    bool running = R_FAILED(smRegisterService(&handle, service_name, false, 1));
    svcCloseHandle(handle);
    if (!running)
        smUnregisterService(service_name);

    return running;
}

CFW getCFW(){
    if(isServiceRunning("rnx"))         return rnx;
    else if(isServiceRunning("tx"))     return sxos;
    else                                return ams;
}

std::vector<std::string> htmlProcess(std::string s, std::regex rgx){
    //std::vector<std::regex> rgx = {std::regex(">(?:(?!>).)*?</a>"), std::regex(">(?:(?!>).)*?</a>")};
    std::vector<std::string> res;
    std::smatch match;
    while (std::regex_search(s, match, rgx)){
        res.push_back(match[0].str());
        s = match.suffix();
    }
    return res;
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
    createTree(DOWNLOAD_PATH);
    switch(type){
        case sigpatches:
            downloadFile(url.c_str(), SIGPATCHES_FILENAME, OFF);
            break;
        case cheats:
            downloadFile(url.c_str(), CHEATS_FILENAME, OFF);
            break;
        case fw:
            downloadFile(url.c_str(), FIRMWARE_FILENAME, OFF);
            break;
        case app:
            downloadFile(url.c_str(), APP_FILENAME, OFF);
            break;
        case cfw:
            downloadFile(url.c_str(), CFW_FILENAME, OFF);
            break;
    }
}

void extractArchive(archiveType type){
    std::vector<Title> titles;
    switch(type){
        case sigpatches:
            if(isArchive(SIGPATCHES_FILENAME)) extract(SIGPATCHES_FILENAME);
            break;
        case cheats: 
            titles = getInstalledTitlesNs();
            titles = excludeTitles((std::string(DOWNLOAD_PATH) + "exclude.txt").c_str(), titles);
            extractCheats(CHEATS_FILENAME, titles, getCFW());
            break;
        case fw:
            createTree(FIRMWARE_PATH);
            extract(FIRMWARE_FILENAME, FIRMWARE_PATH);
            break;
        case app:
            extract(APP_FILENAME);
            break;
        case cfw:
            extract(CFW_FILENAME);
            break;
    }
}

void progressTest(std::string url, archiveType type){
    ProgressEvent::instance().reset();
    ProgressEvent::instance().setTotalSteps(2);
    ProgressEvent::instance().setStep(1);
    ProgressEvent::instance().setStep(2);
}

std::string formatListItemTitle(const std::string str, size_t maxScore) {
    size_t score = 0;
    for (size_t i = 0; i < str.length(); i++)
    {
        score += std::isupper(str[i]) ? 4 : 3;
        if(score > maxScore)
        {
            return str.substr(0, i-1) + "\u2026";
        }
    }

    return str;
}

std::string formatApplicationId(u64 ApplicationId){
    std::stringstream strm;
    strm << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << ApplicationId;
    return strm.str();
}

std::set<std::string> readLineByLine(const char * path){
    std::set<std::string> titles;
    std::string str;
    std::ifstream in(path);
    if(in){
        while (std::getline(in, str))
        {
            // Line contains string of length > 0 then save it in vector
            if(str.size() > 0)
                titles.insert(str);
        }
        in.close();
    }
    return titles;
}

std::vector<std::string> fetchPayloads(){
    std::vector<std::string> payloadPaths;
    payloadPaths.push_back(ROOT_PATH);
    if(std::filesystem::exists(PAYLOAD_PATH))       payloadPaths.push_back(PAYLOAD_PATH);
    if(std::filesystem::exists(AMS_PATH))           payloadPaths.push_back(AMS_PATH);
    if(std::filesystem::exists(REINX_PATH))         payloadPaths.push_back(REINX_PATH);
    if(std::filesystem::exists(BOOTLOADER_PL_PATH)) payloadPaths.push_back(BOOTLOADER_PL_PATH);
    if(std::filesystem::exists(SXOS_PATH))          payloadPaths.push_back(SXOS_PATH);
    std::vector<std::string> res;
    for (auto& path : payloadPaths){
        for (const auto & entry : std::filesystem::directory_iterator(path)){
            if(entry.path().extension().string() == ".bin")
                res.push_back(entry.path().string().c_str());
        }

    }
    return res;
}

void shut_down(bool reboot){
    bpcInitialize();
    if(reboot) bpcRebootSystem();
    else bpcShutdownSystem();
    bpcExit();
}
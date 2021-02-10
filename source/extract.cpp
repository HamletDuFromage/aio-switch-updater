#include "extract.hpp"
#include "utils.hpp"
#include "download.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <filesystem>
#include <fstream>
#include <set>
#include <unzipper.h>
#include "progress_event.hpp"
 
namespace i18n = brls::i18n;
using namespace i18n::literals;
void extract(const char * filename, const char* workingPath, int overwriteInis){
    ProgressEvent::instance().reset();
    ProgressEvent::instance().setStep(1);
    chdir(workingPath);
    std::set<std::string> ignoreList = readLineByLine(FILES_IGNORE);
    std::set<std::string>::iterator it;
    zipper::Unzipper unzipper(filename);
    std::vector<zipper::ZipEntry> entries = unzipper.entries();
    int k = -1;
    bool isIgnored;
    ProgressEvent::instance().setTotalSteps(entries.size() + 1);
    for (int i = 0; i < (int) entries.size(); i++){
        isIgnored = false;
        it = ignoreList.begin();
        if(overwriteInis == 0 && entries[i].name.substr(entries[i].name.length() - 4) == ".ini"){
            if(!std::filesystem::exists("/" + entries[i].name)){
                unzipper.extractEntry(entries[i].name);
            }
            continue;
        }
        while (it != ignoreList.end() ){
            k = ("/" + entries[i].name).find((*it));
            if(k == 0 || k == 1){
                isIgnored = true;
                if(!std::filesystem::exists("/" + entries[i].name)){
                    unzipper.extractEntry(entries[i].name);
                }
                break;
            }
            it++;
        }
        if(!isIgnored){
            if(entries[i].name == "sept/payload.bin" || entries[i].name == "atmosphere/fusee-secondary.bin"){
                unzipper.extractEntry(entries[i].name, CONFIG_PATH_UNZIP);
            }
            else if(entries[i].name.substr(0, 13) == "hekate_ctcaer"){
                unzipper.extractEntry(entries[i].name);
                int c = 0;
                while(R_FAILED(CopyFile(("/" + entries[i].name).c_str(), UPDATE_BIN_PATH)) && c < 10){
                    c++;
                }
            }
            else{
                unzipper.extractEntry(entries[i].name);
            }
        }
        
        ProgressEvent::instance().setStep(i);
    }
    unzipper.close();
    ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
}

void extract(const char * filename, const char* workingPath, const char* toExclude){
    ProgressEvent::instance().reset();
    ProgressEvent::instance().setStep(1);
    chdir(workingPath);
    std::set<std::string> ignoreList = readLineByLine(FILES_IGNORE);
    ignoreList.insert(toExclude);
    std::set<std::string>::iterator it;
    zipper::Unzipper unzipper(filename);
    std::vector<zipper::ZipEntry> entries = unzipper.entries();
    int k = -1;
    bool isIgnored;
    ProgressEvent::instance().setTotalSteps(entries.size() + 1);
    for (int i = 0; i < (int) entries.size(); i++){
        isIgnored = false;
        it = ignoreList.begin();
        while (it != ignoreList.end()){
            k = ("/" + entries[i].name).find((*it));
            if(k == 0 || k == 1){
                isIgnored = true;
                if(!std::filesystem::exists("/" + entries[i].name)){
                    unzipper.extractEntry(entries[i].name);
                }
                break;
            }
            it++;
        }
        if(!isIgnored) {
            unzipper.extractEntry(entries[i].name);
        }
        
        ProgressEvent::instance().setStep(i);
    }
    unzipper.close();
    ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
}

std::vector<Title> getInstalledTitlesNs(){
    // This function has been cobbled together from the "app_controldata" example in devkitpro.

    // Set the rc variable to begin with
    Result rc = 0;

    // Initialise a vector of tuples, storing the title ID and the title name.
    std::vector<Title> titles;

    // Initialise an application record array, where the size is MaxTitleCount
    NsApplicationRecord *recs = new NsApplicationRecord[MaxTitleCount]();

    // Set the buffer to NULL
    NsApplicationControlData *buf=NULL;
    // Set outsize to 0
    u64 outsize=0;
    // Set the language entry to NULL
    NacpLanguageEntry *langentry = NULL;

    // Create a char array to store the name of the title
    char name[0x201];

    // Set the total records to 0
    s32 total = 0;
    // Set a failed counter to 0
    int totalFailed = 0;
    // Fill the recs array with application records
    rc = nsListApplicationRecord(recs, MaxTitleCount, 0, &total);

    // If filling the recs array was successful
    if (R_SUCCEEDED(rc)){
        // Loop through records
        for (s32 i = 0; i < total; i++){

            // Reset varaibles for accessing memory
            outsize=0;
            buf = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
            if (buf==NULL) {
                rc = MAKERESULT(Module_Libnx, LibnxError_OutOfMemory);
            } else {
                memset(buf, 0, sizeof(NsApplicationControlData));
            }
            
            if (R_SUCCEEDED(rc)) {
                rc = nsInitialize();
                if (R_FAILED(rc)) {
                    printf("nsInitialize() failed: 0x%x\n", rc);
                }
            }
            // Get the application control data for the current record
            rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, recs[i].application_id, buf, sizeof(NsApplicationControlData), &outsize);

            if (R_FAILED(rc)) {
                totalFailed++;
            }

            if (outsize < sizeof(buf->nacp)) {
                rc = -1;
            }

            // If application control data was retrieved successfully
            if (R_SUCCEEDED(rc)) {
                rc = nacpGetLanguageEntry(&buf->nacp, &langentry);
            }

            // If nacp language entry was retrieved successfully
            if (R_SUCCEEDED(rc)) {
                memset(name, 0, sizeof(name));
                strncpy(name, langentry->name, sizeof(name)-1); //Don't assume the nacp string is NUL-terminated for safety.
                titles.push_back({formatApplicationId(recs[i].application_id), name});
            }

            nsExit();
        }
    }

    free(buf);
    delete[] recs;
    std::sort(titles.begin(), titles.end());
    return titles;
}

std::vector<Title> excludeTitles(const char* path, std::vector<Title> listedTitles){
    // Initialise a vector of titles
    std::vector<Title> titles;
    // Open a file stream
    std::ifstream file(path);
    // Declare a total variable and set to 0
    int total = 0;
    // Declare a name variable
    std::string name;


    if (file.is_open()) {
        std::string line;
        // Read each line of the file in
        while (std::getline(file, line)) {
            name = "No name provided";
            // Change the line to uppercase
            std::transform(line.begin(), line.end(), line.begin(), ::toupper);

            for(int i = 0; i < (int)listedTitles.size(); i++) {
                // Iterate through the listedTitles, and compare the id's in the file, to the installed title ids
                // When a match is found, it sets the name of the excluded title to the name of the installed title
                if(listedTitles.at(i).id == line) {
                    name = listedTitles.at(i).name;
                    break;
                }
            }
            // Push a new title to the titles vector and increment the total number of excluded titles
            titles.push_back({line, name});
            total++;
        }
        // Close the file
        file.close();
    }

    // Sort the titles list (uses an overloaded < operator to sort based on ID's)
    std::sort(titles.begin(), titles.end());

    // Create a vector to store the difference between the excluded titles list, and the installed title list
    std::vector<Title> diff;
    // Populate the difference vector
    std::set_difference(listedTitles.begin(), listedTitles.end(), titles.begin(), titles.end(), 
                         std::inserter(diff, diff.begin()));
    return diff;
}

bool caselessCompare (const std::string& a, const std::string& b){
    return strcasecmp(a.c_str(), b.c_str()) < 0;
}

void extractCheats(const char * zipPath, std::vector<Title> titles, CFW cfw, bool credits){
    //TODO: REWRITE WITH SETS INSTEAD OF VECTORS
    ProgressEvent::instance().reset();
    zipper::Unzipper unzipper(zipPath);
    std::vector<zipper::ZipEntry> entries = unzipper.entries();
    std::set<std::string> extractedTitles;
    int offset = 0;
    switch(cfw){
        case ams:
            offset = std::string(CONTENTS_PATH).length();
            std::filesystem::create_directory(AMS_PATH);
            std::filesystem::create_directory(AMS_CONTENTS);
            chdir(AMS_PATH);
            break;
        case rnx:
            offset = std::string(CONTENTS_PATH).length();
            std::filesystem::create_directory(REINX_PATH);
            std::filesystem::create_directory(REINX_CONTENTS);
            chdir(REINX_PATH);
            break;
        case sxos:
            offset = std::string(TITLES_PATH).length();
            std::filesystem::create_directory(SXOS_PATH);
            std::filesystem::create_directory(SXOS_TITLES);
            chdir(SXOS_PATH);
            break;
    }

    std::vector<std::string> entriesNames;
    std::vector<int> parentIndexes;
    for (size_t i = 1; i < entries.size(); i++){
        entriesNames.push_back(entries[i].name);
    }


    std::sort(entriesNames.begin(), entriesNames.end(), caselessCompare);

    std::vector<std::string> parents;
    std::vector<std::vector<std::string>> children;
    std::vector<std::string> tempChildren;


    size_t k = 0;
    while(k < entriesNames.size()){
        if(entriesNames[k].length() == (size_t) (offset + 17)){
            parents.push_back(entriesNames[k]);
            k++;
            while(entriesNames[k].length() != (size_t) (offset + 17) && k < entriesNames.size()){
                if(credits == false){ 
                    if(strcasecmp(entriesNames[k].substr(offset + 16, 7).c_str(), "/cheats") == 0){
                        tempChildren.push_back(entriesNames[k]);
                    }
                }
                else{
                    tempChildren.push_back(entriesNames[k]);
                }
                k++;
            }
            children.push_back(tempChildren);
            tempChildren.clear (); 
        }
        else{
            k++;
        }
    }

    size_t lastL = 0;
    std::string name;
    std::string id;
    ProgressEvent::instance().setTotalSteps(titles.size());
    for(size_t j = 0; j < titles.size(); j++){
        for(size_t l = lastL; l < parents.size(); l++){
            if(strcasecmp((titles.at(j).id).c_str(), parents[l].substr(offset, 16).c_str()) == 0){
                unzipper.extractEntry(parents[l]);
                for(auto& e : children[l]){
                    unzipper.extractEntry(e);
                    extractedTitles.insert(id);
                    ProgressEvent::instance().setStep(j);
                    name = "No name retrieved.";
                    id = e.substr(offset, 16);
                    std::transform(id.begin(), id.end(), id.begin(), ::toupper);
                }
                lastL = l;
                break;
            }
        }
    }
    unzipper.close();
    writeTitlesToFile(extractedTitles, UPDATED_TITLES_PATH);
    saveVersion(fetchTitle(CHEATS_RELEASE_URL), CHEATS_VERSION);
    ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
}

void writeTitlesToFile(std::set<std::string> titles, const char* path){
    std::ofstream updatedTitlesFile;
    std::set<std::string>::iterator it = titles.begin();
    updatedTitlesFile.open(path, std::ofstream::out | std::ofstream::trunc);
    if(updatedTitlesFile.is_open()) {
        while (it != titles.end()){
            updatedTitlesFile << (*it) << std::endl;
            it++;
        }
        updatedTitlesFile.close();
    }
}

void removeCheats(CFW cfw){
    ProgressEvent::instance().reset();
    ProgressEvent::instance().setStep(1);
    std::string path;
    switch(cfw){
        case ams:
            path = std::string(AMS_PATH) + std::string(CONTENTS_PATH);
            break;
        case rnx:
            path = std::string(REINX_PATH) + std::string(CONTENTS_PATH);
            break;
        case sxos:
            path = std::string(SXOS_PATH) + std::string(TITLES_PATH);
            break;
    }
    for (const auto & entry : std::filesystem::directory_iterator(path)){
        std::string cheatsPath =  entry.path().string() + "/cheats";
        if(std::filesystem::exists(cheatsPath)){
            for (const auto & cheat : std::filesystem::directory_iterator(cheatsPath)){
                std::filesystem::remove(cheat);
            }
            rmdir(cheatsPath.c_str());
            if(std::filesystem::is_empty(entry)){
                rmdir(entry.path().string().c_str());
            }
        }
    }
    std::filesystem::remove(UPDATED_TITLES_PATH);
    std::filesystem::remove(CHEATS_VERSION);
    ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
}
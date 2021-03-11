#include "download.hpp"
#include "utils.hpp"
#include <algorithm>
#include <time.h>
#include <math.h>
#include <curl/curl.h>
#include <chrono>

#include <string>
#include <regex>
#include <switch.h>

#include "progress_event.hpp"

#define API_AGENT           "HamletDuFromage"
#define _1MiB   0x100000

using json = nlohmann::json;

std::chrono::_V2::steady_clock::time_point time_old;
double dlold;

typedef struct
{
    char *memory;
    size_t size;
} MemoryStruct_t;

typedef struct
{
    u_int8_t *data;
    size_t data_size;
    u_int64_t offset;
    FILE *out;
} ntwrk_struct_t;

static size_t WriteMemoryCallback(void *contents, size_t size, size_t num_files, void *userp)
{
    ntwrk_struct_t *data_struct = (ntwrk_struct_t *)userp;
    size_t realsize = size * num_files;

    if (realsize + data_struct->offset >= data_struct->data_size)
    {
        fwrite(data_struct->data, data_struct->offset, 1, data_struct->out);
        data_struct->offset = 0;
    }

    memcpy(&data_struct->data[data_struct->offset], contents, realsize);
    data_struct->offset += realsize;
    data_struct->data[data_struct->offset] = 0;
    return realsize;
}

int download_progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
{
    if (dltotal <= 0.0) return 0;

    double fractionDownloaded = dlnow / dltotal;
    int counter = (int) (fractionDownloaded * ProgressEvent::instance().getMax()); //20 is the number of increments
    ProgressEvent::instance().setStep(std::min(ProgressEvent::instance().getMax() - 1, counter));
    ProgressEvent::instance().setNow(dlnow);
    ProgressEvent::instance().setTotalCount(dltotal);
    auto time_now = std::chrono::steady_clock::now();
    double elasped_time = ((std::chrono::duration<double>) (time_now - time_old)).count();
    if(elasped_time > 1.0f) {
        ProgressEvent::instance().setSpeed((dlnow - dlold) / elasped_time);
        dlold = dlnow;
        time_old = time_now;
    }
    return 0;
}

std::vector<std::uint8_t> downloadFile(const char *url, const char *output, int api)
{
    ProgressEvent::instance().reset();
    CURL *curl = curl_easy_init();
    ntwrk_struct_t chunk = {0};
    time_old = std::chrono::steady_clock::now();
    dlold = 0.0f;
    if (curl)
    {
        FILE *fp = fopen(output, "wb");
        if (fp || *output == 0)
        {
            chunk.data = static_cast<u_int8_t *>(malloc(_1MiB));
            chunk.data_size = _1MiB;
            chunk.out = fp;

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, API_AGENT);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            // write calls
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

            if (api == OFF)
            {
                curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
                curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, download_progress);
            }
            curl_easy_perform(curl);
            if (fp && chunk.offset)
              fwrite(chunk.data, 1, chunk.offset, fp);

            curl_easy_cleanup(curl);
            ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
        }
    }

    if (*output == 0) {
        std::vector<std::uint8_t> res(chunk.data, chunk.data + chunk.offset);
        free(chunk.data);
        fclose(chunk.out);
        return res;
    }
    else {
        free(chunk.data);
        fclose(chunk.out);
        return (std::vector<std::uint8_t>){};
    }

}

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback2(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = static_cast<char *>(realloc(mem->memory, mem->size + realsize + 1));
    if(ptr == NULL) {
        /* out of memory! */ 
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

std::string fetchTitle(const char *url){
    CURL *curl_handle; 
    struct MemoryStruct chunk;
 
    chunk.memory = static_cast<char *>(malloc(1));  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */ 
 
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback2);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, API_AGENT);

    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_perform(curl_handle);
 
  /* check for errors */ 
    std::string ver = "-1";
    std::string s = std::string(chunk.memory);
    std::regex rgx("<title>.+</title>");
    std::smatch match;
    if (std::regex_search(s, match, rgx)){
        //ver = std::stoi(match[0]);
        //std::cout << match[0].str().substr(match[0].str().find(" ") + 1, 6) << std::endl;
        ver = match[0].str().substr(match[0].str().find(" ") + 1, 5);
    }
    curl_easy_cleanup(curl_handle);
    free(chunk.memory);
 
    curl_global_cleanup();
 
    return ver;
}

std::string downloadPage(const char* url, std::vector<std::string> headers, std::string body){
    std::string res;
    CURL *curl_handle; 
    struct MemoryStruct chunk;
    struct curl_slist *list = NULL;
 
    chunk.memory = static_cast<char *>(malloc(1));  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */ 
 
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    if(!headers.empty()){
        for (auto& h : headers){
            list = curl_slist_append(list, h.c_str());
        }
        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, list);
    }
    if(body != "") {
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, body.c_str());
    }

    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback2);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, API_AGENT);

    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_perform(curl_handle);
    curl_easy_cleanup(curl_handle);
    res = std::string(chunk.memory);
    free(chunk.memory);
 
    curl_global_cleanup();
    return res;
}

std::vector<std::uint8_t> downloadPageBinary(const char* url, std::vector<std::string> headers, std::string body){
    CURL *curl_handle; 
    struct MemoryStruct chunk;
    struct curl_slist *list = NULL;
 
    chunk.memory = static_cast<char *>(malloc(1));  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */ 
 
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    if(!headers.empty()){
        for (auto& h : headers){
            list = curl_slist_append(list, h.c_str());
        }
        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, list);
    }
    if(body != "") {
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, body.c_str());
    }

    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback2);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, API_AGENT);

    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_perform(curl_handle);
    curl_easy_cleanup(curl_handle);
    std::vector<std::uint8_t> res(chunk.memory, chunk.memory + ((sizeof(chunk.memory)) * (chunk.size + 16)));
    free(chunk.memory);
 
    curl_global_cleanup();
    return res;
}

nlohmann::ordered_json getRequest(std::string url, std::vector<std::string> headers, std::string body) {
    std::string request;
    request = downloadPage(url.c_str(), headers, body);

    if(json::accept(request))   return nlohmann::ordered_json::parse(request);
    else                        return nlohmann::ordered_json::object();
}

std::vector<std::pair<std::string, std::string>> getLinks(const char *url) {
    std::string request;
    request = downloadPage(url);

    nlohmann::ordered_json jason = json::accept(request) ? nlohmann::ordered_json::parse(request) : nlohmann::ordered_json::object();

    std::vector<std::pair<std::string, std::string>> res;
    for (auto it = jason.begin(); it != jason.end(); ++it) {
        res.push_back(std::make_pair(it.key(), it.value()));
    }
    return res;
}